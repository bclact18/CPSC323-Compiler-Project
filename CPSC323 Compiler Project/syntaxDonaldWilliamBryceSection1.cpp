/****************************************************************
* Copyright 2023 Donald Novasky, William Phung, Bryce Lin
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
* CPSC323 Compiler Project
*	This program apply syntax analysis to output .RAT23S.txt
*	file. The output file will identify each token and rule
*	used.
*
* file name: syntaxDonaldWilliamBryceSection1.cpp
****************************************************************/

#include<iostream>
#include<string>
#include<fstream>

//debug on/off
bool debug = false;

//Keywords, separators, operators
const char KEYWORDS[14][9] = { "function", "int", "bool", "real", "if",
    "fi", "else", "return", "put", "get", "while", "endwhile", "true", "false" };
const char SEPARATORS[10] = { '#', '(', ')', ',', '{', '}', ';' };
const char OPERATORS[12][3] = { "=", "==", "!=", ">", "<", "<=", "=>", "+", "-", "*", "/", "NA" };

//identifier, integer, real
//accepting state / rejected:
//identifier    = 2/3
//integer       = 2/3
//real          = 4/5
const int FSM[3][6][5] = {
    {{2, 3, 3, 3, 3},
    {2, 2, 2, 3, 3},
    {3, 3, 3, 3, 3}},
    {{3, 2, 3, 3, 3},
    {3, 2, 3, 3, 3},
    {3, 3, 3, 3, 3}},
    {{5, 2, 5, 5, 5},
    {5, 2, 5, 3, 5},
    {5, 4, 5, 5, 5},
    {5, 4, 5, 5, 5},
    {5, 5, 5, 5, 5}}
};


//Return the input code.
int pos(char obj) {
    if (obj >= 'a' && obj <= 'z')
        return 1;   //Character
    else if (obj >= '0' && obj <= '9')
        return 2;   //Digit
    else if (obj == '_')
        return 3;   //_
    else if (obj == '.')
        return 4;   //.
    else if (obj == '=' || obj == '!' || obj == '>' || obj == '<' || obj == '+' || obj == '-'
        || obj == '*' || obj == '/')
        return 5;   //ops
    return 6;       //other.
}

int lexerAnalysis(std::string& arg) {
    //open input and output files
    std::string fileName = arg;
    std::ifstream program;
    program.open(fileName);

    fileName = fileName + ".txt";
    std::ofstream output;
    output.open(fileName);

    //Put everything in input file as one string
    std::string input, cache;
    while (std::getline(program, cache))
        input = input + cache + " ";
    input = input + " ";        //help ID the last lexeme

    if (input.compare(" ") == 0) {
        std::cout << "\n\nPlease check your input file or argument\n\n" << std::endl;
        return -1;
    }

    //Initialize states and cache
    int state[3] = { 1, 1, 1 };
    int state_other = 1;
    cache = "";

    //Title
    if(debug)std::cout << "Token\t\tlexeme\n" << std::endl;
    output << "Token\t\tlexeme" << std::endl;

    //Loop for all characters in input.
    for (int i = 0; i < input.length(); i++) {
        char character = tolower(input[i]);
        int code = pos(character);

        // check if character is (d | l | _ | .) or not.
        if (code < 5) {
            for (int j = 0; j < 3; j++) {
                state[j] = FSM[j][state[j] - 1][code - 1];
            }
            cache = cache + character;
        }
        //Case for Keyword, Identifier, Integer, Real
        else if (cache.compare("") != 0) {

            //Check if is Identifier
            for (int j = 0; j < sizeof(KEYWORDS) / sizeof(KEYWORDS[0]); j++) {
                if (cache.compare(KEYWORDS[j]) == 0) {
                    if(debug)std::cout << "Keyword\t\t" << cache << std::endl;
                    output << "Keyword\t\t" << cache << std::endl;
                    state[0] = 1;   //so won't print again as ID
                    break;
                }
            }
            //print out token type Identifier / Integer / Real / Unknown
            if (state[0] == 2) {
                if(debug)std::cout << "Identifier\t" << cache << std::endl;
                output << "Identifier\t" << cache << std::endl;
            }
            else if (state[1] == 2) {
                if(debug)std::cout << "Integer\t\t" << cache << std::endl;
                output << "Integer\t\t" << cache << std::endl;
            }
            else if (state[2] == 4) {
                if(debug)std::cout << "Real\t\t" << cache << std::endl;
                output << "Real\t\t" << cache << std::endl;
            }
            else if (state[0] != 1) {
                if(debug)std::cout << "Unknown\t\t" << cache << std::endl;
                output << "Unknown\t\t" << cache << std::endl;
            }
            //Reset all state and output
            for (int j = 0; j < 3; j++)
                state[j] = 1;
            cache = "";
            //Backward one character.
            i--;
        }
        else {
            cache = character;
            //Ignoring all the comments
            if (character == '[' && input[i + 1] == '*') {
                while (!(input[i] == '*' && input[i + 1] == ']')) {
                    i++;
                }
                i++;
                cache = "";
                continue;
            }
            //Operator case
            if (code == 5) {
                cache = character;
                if (pos(input[i + 1]) == 5) {
                    cache = cache + input[i + 1];
                    i++;    //two character ops
                }
		        int success = 0;
                for (int j = 0; j < sizeof(OPERATORS) / sizeof(OPERATORS[0]); j++) {
                    if (cache.compare(OPERATORS[j]) == 0) {
                        if(debug)std::cout << "Operator\t" << cache << std::endl;
                        output << "Operator\t" << cache << std::endl;
                        state_other = 0;
			            success = 1;
                        break;
                    }
                }
		        //if two op case failed, it is one op.
		        if(success == 0){
                    if(debug)std::cout << "Operator\t" << cache[0] << std::endl;
			        output << "Operator\t" << cache[0] << std::endl;
			        i--;
			        state_other = 0;
		        }
            }
            //Separator case
            for (int j = 0; j < sizeof(SEPARATORS) / sizeof(SEPARATORS[0]); j++) {
                if (character == SEPARATORS[j]) {
                    if(debug)std::cout << "Separator\t" << character << std::endl;
                    output << "Separator\t" << character << std::endl;
                    state_other = 0;
                    break;
                }
            }
            //If spaces
            if (character == ' ' || character == '\t' || character == '\n')
                state_other = 0;
            //If none of the cases above.
            if (state_other == 1) {
                if(debug)std::cout << "Unknown\t\t" << cache << std::endl;
                output << "Unknown\t\t" << cache << std::endl;
            }
            //Reset state
            state_other = 1;
            cache = "";
        }
    }
    std::cout << "\nOutput File is stored in [" << fileName << "]\n" << std::endl;

    std::cout << "Lexer Analysis Executed Successfully\n" << std::endl;
    return 0;
}
//-------------------------------------------------------
//Object Code Generation

