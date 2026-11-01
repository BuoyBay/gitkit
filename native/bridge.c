/* Pointer/scalar boundary for read-only local libgit2 inspection. */
#include <git2.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifdef _WIN32
#define BGTK_EXPORT __declspec(dllexport)
#else
#define BGTK_EXPORT __attribute__((visibility("default")))
#endif
#ifdef _MSC_VER
#define BGTK_TLS __declspec(thread)
#else
#define BGTK_TLS _Thread_local
#endif

static BGTK_TLS char last_error[2048];
static void set_error(const char *fallback) {
    const git_error *error = git_error_last();
    snprintf(last_error, sizeof(last_error), "%s", error && error->message ? error->message : fallback);
}
static void invalid(const char *message) { snprintf(last_error, sizeof(last_error), "%s", message); }
BGTK_EXPORT const char *bgtk_last_error(void) { return last_error; }

typedef struct { git_repository *repository; } BGTKRepository;
typedef struct { char *branch; char oid[GIT_OID_MAX_HEXSIZE + 1]; int unborn; int detached; } BGTKHead;
typedef struct { char *data; size_t size; size_t capacity; uint64_t limit; int failed; } BGTKBuffer;

BGTK_EXPORT void *bgtk_open(const char *directory, int64_t discover) {
    last_error[0] = 0;
    if (!directory || !*directory) { invalid("A nonempty repository directory is required"); return NULL; }
    if (git_libgit2_init() < 0) { set_error("Cannot initialize libgit2"); return NULL; }
    BGTKRepository *owner = calloc(1, sizeof(*owner));
    if (!owner) { invalid("Repository allocation failed"); git_libgit2_shutdown(); return NULL; }
    unsigned int flags = discover ? 0 : GIT_REPOSITORY_OPEN_NO_SEARCH;
    if (git_repository_open_ext(&owner->repository, directory, flags, NULL) < 0) {
        set_error("Cannot open repository"); free(owner); git_libgit2_shutdown(); return NULL;
    }
    return owner;
}
BGTK_EXPORT void bgtk_close(BGTKRepository *owner) {
    if (!owner) return;
    git_repository_free(owner->repository);
    free(owner);
    git_libgit2_shutdown();
}
BGTK_EXPORT const char *bgtk_repository_path(const BGTKRepository *owner) {
    return owner ? git_repository_path(owner->repository) : "";
}
BGTK_EXPORT const char *bgtk_workdir(const BGTKRepository *owner) {
    const char *directory = owner ? git_repository_workdir(owner->repository) : NULL;
    return directory ? directory : "";
}
BGTK_EXPORT int64_t bgtk_is_bare(const BGTKRepository *owner) {
    return owner ? git_repository_is_bare(owner->repository) : 0;
}
static char *copy_text(const char *text) {
    if (!text) text = "";
    size_t length = strlen(text);
    char *copy = malloc(length + 1);
    if (copy) memcpy(copy, text, length + 1);
    return copy;
}
BGTK_EXPORT void bgtk_head_free(BGTKHead *head) { if (head) { free(head->branch); free(head); } }
BGTK_EXPORT void *bgtk_head(BGTKRepository *owner) {
    if (!owner) { invalid("Repository is closed"); return NULL; }
    BGTKHead *snapshot = calloc(1, sizeof(*snapshot));
    if (!snapshot) { invalid("HEAD snapshot allocation failed"); return NULL; }
    git_reference *head = NULL;
    int rc = git_repository_head(&head, owner->repository);
    if (rc == GIT_EUNBORNBRANCH) {
        snapshot->unborn = 1;
        if (git_reference_lookup(&head, owner->repository, "HEAD") < 0) goto failure;
        const char *target = git_reference_symbolic_target(head);
        if (target && !strncmp(target, "refs/heads/", 11)) target += 11;
        snapshot->branch = copy_text(target);
    } else if (rc < 0) {
        goto failure;
    } else {
        const git_oid *oid = git_reference_target(head);
        if (!oid || !git_oid_tostr(snapshot->oid, sizeof(snapshot->oid), oid)) goto failure;
        int detached = git_repository_head_detached(owner->repository);
        if (detached < 0) goto failure;
        snapshot->detached = detached;
        snapshot->branch = copy_text(detached ? "" : git_reference_shorthand(head));
    }
    git_reference_free(head);
    if (!snapshot->branch) { bgtk_head_free(snapshot); invalid("HEAD name allocation failed"); return NULL; }
    return snapshot;
failure:
    set_error("Cannot resolve HEAD");
    git_reference_free(head);
    bgtk_head_free(snapshot);
    return NULL;
}
BGTK_EXPORT const char *bgtk_head_branch(const BGTKHead *head) { return head ? head->branch : ""; }
BGTK_EXPORT const char *bgtk_head_oid(const BGTKHead *head) { return head ? head->oid : ""; }
BGTK_EXPORT int64_t bgtk_head_unborn(const BGTKHead *head) { return head ? head->unborn : 0; }
BGTK_EXPORT int64_t bgtk_head_detached(const BGTKHead *head) { return head ? head->detached : 0; }

