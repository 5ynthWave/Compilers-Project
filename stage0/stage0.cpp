// Erik Zuniga && Roberto Lopez
// CS4301 - Compilers Project Stage-0

// Test push

#include <stage0.h>
#include <string>
using namespace std;

// Constructor
Compiler(char **argv) {
 /* open sourceFile using argv[1]
    open listingFile using argv[2]
    open objectFile using argv[3] */
  
}
// Destructor
~Compiler() {
 /* close all open files */
  
}

void createListingHeader() {
 /* print "STAGE0:", name(s), DATE, TIME OF DAY
    print "LINE NO:", "SOURCE STATEMENT"
    - line numbers and source statements should be aligned under the headings */
  
}
void parser() {
 /* nextChar() 
    // ch must be initialized to the first character of the source file 
    if (nextToken() != "program")
      processError(keyword "program" expected)
    // a call to nextToken() has two effects
    // (1) the variable, token, is assigned the value of the next token
    // (2) the next token is read from the source file in order to make
    // the assignment. The value returned by nextToken() is also 
    // the next token.
    prog()
    // parser implements the grammar rules, calling first rule */

}
void createListingTrailer() {
 /* print "COMPILATION TERMINATED", "# ERRORS ENCOUNTERED" */
  
}

// Methods implementing the grammar productions:
// Stage 0, Production 1 -> Token should be "program"
void prog() {
 /* if (token != "program")
      processError(keyword "program" expected)
    progStmt()
    if (token == "const")
      consts()
    if (token == "var")
      vars()
    if (token != "begin")
      processError(keyword "begin" expected)
    beginEndStmt()
    if (token != END_OF_FILE)
      processError(no text may follow "end”) */
  
}
// Stage 0, Production 2 -> Token should be "program"
void progStmt() {
 /* string x
    if (token != "program")
      processError(keyword "program" expected)
    x = nextToken()
    if (token is not a NON_KEY_ID)
      processError(program name expected)
    if (nextToken() != ";")
      processError(semicolon expected)
    nextToken()
    code("program", x)
    insert(x,PROG_NAME,CONSTANT,x,NO,0) */
  
}
// Stage 0, Production 3 -> Token should be "const"
void consts() {
 /* if (token != "const")
      processError(keyword "const" expected)
    if (nextToken() is not a NON_KEY_ID)
      processError(non-keyword identifier must follow "const")
    constStmts() */
  
}
// Stage 0, Production 4 -> Token should be "var"
void vars() {
 /* if (token != "var")
      processError(keyword "var" expected)
    if (nextToken() is not a NON_KEY_ID)
      processError(non-keyword identifier must follow "var")
    varStmts() */
  
}
// Stage 0, Production 5 -> Token should be "begin"
void beginEndStmt() {
 /* if (token != "begin")
      procesError(keyword "begin" expected)
    if (nextToken() != "end")
      processError(keyword "end" expected)
    if (nextToken() != ".")
      processError(period expected)
    nextToken()
    code("end", ".") */
  
}
// Stage 0, Production 6 -> Token should be NON_KEY_ID
void constStmts() {
 /* string x,y
    if (token is not a NON_KEY_ID)
      processError(non-keyword identifier expected)
    x = token
    if (nextToken() != "=")
      processError("=" expected)
    y = nextToken()
    if (y is not one of "+","-","not",NON_KEY_ID,"true","false",INTEGER)
      processError(token to right of "=" illegal)
    if (y is one of "+","-") {
      if (nextToken() is not an INTEGER)
        processError(integer expected after sign)
      y = y + token;
    }
    if (y == "not") {
      if (nextToken() is not a BOOLEAN)
        processError(boolean expected after "not")
      if (token == "true")
        y = "false"
      else
        y = "true"
    }
    if (nextToken() != ";")
      processError(semicolon expected)
    if (the data type of y is not INTEGER or BOOLEAN)
      processError(data type of token on the right-hand side must be INTEGER or BOOLEAN)
    insert(x,whichType(y),CONSTANT,whichValue(y),YES,1)
    x = nextToken()
    if (x is not one of "begin","var",NON_KEY_ID)
      processError(non-keyword identifier, "begin", or "var" expected)
    if (x is a NON_KEY_ID)
      constStmts() */
  
}
// Stage 0, Production 7 -> Token should be NON_KEY_ID
void varStmts() {
 /* string x,y
    if (token is not a NON_KEY_ID)
      processError(non-keyword identifier expected) 
    x = ids()
    if (token != ":")
      processError(":" expected)
    if (nextToken() is not one of "integer","boolean")
      processError(illegal type follows ":") 
    y = token
    if (nextToken() != ";")
      processError(semicolon expected)
    insert(x,y,VARIABLE,"",YES,1)
    if (nextToken() is not one of "begin",NON_KEY_ID)
      processError(non-keyword identifier or "begin" expected)
    if (token is a NON_KEY_ID)
      varStmts() */
  
}
// Stage 0, Production 8 -> Token should be NON_KEY_ID
string ids() {
 /* string temp,tempString
    if (token is not a NON_KEY_ID)
      processError(non-keyword identifier expected) 
    tempString = token
    temp = token
    if (nextToken() == ",") { 
      if (nextToken() is not a NON_KEY_ID)
        processError(non-keyword identifier expected) 
      tempString = temp + "," + ids()
    } 
    return tempString */
  
}

