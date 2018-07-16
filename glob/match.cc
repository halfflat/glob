#include <list>

#include <cstdio>

using std::printf;
using std::puts;

#include "match.h"

// Simple back-tracking matcher.
//
// Metacharacters:
//     ?     Match any one character.
//     *     Match any string of characters.
//     \     Match against the next pattern character literally.

namespace hf {

namespace backtrack {

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

namespace nfa {

bool match(const char* p, const char* t) {
    std::list<const char*> state = {p};

    char c;
    do {
        c = *t++;
        for (auto i = state.begin(); i!=state.end();) {
            switch (**i) {
            case '*':
                if (i==state.begin() || *std::prev(i)!=*i) {
                    state.insert(i, *i);
                }
                while (**i=='*') ++*i;
                continue;
            case '?':
                if (c) goto advance;
                else goto fail;
            case '\\':
                ++*i; // fall-through
            default:
                if (**i==c) goto advance;
                else goto fail;
            }

        fail:
            i = state.erase(i);
            continue;

        advance:
            *i += !!c;
            ++i;
            continue;
        }
    } while (c && !state.empty());

    return !state.empty() && !*state.back();
}

} // namespace nfa

} // namespace hf
