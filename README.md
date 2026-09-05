MINI CompileFix COMPILER:
------------ MAIN MENU ---------------

1. Lexical Analysis
-Remove Comments
-Tokenization

2. Syntax Analysis
-Phrase Level Recovery
-Panic Mode Recovery

3. Semantic Analysis
-Symbol Table
-Type Checking

4. Exit

---

#Enter your choice:
Typical session:
1. Put broken C-like code in test3.c
2. Choose 1 — produces cleaned source + tokens
3. Choose 2 — produces fixed source + syntax error log
4. Choose 3 — produces symbol table + semantic messages
5. Inspect the generated files


#Output files:

1. cleaned_code.c
-Produced by: Lexical
-Contents: Source with comments stripped, layout preserved

2. tokens.txt
-Produced by: Lexical
-Contents: TYPE: value : lineNumber per token

3. syntax_fixed.c
-Produced by: Syntax
-Contents: Auto-repaired program

4. syntax_errors.txt
-Produced by: Syntax
-Contents: Line-by-line list of applied fixes

5. semantic_report.txt
-Produced by: Semantic
-Contents: Errors/warnings + symbol table

---Token kinds: KEYWORD, IDENTIFIER, NUMBER, OPERATOR, SEPARATOR

#Sample run:
Input (test3.c):
int x = 5)
int m=9
int a=3
/nxwnw
wxwqx
xexln/
int y = 10
z = x + y)
w = x * y + z
//bjcbewjweq//
if x+2 > y {
z = z + 1
} else {
z = z - 1
}

#What CompileFix does:
Drops both comment blocks
Removes extra ) after 5 and after x + y
Inserts missing ; on declaration and assignment lines
Rewrites if x+2 > y {  into  if ( x+2 > y ) {
Builds a symbol table for x, m, a, y
Reports uses of undeclared z / w (depending on whether they were declared)

---Open syntax_fixed.c and syntax_errors.txt after phase 2 to see the repaired program and the log.

#Project structure:
├── CompileFix.cpp      # Lexer + parser + semantic + main menu
├── test3.c             # Sample / your input program
├── cleaned_code.c      # Generated
├── tokens.txt          # Generated
├── syntax_fixed.c      # Generated
├── syntax_errors.txt   # Generated
├── semantic_report.txt # Generated
└── README.md
---Single-file implementation for the lab: easier to read, compile, and demo.

#Implementation notes:
Lexical analysis:
Character scan with string-literal awareness so // and /* inside strings are not treated as comments
Multi-character operators: ==, !=, <=, >=, ++, --
Keywords matched against a small set
Numbers allow one decimal point

Syntax analysis:
Line-oriented recovery rather than a full CFG parser
Counts ( / ) and { / } globally for panic-mode close
Avoids inserting ) into control-flow headers that the if-rewriter already handles

Semantic analysis:
Sequential pass over tokens.txt
Declaration mode after int / float / char until ;
Tracks identifiers already declared in the current statement
Warns when a float value is assigned into an int variable
---This is token-stream semantics, not AST-based analysis. It is enough for the lab subset and for showing symbol tables, not a production type checker.

#Limitations:
Multiple declarators in one statement (int a, b;) are only partially handled
No nested scopes — all names are file-level
No functions, arrays, pointers, or structs
Recovery can still cascade on heavily broken input
No intermediate representation or code generation
printf / scanf and preprocessor directives are not recognized
---These limits are documented in the course report and are expected for a mini front-end.

#Future work:
Formal LL(1) / recursive-descent parser with FIRST / FOLLOW
Abstract syntax tree for semantic analysis and later codegen
Stack of symbol tables for block scope
Full declarator lists
Function declarations, calls, and return-type checks
Arrays and basic pointers
Better diagnostics (warning vs error, source snippets)
Simple backend (three-address code or C dump)


#Team:
Course: Compiler Lab (CSE-3528)
Supervisor: Mrs. Israt Binte Habib
Institution: International Islamic University Chittagong
Submitted: January 22, 2026
Tahasina Tasnim Afra (C233456) — Primary developer (design, implementation, testing, report)
Nafia Nowshin (C233466) — Team member
Nusrath Jahan Shawon (C233449) — Team member

#Academic note
This repository is an educational compiler front-end. It is not a general-purpose C compiler. Use it to study scanning, error recovery, and a first symbol table — not to compile real C programs.

#License
Educational use. You may study, fork, and adapt this project for learning(Please give authorship).
