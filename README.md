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

`src/` contains source for a command-line wrapper around the
glob functionality.

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

Two different implementations of file system queries are provided:
one uses the C++17 standard library `std::filesystem` and the other
uses POSIX library functions directly.

By default the POSIX implementation is used; add `with-std-filysystem=1`
to the `make` invocation in order to use `std::filesystem` instead.

