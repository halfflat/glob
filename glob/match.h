#pragma once

namespace hf {

// Basic (*, ?, \) pattern matchers.

// Simple backtracking implementation:

namespace basic_backtrack {
    bool match(const char* pattern, const char* text);
}

}
