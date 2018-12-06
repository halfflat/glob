The use of POSIX glob() in another project stops it building on Android.

We don't need all the globbing bells and whistles, so this repo
is just a place to try out implementations.

## Code organization

`match/` contains generic matchers for simple glob syntax, with
a `backtrack::match` and `nfa::match` implementation.

`glob/` contains the actual glob code, together with an NFA matcher
that understands glob quirks (i.e. ? and * cannot match a period
at the beginning of a filename component.)

The main glob implementation has a callback interface for handling
results, and takes a `glob_fs_provider` as an argument. This object
abstracts the needed file system queries, and is also used to
provide a mock filesystem for unit testing.

## Supported features

Current state:

* Only provides a narrow `char` interface.
* Multibyte encodings are ignored: this is primarily a problem for
  character ranges in UTF-8, but can't really presume that
  we live in a single-byte encoding or UTF-8 world.
* Supports metacharacters '*', '?', '['.
* Character classes, e.g. '[:alnum:]', are not supported.
* Character ranges are computed in code points, not in collation order.

## Dependencies

The implementation of file system operations is based on the C++17
standard library `std::filesystem` functionality, but the goal is
to replace that with C++14 code and POSIX calls.

