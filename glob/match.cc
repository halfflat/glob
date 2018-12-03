#include <list>

#include <cstdio>

using std::printf;
using std::puts;

#include "match.h"

// Metacharacters:
//     ?     Match any one character.
//     *     Match any string of characters.
//     \     Match against the next pattern character literally.
//     [     Begin character class.
//     ]     End character class.
//
// A character class matches exactly one character of those
// listed between the [ and ] characters.

namespace hf {

namespace backtrack {

bool match_char_class(const char*& p, char c) {
    // Special cases for first character:
    // ! => negate test defined from following char.
    // - => treat '-' as literal.
    // ] => treat ']' as literal.

    if (*p!='[') return false;
    ++p;

    bool negate = false;
    bool match = false;

    if (*p=='!') {
        negate = true;
        ++p;
    }

    bool first = true;
    char lrange = 0;
    for (; !match && *p && (first || *p!=']'); ++p) {

        bool last = *p && p[1]==']';
        if (*p=='-' && lrange && !first && !last) {
            match = c>=lrange && c<=*++p;
            lrange = 0;
            continue;
        }

        lrange = c;
        match = c==*p;
        first = false;
    }

    while (*p && *p!=']') ++p;
    if (!*p) return false;

    return match^negate;
}

bool match(const char* p, const char* t) {
    for (; ; ++p, ++t) {
        switch (*p) {
        case '?':
            if (!*t) return false;
            else break;
        case '*':
            return match(p+1, t) || *t && match(p, t+1);
        case '[':
            if (!*t) return false;
            else if (!match_char_class(p, *t)) return false;
            else continue;
        case '\\':
            ++p; // fall-through
        default:
            if (!*p) return !*t;
            else if (*p!=*t) return false;
        }
    }
}

} // namespace backtrack

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
