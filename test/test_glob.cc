#include <iterator>
#include <string>
#include <unordered_map>

#include <gtest/gtest.h>

#include "glob/glob.h"

namespace hf {

struct mock_fs_provider {
    using action_type = glob_fs_provider::action_type;

    std::unordered_multimap<std::string, std::string> tree;

    mock_fs_provider() = default;

    template <typename... Tail>
    mock_fs_provider(const char* name, Tail... tail) {
        add_path(name, tail...);
    }

    void add_path() const {}

    template <typename... Tail>
    void add_path(const char* name, Tail... tail) {
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

        add_path(tail...);
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

std::vector<std::string> sort_glob(const char* pattern, const glob_fs_provider& fs) {
    auto results = glob(pattern, fs);
    std::sort(results.begin(), results.end());
    return results;
}

TEST(glob, simple_patterns) {
    glob_fs_provider fs = mock_fs_provider{"fish", "fop", "barf", "barry", "tip"};

    using svector = std::vector<std::string>;

    EXPECT_EQ(svector({"fish", "fop"}), sort_glob("f*", fs));
    EXPECT_EQ(svector({"fop", "tip"}), sort_glob("??p", fs));
    EXPECT_EQ(svector(), sort_glob("x*", fs));
}

TEST(glob, literals) {
    glob_fs_provider fs = mock_fs_provider{
        "/abc/def/ghi",
        "/abc/de",
        "/xyz",
        "pqrs/tuv/w",
        "pqrs/tuv/wxy"
    };

    using svector = std::vector<std::string>;

    EXPECT_EQ(svector({"/abc/def/ghi"}), sort_glob("/abc/def/ghi", fs));
    EXPECT_EQ(svector({"/abc/def/ghi"}), sort_glob("/*/def/ghi", fs));
    EXPECT_EQ(svector({"/abc/def/ghi"}), sort_glob("/*/*/ghi", fs));
    EXPECT_EQ(svector({"/abc/def/ghi"}), sort_glob("/abc/def/*", fs));
    EXPECT_EQ(svector({"/abc/def/ghi"}), sort_glob("/abc/*/*", fs));
    EXPECT_EQ(svector({"pqrs/tuv/w", "pqrs/tuv/wxy"}), sort_glob("pqrs/tuv/w*", fs));
    EXPECT_EQ(svector({"pqrs/tuv/w", "pqrs/tuv/wxy"}), sort_glob("*/tuv/w*", fs));
    EXPECT_EQ(svector({"pqrs/tuv/w", "pqrs/tuv/wxy"}), sort_glob("pqrs/t*/w*", fs));
}

TEST(glob, multidir) {
    glob_fs_provider fs = mock_fs_provider{
        "abc/fab/x",
        "abc/fab/yz",
        "abc/flib/x",
        "abc/flib/yz",
        "abc/rib/x",
        "def/rib/yz",
        "def/fab/x",
        "def/fab/yz",
        "def/rib/x",
        "def/rib/yz"
    };

    using svector = std::vector<std::string>;

    EXPECT_EQ(svector({"abc/fab/x", "abc/flib/x"}), sort_glob("*c/f*b/?", fs));
}

TEST(glob, dots) {
    glob_fs_provider fs = mock_fs_provider{
        "f.oo/b.ar", "f.oo/.bar",
        ".foo/b.ar", ".foo/.bar"
    };

    using svector = std::vector<std::string>;

    EXPECT_EQ(svector({"f.oo/b.ar"}), sort_glob("*/*", fs));
    EXPECT_EQ(svector({".foo/b.ar"}), sort_glob(".*/*", fs));
    EXPECT_EQ(svector({"f.oo/b.ar"}), sort_glob("f[.z]oo/*", fs));
    EXPECT_EQ(svector({"f.oo/b.ar"}), sort_glob("f?oo/*", fs));
    EXPECT_EQ(svector(), sort_glob("[.z]foo/*", fs));
    EXPECT_EQ(svector(), sort_glob("?foo/*", fs));
}

} // namespace hf
