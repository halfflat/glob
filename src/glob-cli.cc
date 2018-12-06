#include <iostream>

#include "glob/glob.h"

int main(int argc, char** argv) {
    for (char** arg = argv+1; *arg; ++arg) {
        auto matches = hf::glob(*arg);
        for (auto& p: matches) {
            std::cout << p << "\n";
        }
    }
    return 0;
}
