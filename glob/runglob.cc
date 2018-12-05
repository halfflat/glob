#include <cstring>
#include <filesystem>
#include <string>
#include <vector>

#include "match.h"
#include "runglob.h"

namespace hf {

// Return first component, overwriting delimitter with NUL.
// Set pattern to beginning of next path component, skipping delimiters.

pattern_component tokenize(char*& pattern) {
    if (!*pattern) return {pattern, true, false};

    char* s = nullptr;
    char* p = pattern;
    bool meta = false;

    do {
        while (*p=='/') ++p;

        bool in_charclass = false;
        bool escape = false;
        for (;*p && *p!='/'; ++p) {
            switch (*p) {
            case '[':
                if (!escape) {
                    in_charclass = true;
                    meta = true;
                }
                break;
            case '*':
                if (!escape) meta = true;
                break;
            case '?':
                if (!escape) meta = true;
                break;
            case '\\':
                if (!escape && !in_charclass) escape = true;
                break;
            case ']':
                if (in_charclass) in_charclass = false;
                break;
            default: ;
            }
        }
        if (!meta) s = p;
    } while (!meta && *p);

    pattern_component k = { pattern };
    k.literal = (bool)s;

    if (!s) s = p;
    k.directory = !*s;

    pattern = s;
    while (*pattern=='/') ++pattern;

    *s = 0;
    return k;
}

// let's just use std::filesystem for C++17 ...

namespace fs = std::filesystem;

struct posix_impl {
    static constexpr auto diropts = fs::directory_options::follow_directory_symlink
                                  | fs::directory_options::skip_permission_denied;

    bool is_directory(const char* path) const {
        return fs::is_directory(path);
    }

    bool exists(const char* path) const {
        return fs::exists(path);
    }

    void for_each_directory(const char* path, glob_fs_provider::action_type action) const {
        for (const auto& p: fs::directory_iterator(path, diropts)) {
            std::error_code ec;
            if (fs::is_directory(p.path(), ec)) action(p.path().c_str());
        }
    }

    void for_each_entry(const char* path, glob_fs_provider::action_type action) const {
        for (const auto& p: fs::directory_iterator(path, diropts)) {
            std::error_code ec;
            action(p.path().c_str());
        }
    }
} posix_impl_;

glob_fs_provider glob_posix_provider{posix_impl_};

void glob(const char* pattern, std::function<void (const char*)> callback, glob_fs_provider fs) {
    if (!*pattern) return;

    std::vector<char> pcopy(1+std::strlen(pattern));
    std::strcpy(pcopy.data(), pattern);
    char* c = pcopy.data();
    if (!*c) return;

    std::vector<std::string> paths, new_paths;
    paths.push_back("");

    do {
        pattern_component component = tokenize(c);

        if (component.literal) {
            for (auto p: paths) {
                p += '/';
                p += component.pattern;

                if (component.directory) {
                    if (fs.is_directory(p.c_str())) new_paths.push_back(std::move(p));
                }
                else {
                    if (fs.exists(p.c_str())) new_paths.push_back(std::move(p));
                }
            }
        }
        else {
            auto push_if_match = [&new_paths, pattern = component.pattern](const char* x) {
                const char* tail = std::strrchr(x, '/');
                if (!tail) tail = x;
                if (nfa::match(pattern, tail)) new_paths.push_back(x);
            };

            for (auto p: paths) {
                if (component.directory) fs.for_each_directory(p.c_str(), push_if_match);
                else fs.for_each_entry(p.c_str(), push_if_match);
            }
        }

        std::swap(paths, new_paths);
        new_paths.clear();
    } while (*c);

    for (auto& p: paths) {
        callback(p.c_str());
    }
}

}