//Symbol Table
struct{
	int addr = 0;
	std::string id = "";
	std::string type = "";
}symbolTable[1000];
int currAddrSymbol = 0;
const int offset = 5000;

//Check Symbol Duplicate
bool checkSymbol(){
	for(int i = 0; i < currAddrSymbol; i++){
		if(!symbolTable[i].id.compare(symbolTable[currAddrSymbol].id))
			return true;
	}
	return false;
}
void fillType(std::string type){
	for(int i = currAddrSymbol-1; i >= 0; i--){
		if(!symbolTable[i].type.compare(""))
			symbolTable[i].type = type;
		else
			break;
	}
}

//Instruction Table
struct {
    int addr = 0;
    std::string op = "";
    std::string oprnd = "";
}instructionTable[1000];
int currAddrInstruction = 0;
void gen_instr(std::string op, std::string oprnd = "") {
    instructionTable[currAddrInstruction].addr = currAddrInstruction + 1;
    instructionTable[currAddrInstruction].op = op;
    instructionTable[currAddrInstruction].oprnd = oprnd;
    currAddrInstruction++;
}
int get_address(std::string symbol) {
    for (int i = 0; i < currAddrSymbol; i++) {
        if (!symbolTable[i].id.compare(symbol))
            return symbolTable[i].addr;
    }
    return -1;
}
int missing_address[50];
int currAddrMissing = -1;
void push_jumpstack(int address) {
    currAddrMissing++;
    missing_address[currAddrMissing] = address;
}
void pop_jumpstack(int address) {
    instructionTable[missing_address[currAddrMissing]].oprnd = std::to_string(address);
    currAddrMissing--;
}

//--------------------------------------------------------
//Syntax Analysis

//Global Variable
std::string tok, lex;
std::ifstream syntaxFile;

int lexer() {
    if (!(syntaxFile >> tok))
        return 1;

    if (!(syntaxFile >> lex))
        return 1;
    return 0;
}

//declaration
//R1
int rat23();
//R2
int optFunctionDef();
//R3
int funcdef();
int funcdef2();
//R4
int fun();
//R5
int optParameterList();
//R6
int parameterList();
int parameterList2();
//R7
int parameter();
//R8
int qualifier();
//R9
int body();
//R10
int optDeclarationList();
//R11
int declarationList();
int declarationList2();
//R12
int declaration();
//R13
int ids(int declare);
int ids2(int declare);
//R14
int statementList();
int statementList2();
//R15
int statement();
//R16 ok?
int compound();
//R17 ok
int assign();
//R18
int ifState();
int ifState2();
//R19
int returnState();
//R20
int printState();
//R21
int getState();
//R22
int whileState();
//R23 ok
int condition();
//R24 ok
int relop();
//R25
int expression();
int expressionP();
//R26
int term();
int termP();
//R27
int factor();
//R28
int primary();
//R29
int empty();


