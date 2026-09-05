#include <bits/stdc++.h>
#include <fstream>
#include <string>
#include <cctype>
#include <vector>
#include <algorithm>
#include <map>
using namespace std;

bool isNumberToken(const string& token)
{
    bool dotSeen = false;

    for (char c : token)
    {
        if (c == '.') // for floating numbers
        {
            if (dotSeen) return false;
            dotSeen = true;
        }
        else if (!isdigit(c))
            return false;
    }

    return !token.empty();
}



void lexicalPhase()
{
    ifstream in("test3.c");
    ofstream cleanOut("cleaned_code.c");
    ofstream tokenOut("tokens.txt");

    if (!in || !cleanOut || !tokenOut)
    {
        cout << "File error in lexical phase.\n";
        return;
    }

    string code((istreambuf_iterator<char>(in)), istreambuf_iterator<char>());
    string cleanCode = "";

    // ---------- COMMENT REMOVAL (SAFE VERSION) ----------
    bool inString = false;

    for (size_t i = 0; i < code.length(); i++)
    {
        char current = code[i];
        char next = (i + 1 < code.length()) ? code[i + 1] : '\0';

        // toggle string mode (ignore escaped quotes)
        if (current == '"' && (i == 0 || code[i - 1] != '\\'))
        {
            inString = !inString;
            cleanCode += current;
            continue;
        }

        // remove single-line comments (only if not in string)
        if (!inString && current == '/' && next == '/')
        {
            while (i < code.length() && code[i] != '\n')
                i++;
            cleanCode += '\n'; // keep line structure
            continue;
        }

        // remove multi-line comments (only if not in string)
        if (!inString && current == '/' && next == '*')
        {
            i += 2;
            while (i + 1 < code.length() &&
                !(code[i] == '*' && code[i + 1] == '/'))
            {
                i++;
            }
            i++; // skip '/'
            continue;
        }

        // normal character
        cleanCode += current;
    }

    cleanOut << cleanCode;

    // ---------- TOKENIZATION ----------
    int lineNo = 1;
    string token = "";
    string operators = "+-*/=<>!";         // single-char operators
    string separators = "();{},";          // separators like ;, (, {, )
    unordered_set<string> keywords = {
        "int","float","char","if","else","for","while","return"
    };

    for (size_t i = 0; i < cleanCode.length(); i++)
    {
        char ch = cleanCode[i];

        if (ch == '\n') lineNo++;

        // ---- Handle whitespace ----
        if (isspace(ch))
        {
            if (!token.empty())
            {
                // check if keyword
                if (find(keywords.begin(), keywords.end(), token) != keywords.end())
                    tokenOut << "KEYWORD: " << token << " : " << lineNo << endl;

                else if (isNumberToken(token))
                    tokenOut << "NUMBER: " << token << " : " << lineNo << endl;

                else
                    tokenOut << "IDENTIFIER: " << token << " : " << lineNo << endl;


                token.clear();
            }
            continue;
        }

        // ---- Handle separators ----
        if (separators.find(ch) != string::npos)
        {
            if (!token.empty())
            {
                // flush current token
                if (find(keywords.begin(), keywords.end(), token) != keywords.end())
                    tokenOut << "KEYWORD: " << token << " : " << lineNo << endl;

                else if (isNumberToken(token))
                    tokenOut << "NUMBER: " << token << " : " << lineNo << endl;

                else
                    tokenOut << "IDENTIFIER: " << token << " : " << lineNo << endl;


                token.clear();
            }
            tokenOut << "SEPARATOR: " << ch << " : " << lineNo << endl;

            continue;
        }

        // ---- Handle operators (multi-char) ----
        if (operators.find(ch) != string::npos)
        {
            if (!token.empty())
            {
                if (find(keywords.begin(), keywords.end(), token) != keywords.end())
                    tokenOut << "KEYWORD: " << token << " : " << lineNo << endl;

                else if (isNumberToken(token))
                    tokenOut << "NUMBER: " << token << " : " << lineNo << endl;

                else
                    tokenOut << "IDENTIFIER: " << token << " : " << lineNo << endl;


                token.clear();
            }

            // check for two-character operators like ==, >=, <=, !=, ++, --
            string op(1, ch);
            if (i + 1 < cleanCode.length())
            {
                char nextCh = cleanCode[i + 1];
                if ((ch == '+' && nextCh == '+') || (ch == '-' && nextCh == '-') ||
                        (ch == '=' && nextCh == '=') || (ch == '!' && nextCh == '=') ||
                        (ch == '<' && nextCh == '=') || (ch == '>' && nextCh == '='))
                {
                    op += nextCh;
                    i++; // skip next char
                }
            }

            tokenOut << "OPERATOR: " << op << " : " << lineNo << endl;

            continue;
        }

        // ---- Otherwise accumulate token ----
        token += ch;
    }

// ---- flush remaining token ----
    if (!token.empty())
    {
        if (find(keywords.begin(), keywords.end(), token) != keywords.end())
            tokenOut << "KEYWORD: " << token << " : " << lineNo << endl;

        else if (isNumberToken(token))
            tokenOut << "NUMBER: " << token << " : " << lineNo << endl;

        else
            tokenOut << "IDENTIFIER: " << token << " : " << lineNo << endl;

    }


    cout << "Lexical Analysis Completed.\n";
    cout << "output done: cleaned_code.c\n";
    cout << "output done: tokens.txt\n";
}


