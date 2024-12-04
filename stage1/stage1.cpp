// Erik Zuniga && Roberto Lopez
// CS4301 - stage1

#include <stage1.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <ctime>
using namespace std;

 /* 'not' '-'(unary) '+'(unary)     ↓
    '*' 'div' 'mod' 'and'           Decreasing
    '+'(binary) '-'(binary) 'or'    order of
    '=' '<' '>' '<=' '>=' '<>'      Precedence
    ':='                            ↓          */

// Constructor
Compiler::Compiler(char **argv) {
  sourceFile.open(argv[1]);
  listingFile.open(argv[2]);
  objectFile.open(argv[3]);
}
// Destructor
Compiler::~Compiler() {
  sourceFile.close();
  listingFile.close();
  objectFile.close();
}

void Compiler::createListingHeader() {
  time_t now = time(0);
  listingFile << "STAGE 1: " << "Erik Zuniga & Roberto Lopez" << ctime(&now) << endl;
  listingFile << "LINE NO." << setw(10) << "SOURCE STATEMENT\n" << endl;
}
void Compiler::parser() {
  nextChar();
  if(nextToken() != "program") {
    processError("keyword \"program\" expected");
  }
  prog();
}
void Compiler::createListingTrailer() {
  listingFile << "\nCOMPILATION TERMINATED" << setw(6) << "" << right
    << errorCount << (errorCount == 1 ? " ERROR" : " ERRORS") << " ENCOUNTERED" << endl;
}

// Methods implementing the grammar productions
// Stage 0, Production 1 ..
void Compiler::prog() {
	if (token != "program")
		processError("keyword \"program\" expected");
	progStmt();
	if (token == "const")
		consts();
	if (token == "var")
		vars();
	if (token != "begin")
		processError("keyword\"begin\" expected");
	beginEndStmt();
	if (token[0] != END_OF_FILE)
		processError("no text may follow \"end\"");
}
void Compiler::progStmt() {
	string x;
	if (token != "program")
		processError("keyword \"program\" expected");
	x = nextToken(); 
	if (!isNonKeyId(token))
		processError("program name expected");
	if (nextToken() != ";")
		processError("semicolon expected");
	nextToken();
	code("program", x);
	insert(x, PROG_NAME, CONSTANT, x, NO, 0);
}
void Compiler::consts() {
	if (token != "const")
		processError("keyword \"const\" expected");
	if (!isNonKeyId(nextToken()))
		processError("non-keyword identifier must follow \"const\"");
	constStmts();
}
void Compiler::vars() {
  if (token != "var")
		processError("keyword \"var\" expected");
	if (!isNonKeyId(nextToken()))
		processError("non-keyword identifier must follow \"var\"");
	varStmts();
}
void Compiler::beginEndStmt() {
  if (token != "begin")
    processError("keyword \"begin\" expected");
  // Call next token to validate it
  nextToken();
  if (isNonKeyId(token) || token == "begin" || token == "read" || token == "write" || token == ";")
    execStmts();
  if (nextToken() != "end")
    processError("keyword \"end\" expected");
  if (nextToken() != ".")
    processError("period expected");
  nextToken();
  code("end", ".");
}
void Compiler::constStmts() {
  string x,y;
  if(!isNonKeyId(token))
	  processError("non-keyword identifier expected");
  
  x = token;
  if (nextToken() != "=")
	  processError("\"=\" expected");
  y = nextToken();
  
  if(y != "+" && y != "-"  && y != "not" && !isNonKeyId(y) && y != "true" && y != "false" && !isInteger(y))
	  processError("token to right of \"=\" illegal");
  if (y == "+" || y == "-") {
	  if(!isInteger(nextToken()))
		  processError("integer expected after sign");
	  y = y + token;
  }
  
  if (y == "not") {
	  if(!(isBoolean(nextToken())))
	    processError("boolean expected after \"not\"");
	  if (token == "true")
	  	y = "false";
	  else
	  	y = "true";
  } 
  
  if (nextToken() != ";")
	  processError("semicolon expected");
  
  storeTypes type = whichType(y);
  if (type != INTEGER && type != BOOLEAN)
	  processError("data type of token on the right-hand side must be INTEGER or BOOLEAN");
  insert(x, whichType(y), CONSTANT, whichValue(y), YES, 1);
  x = nextToken();
  if (x != "begin" && x != "var" && !isNonKeyId(x))
	  processError("non-keyword identifier, \"begin\", or \"var\" expected");
  if (isNonKeyId(x))
	  constStmts();
}
void Compiler::varStmts() {
  string x, y;
  if (!isNonKeyId(token))
	  processError("non-keyword identifier expected");
  x = ids();
  if (token != ":")
	  processError("\":\" expected");
  
  nextToken();
  // Check that the token is integer or boolean
  if(token != "integer" && token != "boolean" )
	  processError("illegal type follows \":\"");
  y = token;
  if (nextToken() != ";")
	  processError("semicolon expected");

  // Convert string types to storeType enum
  storeTypes myType;
  if (y == "integer")
  	myType = INTEGER;
  else
  	myType = BOOLEAN;
  insert(x, myType, VARIABLE, "", YES, 1);

	nextToken();
  if (token != "begin" && !isNonKeyId(token))
	  processError("non-keyword identifier or \"begin\" expected");
  if (isNonKeyId(token))
	  varStmts();
}
// .. Stage 0, Production 8
string Compiler::ids() {
  string temp,tempString;
  if (!isNonKeyId(token))
	  processError("non-keyword identifier expected");
  tempString = token;
  temp = token;
  if (nextToken() == ",") {
	  if (!isNonKeyId(nextToken()))
	  	processError("non-keyword identifier expected");
    tempString = temp + "," + ids();
  }
  return tempString;
}

