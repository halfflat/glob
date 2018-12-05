#include <iterator>
#include <string>
#include <unordered_map>

#include <gtest/gtest.h>

#include "glob/runglob.h"


struct mock_fs_provider {
    std::unordered_multimap<std::string, std::string> tree;

    void add_path(const char* name) {
        if (!*name) return;

        const char* p = *name=='/'? name+1: name;

        for (const char* c = p; *p; p = c++) {
            while (*c && *c!='/') ++c;

            std::pair<std::string, std::string> entry{std::string{name, p}, std::string{name, c}};
            if (tree.find(entry.second)==tree.end()) {
                tree.insert(entry);
                tree.insert({entry.second, std::string{}});
            }
        }
    }

    static std::string canonical_key(const char* path) {
        std::string key(path);
        return key.length()>1 && key.back()=='/'? key.substr(0, key.length()-1): key;
    }

    bool is_directory(const char* path) const {
        auto r = tree.equal_range(canonical_key(path));
        return r.first!=r.second && std::next(r.first)!=r.second;
    }

    bool exists(const char* path) const {
        return tree.find(canonical_key(path))!=tree.end();
    }

    void for_each_directory(const char* path, action_type action) const {
        auto r = tree.equal_range(canonical_key(path));
        for (auto i = r.first; i!=r.second; ++i) {
            auto entry = i->second;
            if (entry.empty()) continue;

            auto s = tree.equal_range(entry);
            if (s.first!=s.second && std::next(s.first)!=s.second) action(entry.c_str());
        }
    }

    void for_each_entry(const char* path, action_type action) const {
        auto r = tree.equal_range(canonical_key(path));
        for (auto i = r.first; i!=r.second; ++i) {
            auto entry = i->second;
            if (!entry.empty()) action(entry.c_str());
        }
    }
};

TEST(glob, todo) {

}

