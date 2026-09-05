# MINI CompileFix Compiler

A console-based mini compiler in C++ for a small C-like language. Instead of stopping at the first syntax error, CompileFix tries to **automatically detect and fix common mistakes** (missing semicolons, unbalanced parentheses, missing braces, etc.) so beginners can see corrected code instead of just an error dump.

Built for the **Compiler Lab (CSE-3528)**, Autumn 2025, Department of CSE, International Islamic University Chittagong (IIUC).

## Team

- Tahasina Tasnim Afra (C233456)
- Nafia Nowshin (C233466)
- Nusrath Jahan Shawon (C233449)

**Supervisor:** Mrs. Israt Binte Habib

## Overview

CompileFix processes source code through a simple multi-phase pipeline:

1. **Cleaning** — strips single-line and multi-line comments, normalizes whitespace, while correctly ignoring comment-like sequences inside string literals.
2. **Lexical Analysis** — scans the cleaned code into tokens (keywords, identifiers, numbers, operators, separators), recording line numbers for each token.
3. **Syntax Analysis & Auto-Fix** — checks the token/line stream against basic grammar expectations and applies the smallest fix it can (insert a missing `;`, close an unbalanced `(` or `{`, add parentheses around an `if` condition, drop a stray `)`), logging every change it makes.
4. **Semantic Analysis** — builds a symbol table from declarations and flags undeclared variables and a few basic type mismatches (e.g. assigning a float to an int).

The tool favors **error recovery over early termination** — it keeps going after a problem so it can report and fix as many issues as possible in one run, rather than stopping at the first one.

## Scope

Supports a small subset of C: variable declarations, assignment statements, simple `if` statements, and basic arithmetic/relational expressions.

**Not supported:** functions, arrays, pointers, structs/unions, preprocessor directives, scoped/nested variable resolution, and code generation.

## Project Structure

- `main.cpp` — entry point with a menu (`lexicalPhase`, `syntaxPhase`, `semanticPhase`)
- Core routines:
  - `isNumberToken()` — helper to classify numeric tokens
  - `lexicalPhase()` — comment removal + tokenization
  - `syntaxPhase()` — phrase-level and panic-mode error recovery
  - `semanticPhase()` — symbol table construction and basic type/declaration checks

## Build & Run

```bash
g++ -std=c++17 -o compilefix main.cpp
./compilefix
```

You'll get an interactive menu:

```
1. Lexical Analysis   (comment removal, tokenization)
2. Syntax Analysis    (phrase-level & panic-mode recovery)
3. Semantic Analysis  (symbol table, type checking)
4. Exit
```

Place your input file as `test3.c` in the working directory (or update the filename in `lexicalPhase()`), then run phases 1 → 2 → 3 in order, since each phase reads the previous phase's output.

## Outputs

| File | Description |
|---|---|
| `cleaned_code.c` | Source after comment removal and whitespace normalization |
| `tokens.txt` | Token stream with type and line number |
| `syntax_fixed.c` | Recovered code after auto-fixing semicolons/parentheses/braces |
| `syntax_errors.txt` | Log of every syntax fix applied and why |
| `semantic_report.txt` | Symbol table plus detected semantic errors/warnings |

## Example

Given an input with missing semicolons, an unparenthesized `if` condition, and unbalanced parentheses, CompileFix inserts the missing symbols, completes the `if (...)` syntax, and reports each change in `syntax_errors.txt` — while still producing a fully corrected `syntax_fixed.c`.

## Known Limitations

- Multiple declarations in one statement (`int a, b;`) aren't fully handled — later variables can be misreported as undeclared.
- Parsing is hand-crafted pattern/line-based recursive descent, not a formal LL(1)/LR(1) grammar — it can fail on deeply nested or ambiguous constructs.
- No scoping: all variables are effectively global, so shadowing inside `{}` blocks isn't handled correctly.
- No function semantics, arrays, pointers, or structs.
- Semantic checks run directly on the token stream rather than an AST, limiting how much structure can be analyzed.
- No code generation, constant folding, or optimization — the pipeline stops after semantic analysis.

## Future Improvements

- Formal LL(1)/LR(1) parsing (e.g. via ANTLR/Bison) and a real AST for semantic analysis and future codegen.
- Scoped symbol tables (stack-based, push/pop per block).
- Declarator-list support for multi-variable declarations.
- Function declaration/definition checking (params, return types, call resolution).
- Array and pointer support.
- A simple codegen backend (LLVM IR, x86 assembly, or C output).
- Better diagnostics (warning vs. error categorization, phrase-level repair).
- Preprocessor directive support (`#define`, `#include`).
- Compile-time constant folding.

## Conclusion

CompileFix demonstrates a practical compiler front-end workflow — cleaning, lexical analysis, syntax checking with auto-fix, and basic semantic analysis — aimed at helping beginner programmers understand and correct common mistakes in their code.
