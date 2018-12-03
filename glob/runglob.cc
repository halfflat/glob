#include <vector>
#include <filesystem>


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
    } while (!meta && *p);

    if (s) k.literal = true;
    else s = p;

    pattern = *s? s+1: nullptr;
    *s = 0;
    return k;
}