//R1
int rat23() {
    if(debug)std::cout << "\t<Rat23S> -> <Opt Function Definitions> # <Opt Declaration List> # <Statement List>" << std::endl;
    int error_code = optFunctionDef();
    if (error_code != 0)
        return error_code;
    if (lex == "#") {
        if(debug)std::cout << "Token: " << tok << "\t\tLexeme: " << lex << std::endl;
        lexer();
    }
    else {
        std::cout << "\n\n Error: Separator '#' Expected in between optFunctionDef and Opt Declaration List" << std::endl;
        return 1;
    }
    error_code = optDeclarationList();
    if (error_code != 0)
        return error_code;
    if (lex == "#") {
        if(debug)std::cout << "Token: " << tok << "\t\tLexeme: " << lex << std::endl;
        lexer();
    }
    else {
        std::cout << "\n\n Error: Separator '#' Expected in between Opt Declaration List and statement List" << std::endl;
        return 1;
    }
    return statementList();
}

//R2
int optFunctionDef() {
    if(debug)std::cout << "\t<Opt Function Definitions> -> <Function Definitions> | <Empty>" << std::endl;
    if (lex.compare("#") != 0) {
        return funcdef();
    }
    else {
        return empty();
    }
}

//R3
int funcdef() {
    if(debug)std::cout << "\t<Function Definitions> -> <Function> <Function Definitions 2>" << std::endl;
    int error_code = fun();
    if (error_code != 0)
        return error_code;
    return funcdef2();
}
int funcdef2() {
    if(debug)std::cout << "\t<Function Definitions 2> -> <Function Definitions> | �`" << std::endl;
    if (lex.compare("function") == 0) {
        funcdef();
    }
    return empty();
}

//R4
int fun() {
    if(debug)std::cout << "\t<Function> -> function <Identifier> (<Opt Parameter List>) <Opt Declaration List> <Body>" << std::endl;
    if (lex.compare("function") == 0) {
        if(debug)std::cout << "Token: " << tok<<"\t\tLexeme: " << lex << std::endl;
        lexer();
    }else {
        std::cout << "\n\n Error: Keyword 'Function' Expected" << std::endl;
        return -1;
    }
    if (tok.compare("Identifier") == 0) {
        if(debug)std::cout << "Token: " << tok<<"\t\tLexeme: " << lex << std::endl;
        lexer();
    }
    else {
        std::cout << "\n\n Error: Identifier Expected" << std::endl;
        return -1;
    }
    if (lex.compare("(") == 0) {
        if(debug)std::cout << "Token: " << tok<<"\t\tLexeme: " << lex << std::endl;
        lexer();
    }
    else {
        std::cout << "\n\n Error: Separator '(' Expected" << std::endl;
        return -1;
    }
    int error_code = optParameterList();
    if (error_code != 0)
        return error_code;
    if (lex.compare(")") == 0) {
        if(debug)std::cout << "Token: " << tok<<"\t\tLexeme: " << lex << std::endl;
        lexer();
    }
    else {
        std::cout << "\n\n Error: Separator ')' Expected" << std::endl;
        return -1;
    }
    error_code = optDeclarationList();
    if (error_code != 0)
        return error_code;
    return body();
}

//R5
int optParameterList() {
    if(debug)std::cout << "\t<Opt Parameter List> -> <Parameter List> | <Empty>" << std::endl;
    if (lex.compare(")") == 0) {
        return empty();
    }
    else {
        return parameterList();
    }
}

//R6
int parameterList() {
    if(debug)std::cout << "\t<Parameter List> -> <Parameter> <Parameter List 2>" << std::endl;
    int error_code = parameter();
    if (error_code != 0)
        return error_code;
    return parameterList2();
}
int parameterList2() {
    if(debug)std::cout << "\t<Parameter List 2> -> , <Parameter List> | �`" << std::endl;
    if (lex.compare(",") == 0) {
        lexer();
        return parameterList();
    }
    return 0;
}

//R7
int parameter() {
    if(debug)std::cout << "\t<Parameter> -> <IDs> <Qualifier>" << std::endl;
    int error_code = ids(0);
    if (error_code != 0)
        return error_code;
    return qualifier();
}

//R8
int qualifier() {
    if(debug)std::cout << "\t<Qualifier> -> int | bool | real" << std::endl;
    if (lex.compare("int") == 0 || lex.compare("bool") == 0 || lex.compare("real") == 0) {
        if(debug)std::cout << "Token: " << tok<<"\t\tLexeme: " << lex << std::endl;
        lexer();
        return 0;
    }
    else {
        std::cout << "\n\n Error: Keyword: (int | bool | real) Expected" << std::endl;
        return -1;
    }
    return 0;
}

