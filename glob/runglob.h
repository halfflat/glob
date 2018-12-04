#pragma once

#include <filesystem>
#include <vector>

namespace hf {

struct pattern_component {
    const char* pattern = nullptr;
    bool literal = false;
};

// Return first component, writing NUL at end.
// Update pattern to one-past-the-end of component,
// or nullptr if last component.

pattern_component tokenize(char*& pattern);

bool path_is_directory(const std::filesystem::path& p) {
    return p.empty() || std::prev(p.end())->empty();

}

// Sticking with narrow string interface and
// native char representation for patterns,
// because POSIX is what I have handy for testing.
//
// Add UTF8 version?

std::vector<std::filesystem::path> glob(std::string pattern);

} // namespace hf
