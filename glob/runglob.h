#pragma once

#include <memory>

namespace hf {

struct pattern_component {
    const char* pattern = nullptr;
    bool literal = false;
    bool directory = false;
};

// Return first component, writing NUL at end.
// Update pattern to one-past-the-end of component,
// or nullptr if last component.

pattern_component tokenize(char*& pattern);

bool path_is_directory(const std::filesystem::path& p) {
    return p.empty() || std::prev(p.end())->empty();

}

// Abstract of filesystem i/face for mocking/testing.
// (Assuming everything we need can be represented by
// narrow C strings.)

struct glob_fs_provider {
    using action_type = std::function<void (const char*)>;

    template <typename Impl>
    glob_fs_provider(Impl impl): inner_(new wrap<Impl>(std::move(impl))) {}

    bool is_directory(const char* path) const {
        return inner->is_directory(path);
    }
    bool exists(const char* path) const {
        return inner->exists(path);
    }
    void for_each_directory(const char* path, action_type action) const {
        inner->for_each_directory(path, action);
    }
    void for_each_entry(const char* path, action_type action) const {
        inner->for_each_entry(path, action);
    }

private:
    struct base {
        virtual bool is_directory(const char* path) const = 0;
        virtual bool exists(const char* path) const = 0;
        virtual void for_each_directory(const char* path, action_type action) const = 0;
        virtual void for_each_entry(const char* path, action_type action) const = 0;
    };

    template <typename Impl>
    struct wrap: base {
        wrap(Impl impl): impl_(std::move(impl)) {}

        bool is_directory(const char* path) const override {
            return impl.is_directory(path);
        }
        bool exists(const char* path) const override {
            return impl.exists(path);
        }
        void for_each_directory(const char* path, action_type action) const override {
            impl.for_each_directory(path, action);
        }
        void for_each_entry(const char* path, action_type action) const override {
            impl.for_each_entry(path, action);
        }

        Impl impl_;
    };

    std::unique_ptr<base> inner_;
};

extern glob_fs_provider glob_posix_provider;

void glob(const char* pattern, std::function<void (const char*)> callback, glob_fs_provider = glob_posix_provider);

} // namespace hf
