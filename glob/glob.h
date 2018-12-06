#pragma once

#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace hf {

// Abstract of filesystem interface for potential support of
// non-POSIX systems and to provide a mock interface for unit
// testing.

struct glob_fs_provider {
    using action_type = std::function<void (const char*)>;

    template <typename Impl>
    glob_fs_provider(Impl impl): inner_(new wrap<Impl>(std::move(impl))) {}

    glob_fs_provider(const glob_fs_provider& x): inner_(x.inner_->clone()) {}

    bool is_directory(const char* path) const {
        return inner_->is_directory(path);
    }
    bool exists(const char* path) const {
        return inner_->exists(path);
    }
    void for_each_directory(const char* path, action_type action) const {
        inner_->for_each_directory(path, action);
    }
    void for_each_entry(const char* path, action_type action) const {
        inner_->for_each_entry(path, action);
    }

private:
    struct base {
        virtual bool is_directory(const char* path) const = 0;
        virtual bool exists(const char* path) const = 0;
        virtual void for_each_directory(const char* path, action_type action) const = 0;
        virtual void for_each_entry(const char* path, action_type action) const = 0;
        virtual base* clone() const = 0;
    };

    template <typename Impl>
    struct wrap: base {
        wrap(Impl impl): impl_(std::move(impl)) {}

        bool is_directory(const char* path) const override {
            return impl_.is_directory(path);
        }
        bool exists(const char* path) const override {
            return impl_.exists(path);
        }
        void for_each_directory(const char* path, action_type action) const override {
            impl_.for_each_directory(path, action);
        }
        void for_each_entry(const char* path, action_type action) const override {
            impl_.for_each_entry(path, action);
        }
        base* clone() const override {
            return new wrap(impl_);
        }

        Impl impl_;
    };

    std::unique_ptr<base> inner_;
};

extern glob_fs_provider glob_native_provider;

std::vector<std::string> glob(const char* pattern, const glob_fs_provider& = glob_native_provider);


} // namespace hf
