# kononovarseniy/common

The repository contains some functions and types that are frequently used in my C++ projects.

# Building the library

Execute these commands before the first build and rerun them after adding new dependencies.

`conan install . --build=missing -s build_type=Release`
`conan install . --build=missing -s build_type=Debug`

Now you can use CMake presets `conan-release` and `conan-debug`.

# AI usage

This project uses AI as an assistant tool in the following areas:

- **Test coverage.** AI is used to generate simple and boilerplate tests, freeing up developer time.
  Edge cases, complex scenarios and tests for critical code paths are written and verified manually.
- **Documentation.** AI improves readability of existing docs and generates boilerplate comments for public APIs to keep documentation complete and consistent.
- **Bug detection and code review.** AI is used to catch potential issues, suggest improvements and provide an additional review perspective.

All critical and production-relevant changes are reviewed and verified by a human developer before being merged.
AI-generated content is treated as a first draft that requires human judgment for correctness and appropriateness.
