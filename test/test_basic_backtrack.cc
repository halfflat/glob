#include <gtest/gtest.h>

#include "glob/match.h"

// Simple backtracking matchers.

using hf::basic_backtrack::match;

TEST(basic_backtrack, literal) {
    EXPECT_TRUE(match("", ""));
    EXPECT_TRUE(match("abcd", "abcd"));

    EXPECT_FALSE(match("x", ""));
    EXPECT_FALSE(match("", "x"));

    EXPECT_FALSE(match("abcd", "abcde"));
    EXPECT_FALSE(match("abcd", "0abcd"));
    EXPECT_FALSE(match("abcd", "abc"));
}

TEST(basic_backtrack, query) {
    EXPECT_FALSE(match("?", ""));
    EXPECT_TRUE(match("?", "a"));
    EXPECT_FALSE(match("?", "ab"));

    EXPECT_TRUE(match("a?b", "acb"));
    EXPECT_FALSE(match("a?b", "abc"));
    EXPECT_FALSE(match("a?b", "acbb"));
    EXPECT_FALSE(match("a?b", "ab"));

    EXPECT_TRUE(match("a??b", "acab"));
    EXPECT_FALSE(match("a??b", "acb"));
    EXPECT_FALSE(match("a??b", "abbcb"));
    EXPECT_FALSE(match("a??b", "ccab"));
}

TEST(basic_backtrack, star) {
    EXPECT_TRUE(match("*", ""));
    EXPECT_TRUE(match("*", "a"));
    EXPECT_TRUE(match("*", "abc"));

    EXPECT_TRUE(match("ab*", "ab"));
    EXPECT_TRUE(match("ab*", "abcdef"));
    EXPECT_FALSE(match("ab*", "acdef"));
    EXPECT_FALSE(match("ab*", "bcdef"));
    EXPECT_FALSE(match("ab*", "a"));

    EXPECT_TRUE(match("*ab", "ab"));
    EXPECT_TRUE(match("*ab", "cab"));
    EXPECT_TRUE(match("*ab", "abababab"));
    EXPECT_FALSE(match("*ab", "abc"));

    EXPECT_TRUE(match("a*b", "ab"));
    EXPECT_TRUE(match("a*b", "acbcb"));
    EXPECT_FALSE(match("a*b", "acbc"));
}

TEST(basic_backtrack, escape) {
    EXPECT_TRUE(match("a\\?b", "a?b"));
    EXPECT_TRUE(match("a\\?*\\*b", "a?*b"));
    EXPECT_TRUE(match("a\\?*\\*b", "a?xx*b"));

    EXPECT_FALSE(match("a\\?b", "azb"));
    EXPECT_FALSE(match("a\\?b", "a\\zb"));
    EXPECT_FALSE(match("a\\?*\\*b", "a?zb"));
    EXPECT_FALSE(match("a\\?*\\*b", "a?*zb"));

    EXPECT_TRUE(match("\\x", "x"));
    EXPECT_TRUE(match("\\\\", "\\"));
    EXPECT_TRUE(match("\\", ""));
}

TEST(basic_backtrack, combined) {
    EXPECT_TRUE(match("***", ""));
    EXPECT_TRUE(match("***", "a"));
    EXPECT_TRUE(match("***", "abc"));

    EXPECT_TRUE(match("ab*c*c*c*c?c", "abcccccc"));
    EXPECT_TRUE(match("ab*c*c*c*c?c", "abccccxc"));
    EXPECT_TRUE(match("ab*c*c*c*c?c", "abacbbcabccxc"));
    EXPECT_FALSE(match("ab*c*c*c*c?c", "abccccc"));
    EXPECT_FALSE(match("ab*c*c*c*c?c", "abcrccxc"));

    EXPECT_TRUE(match("*x.???", "fox.123"));
    EXPECT_TRUE(match("*x.???", "x...."));
    EXPECT_FALSE(match("*x.???", "fox.1234"));
}

TEST(match, long_no_match) {
    // Works, but exponentially slow in n owing to backtracking.
    int n = 14;
    std::string stars(n, '*');
    std::string exes(n, 'x');
    EXPECT_FALSE(match((stars+"_").c_str(),  exes.c_str()));
}