//R9
int body() {
    if(debug)std::cout << "\t<Body> -> {<Statement List>}" << std::endl;
    int error_code = 0;
    if (lex.compare("{") == 0) {
        if(debug)std::cout << "Token: " << tok<<"\t\tLexeme: " << lex << std::endl;
        lexer();
    }
    else {
        std::cout << "\n\n Error: Separator '{' Expected" << std::endl;
        return -1;
    }
    error_code = statementList();
    if (error_code != 0)
        return error_code;
    if (lex.compare("}") == 0) {
        if(debug)std::cout << "Token: " << tok<<"\t\tLexeme: " << lex << std::endl;
        lexer();
    }
    else {
        std::cout << "\n\n Error: Separator '}' Expected" << std::endl;
        return -1;
    }
    return error_code;
}

//R10
int optDeclarationList() {
    if(debug)std::cout << "\t<Opt Declaration List> -> <Declaration List> | <Empty>" << std::endl;
    int error_code = 0;
    if (lex.compare("bool") == 0 || lex.compare("real") == 0 || lex.compare("int") == 0){
        error_code = declarationList();
	}
	else {
        error_code = empty();
    }
    return error_code;
}

//R11
int declarationList() {
    if(debug)std::cout << "\t<Declaration List> -> <Declaration> ; <DeclarationList2>" << std::endl;
    int error_code = declaration();
    if (error_code != 0)
        return error_code;
    if (lex.compare(";") == 0) {
        if(debug)std::cout << "Token: " << tok<<"\t\tLexeme: " << lex << std::endl;
        lexer();
    }
    else {
        std::cout << "\n\nError: Separator ';' Expected" << std::endl;
        return -1;
    }
    return declarationList2();
}
int declarationList2() {
    if(debug)std::cout << "\t<DeclarationList2> -> <DeclarationList> | �`" << std::endl;
    if (lex.compare("#") == 0 || lex.compare("{") == 0) {
        return 0;
    }
    return declarationList();
}

//R12
int declaration() {
    if(debug)std::cout << "\t<Declaration> -> <Qualifier> <IDs>" << std::endl;
    int error_code = qualifier();	//int = 0, real = 1, bool = 2
    if (error_code < 0 || error_code > 3)
        return error_code;
	int t = error_code;
	error_code = ids(1);
	//Fill in the data type.
    switch(t){
	case 0:
		fillType("Integer");
		break;
	case 1:
		fillType("Real");
		break;
	case 2:
		fillType("Bool");
		break;
	default:
		return error_code;
	}
    return error_code;
}

//R13
int ids(int declare = 0) {
    if(debug)std::cout << "\t<ID> -> <Identifier> <ID2>" << std::endl;
    if (tok.compare("Identifier") == 0) {
        if(debug)std::cout << "Token: " << tok<<"\t\tLexeme: " << lex << std::endl;
		if(declare == 1){
			symbolTable[currAddrSymbol].id = lex;
			symbolTable[currAddrSymbol].addr = offset+currAddrSymbol;
            currAddrSymbol++;
        }
		if(checkSymbol()){
			std::cout << "Repeat Declaration of " << lex << std::endl;
			return 10;
		}
		lexer();
    }
    else {
        std::cout << "\n\nError: Identifier Expected" << std::endl;
        return -1;
    }
    return ids2(declare);
}
int ids2(int declare) {
    if(debug)std::cout << "\t<ID2> -> ,<ID> | �`" << std::endl;
    if (lex.compare(",") == 0) {
        if(debug)std::cout << "Token: " << tok<<"\t\tLexeme: " << lex << std::endl;
        lexer();
        return ids(declare);
    }
    else {
        return 0;
    }
    return 0;
}

//R14
int statementList() {
    if(debug)std::cout << "\t<Statement List> -> <Statement> <Statement List2>" << std::endl;
    int error_code = statement();
    if (error_code != 0)
        return error_code;
    return statementList2();
    }
int statementList2() {
    if(debug)std::cout << "\t<Statement List2> -> <Statement List> | �`" << std::endl;
    if (lex.compare("{") == 0 || tok.compare("Identifier") == 0 || lex.compare("if") == 0
        || lex.compare("return") == 0 || lex.compare("put") == 0 || lex.compare("get") == 0
        || lex.compare("while") == 0)
        statementList();
    return 0;
}

//R15
int statement() {
    int error_code = 0;
    if (lex.compare("{") == 0) {
        if(debug)std::cout << "\t<Statement> -> <Compound>" << std::endl;
        error_code = compound();
    }
    else if (tok.compare("Identifier") == 0) {
        if(debug)std::cout << "\t<Statement> -> <Identifier>" << std::endl;
        error_code = assign();
    }
    else if (lex.compare("if") == 0) {
        if(debug)std::cout << "\t<Statement> -> <If>" << std::endl;
        error_code = ifState();
    }
    else if (lex.compare("return") == 0) {
        if(debug)std::cout << "\t<Statement> -> <Return>" << std::endl;
        error_code = returnState();
    }
    else if (lex.compare("put") == 0) {
        if(debug)std::cout << "\t<Statement> -> <Print>" << std::endl;
        error_code = printState();
    }
    else if (lex.compare("get") == 0) {
        if(debug)std::cout << "\t<Statement> -> <Scan>" << std::endl;
        error_code = getState();
    }
    else if (lex.compare("while") == 0) {
        if(debug)std::cout << "\t<Statement> -> <While>" << std::endl;
        error_code = whileState();
    }
    else {
        std::cout << "Error: Statement Expected, No Statement found" << std::endl;
        return -1;
    }
    return error_code;
}

