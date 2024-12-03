// Erik Zuniga && Roberto Lopez
// CS4301 - stage1

#include <stage1.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <ctime>
using namespace std;

// Constructor
Compiler(char **argv) {
  sourceFile.open(argv[1]);
  listingFile.open(argv[2]);
  objectFile.open(argv[3]);
}
// Destructor
~Compiler() {
  sourceFile.close();
  listingFile.close();
  objectFile.close();
}

void createListingHeader() {
  time_t now = time(0);
  listingFile << "STAGE 1: " << "Erik Zuniga & Roberto Lopez" << ctime(&now) << endl;
  listingFile << "LINE NO." << setw(10) << "SOURCE STATEMENT\n" << endl;
}
void parser() {
  nextChar();
  if(nextToken() != "program") {
    processError("keyword \"program\" expected");
  }
  prog();
}
void createListingTrailer() {
  listingFile << "\nCOMPILATION TERMINATED" << setw(6) << "" << right
    << errorCount << (errorCount == 1 ? " ERROR" : " ERRORS") << " ENCOUNTERED" << endl;
}

// Methods implementing the grammar productions
// Stage 0, Production 1 ..
void prog() {
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
void progStmt() {
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
void consts() {
	if (token != "const")
		processError("keyword \"const\" expected");
	if (!isNonKeyId(nextToken()))
		processError("non-keyword identifier must follow \"const\"");
	constStmts();
}
void vars() {
  if (token != "var")
		processError("keyword \"var\" expected");
	if (!isNonKeyId(nextToken()))
		processError("non-keyword identifier must follow \"var\"");
	varStmts();
}
void beginEndStmt() {
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
void constStmts() {
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
void varStmts() {
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
string ids() {
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

void execStmts() {      // Stage 1, Production 2

}
void execStmt() {       // Stage 1, Production 3

}
void assignStmt() {     // Stage 1, Production 4

}
void readStmt() {       // Stage 1, Production 5

}
void writeStmt() {      // Stage 1, Production 7

}
void express() {        // Stage 1, Production 9

}
void expresses() {      // Stage 1, Production 10

}
void term() {           // Stage 1, Production 11

}
void terms() {          // Stage 1, Production 12

}
void factor() {         // Stage 1, Production 13

}
void factors() {        // Stage 1, Production 15

}
void part() {

}

// Helper functions for the Pascallite lexicon
bool isKeyword(string s) {          // Determines if s is a keyword

}
bool isSpecialSymbol(char c) {      // Determines if c is a special symbol

}
bool isNonKeyId(string s) {         // Determines if s is a non_key_id

}
bool isInteger(string s) {          // Determines if s is an integer

}
bool isBoolean(string s) {          // Determines if s is a boolean

}
bool isLiteral(string s) {          // Determines if s is a literal

}

// Action routines
void insert(string externalName, storeTypes inType, modes inMode,
            string inValue, allocation inAlloc, int inUnits) {

}
storeTypes whichType(string name) { // Tells which data type a name has

}
string whichValue(string name) {    // Tells which value a name has

}
void code(string op, string operand1 = "", string operand2 = "") {
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
void pushOperator(string op) {

}
string popOperator() {

}
void pushOperand(string operand) {

}
string popOperand() {

}

// Emit Functions
void emit(string label = "", string instruction = "", string operands = "",
          string comment = "") {

}
void emitPrologue(string progName, string = "") {

}
void emitEpilogue(string = "", string = "") {

}
void emitStorage() {

}
void emitReadCode(string operand, string = "") {

}
void emitWriteCode(string operand, string = "") {

}
void emitAssignCode(string operand1, string operand2) {               // op2 = op1

}
void emitAdditionCode(string operand1, string operand2) {             // op2 +  op1

}
void emitSubtractionCode(string operand1, string operand2) {          // op2 -  op1

}
void emitMultiplicationCode(string operand1, string operand2) {       // op2 *  op1

}
void emitDivisionCode(string operand1, string operand2) {             // op2 /  op1

}
void emitModuloCode(string operand1, string operand2) {               // op2 %  op1

}
void emitNegationCode(string operand1, string = "") {                 // -op1

}
void emitNotCode(string operand1, string = "") {                      // !op1

}
void emitAndCode(string operand1, string operand2) {                  // op2 && op1

}
void emitOrCode(string operand1, string operand2) {                   // op2 || op1

}
void emitEqualityCode(string operand1, string operand2) {             // op2 == op1

}
void emitInequalityCode(string operand1, string operand2) {           // op2 != op1

}
void emitLessThanCode(string operand1, string operand2) {             // op2 <  op1

}
void emitLessThanOrEqualToCode(string operand1, string operand2) {    // op2 <= op1

}
void emitGreaterThanCode(string operand1, string operand2) {          // op2 >  op1

}
void emitGreaterThanOrEqualToCode(string operand1, string operand2) { // op2 >= op1

}

// Lexical routines
char nextChar() {       // Returns the next character or END_OF_FILE marker

}
string nextToken() {    // Returns the next token or END_OF_FILE marker

}

// Other routines
string genInternalName(storeTypes stype) {
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
void processError(string err) {
  listingFile << "\nError: Line " << lineNo << ": " << err << endl;
  ++errorCount;
  createListingTrailer();

  listingFile.close();
  objectFile.close();
  exit(EXIT_FAILURE);
}
void freeTemp() {
  --currentTempNo;
  if(currentTempNo < -1) {
    processError("compiler error: currentTempNo should be greater than or equal to -1");
  }
}
string getTemp() {
  ++currentTempNo;
  // Initialize a new temporary string
  string temp;
  temp = "T" + to_string(currentTempNo);
  if(currentTempNo > maxTempNo) {
    // Insert inside the symbol table and assign the internal name
    insert(temp, UNKNOWN, VARIABLE, "1", NO, 1);
    symbolTable[temp].setInternalName(temp);
    ++maxTempNo;
  }
  return temp;
}
string getLabel() {
  string internalName;
  // Number of labels
  static int numsL = 0;
  internalName = "L" + to_string(numsL);
  ++L;
  return internalName;
}
bool isTemporary(string s) {        // Determines if s represents a temporary
  return (s[0] == 'T');
}