BGTK_EXPORT void *bgtk_status(BGTKRepository *owner) {
    if (!owner) { invalid("Repository is closed"); return NULL; }
    git_status_options options = GIT_STATUS_OPTIONS_INIT;
    options.show = GIT_STATUS_SHOW_INDEX_AND_WORKDIR;
    options.flags = GIT_STATUS_OPT_INCLUDE_UNTRACKED | GIT_STATUS_OPT_RECURSE_UNTRACKED_DIRS |
                    GIT_STATUS_OPT_RENAMES_HEAD_TO_INDEX | GIT_STATUS_OPT_RENAMES_INDEX_TO_WORKDIR |
                    GIT_STATUS_OPT_SORT_CASE_SENSITIVELY | GIT_STATUS_OPT_INCLUDE_UNREADABLE;
    /* Never request GIT_STATUS_OPT_UPDATE_INDEX: status must not write an index. */
    git_status_list *list = NULL;
    if (git_status_list_new(&list, owner->repository, &options) < 0) {
        set_error("Cannot read repository status"); return NULL;
    }
    return list;
}
BGTK_EXPORT void bgtk_status_free(git_status_list *list) { git_status_list_free(list); }
BGTK_EXPORT int64_t bgtk_status_count(git_status_list *list) {
    return list ? (int64_t)git_status_list_entrycount(list) : -1;
}
static const git_status_entry *entry_at(git_status_list *list, int64_t index) {
    if (!list || index < 0 || (uint64_t)index >= git_status_list_entrycount(list)) return NULL;
    return git_status_byindex(list, (size_t)index);
}
BGTK_EXPORT int64_t bgtk_status_flags(git_status_list *list, int64_t index) {
    const git_status_entry *entry = entry_at(list, index);
    return entry ? (int64_t)entry->status : -1;
}
BGTK_EXPORT const char *bgtk_status_path(git_status_list *list, int64_t index) {
    const git_status_entry *entry = entry_at(list, index);
    if (!entry) return "";
    const git_diff_delta *delta = entry->index_to_workdir ? entry->index_to_workdir : entry->head_to_index;
    return delta && delta->new_file.path ? delta->new_file.path : "";
}
BGTK_EXPORT const char *bgtk_status_old_path(git_status_list *list, int64_t index) {
    const git_status_entry *entry = entry_at(list, index);
    if (!entry) return "";
    const git_diff_delta *delta = entry->head_to_index ? entry->head_to_index : entry->index_to_workdir;
    return delta && delta->old_file.path ? delta->old_file.path : "";
}

BGTK_EXPORT void bgtk_buffer_free(BGTKBuffer *buffer) {
    if (!buffer) return;
    free(buffer->data); free(buffer);
}
/* Stop before retaining a patch chunk beyond the caller's byte budget.
 * libgit2 still owns its input/patch working memory; this is an output cap. */
