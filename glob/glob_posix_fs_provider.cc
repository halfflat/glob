#include <functional>
#include <utility>

extern "C" {
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
}

#include "glob_posix_fs_provider.h"

template <typename F>
struct scope_exit {
    explicit scope_exit(F&& f): f(std::forward<F>(f)) {}
    scope_exit() = delete;
    scope_exit(scope_exit&& g): f(std::move(g.f)) { g.moved = true; }

    ~scope_exit() { if (!moved) f(); }
    F f;
    bool moved = false;
};

template <typename F>
static scope_exit<F> on_scope_exit(F&& f) { return scope_exit<F>(std::forward<F>(f)); }

namespace hf {

using action_type = std::function<void (const char*)>;

bool glob_posix_fs_impl::is_directory(const char* path) const {
    struct stat st;
    int r = stat(*path? path: ".", &st);
    return !r && st.st_mode&S_IFDIR;
}

bool glob_posix_fs_impl::exists(const char* path) const {
    struct stat st;
    int r = stat(*path? path: ".", &st);
    return !r;
}

void glob_posix_fs_impl::for_each_directory(const char* path, action_type action) const {
    DIR* dir = opendir(*path? path: ".");
    auto cleanup = on_scope_exit([dir]() { if (dir) closedir(dir); });

    if (dir) {
        while (dirent* entry = readdir(dir)) {
            if (entry->d_type==DT_DIR) action(entry->d_name);
        }
    }
}

void glob_posix_fs_impl::for_each_entry(const char* path, action_type action) const {
    DIR* dir = opendir(*path? path: ".");
    auto cleanup = on_scope_exit([dir]() { if (dir) closedir(dir); });

    if (dir) {
        while (dirent* entry = readdir(dir)) {
            action(entry->d_name);
        }
    }
}

} // namespace hf
