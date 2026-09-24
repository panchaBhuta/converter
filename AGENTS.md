# Converter Project – Codex Instructions

## Project Overview

This is a header-only C++ conversion library.

The library provides conversions:

- string -> strongly typed value
- value -> string

The project primarily targets C++20 and supports multiple
compilers and standard-library implementations.

Supported environments include combinations of:

- GCC
- Clang
- MSVC
- clang-cl
- different versions of the C++ standard library

Portability is a major design requirement.

---

## Important Design Principle

Do not make changes merely to make one compiler/build pass.

Changes must preserve portability across the supported compiler/library
matrix.

Before changing implementation code, determine whether the problem is:

1. a compiler limitation
2. a standard-library limitation
3. a language-standard issue
4. a CMake detection/configuration issue
5. an actual converter-library defect

---

## C++ Standard

The project uses C++20 features extensively.

Important areas include:

- templates
- concepts
- constexpr
- non-type template parameters (NTTP)
- std::chrono
- std::chrono::year_month_day
- std::chrono::from_stream
- format strings
- type traits
- compile-time dispatch

Do not replace a C++20 solution with a less-capable workaround
unless there is a demonstrated portability reason.

---

## Capability Detection

The project intentionally separates feature detection from
feature configuration.

Use these naming conventions:

### SUPPORTED_

Indicates that a compiler/standard-library combination supports
a particular language/library capability.

Example:

SUPPORTED_DATE_LIB_FOR_FROMSTREAM

These are normally determined by compile-time capability probes.

### HAS_

Indicates that a capability exists but is not necessarily used
by the converter library.

### ENABLE_

Indicates a configuration or feature toggle controlling whether
a capability should be used.

Do not introduce generic USE_ macros unless there is a strong
architectural reason.

---

## CMake Architecture

CMake code is intentionally being organized into separate concerns.

Capability detection belongs in:

cmake/capability-probe-framework.cmake

Installation/package logic belongs in:

cmake/converter_install.cmake

Do not mix capability probing with installation logic.

Avoid introducing wrappers around try_compile()/try_run()
unless they provide meaningful additional behavior.

---

## Capability Probes

Capability probes exist because compiler/library behavior differs
between environments.

When modifying a capability probe:

1. Understand exactly what behavior is being tested.
2. Prefer testing the actual required functionality.
3. Do not assume compiler version alone determines capability.
4. Do not replace a functional test with a version check unless
   absolutely necessary.
5. Preserve diagnostic information from failed probes.

---

## try_compile / try_run

try_compile() and try_run() are intentionally used for capability
detection.

Some probes may require execution rather than compilation.

Do not remove try_run() simply because try_compile() appears
sufficient.

If a try_run() hangs:

1. determine what the generated test executable is doing;
2. inspect the generated build files;
3. determine whether the problem is the executable,
   compiler, linker, MSBuild, or CMake;
4. avoid simply adding arbitrary timeouts.

---

## Date Handling

Date/time functionality is particularly important.

The project may support different implementations of date parsing
depending on compiler/standard-library capabilities.

Examples include:

std::chrono
date/date.h

Do not assume std::chrono::from_stream is available merely because
the compiler supports C++20.

Check the actual standard-library functionality.

---

## Template Design

Preserve compile-time behavior wherever possible.

The project intentionally uses templates to allow:

- compile-time type selection
- compile-time format selection
- NTTP format strings
- specialization/constraints
- compile-time dispatch

When modifying templates, inspect all relevant instantiations
before changing an interface.

Do not solve a template error by unnecessarily removing
compile-time information.

---

## Architecture

---

## Code Inspection

All the converter-library C++ code resides in './include' folder.
All the cpp files used for probing by cmake resides in './cmake' folder.
After probing, cmake sets up a config file with macros set with values based on results from probing.
cmake uses template-file './cmake/_workaroundConfig.h.in' to create this config-file.

Ignore the files and folders mentioned in the file './.gitignore'

---

## Testing

Command to build and run a test:
   ./make.sh tests [testName]

- optional [testName] to run a single test. If not provided runs all the tests.
- [testName] is all the cpp files in the tests folder without the extension '.cpp'
     ./tests/[testName].cpp


Before modifying production code:

1. identify the relevant existing tests;
2. reproduce the failure;
3. make the smallest architectural change necessary;
4. run the relevant tests;
5. run broader tests when the change affects shared templates
   or conversion infrastructure.

Do not modify tests merely to make an incorrect implementation pass.

---

## Git Safety

Do not:

- rewrite unrelated files;
- perform large-scale refactoring without being asked;
- delete existing tests;
- change public APIs unnecessarily;
- modify CMake architecture without explaining the reason.

Before making a substantial change:

1. explain the proposed approach;
2. identify affected files;
3. identify potential portability consequences.

Prefer small, reviewable changes.

---

## When Debugging

When given a compiler error:

Do not immediately propose a fix.

First determine:

1. what the compiler is actually saying;
2. the template instantiation chain;
3. the relevant source line;
4. whether the behavior is standard-conforming;
5. whether another compiler behaves differently;
6. whether the issue is compiler/library-specific.

Then propose the smallest appropriate fix.

---

## General Rule

When uncertain, inspect the repository before making assumptions.

The existing architecture and tests are authoritative.

Prefer:

understand -> reproduce -> explain -> modify -> test

rather than:

guess -> modify -> hope
