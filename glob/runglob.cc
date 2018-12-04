#include <vector>
#include <filesystem>

#include "match.h"

struct pattern_component {
    const char* p = nullptr;
    bool literal = false;
};

// Return first component, writing NUL at end.
// Update pattern to one-past-the-end of component,
// or nullptr if last component.

pattern_component tokenize(char*& pattern) {
    pattern_component k = {pattern, false};

    char* s = nullptr; // end of literal component, nullptr => none
    char* p = pattern;
    bool meta = false;

    do {
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
            case '\':
                if (!escape && !in_charclass) escape = true;
                break;
            case ']':
                if (in_charclass) in_chaclass = false;
                break;
            default: ;
            }
        }
        if (!meta) s = p;
        if (*p) ++p;
    } while (!meta && *p);

    if (s) k.literal = true;
    else s = p;

    pattern = *s? s+1: nullptr;
    *s = 0;
    return k;
}

std::vector<std::filesystem::path> glob(std::string pattern) {
    namespace fs = std::filesystem;
    constexpr auto diropts = fs::directory_options::follow_directory_symlink
                           | fs::directory_options::skip_permission_denied;

    char* c = pattern.data();
    if (!*c) return {};

    std::vector<fs::path> paths(1), new_paths;
    if (c[0]=='/') {
        paths[0]="/";
        ++c;
    }

    do {
        pattern_component component = tokenize(c);
        auto add_conditionally = [&new_path, dir = !c](const fs::path& p) {
            if (dir? fs::is_directory(p): fs::exists(p)) new_path.push_back(p);
        };

        if (component.literal) {
            fs::path suffix(component.pattern);
            for (auto p: paths) add_conditionally(p/suffix);
        }
        else {
            for (const auto& prefix: paths) {
                for (const auto& suffix: fs::directory_iterator(p, dirops)) {
                    auto path = prefix/suffix;
                    if (nfa::match(component.pattern, path.c_str())) add_conditionally(path);
                }
            }
        }

        std::swap(paths, new_paths);
        new_paths.clear();
    } while (c);

    return paths;
}