// Stage 1 Productions
void Compiler::execStmts() {      // Stage 1, Production 2
  // -> EXEC_STMT | EXEC_STMTS
  // -> ε
  if(isNonKeyId(token) || token == "read" || token == "write" || token == ";") {
    // The token will be at the end of the last execStmt, so
    execStmt();
    // advance token and
    nextToken();
    // then recurse
    execStmts();
  }
  else if(token == "end") { /*Do nothing*/ }
  else
    processError("\";\", \"begin\", \"read\", \"write\", \"end\" expected");
}

void Compiler::execStmt() {       // Stage 1, Production 3
  // If the token is a non-keyword identifier, then it is an assignment statement
  if(isNonKeyId(token))
    assignStmt();
  // If the token is "read", then it is a read statement
  else if(token == "read")
    readStmt();
  // If the token is "write", then it is a write statement
  else if(token == "write")
    writeStmt();
  else
    processError("non-keyword identifier, \"read\", or \"write\" expected");
}

void Compiler::assignStmt() {     // Stage 1, Production 4
  // -> NON_KEY_ID ':=' EXPRESS ';'
  // Token must be a non-keyword identifier and defined
  if(!isNonKeyId(token))
    processError("non-keyword identifier expected");
  if(symbolTable.count(token) == 0)
    processError("reference to undefined constant");
  
  string op1, op2;
  pushOperand(token);
  nextToken();

  // Token must be ":="
  if(token != ":=")
    processError("\":=\" expected; found " + token);
  // Push the operator onto the operator stack
  else
    pushOperator(token);
  nextToken();

  // Validate that token is an expression
  if(!isBoolean(token) && !isInteger(token) && !isNonKeyId(token)
    && token != "(" && token != "+" && token != '-' && token != "not" && token != ";")
    processError(one of \"*\", \"and\", \"div\", \"mod\", \")\", \"+\", \"-\", \";\", \"<\", \"<=\", \"<>\", \"=\", \">\", \">=\", or \"or\" expected")
  // Call the express() production
  else
    express();

  // Pop the operator and operands from the stacks
  op2 = popOperand();
  op1 = popOperand();
  code(popOperator(), op2, op1);
}

void Compiler::readStmt() {       // Stage 1, Production 5
  // Read list
  string list;
  // List item
  string listItem;
  // and the counter
  uint i;

  // Double check for a "read" token
  if (token != "read")
    processError("keyword \"read\" expected");
  nextToken();

  // Make sure the next token is a "("
  if (token != "(")
    processError("\"(\" expected");
  // otherwise it's a ")"
  else {
    nextToken();
    // Grab non-key-ids to advance token
    list = ids();

    // Loop through the characters of the list
    for (i = 0; i < list.length(); ++i) {
      if (list[ i ] == ',') {
        // If we have a ',' then code the current list item
        code("read", listItem);
        // Reset list item for next characters
        listItem = "";
      }
      // If we don't have a ',' then add characters to the list item
      else
        listItem += list[i];
    }

    // Code the current listItem
    code("read", listItem);

    // Look for a ")" prior to ";"
    if (token != ")")
      processError("\",\" or \")\" expected after non-keyword identifier");

    // Next token must be ";" to end the read statement
    nextToken();
    if (token != ";")
      processError("\";\" expected");
  }
}

void Compiler::writeStmt() {      // Stage 1, Production 7

}

void Compiler::express() {        // Stage 1, Production 9

}

