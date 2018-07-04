The use of POSIX glob() in another project stops it building on Android.

We don't need all the globbing bells and whistles, so this repo
is just a place to try out implementations.

Initially, just playing with string pattern matching. Is it worth
using a non-backtracking matcher to handle horrible exponential cases?
