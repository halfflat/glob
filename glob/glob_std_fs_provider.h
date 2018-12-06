#include <functional>

namespace hf {

struct glob_std_fs_impl {
    using action_type = std::function<void (const char*)>;

    bool is_directory(const char* path) const;
    bool exists(const char* path) const;
    void for_each_directory(const char* path, action_type action) const;
    void for_each_entry(const char* path, action_type action) const;
};

} // namespace hf