void Compiler::expresses() {      // Stage 1, Production 10

}

void Compiler::term() {           // Stage 1, Production 11

}

void Compiler::terms() {          // Stage 1, Production 12

}

void Compiler::factor() {         // Stage 1, Production 13

}

void Compiler::factors() {        // Stage 1, Production 15

}

void Compiler::part() {

}

// Helper functions for the Pascallite lexicon
bool Compiler::isKeyword(string s) const {          // Determines if s is a keyword
  return (s == "program" || s == "var" || s == "begin" || s == "end"
    || s == "true" || s == "false" || s == "not" || s == "const"
    || s == "integer" || s == "boolean" || s == "read" || s == "write"
    || s == "and" || s == "or" || s == "div" || s == "mod");
}
bool Compiler::isSpecialSymbol(char c) const {      // Determines if c is a special symbol
  return (c == ':' || c == ',' || c == ';'|| c == '=' || c == '+'
    || c == '-' || c == '.' || c == '*' || c == '(' || c == ')'
    || c == '>' || c == '<');
}
bool Compiler::isNonKeyId(string s) const {         // Determines if s is a non_key_id
	if (isKeyword(s)) return false;
	if (!isalpha(s[0]) || !islower(s[0])) return false;
	if (isdigit(s[0]) || s[0] == '_' ) return false;
	
	for (size_t i = 1; i < s.length(); ++i) {
    // '_' is the only allowed special character
		if (isSpecialSymbol(s[i]) && s[i] != '_') return false;
		// All lowercase
		if (isalpha(s[i]) && !islower(s[i])) return false;
    // '_' cannot proceed '_'
		if (s[i] == '_' && s[i-1] == '_') return false;
	}
  // '_' cannot be the last character
	if (s[s.length() - 1] == '_') return false;
	return true;
}
bool Compiler::isInteger(string s) const {          // Determines if s is an integer
	// Iterate through the token and check if each character is an integer
  for (uint i = 0; i < s.length(); i++) {
	  if (s[i] == ';') break;
	  if (!isdigit(s[i])) return false;
	}
	return true;   
}
bool Compiler::isBoolean(string s) const {          // Determines if s is a boolean
  return (s == "true" || s == "false");
}
bool Compiler::isLiteral(string s) const {          // Determines if s is a literal
  if (isInteger(s) || isBoolean(s)) return true;
  // If the variable starts with a +,- then check that the rest is an integer
  if (s[0] == '+' || s[0] == '-') {
	  for (uint i = 1; i < s.length(); i++) {
		  if (!isdigit(s[i])) return false;
	  }
	  return true;
  }
  return false;
}

// Action routines
void Compiler::insert(string externalName, storeTypes inType, modes inMode,
            string inValue, allocation inAlloc, int inUnits) {

}
storeTypes Compiler::whichType(string name) { // Tells which data type a name has

}
string Compiler::whichValue(string name) {    // Tells which value a name has

}
void Compiler::code(string op, string operand1, string operand2) {
  if(op == "program") {
    emitPrologue(operand1);
  } else if(op == "end") {
    emitEpilogue();
  } else if(op == "read") {
    emitReadCode(operand1);
  } else if(op == "write") {
    emitWriteCode(operand1);
  } else if(op == ":=") {
    emitAssignCode(operand1, operand2);
  } else if(op == "+") {
    emitAdditionCode(operand1, operand2);
  } else if(op == "-") {
    emitSubtractionCode(operand1, operand2);
  } else if(op == "*") {
    emitMultiplicationCode(operand1, operand2);
  } else if(op == "div") {
    emitDivisionCode(operand1, operand2);
  } else if(op == "mod") {
    emitModuloCode(operand1, operand2);
  } else if(op == "neg") {
    emitNegationCode(operand1);
  } else if(op == "not") {
    emitNotCode(operand1);
  } else if(op == "and") {
    emitAndCode(operand1, operand2);
  } else if(op == "or") {
    emitOrCode(operand1, operand2);
  } else if(op == "=") {
    emitEqualityCode(operand1, operand2);
  } else if(op == "<>") {
    emitInequalityCode(operand1, operand2);
  } else if(op == "<") {
    emitLessThanCode(operand1, operand2);
  } else if(op == "<=") {
    emitLessThanOrEqualToCode(operand1, operand2);
  } else if(op == ">") {
    emitGreaterThanCode(operand1, operand2);
  } else if(op == ">=") {
    emitGreaterThanOrEqualToCode(operand1, operand2);
  } else {
    processError("compiler error since function code should not be called with illegal arguments");
  }
}