//R16
int compound() {
    if(debug)std::cout << "\t<Compound> -> {<Statement List>}" << std::endl;
    if (lex.compare("{") == 0) {
        if(debug)std::cout << "Token: " << tok<<"\t\tLexeme: " << lex << std::endl;
        lexer();
    }
    else {
        std::cout << "\n\n Error: Separator '{' Expected" << std::endl;
        return -1;
    }
    int error_code = statementList();
    if (error_code != 0)
        return error_code;
    if (lex.compare("}") == 0) {
        if(debug)std::cout << "Token: " << tok<<"\t\tLexeme: " << lex << std::endl;
        lexer();
    }
    else {
        std::cout << "\n\n Error: Separator '}' Expected" << std::endl;
        return -1;
    }
    return error_code;
}

//R17 ok
int assign() {
    std::string save = "";
    if(debug)std::cout << "\t<Assign> -> <Identifier> = <Expression>" << std::endl;
    if (tok.compare("Identifier") == 0) {
        if(debug)std::cout << "Token: " << tok<<"\t\tLexeme: " << lex << std::endl;
        save = lex;
        lexer();
    }
    else {
        std::cout << "\n\n Error: Identifier Expected" << std::endl;
        return -1;
    }
    if (lex.compare("=") == 0) {
        if(debug)std::cout << "Token: " << tok<<"\t\tLexeme: " << lex << std::endl;
        lexer();
    }
    else {
        std::cout << "\n\n Error: Operator '=' Expected" << std::endl;
        return -1;
    }
    int error_code = expression();
    if (error_code != 0)
        return error_code;
    if (lex.compare(";") == 0) {
        if(debug)std::cout << "Token: " << tok<<"\t\tLexeme: " << lex << std::endl;
        lexer();
        int address = get_address(save);
        if (address == -1) {
            std::cout << "Variable " << save << " was not declared..." << std::endl;
            return -1;
        }
        gen_instr("POPM", std::to_string(address));
    }
    else {
        std::cout << "\n\n Error: Separator ';' Expected" << std::endl;
        return -1;
    }
    return error_code;
}

//R18 ok
int ifState() {
    if(debug)std::cout << "\t<If> -> if (<Condition>) <Statement> <If2>" << std::endl;;
    if (lex.compare("if") == 0) {
        if(debug)std::cout << "Token: " << tok<<"\t\tLexeme: " << lex << std::endl;
        lexer();
    }
    else {
        std::cout << "\n\n Error: Keyword 'if' Expected" << std::endl;
        return -1;
    }
    if (lex.compare("(") == 0) {
        if(debug)std::cout << "Token: " << tok<<"\t\tLexeme: " << lex << std::endl;
        lexer();
    }
    else {
        std::cout << "\n\n Error: Separator '(' Expected" << std::endl;
        return -1;
    }
    int error_code = condition();
    if (error_code != 0)
        return error_code;
    if (lex.compare(")") == 0) {
        if(debug)std::cout << "Token: " << tok<<"\t\tLexeme: " << lex << std::endl;
        lexer();
    }
    else {
        std::cout << "\n\n Error: Separator ')' Expected" << std::endl;
        return -1;
    }
    error_code = statement();
    if (error_code != 0)
        return error_code;
    return ifState2();
}
int ifState2() {
    if (lex.compare("fi") == 0) {
        if(debug)std::cout << "\t<If2> -> fi" << std::endl;
        if(debug)std::cout << "Token: " << tok<<"\t\tLexeme: " << lex << std::endl;
        gen_instr("LABEL");
        pop_jumpstack(currAddrInstruction);
        lexer();
        return 0;
    }
    else if (lex.compare("else") == 0) {
        if(debug)std::cout << "\t<If2> -> else <statement fi>" << std::endl;
        if(debug)std::cout << "Token: " << tok<<"\t\tLexeme: " << lex << std::endl;
        gen_instr("JMP");
        gen_instr("LABEL");
        pop_jumpstack(currAddrInstruction);
        push_jumpstack(currAddrInstruction-2);
        lexer();
    }
    else {
        std::cout << "\n\n Error: Keyword 'fi' or 'else' Expected" << std::endl;
        return -1;
    }
    statement();
    if (lex.compare("fi") == 0) {
        if(debug)std::cout << "Token: " << tok<<"\t\tLexeme: " << lex << std::endl;
        lexer();
        gen_instr("LABEL");
        pop_jumpstack(currAddrInstruction);
        return 0;
    }
    else {
        std::cout << "\n\n Error: Keyword 'fi' Expected" << std::endl;
        return -1;
    }
}