void syntaxPhase()
{
    ifstream in("cleaned_code.c");
    ofstream fixed("syntax_fixed.c");
    ofstream errors("syntax_errors.txt");

    if (!in || !fixed || !errors)
    {
        cout << "Syntax phase file error.\n";
        return;
    }

    string line;
    int lineNo = 1;
    int openParen = 0, openBrace = 0;

    while (getline(in, line))
    {
        string newLine = "";
        int localParen = 0;   // per-line parentheses

        // ---- FIX EXTRA ')' ----
        for (char c : line)
        {
            if (c == '(')
            {
                localParen++;
                openParen++;
                newLine += c;
            }
            else if (c == ')')
            {
                if (localParen > 0)
                {
                    localParen--;
                    openParen--;
                    newLine += c;
                }
                else
                {
                    errors << "Line " << lineNo
                           << ": Extra ')' removed.\n";
                    // skip extra ')'
                }
            }
            else
            {
                newLine += c;
            }
        }

        line = newLine;

        // ---- FIX MISSING ')' IN SAME STATEMENT (PHRASE LEVEL) ----
        int countOpen = 0, countClose = 0;
        for (char c : line)
        {
            if (c == '(') countOpen++;
            if (c == ')') countClose++;
        }

        if (countOpen > countClose &&
                line.find("if") == string::npos &&
                line.find("for") == string::npos &&
                line.find("while") == string::npos)
        {

            int missing = countOpen - countClose;
            while (missing--)
            {
                line += ")";
                openParen--;  // prevent panic-mode adding it later
                errors << "Line " << lineNo
                       << ": Missing ')' in statement. Auto-fixed.\n";
            }
        }


        // ---- FIX IF CONDITION (missing parentheses) ----
        if (line.find("if") != string::npos &&
                line.find("(") == string::npos &&
                line.find(")") == string::npos)
        {

            size_t ifPos = line.find("if");
            size_t bracePos = line.find("{");

            if (bracePos != string::npos)
            {
                string condition = line.substr(ifPos + 2, bracePos - (ifPos + 2));
                line = "if (" + condition + ") {";

                errors << "Line " << lineNo
                       << ": Missing parentheses in if-condition. Auto-fixed.\n";
            }
        }

        // ---- COUNT BRACES ----
        for (char c : line)
        {
            if (c == '{') openBrace++;
            if (c == '}') openBrace--;
        }

        // ---- FIX MISSING SEMICOLON ----
        if (!line.empty() &&
                line.find("if") == string::npos &&
                line.find("else") == string::npos &&
                line.back() != ';' &&
                line.back() != '{' &&
                line.back() != '}')
        {

            line += ";";
            errors << "Line " << lineNo
                   << ": Missing semicolon. Auto-fixed.\n";
        }

        fixed << line << endl;
        lineNo++;
    }

    // ---- PANIC MODE: MISSING CLOSING SYMBOLS ----
    while (openParen > 0)
    {
        fixed << ")" << endl;
        errors << "Panic Mode: Missing ')'. Inserted.\n";
        openParen--;
    }

    while (openBrace > 0)
    {
        fixed << "}" << endl;
        errors << "Panic Mode: Missing '}'. Inserted.\n";
        openBrace--;
    }

    cout << "Syntax Analysis Completed.\n";
    cout << "Output done: syntax_fixed.c\n";
    cout << "Output done: syntax_errors.txt\n";
}