static int append_diff_line(const git_diff_delta *delta, const git_diff_hunk *hunk,
                            const git_diff_line *line, void *payload) {
    (void)delta; (void)hunk;
    BGTKBuffer *buffer = payload;
    size_t prefix = line->origin == GIT_DIFF_LINE_CONTEXT ||
                    line->origin == GIT_DIFF_LINE_ADDITION ||
                    line->origin == GIT_DIFF_LINE_DELETION;
    if (line->content_len > buffer->limit - buffer->size ||
        prefix > buffer->limit - buffer->size - line->content_len ||
        prefix > SIZE_MAX - buffer->size ||
        line->content_len > SIZE_MAX - buffer->size - prefix) {
        invalid("Diff output exceeds the configured byte limit");
        buffer->failed = 1;
        return -1;
    }
    size_t needed = buffer->size + prefix + line->content_len;
    if (needed > buffer->capacity) {
        size_t capacity = buffer->capacity ? buffer->capacity : 4096;
        if ((uint64_t)capacity > buffer->limit) capacity = (size_t)buffer->limit;
        while (capacity < needed) {
            if (capacity > SIZE_MAX / 2 || (uint64_t)capacity * 2 > buffer->limit) {
                capacity = needed;
                break;
            }
            capacity *= 2;
        }
        char *data = realloc(buffer->data, capacity);
        if (!data) {
            invalid("Diff allocation failed"); buffer->failed = 1; return -1;
        }
        buffer->data = data; buffer->capacity = capacity;
    }
    if (prefix) buffer->data[buffer->size++] = line->origin;
    if (line->content_len) memcpy(buffer->data + buffer->size, line->content, line->content_len);
    buffer->size = needed;
    return 0;
}
BGTK_EXPORT void *bgtk_diff(BGTKRepository *owner, int64_t staged, int64_t context_lines, int64_t max_bytes) {
    if (!owner || context_lines < 0 || context_lines > 1000 || max_bytes < 0) {
        invalid("Invalid diff arguments or closed repository"); return NULL;
    }
    git_diff_options options = GIT_DIFF_OPTIONS_INIT;
    options.context_lines = (uint32_t)context_lines;
    options.max_size = 8 * 1024 * 1024; /* Larger individual files are treated as binary. */
    git_diff *diff = NULL;
    git_tree *tree = NULL;
    git_object *object = NULL;
    int rc;
    if (staged) {
        rc = git_revparse_single(&object, owner->repository, "HEAD^{tree}");
        if (rc == GIT_EUNBORNBRANCH || rc == GIT_ENOTFOUND) {
            /* Only an actually unborn HEAD permits using the empty baseline. */
            int unborn = git_repository_head_unborn(owner->repository);
            if (unborn != 1) goto failure;
        } else if (rc < 0) goto failure;
        tree = (git_tree *)object;
        rc = git_diff_tree_to_index(&diff, owner->repository, tree, NULL, &options);
    } else {
        rc = git_diff_index_to_workdir(&diff, owner->repository, NULL, &options);
    }
    if (rc < 0) goto failure;
    BGTKBuffer *buffer = calloc(1, sizeof(*buffer));
    if (!buffer) { invalid("Diff allocation failed"); goto allocation_failure; }
    buffer->limit = (uint64_t)max_bytes;
    if (git_diff_print(diff, GIT_DIFF_FORMAT_PATCH, append_diff_line, buffer) < 0) {
        if (!buffer->failed) set_error("Cannot render diff");
        bgtk_buffer_free(buffer); goto allocation_failure;
    }
    git_diff_free(diff); git_object_free(object);
    return buffer;
failure:
    set_error("Cannot read repository diff");
allocation_failure:
    git_diff_free(diff); git_object_free(object);
    return NULL;
}
BGTK_EXPORT const void *bgtk_buffer_data(const BGTKBuffer *buffer) {
    return buffer ? buffer->data : NULL;
}
BGTK_EXPORT int64_t bgtk_buffer_size(const BGTKBuffer *buffer) {
    return buffer ? (int64_t)buffer->size : -1;
}