// Helper functions for the Pascallite lexicon:
// Determines if s is a keyword
bool isKeyword(string s) const {

}
// Determines if c is a special symbol
bool isSpecialSymbol(char c) const {

}
// Determines if s is a non_key_id
bool isNonKeyId(string s) const {

}
// Determines if s is an integer
bool isInteger(string s) const {

}
// Determines if s is a boolean
bool isBoolean(string s) const {

}
// Determines if s is a literal
bool isLiteral(string s) const {

}

// Action routines:
// Create symbol table entry for each identigier in list of external names
// multiply inserted names (severeal at once) are illegal
void insert(string externalName, storeTypes inType, modes inMode,
            string inValue, allocation inAlloc, int inUnits) {
 /* string name
    while (name broken from list of external names and put into name != "") {
      if (symbolTable[name] is defined)
        processError(multiple name definition)
      else if (name is a keyword)
        processError(illegal use of keyword)
      else { // Create table entry
        if (name begins with uppercase)
          symbolTable[name]=(name,inType,inMode,inValue,inAlloc,inUnits)
        else
          symbolTable[name]=(genInternalName(inType),inType,inMode,inValue,inAlloc,inUnits)
      }
    } */
  
}
// Tells which data type a name has
storeTypes whichType(string name) {
 /* if (name is a literal) {
      if (name is a boolean literal)
        data type = BOOLEAN
      else
        data type = INTEGER
    }
    else { // Name is an identifier and hopefully a constant
      if (symbolTable[name] is defined)
        data type = type of symbolTable[name]
      else 
        processError(reference to undefined constant)
    }
    return data type */
  
}
// Tells which value a name has
string whichValue(string name) {
 /* if (name is a literal)
      value = name 
    else { // Name is an identifier and hopefully a constant
      if (symbolTable[name] is defined and has a value)
        value = value of symbolTable[name]
      else
        processError(reference to undefined constant)
    }
    return value */
  
}
void code(string op, string operand1 = "", string operand2 = "") {
 /* if (op == "program")
      emitPrologue(operand1)
    else if (op == "end")
      emitEpilogue()
    else
      processError(compiler error since function code should not be called with illegal arguments) */
  
}

// Emit functions:
void emit(string label = "", string instruction = "", string operands = "",
          string comment = "") {
 /* Turn on left justification in objectFile
    Output label in a field of width 8
    Output instruction in a field of width 8
    Output the operands in a field of width 24
    Output the comment */
  
}
void emitPrologue(string progName, string = "") {
 /* Output identifying comments at beginning of objectFile
    Output the %INCLUDE directives
    emit("SECTION", ".text")
    emit("global", "_start", "", "; program " + progName)
    emit("_start:") */
  
}
void emitEpilogue(string = "", string = "") {
 /* emit("","Exit", "{0}")
    emitStorage(); */
  
}
void emitStorage() {
 /* emit("SECTION", ".data")
    for (those entries in the symbolTable that have 
    an allocation of YES and a storage mode of CONSTANT) {
      call emit to output a line to objectFile
    }
    emit("SECTION", ".bss")
    for (those entries in the symbolTable that have
    an allocation of YES and a storage mode of VARIABLE) {
      call emit to output a line to objectFile 
    } */
  
}

// Lexical routines:
// Returns the next character or END_OF_FILE marker
char nextChar() {
 /* read in next character
    if (end of file) 
      ch = END_OF_FILE // Use a special character to designate end of file
                       // END_OF_FILE is defined in stage0.h
    else {
      ch = next character
      print to listing file (starting new line if necessary)
    }
    return ch */
  
}
// Returns the next token or END_OF_FILE marker
string nextToken() {
 /* token = "";
    while (token == "") {
      switch(ch) {
        case '{' : // Process comment
                      while (nextChar() is not one of END_OF_FILE, '}'}
                      { //empty body }
                      if (ch == END_OF_FILE)
                        processError(unexpected end of file)
                      else
                        nextChar()
        case '}' : processError('}' cannot begin token)
        case isspace(ch) : nextChar()
        case isSpecialSymbol(ch): token = ch
                                  nextChar()
        case islower(ch) : token = ch
                           while (nextChar() is one of letter, digit, or 
                           '_' but not END_OF_FILE) {
                             token+=ch
                           }
                           if (ch is END_OF_FILE)
                             processError(unexpected end of file)
        case isdigit(ch) : token = ch
                           while (nextChar() is digit but not END_OF_FILE) {
                             token+=ch
                           }
                           if (ch is END_OF_FILE)
                             processError(unexpected end of file)
        case END_OF_FILE : token = ch
        default : processError(illegal symbol)
      }
    }
    return token */
  
}

// Other routines:
string genInternalName(storeTypes stype) const {

}
void processError(string err) {
 /* Output err to listingFile
    Call exit(EXIT_FAILURE) to terminate program */
}