void semanticPhase() {
    ifstream in("tokens.txt");
    ofstream report("semantic_report.txt");

    if (!in || !report) {
        cout << "Semantic phase file error.\n";
        return;
    }

    map<string, string> symbolTable; // variable -> type
    vector<string> errors;

    string currentType = "";
    bool declarationMode = false;
    set<string> declaredThisStatement; // identifiers in current statement
    string lastAssignedType = "";
    string lastTokenType = "";
    string lastTokenValue = "";

    string line;
    while (getline(in, line)) {
        if (line.empty()) continue;

        // Expected format: TYPE: value : lineNo
        size_t firstColon = line.find(":");
        size_t secondColon = line.rfind(":");

        if (firstColon == string::npos ||
            secondColon == string::npos ||
            firstColon == secondColon)
            continue;

        string tokenType = line.substr(0, firstColon);
        string tokenValue = line.substr(firstColon + 1,
                                        secondColon - firstColon - 1);
        int tokenLine = stoi(line.substr(secondColon + 1));

        // trim spaces
        tokenType.erase(remove(tokenType.begin(), tokenType.end(), ' '),
                        tokenType.end());
        tokenValue.erase(0, tokenValue.find_first_not_of(" "));
        tokenValue.erase(tokenValue.find_last_not_of(" ") + 1);

        // ---- Type keywords ----
        if (tokenType == "KEYWORD") {
            if (tokenValue == "int" || tokenValue == "float" || tokenValue == "char") {
                currentType = tokenValue;
                declarationMode = true;
                declaredThisStatement.clear();
            } else {
                declarationMode = false; // if, else, return, etc
                currentType = "";
                declaredThisStatement.clear();
            }
            lastTokenType = tokenType;
            lastTokenValue = tokenValue;
            continue;
        }

        // ---- Numbers ----
        if (tokenType == "NUMBER") {
            if (tokenValue.find('.') != string::npos)
                lastAssignedType = "float";
            else
                lastAssignedType = "int";
            lastTokenType = tokenType;
            lastTokenValue = tokenValue;
            continue;
        }

        // ---- Identifiers ----
        if (tokenType == "IDENTIFIER") {
            string varName = tokenValue;

            if (declarationMode && (lastTokenType == "KEYWORD" || (lastTokenType == "SEPARATOR" && lastTokenValue == ","))) {
                // It's a declaration
                if (declaredThisStatement.count(varName)) {
                    errors.push_back("Error at line " + to_string(tokenLine) +
                                    ": Multiple declaration of " + varName);
                } else if (symbolTable.count(varName)) {
                    errors.push_back("Error at line " + to_string(tokenLine) +
                                    ": Variable already declared previously");
                } else {
                    symbolTable[varName] = currentType;
                    declaredThisStatement.insert(varName);
                }
            } else {
                // It's a use
                declarationMode = false; // exit declaration mode if it was on
                if (!symbolTable.count(varName)) {
                    errors.push_back("Semantic Error at line " + to_string(tokenLine) +
                                    ": Undeclared variable " + varName);
                } else {
                    string varType = symbolTable[varName];
                    if (!lastAssignedType.empty() && varType == "int" && lastAssignedType == "float") {
                        errors.push_back("Warning at line " + to_string(tokenLine) +
                                    ": Assigning float to int may lose data");
                    }
                }
            }
            lastTokenType = tokenType;
            lastTokenValue = tokenValue;
            continue;
        }

        // ---- Separators ----
        if (tokenType == "SEPARATOR") {
            if (tokenValue == ";") {
                declarationMode = false;
                currentType = "";
                declaredThisStatement.clear();
                lastAssignedType = "";
            }
            lastTokenType = tokenType;
            lastTokenValue = tokenValue;
            continue;
        }

        // ---- Ignore operators ----
        if (tokenType == "OPERATOR") {
            lastTokenType = tokenType;
            lastTokenValue = tokenValue;
            continue;
        }
    }

    // ---- Output errors ----
    for (auto &e : errors) report << e << endl;

    // ---- Output symbol table ----
    report << "\nSymbol Table:\n";
    for (auto &s : symbolTable)
        report << s.first << " : " << s.second << endl;

    cout << "Semantic Analysis Completed.\n";
    cout << "Output done: semantic_report.txt\n";
}



int main()
{
    int choice;

    cout << "=====================================\n";
    cout << "        MINI CompileFix COMPILER         \n";
    cout << "=====================================\n";

    do
    {
        cout << "\n------------ MAIN MENU ---------------\n";
        cout << "1. Lexical Analysis\n";
        cout << "   - Remove Comments\n";
        cout << "   - Tokenization\n";
        cout << "2. Syntax Analysis\n";
        cout << "   - Phrase Level Recovery\n";
        cout << "   - Panic Mode Recovery\n";
        cout << "3. Semantic Analysis\n";
        cout << "   - Symbol Table\n";
        cout << "   - Type Checking\n";
        cout << "4. Exit\n";
        cout << "-------------------------------------\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice)
        {
        case 1:
            cout << "\n>>> Starting Lexical Analysis...\n";
            lexicalPhase();
            cout << ">>> Lexical Analysis Finished.\n";
            break;

        case 2:
            cout << "\n>>> Starting Syntax Analysis...\n";
            syntaxPhase();
            cout << ">>> Syntax Analysis Finished.\n";
            break;

        case 3:
            cout << "\n>>> Starting Semantic Analysis...\n";
            semanticPhase();
            cout << ">>> Semantic Analysis Finished.\n";
            break;

        case 4:
            cout << "\nExiting Mini Compiler. Goodbye!\n";
            break;

        default:
            cout << "\nInvalid choice! Please try again.\n";
        }

    }
    while (choice != 4);

    return 0;
}