//R19 ok
int returnState() {
    if(debug)std::cout << "\t<Return> -> return; | return <Expression>" << std::endl;
    int error_code = 0;
    if (lex.compare("return") == 0) {
        if(debug)std::cout << "Token: " << tok<<"\t\tLexeme: " << lex << std::endl;
        lexer();
    }
    else {
        std::cout << "\n\n Error: Keyword 'return' Expected" << std::endl;
        return -1;
    }
    if (lex.compare(";") != 0) {
        error_code = expression();
        if (error_code != 0)
            return error_code;
    }
    if (lex.compare(";") == 0) {
        if(debug)std::cout << "Token: " << tok<<"\t\tLexeme: " << lex << std::endl;
        lexer();
        return 0;
    }else {
        std::cout << "\n\n Error: Separator ';' Expected" << std::endl;
        return -1;
    }
    return error_code;
}

//R20 ok
int printState() {
    if(debug)std::cout << "\t<Print> -> put (<Expression>)" << std::endl;
    if (lex.compare("put") == 0) {
        if(debug)std::cout << "Token: " << tok<<"\t\tLexeme: " << lex << std::endl;
        lexer();
    }
    else {
        std::cout << "\n\n Error: Keyword 'put' Expected" << std::endl;
        return -1;
    }
    if (lex.compare("(") == 0) {
        if(debug)std::cout << "Token: " << tok<<"\t\tLexeme: " << lex << std::endl;
        lexer();
    }
    else {
        std::cout << "\n\n Error: Separator '(' Expected" << std::endl;
        return -1;
    }
    int error_code = expression();
    if (error_code != 0)
        return error_code;
    if (lex.compare(")") == 0) {
        if(debug)std::cout << "Token: " << tok<<"\t\tLexeme: " << lex << std::endl;
        lexer();
    }
    else {
        std::cout << "\n\n Error: Separator ')' Expected" << std::endl;
        return -1;
    }
    gen_instr("OUT");
    if (lex.compare(";") == 0) {
        if(debug)std::cout << "Token: " << tok<<"\t\tLexeme: " << lex << std::endl;
        lexer();
    }
    else {
        std::cout << "\n\n Error: Separator ';' Expected" << std::endl;
        return -1;
    }
    return error_code;
}

//R21 ok
int getState() {
    if(debug)std::cout << "\t<Scan> -> get (<IDs>)" << std::endl;
    if (lex.compare("get") == 0) {
        if(debug)std::cout << "Token: " << tok<<"\t\tLexeme: " << lex << std::endl;
        lexer();
    }
    else {
        std::cout << "\n\n Error: Keyword 'get' Expected" << std::endl;
        return -1;
    }
    gen_instr("IN");
    if (lex.compare("(") == 0) {
        if(debug)std::cout << "Token: " << tok<<"\t\tLexeme: " << lex << std::endl;
        lexer();
    }
    else {
        std::cout << "\n\n Error: Separator '(' Expected" << std::endl;
        return -1;
    }
    std::string save = lex;
    int error_code = ids(0);
    if (error_code != 0)
        return error_code;
    if (lex.compare(")") == 0) {
        if(debug)std::cout << "Token: " << tok<<"\t\tLexeme: " << lex << std::endl;
        lexer();
    }
    else {
        std::cout << "\n\n Error: Separator ')' Expected" << std::endl;
        return -1;
    }
    int address = get_address(save);
    if (address == -1) {
        std::cout << "Variable " << save << " was not declared..." << std::endl;
        return -1;
    }
    gen_instr("POPM", std::to_string(address));
    if (lex.compare(";") == 0) {
        if(debug)std::cout << "Token: " << tok << "\t\tLexeme: " << lex << std::endl;
        lexer();
    }
    else {
        std::cout << "\n\n Error: Separator ';' Expected" << std::endl;
        return -1;
    }
    return error_code;
}

