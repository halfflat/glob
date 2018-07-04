#include <vector>

#include "match.h"

// Simple back-tracking matcher.
//
// Metacharacters:
//     ?     Match any one character.
//     *     Match any string of characters.
//     \     Match against the next pattern character literally.

namespace hf {

namespace basic_backtrack {

bool match(const char* p, const char* t) {
    for (; ; ++p, ++t) {
        switch (*p) {
        case '?':
            if (!*t) return false;
            else break;
        case '*':
            return match(p+1, t) || *t && match(p, t+1);
        case '\\':
            ++p; // fall-through
        default:
            if (!*p) return !*t;
            else if (*p!=*t) return false;
        }
    }
}

} // namespace basic_backtrack
} // namespace hf