// Push/Pop helper functions
void Compiler::pushOperator(string op) {
  operatorStk.push(op);
}
string Compiler::popOperator() {
  if(!operatorStk.empty()) {
    string op = operatorStk.top();
    operatorStk.pop();
  } else { processError("operator stack underflow"); }
  return op;
}
void Compiler::pushOperand(string op) {
  // Push name onto operandStk
  // If name is a literal, also create a symbol table entry for it
  if(symbolTable.count(op) == 0 && (isInteger(op) || isBoolean(op)))
    insert(op, whichType(op), CONSTANT, whichValue(op), YES, 1);
  operandStk.push(op);
}
string Compiler::popOperand() {
  if(!operandStk.empty()) {
    string op = operandStk.top();
    operandStk.pop();
  } else { processError("operand stack underflow"); }
  return op;
}

// Emit Functions
void Compiler::emit(string label, string instruction, string operands,
          string comment) {

}
void Compiler::emitPrologue(string progName, string operand2) {

}
void Compiler::emitEpilogue(string operand1, string operand2) {

}
void Compiler::emitStorage() {

}
void Compiler::emitReadCode(string operand, string operand2) {

}
void Compiler::emitWriteCode(string operand, string operand2) {

}
void Compiler::emitAssignCode(string operand1, string operand2) {               // op2 = op1

}
void Compiler::emitAdditionCode(string operand1, string operand2) {             // op2 +  op1

}
void Compiler::emitSubtractionCode(string operand1, string operand2) {          // op2 -  op1

}
void Compiler::emitMultiplicationCode(string operand1, string operand2) {       // op2 *  op1

}
void Compiler::emitDivisionCode(string operand1, string operand2) {             // op2 /  op1

}
void Compiler::emitModuloCode(string operand1, string operand2) {               // op2 %  op1

}
void Compiler::emitNegationCode(string operand1, string operand2) {             // -op1

}
void Compiler::emitNotCode(string operand1, string operand2) {                  // !op1

}
void Compiler::emitAndCode(string operand1, string operand2) {                  // op2 && op1

}
void Compiler::emitOrCode(string operand1, string operand2) {                   // op2 || op1

}
void Compiler::emitEqualityCode(string operand1, string operand2) {             // op2 == op1

}
void Compiler::emitInequalityCode(string operand1, string operand2) {           // op2 != op1

}
void Compiler::emitLessThanCode(string operand1, string operand2) {             // op2 <  op1

}
void Compiler::emitLessThanOrEqualToCode(string operand1, string operand2) {    // op2 <= op1

}
void Compiler::emitGreaterThanCode(string operand1, string operand2) {          // op2 >  op1

}
void Compiler::emitGreaterThanOrEqualToCode(string operand1, string operand2) { // op2 >= op1

}

// Lexical routines
char Compiler::nextChar() {       // Returns the next character or END_OF_FILE marker

}
string Compiler::nextToken() {    // Returns the next token or END_OF_FILE marker

}

// Other routines
string Compiler::genInternalName(storeTypes stype) const {
  string name;
	static int numsI = 0, numsB = 0, numsU = 0; 
	switch(stype) {
		case PROG_NAME: {
			name = "P0";
			break;
		}
		case INTEGER: {
			name = "I" + to_string(numsI);
			++numsI;
			break;
		}
		case BOOLEAN: {
			name = "B" + to_string(numsB);
			++numsB;
			break;
		}
    case UNKNOWN: {
      name = "U" + to_string(numsU);
      ++numsU;
      break;
    }
	}
	return name;
}
void Compiler::processError(string err) {
  listingFile << "\nError: Line " << lineNo << ": " << err << endl;
  ++errorCount;
  createListingTrailer();

  listingFile.close();
  objectFile.close();
  exit(EXIT_FAILURE);
}
void Compiler::freeTemp() {
  --currentTempNo;
  if(currentTempNo < -1)
    processError("compiler error: currentTempNo should be greater than or equal to -1");
}
string Compiler::getTemp() {
  ++currentTempNo;
  // Initialize a new temporary string
  string temp;
  temp = "T" + to_string(currentTempNo);
  if(currentTempNo > maxTempNo) {
    // Insert inside the symbol table and assign the internal name
    insert(temp, UNKNOWN, VARIABLE, "", NO, 1);
    ++maxTempNo;
  }
  return temp;
}
string Compiler::getLabel() {
  string internalName;
  // Number of labels
  static int numsL = 0;
  internalName = "L" + to_string(numsL);
  ++numsL;
  return internalName;
}
bool Compiler::isTemporary(string s) const {    // Determines if s represents a temporary
  return (s[0] == 'T');
}
