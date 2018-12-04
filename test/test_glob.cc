#include <gtest/gtest.h>

#include "glob/runglob.h"


#if 0
struct mock_fs_provider {
    std::unordered_map<std::string directory, std::unordered_set<std::string> entries> tree;

    add_path(const char* name) {
        using fs = std::file_system;
        fs::path p(name);

        auto b = p.begin();
        if (b==p.end()) return;

        fs::path prefix = *b++;
...
    }
#endif

TEST(glob, todo) {
}