//R22 ok?
int whileState() {
    int start_address;
    if(debug)std::cout << "\t<While> -> while (<Condition>) <Statement> endwhile" << std::endl;
    if (lex.compare("while") == 0) {
        if(debug)std::cout << "Token: " << tok<<"\t\tLexeme: " << lex << std::endl;
        gen_instr("LABEL");
        start_address = currAddrInstruction;
        lexer();
    }
    else {
        std::cout << "\n\n Error: Keyword 'while' Expected" << std::endl;
        return -1;
    }
    if (lex.compare("(") == 0) {
        if(debug)std::cout << "Token: " << tok<<"\t\tLexeme: " << lex << std::endl;
        lexer();
    }
    else {
        std::cout << "\n\n Error: Separator '(' Expected" << std::endl;
        return -1;
    }
    int error_code = condition();
    if (error_code != 0)
        return error_code;
    if (lex.compare(")") == 0) {
        if(debug)std::cout << "Token: " << tok<<"\t\tLexeme: " << lex << std::endl;
        lexer();
    }
    else {
        std::cout << "\n\n Error: Separator ')' Expected" << std::endl;
        return -1;
    }
    error_code = statement();
    if (error_code != 0)
        return error_code;
    std::cout << "debug: " << start_address << std::endl;
    gen_instr("JMP", std::to_string(start_address));
    pop_jumpstack(currAddrInstruction + 1);
    if (lex.compare("endwhile") == 0) {
        if(debug)std::cout << "Token: " << tok<<"\t\tLexeme: " << lex << std::endl;
        lexer();
    }
    else {
        std::cout << "\n\n Error: Keyword 'endwhile' Expected" << std::endl;
        return -1;
    }
    return error_code;
}

//R23 ok
int condition() {
    if(debug)std::cout << "\t<Condition> -> <Expression> <Relop> <Expression>" << std::endl;
    int error_code = expression();
    if (error_code != 0)
        return error_code;
    std::string save = lex;
    error_code = relop();
    if (error_code != 0)
        return error_code;
    error_code = expression();
    if (save.compare("==") == 0) {
        gen_instr("EQU");
    }
    else if (save.compare("!=") == 0) {
        gen_instr("NEQ");
    }
    else if (save.compare(">") == 0) {
        gen_instr("GRT");
    }
    else if (save.compare("<") == 0) {
        gen_instr("LES");
    }
    else if (save.compare("<=") == 0) {
        gen_instr("LEQ");
    }
    else if (save.compare("=>") == 0) {
        gen_instr("GEQ");
    }
    push_jumpstack(currAddrInstruction);
    gen_instr("JMPZ");
    return error_code;
}

//24 ok
int relop() {
    if (lex.compare("==") == 0 || lex.compare("!=") == 0 || lex.compare(">") == 0 ||
        lex.compare("<") == 0 || lex.compare("<=") == 0 || lex.compare("=>") == 0) {
        if (debug)std::cout << "Token: " << tok << "\t\tLexeme: " << lex << std::endl;
        lexer();
        return 0;
    }
    else {
        std::cout << "\n\n Error: Operator Expected for Condition" << std::endl;
        return -1;
    }
    return 0;
}

//R25 ok
int expressionP() {
    bool type;
    if(debug)std::cout << "\t<expressionPrime> -> +<Term><expressionPrime> | -<Term><expressionPrime> | �`" << std::endl;
    if (lex.compare("+") == 0) {
        if(debug)std::cout << "Token: " << tok<<"\t\tLexeme: " << lex << std::endl;
        lexer();
        type = true;
    }
    else if (lex.compare("-") == 0) {
        if(debug)std::cout << "Token: " << tok<<"\t\tLexeme: " << lex << std::endl;
        lexer();
        type = false;
    }
    else {
        empty();
        return 0;
    }
    int error_code = term();
    if (error_code != 0)
        return error_code;
    if (type)
        gen_instr("ADD");
    else
        gen_instr("SUB");
    return expressionP();
}
int expression() {
    if(debug)std::cout << "\t<Expression> -> <Term><expressionPrime>" << std::endl;
    int error_code = term();
    if (error_code != 0)
        return error_code;
    return expressionP();
}

//R26 ok
int termP() {
    bool type;
    if(debug)std::cout << "\t<termPrime> -> *<Factor><termPrime> | /<Factor><termPrime> | �`" << std::endl;
    if (lex.compare("*") == 0) {
        if(debug)std::cout << "Token: " << tok<<"\t\tLexeme: " << lex << std::endl;
        lexer();
        type = true;
    }
    else if (lex.compare("/") == 0) {
        if(debug)std::cout << "Token: " << tok<<"\t\tLexeme: " << lex << std::endl;
        lexer();
        type = false;
    }
    else {
        return empty();
    }
    int error_code = factor();
    if (error_code != 0)
        return error_code;
    if (type)
        gen_instr("MUL");
    else
        gen_instr("DIV");
    return termP();
}
int term() {
    if(debug)std::cout << "\t<term> -> <factor><termPrime>" << std::endl;
    int error_code = factor();
    if (error_code != 0)
        return error_code;
    return termP();
}

//R27 ok
int factor() {
    bool type = false;
    if (lex.compare("-") == 0) {
        if(debug)std::cout << "Token: " << tok<<"\t\tLexeme: " << lex << std::endl;
        lexer();
        type = true;
    }
    if(debug)std::cout << "\t<Factor> -> <Primary>" << std::endl;
    int error_code = primary();
    if (type) {
        gen_instr("PUSHI", "- 1");
        gen_instr("MUL");
    }
    return error_code;
}

