#pragma once

namespace hf {

// Basic (*, ?, \) pattern matchers.

// Simple backtracking implementation:

namespace backtrack {
    bool match(const char* pattern, const char* text);
}

namespace nfa {
    bool match(const char* pattern, const char* text);
}

}
