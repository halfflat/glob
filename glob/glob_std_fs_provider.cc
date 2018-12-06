#include <filesystem>
#include <functional>

#include "glob_std_fs_provider.h"

namespace hf {

namespace fs = std::filesystem;
using action_type = std::function<void (const char*)>;

static constexpr auto diropts =
    fs::directory_options::follow_directory_symlink |
    fs::directory_options::skip_permission_denied;


bool glob_std_fs_impl::is_directory(const char* path) const {
    return fs::is_directory(*path? path: ".");
}

bool glob_std_fs_impl::exists(const char* path) const {
    return fs::exists(*path? path: ".");
}

void glob_std_fs_impl::for_each_directory(const char* path, action_type action) const {
    for (const auto& p: fs::directory_iterator(*path? path: ".", diropts)) {
        std::error_code ec;
        if (fs::is_directory(p.path(), ec)) action(p.path().c_str());
    }
}

void glob_std_fs_impl::for_each_entry(const char* path, action_type action) const {
    for (const auto& p: fs::directory_iterator(*path? path: ".", diropts)) {
        std::error_code ec;
        action(p.path().c_str());
    }
}

} // namespace hf