//R28 ok
int primary() {
    if (lex.compare("true") == 0) {
        if(debug)std::cout << "\t<Primary> -> true" << std::endl;
        if(debug)std::cout << "Token: " << tok<<"\t\tLexeme: " << lex << std::endl;
        gen_instr("PUSHI", "1");
        lexer();
    }
    else if (lex.compare("false") == 0) {
        if(debug)std::cout << "\t<Primary> -> false" << std::endl;
        if(debug)std::cout << "Token: " << tok<<"\t\tLexeme: " << lex << std::endl;
        gen_instr("PUSHI", "0");
        lexer();
    }
    else if (tok.compare("Identifier") == 0) {
        std::string save = lex;
        if(debug)std::cout << "\t<Primary> -> <Identifier>";
        if(debug)std::cout << "Token: " << tok<<"\t\tLexeme: " << lex << std::endl;
        lexer();
        if (lex.compare("(") == 0) {
            if(debug)std::cout << " (<IDs>)";
            if(debug)std::cout << "Token: " << tok<<"\t\tLexeme: " << lex << std::endl;
            lexer();
            int error_code = ids(0);
            if (error_code != 0)
                return error_code;
            if (lex.compare(")") == 0) {
                if(debug)std::cout << "Token: " << tok<<"\t\tLexeme: " << lex << std::endl;
                lexer();
            }
            else {
                std::cout << "\n\n Error: Separator ')' Expected" << std::endl;
                return -1;
            }
            return 0;
        }
        else {
            int address = get_address(save);
            if (address == -1) {
                std::cout << "Variable " << save << " was not declared..." << std::endl;
                return -1;
            }
            gen_instr("PUSHM", std::to_string(address));
        }
        if(debug)std::cout << std::endl;
    }
    else if (tok.compare("Integer") == 0) {
        if(debug)std::cout << "\t<Primary> -> <Integer>" << std::endl;
        if(debug)std::cout << "Token: " << tok<<"\t\tLexeme: " << lex << std::endl;
        gen_instr("PUSHI", lex);
        lexer();
    }
    else if (tok.compare("Real") == 0) {
        if(debug)std::cout << "\t<Primary> -> <Real>" << std::endl;
        if(debug)std::cout << "Token: " << tok<<"\t\tLexeme: " << lex << std::endl;
        lexer();
    }
    else if (lex.compare("(") == 0) {
        if(debug)std::cout << "\t<Primary> -> (<Expression>)" << std::endl;
        if(debug)std::cout << "Token: " << tok<<"\t\tLexeme: " << lex << std::endl;
        lexer();
        int error_code = expression();
        if (error_code != 0)
            return error_code;
        if (lex.compare(")") == 0) {
            if(debug)std::cout << "Token: " << tok<<"\t\tLexeme: " << lex << std::endl;
            lexer();
        }
        else {
            std::cout << "\n\n Error: Separator ')' Expected" << std::endl;
            return -1;
        }
    }
    else {
        std::cout << "\n\n Error: Primary Expected" << std::endl;
        return -1;
    }
    return 0;
}

//R29 ok
int empty() {
    if(debug)std::cout << "\t<Empty> -> �`" << std::endl;
    //do nothing
    return 0;
}


//Main function
int main(int argc, char *argv[]) {

    //Check if input is correct, required input file as argument
    //Total of one argument.
    if (argc < 2 || argc > 3) {
        std::cout << "Please Include the input file as first argument" << std::endl;
        std::cout << "Usage: ./compiler <input file> <option>" << std::endl;
        std::cout << "      <option>" << std::endl;
        std::cout << "      --debug" << std::endl;
        return -1;
    }
    if (argc == 3){
        std::string d = argv[2];
	if(d.compare("--debug") == 0){
	    debug = true;
	}
    }
    std::string fileName = argv[1];
    lexerAnalysis(fileName);
    
    fileName = fileName + ".txt";

    syntaxFile.open(fileName);
    if(debug)std::cout << "\n\nSyntax Analysis:\n" << std::endl;
    lexer();
    if (lex.compare("lexeme") == 0)
        lexer();
    int error_code = rat23();
    
    if (error_code == 0) {
        std::cout << "Syntax Analysis executed Successfully\n" << std::endl;
    }
    else {
        std::cout << "\tFound '" << lex << "' Instead" << std::endl;
        std::cout << "Error Occured, Error Code = " << error_code << std::endl;
    }
	std::cout << "Symbol Table\nIdentifier\tMemoryLocation\tType" << std::endl;
    for(int i = 0; i < currAddrSymbol; i++){
		std::cout << symbolTable[i].id << "\t\t" << symbolTable[i].addr << "\t\t" << symbolTable[i].type << std::endl;
    }
    std::cout << "\nObject Code Table\nAddress\tOp\tOprnd" << std::endl;
    for (int i = 0; i < currAddrInstruction; i++)
        std::cout << instructionTable[i].addr << "\t\t" << instructionTable[i].op << "\t\t" << instructionTable[i].oprnd << std::endl;
    return 0;
}