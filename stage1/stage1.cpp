// Erik Zuniga && Roberto Lopez
// CS4301 - stage1

#include <stage1.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <ctime>
#include <cctype>

using namespace std;

/*
  - 'not' '-'(unary) '+'(unary)     ↓
  - '*' 'div' 'mod' 'and'           Decreasing
  - '+'(binary) '-'(binary) 'or'    order of
  - '=' '<' '>' '<=' '>=' '<>'      Precedence
  - ':='                            ↓
*/

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
    && token != "(" && token != "+" && token != "-" && token != "not" && token != ";")
    processError("one of \"*\", \"and\", \"div\", \"mod\", \")\", \"+\", \"-\", \";\", \"<\", \"<=\", \"<>\", \"=\", \">\", \">=\", or \"or\" expected");
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
  nextToken();
  // Grab non-key-ids and advance token
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

void Compiler::writeStmt() {      // Stage 1, Production 7
  // Read list
  string list;
  // List item
  string listItem;
  // and the counter
  uint i;

  // Double-check for "write" token
  if (token != "write")
    processError("keyword \"write\" expected");
  nextToken();

  // Make sure the next token is a "("
  if (token != "(")
    processError("\"(\" expected");
  nextToken();
  // Grab non-key-ids and advance token
  list = ids();

  // Loop through the characters of the list
  for (i = 0; i < list.length(); ++i) {
    if (list[i] == ',') {
      // If we have a ',' then code current list item
      code("write", listItem);
      // and reset list item for next characters
      listItem = "";
    }
    // If we don't have a ',' then add characters to the list item
    else
      listItem += list[i];
  }

  // Code the current list item
  code("write", listItem);
  // Look for a ")" prior to ";"
  if (token != ")")
    processError("',' or ')' expected after non-keyword identifier");

  // Advance token and check for ";" to end write statement
  nextToken();
  if (token != ";")
    processError("';' expected");
}

void Compiler::express() {        // Stage 1, Production 9
  if (!isInteger(token) && !isNonKeyId(token) && !isBoolean(token) && token != "(" 
    && token != "not" && token != "+" && token != "-")
    processError("\"not\", \"true\", \"false\", \"(\", \"+\", \"-\", non-keyword identifier, or integer expected");
  term();
  if (token == "=" || token == "<" || token == ">" 
    || token == ">=" || token == "<=" || token == "<>")
    expresses();
}

void Compiler::expresses() {      // Stage 1, Production 10

  if (token == "<>" || token == "=" || token == "<=" || 
        token == ">=" || token == "<" || token == ">") {

        //match TERM
        term(); //parse the TERM 

        //recursive call to EXPRESSES
        expresses();
    }
    else if (token == ")" || token == ";") {
        //do nothing (ε production)
        return;
    }
    else {
        //error handling for unexpected tokens
        processError("\")\" or \";\" expected after expression");
    }


}

void Compiler::term() {           // Stage 1, Production 11

  if (isInteger(token) || isNonKeyId(token) || isBoolean(token) || 
        token == "(" || token == "not" || token == "+" || token == "-") {
        //match factor
        factor(); //parse the FACTOR

        //mtch TERMS
        terms(); //parse the continuation (TERMS)
    } else {
        //error handling for unexpected tokens
        processError("\"not\", \"true\", \"false\", \"(\", \"+\", \"-\", non-keyword identifier, or integer expected");
    }
}

void Compiler::terms() {          // Stage 1, Production 12

  if (token == "+" || token == "-" || token == "or") {

        //match factor
        factor(); //parse the FACTOR

        //recursive call to TERMS
        terms();
    }
    else if (token == "<>" || token == "=" || token == "<=" || token == ">=" || 
             token == "<" || token == ">" || token == ")" || token == ";") {
        //do nothing (ε production)
        return;
    }
    else {
        //error handling for unexpected tokens
        processError("\"+\", \"-\", \"or\", \"<>\", \"=\", \"<=\", \">=\", \"<\", \">\", \")\", or \";\" expected");
    }

}

void Compiler::factor() {         // Stage 1, Production 13

  if (isInteger(token) || isNonKeyId(token) || isBoolean(token) || 
        token == "(" || token == "not" || token == "+" || token == "-") {
        //match PART
        part(); //parse the PART

        //match FACTORS
        factors(); //parse the continuation (FACTORS)
    } else {
        //error handling for unexpected tokens
        processError("\"not\", \"true\", \"false\", \"(\", \"+\", \"-\", non-keyword identifier, or integer expected");
    }

}

void Compiler::factors() {        // Stage 1, Production 15


   if (token == "*" || token == "div" || token == "mod" || token == "and") {

        //match PART
        part(); // Parse the PART after the operator

        //recursive call to FACTORS
        factors();
    }
    else if (token == "<>" || token == "=" || token == "<=" || token == ">=" || 
             token == "<" || token == ">" || token == ")" || token == ";" || 
             token == "-" || token == "+" || token == "or") {
        //do nothing (ε production)
        return;
    }
    else {
        //error handling for unexpected tokens
        processError("\"*\", \"div\", \"mod\", \"and\", \"<>\", \"=\", \"<=\", \">=\", \"<\", \">\", \")\", \";\", \"-\", \"+\", or \"or\" expected");
    }
}

void Compiler::part() {

  if (token == "not") {
        //match 'not'
        nextToken(); //consume 'not'
        part();      //recursively call part()
    } 
    else if (token == "+" || token == "-") {

        //match '+' or '-'
        nextToken(); //consume '+' or '-'

        //match '(' EXPRESS ')' | INTEGER | NON_KEY_ID
        if (token == "(") {
            nextToken(); //consume '('
            express();   //parse EXPRESS
            if (token == ")") {
                nextToken(); //consume ')'
            } else {
                processError("\")\" expected after expression");
            }
        } else if (isInteger(token) || isNonKeyId(token)) {
            nextToken(); //xonsume INTEGER or NON_KEY_ID
        } else {
            processError("Integer, non-keyword identifier, or \"(\" expected after '+' or '-'");
        }
    } 
    else if (token == "(") {
        //match '(' EXPRESS ')'
        nextToken(); //consume '('
        express();   //oarse EXPRESS
        if (token == ")") {
            nextToken(); // Consume ')'
        } else {
            processError("\")\" expected after expression");
        }
    } 
    else if (isBoolean(token) || isNonKeyId(token)) {
        //match BOOLEAN or NON_KEY_ID
        nextToken(); //consume BOOLEAN or NON_KEY_ID
    } 
    else {
        //error handling for unexpected tokens
        processError("\"not\", \"+\", \"-\", \"(\", BOOLEAN, or non-keyword identifier expected");
    }

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
  string name;
  string::iterator itr = externalName.begin();

  while(itr < externalName.end()) {
    name = "";
    while (itr < externalName.end() && *itr != ',' ) {
      name = name + *itr;
      ++itr;
    }

    if (!name.empty()) {
      if (symbolTable.count(name) > 0)
        processError("multiple " + name + " name definition");
      else if (isKeyword(name))
        processError("illegal use of keyword");
      else { // Create a symbol table entry 
        if (isupper(name[0])) { // If it's internal name
          symbolTable.insert(pair<string, SymbolTableEntry>(name.substr(0, 15), // key
          SymbolTableEntry(name, inType, inMode, inValue, inAlloc, inUnits))); // value
        }
        else { // It's an external name, need to create an internal name
          symbolTable.insert(pair<string, SymbolTableEntry>(name.substr(0, 15),
          SymbolTableEntry(genInternalName(inType), inType, inMode, inValue, inAlloc, inUnits)));
        }
        if (symbolTable.size() > 256)
          processError("Cannot hold over 256 entries");
      }
    }
    ++itr;
  }
}

storeTypes Compiler::whichType(string name) { // Tells which data type a name has
	storeTypes type;
  // Initially check if the name is a literal
	if (isLiteral(name)) {
    // If it is a proper literal, then check if it's a boolean,
		if (isBoolean(name))
			type = BOOLEAN;
    // otherwise it is an integer
		else
			type = INTEGER;
	} else { // Name is an identifier and hopefully a constant
    // If name is in the symbolTable, then grab its data-type
	  if(symbolTable.count(name) > 0)
	  	type = symbolTable.find(name)->second.getDataType();
    // Otherwise, throw a complaint
	  else {
	  	if(isNonKeyId(name))
	  		processError("reference to undefined constant");
	  	else
	  		processError("non-keyword identifier or literal expected");
	  }
	}
	return type;
}

string Compiler::whichValue(string name) {    // Tells which value a name has
	string value;
	if(isLiteral(name))
		value = name;
	else { // Name is an identifier and hopefully a constant
		if(symbolTable.find(name) != symbolTable.end())
			value = symbolTable.at(name).getValue();
		else
			processError("reference to undefined constant");
	}
	return value;
}

void Compiler::code(string op, string operand1, string operand2) {
  // Stage 0 Functions
  if(op == "program") {
    emitPrologue(operand1);
  } else if(op == "end") {
    emitEpilogue();
  // Stage 1 Functions
  } else if(op == "read") {
    emitReadCode(operand1, operand2);
  } else if(op == "write") {
    emitWriteCode(operand1, operand2);
  } else if(op == ":=") {
    emitAssignCode(operand1, operand2);
  } else if(op == "+") {
    if (operand1 != "" && operand2 != "")
      emitAdditionCode(operand1, operand2);
    else
      processError("compiler error since function code should not be called with illegal arguments");
  } else if(op == "-") {
    if (operand1 != "" && operand2 != "")
      emitSubtractionCode(operand1, operand2);
    else
      processError("compiler error since function code should not be called with illegal arguments");
  } else if(op == "*") {
    if (operand1 != "" && operand2 != "")
      emitMultiplicationCode(operand1, operand2);
    else
      processError("compiler error since function code should not be called with illegal arguments");
  } else if(op == "div") {
    if (operand1 != "" && operand2 != "")
      emitDivisionCode(operand1, operand2);
    else
      processError("compiler error since function code should not be called with illegal arguments");
  } else if(op == "mod") {
    if (operand1 != "" && operand2 != "")
      emitModuloCode(operand1, operand2);
    else
      processError("compiler error since function code should not be called with illegal arguments");
  } else if(op == "neg") {
    if (operand1 != "" && operand2 == "")
      emitNegationCode(operand1);
    else if (operand1 == "" && operand2 != "")
      emitNegationCode(operand2);
    else
      processError("compiler error since function code should not be called with illegal arguments");
  } else if(op == "not") {
    if (operand1 != "" && operand2 == "")
      emitNotCode(operand1);
    else if (operand1 == "" && operand2 != "")
      emitNotCode(operand2);
    else
      processError("compiler error since function code should not be called with illegal arguments");
  } else if(op == "and") {
    if (operand1 != "" && operand2 != "")
      emitAndCode(operand1, operand2);
    else
      processError("compiler error since function code should not be called with illegal arguments");
  } else if(op == "or") {
    if (operand1 != "" && operand2 != "")
      emitOrCode(operand1, operand2);
    else
      processError("compiler error since function code should not be called with illegal arguments");
  } else if(op == "=") {
    if (operand1 != "" && operand2 != "")
      emitEqualityCode(operand1, operand2);
    else
      processError("compiler error since function code should not be called with illegal arguments");
  } else if(op == "<>") {
    emitInequalityCode(operand1, operand2);
  } else if(op == "<") {
    if (operand1 != "" && operand2 != "")
      emitLessThanCode(operand1, operand2);
    else
      processError("compiler error since function code should not be called with illegal arguments");
  } else if(op == "<=") {
    if (operand1 != "" && operand2 != "")
      emitLessThanOrEqualToCode(operand1, operand2);
    else
      processError("compiler error since function code should not be called with illegal arguments");
  } else if(op == ">") {
    if (operand1 != "" && operand2 != "")
      emitGreaterThanCode(operand1, operand2);
    else
      processError("compiler error since function code should not be called with illegal arguments");
  } else if(op == ">=") {
    if (operand1 != "" && operand2 != "")
      emitGreaterThanOrEqualToCode(operand1, operand2);
    else
      processError("compiler error since function code should not be called with illegal arguments");
  } else {
    processError("compiler error since function code should not be called with illegal arguments");
  }
}

// Push/Pop helper functions
void Compiler::pushOperator(string op) {
  operatorStk.push(op);
}

string Compiler::popOperator() {
  string op;
  if(!operatorStk.empty()) {
    op = operatorStk.top();
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
  string op;
  if(!operandStk.empty()) {
    op = operandStk.top();
    operandStk.pop();
  } else { processError("operand stack underflow"); }
  return op;
}

// Emit Functions
void Compiler::emit(string label, string instruction, string operands,
          string comment) {
  objectFile.setf(ios_base::left);      // Set left justification
  objectFile << left << setw(8) << label;
  objectFile << left << setw(8) << instruction;
  objectFile << left << setw(24) << operands;
  objectFile << comment << endl;
}

void Compiler::emitPrologue(string progName, string operand2) {
	time_t current = time(0);
	char* time = ctime(&current);
	objectFile << "; Erik Zuniga and Roberto Lopez       " << right << setw(6) << time; 
	objectFile << "%INCLUDE \"Along32.inc\"\n" << "%INCLUDE \"Macros_Along.inc\"\n" << endl;
	emit("SECTION", ".text");
	emit("global", "_start", "", "; program " + progName);
	objectFile << endl;
	emit("_start:"); 
}

void Compiler::emitEpilogue(string operand1, string operand2) {
  emit("", "Exit", "{0}");
  objectFile << endl;
  emitStorage();
}

void Compiler::emitStorage() {
  // Create a new iterator for the SymbolTable
  map<string, SymbolTableEntry>::iterator iter = symbolTable.begin();

  // Output the data section
  emit("SECTION", ".data")
  for(iter = symbolTable.begin(); iter != symbolTable.end(); ++iter) {
    if((iter->second.getAlloc() == YES) && (iter->second.getMode() == CONSTANT)) {
      string value = iter->second.getValue();
      if(iter->second.getValue() == "false")
        value = "0";
      else if(iter->second.getValue() == "true")
        value = "-1";
      emit(iter->second.getInternalName(), "dd", value, "; " + iter->first);
    }
  }

  // New line on objectFile
  objectFile << "\n";

  // Output the bss section
  emit("SECTION", ".bss");
  for(iter = symbolTable.begin(); iter != symbolTable.end(); ++iter) {                                                              
    if((iter->second.getAlloc() == YES) && (iter->second.getMode() == VARIABLE))
      emit(iter->second.getInternalName(), "resd", "1", "; " + iter->first);  
  }
}

void Compiler::emitReadCode(string operand, string operand2) {
  string name;
  string::iterator iter = operand.begin();   

  while(iter < operand.end()) {
    name = "";      
    while(iter < operand.end() && *iter != ',') {
      name = name + *iter;
      ++iter;
    }

    if(name != "") {
      if(symbolTable.count(name) == 0)  // "name" must be defined
        processError("reference to undefined symbol ");
      if(symbolTable.at(name).getDataType() != INTEGER) // "name" must be integer
        processError("can't read variables of this type"); 
      if(symbolTable.at(name).getMode() != VARIABLE) // "name" cannot be a variable
        processError("attempting to read to a read-only location");
      emit("", "call", "ReadInt", "; read int; value placed in eax");
      emit("", "mov", "[" + symbolTable.at(name).getInternalName() + "],eax", "; store eax at " + name);
      contentsOfAReg = symbolTable.at(name).getInternalName();
    }
    ++iter;
  }
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

  if (symbolTable.count(operand1) == 0)
	{
		processError("reference to undefined symbol " + operand1);
	}
	
	else if (symbolTable.count(operand2) == 0)
	{
		processError("reference to undefined symbol " + operand2);
	}

	if (symbolTable.at(operand1).getDataType() != symbolTable.at(operand2).getDataType())
	{
		processError("incompatible types");
	}

	if (isTemporary(contentsOfAReg) && contentsOfAReg != symbolTable.at(operand1).getInternalName() && contentsOfAReg != symbolTable.at(operand2).getInternalName())
	{
		emit("","mov", "[" + contentsOfAReg + "],eax", "; deassign AReg");
		symbolTable.at(contentsOfAReg).setAlloc(YES);
		contentsOfAReg = "";
	}
	
	if (!isTemporary(contentsOfAReg) && contentsOfAReg != symbolTable.at(operand1).getInternalName() && contentsOfAReg != symbolTable.at(operand2).getInternalName())
	{
		contentsOfAReg = "";
	}

	if (contentsOfAReg != symbolTable.at(operand1).getInternalName() && contentsOfAReg != symbolTable.at(operand2).getInternalName())
	{
		emit("","mov", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; AReg = " + operand2);
		contentsOfAReg = symbolTable.at(operand2).getInternalName();
	}

	if (contentsOfAReg == symbolTable.at(operand2).getInternalName())
	{
		emit("","cmp", "eax,[" + symbolTable.at(operand1).getInternalName() + "]", "; compare " + operand2 + " and " + operand1);
	}
	
	else if (contentsOfAReg == symbolTable.at(operand1).getInternalName())
	{
		emit("", "mov", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; AReg = " + operand2);
		emit("","cmp", "eax,[" + symbolTable.at(operand1).getInternalName() + "]", "; compare " + operand2 + " and " + operand1);
	}

	string newLabel = getLabel();

	if (contentsOfAReg == symbolTable.at(operand2).getInternalName())
	{
		emit("","jl", "." + newLabel, "; if " + operand2 + " < " + operand1 + " then jump to set eax to TRUE");
	}
	
	else if (contentsOfAReg == symbolTable.at(operand1).getInternalName())
	{
		emit("","jl", "." + newLabel, "; if " + operand2 + " < " + operand1 + " then jump to set eax to TRUE");
	}

	emit("", "mov", "eax,[FALSE]", "; else set eax to FALSE");

	if (symbolTable.count("false") == 0)
	{
		insert("false", BOOLEAN, CONSTANT, "0", YES, 1);
	}

	string secondLabel = getLabel();
	emit("","jmp","." + secondLabel, "; unconditionally jump");
	
	emit("." + newLabel + ":");
	emit("", "mov", "eax,[TRUE]", "; set eax to TRUE");

	if (symbolTable.count("true") == 0)
	{
		insert("true", BOOLEAN, CONSTANT, "-1", YES, 1);
	}

	emit("." + secondLabel + ":");

	if (isTemporary(operand1))
	{
		freeTemp();
	}
	
	if (isTemporary(operand2))
	{
		freeTemp();
	}

	contentsOfAReg = getTemp();
	symbolTable.at(contentsOfAReg).setDataType(BOOLEAN);
	pushOperand(contentsOfAReg);

}

void Compiler::emitLessThanOrEqualToCode(string operand1, string operand2) {    // op2 <= op1

  if (symbolTable.count(operand1) == 0)
	{
		processError("reference to undefined symbol " + operand1);
	}
	
	else if (symbolTable.count(operand2) == 0)
	{
		processError("reference to undefined symbol " + operand2);
	}

	if (symbolTable.at(operand1).getDataType() != symbolTable.at(operand2).getDataType())
	{
		processError("incompatible types");
	}

	if (isTemporary(contentsOfAReg) && contentsOfAReg != symbolTable.at(operand1).getInternalName() && contentsOfAReg != symbolTable.at(operand2).getInternalName())
	{
		emit("","mov", "[" + contentsOfAReg + "],eax", "; deassign AReg");
		symbolTable.at(contentsOfAReg).setAlloc(YES);
		contentsOfAReg = "";
	}
	
	if (!isTemporary(contentsOfAReg) && contentsOfAReg != symbolTable.at(operand1).getInternalName() && contentsOfAReg != symbolTable.at(operand2).getInternalName())
	{
		contentsOfAReg = "";
	}

	if (contentsOfAReg != symbolTable.at(operand1).getInternalName() && contentsOfAReg != symbolTable.at(operand2).getInternalName())
	{
		emit("","mov", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; AReg = " + operand2);
		contentsOfAReg = symbolTable.at(operand2).getInternalName();
	}

	if (contentsOfAReg == symbolTable.at(operand2).getInternalName())
	{
		emit("","cmp", "eax,[" + symbolTable.at(operand1).getInternalName() + "]", "; compare " + operand2 + " and " + operand1);
	}
	
	else if (contentsOfAReg == symbolTable.at(operand1).getInternalName())
	{
		emit("", "mov", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; AReg = " + operand2);
		emit("","cmp", "eax,[" + symbolTable.at(operand1).getInternalName() + "]", "; compare " + operand2 + " and " + operand1);
	}

	string newLabel = getLabel();

	if (contentsOfAReg == symbolTable.at(operand2).getInternalName())
	{
		emit("","jle", "." + newLabel, "; if " + operand2 + " <= " + operand1 + " then jump to set eax to TRUE");
	}
	
	else if (contentsOfAReg == symbolTable.at(operand1).getInternalName())
	{
		emit("","jle", "." + newLabel, "; if " + operand2 + " <= " + operand1 + " then jump to set eax to TRUE");
	}

	emit("", "mov", "eax,[FALSE]", "; else set eax to FALSE");

	if (symbolTable.count("false") == 0)
	{
		insert("false", BOOLEAN, CONSTANT, "0", YES, 1);
	}

	string secondLabel = getLabel();
	emit("","jmp","." + secondLabel, "; unconditionally jump");
	
	emit("." + newLabel + ":");
	emit("", "mov", "eax,[TRUE]", "; set eax to TRUE");

	if (symbolTable.count("true") == 0)
	{
		insert("true", BOOLEAN, CONSTANT, "-1", YES, 1);
	}

	emit("." + secondLabel + ":");

	if (isTemporary(operand1))
	{
		freeTemp();
	}
	
	if (isTemporary(operand2))
	{
		freeTemp();
	}

	contentsOfAReg = getTemp();
	symbolTable.at(contentsOfAReg).setDataType(BOOLEAN);
	pushOperand(contentsOfAReg);

}

void Compiler::emitGreaterThanCode(string operand1, string operand2) {          // op2 >  op1

  if (symbolTable.count(operand1) == 0)
	{
		processError("reference to undefined symbol " + operand1);
	}
	
	else if (symbolTable.count(operand2) == 0)
	{
		processError("reference to undefined symbol " + operand2);
	}

	if (symbolTable.at(operand1).getDataType() != symbolTable.at(operand2).getDataType())
	{
		processError("illegal types");
	}

	if (isTemporary(contentsOfAReg) && contentsOfAReg != symbolTable.at(operand1).getInternalName() && contentsOfAReg != symbolTable.at(operand2).getInternalName())
	{
		emit("","mov", "[" + contentsOfAReg + "],eax", "; deassign AReg");
		symbolTable.at(contentsOfAReg).setAlloc(YES);
		contentsOfAReg = "";
	}
	
	if (!isTemporary(contentsOfAReg) && contentsOfAReg != symbolTable.at(operand1).getInternalName() && contentsOfAReg != symbolTable.at(operand2).getInternalName())
	{
		contentsOfAReg = "";
	}

	if (contentsOfAReg != symbolTable.at(operand1).getInternalName() && contentsOfAReg != symbolTable.at(operand2).getInternalName())
	{
		emit("","mov", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; AReg = " + operand2);
		contentsOfAReg = symbolTable.at(operand2).getInternalName();
	}

	if (contentsOfAReg == symbolTable.at(operand2).getInternalName())
	{
		emit("","cmp", "eax,[" + symbolTable.at(operand1).getInternalName() + "]", "; compare " + operand2 + " and " + operand1);
	}
	
	else if (contentsOfAReg == symbolTable.at(operand1).getInternalName())
	{
		emit("", "mov", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; AReg = " + operand2);
		emit("","cmp", "eax,[" + symbolTable.at(operand1).getInternalName() + "]", "; compare " + operand2 + " and " + operand1);
	}

	string newLabel = getLabel();

	if (contentsOfAReg == symbolTable.at(operand2).getInternalName())
	{
		emit("","jg", "." + newLabel, "; if " + operand2 + " > " + operand1 + " then jump to set eax to TRUE");
	}
	
	else if (contentsOfAReg == symbolTable.at(operand1).getInternalName())
	{
		emit("","jg", "." + newLabel, "; if " + operand2 + " > " + operand1 + " then jump to set eax to TRUE");
	}

	emit("", "mov", "eax,[FALSE]", "; else set eax to FALSE");

	if (symbolTable.count("false") == 0)
	{
		insert("false", BOOLEAN, CONSTANT, "0", YES, 1);
	}

	string secondLabel = getLabel();
	emit("","jmp","." + secondLabel, "; unconditionally jump");
	
	emit("." + newLabel + ":");
	emit("", "mov", "eax,[TRUE]", "; set eax to TRUE");

	if (symbolTable.count("true") == 0)
	{
		insert("true", BOOLEAN, CONSTANT, "-1", YES, 1);
	}

	emit("." + secondLabel + ":");

	if (isTemporary(operand1))
	{
		freeTemp();
	}
	
	if (isTemporary(operand2))
	{
		freeTemp();
	}

	contentsOfAReg = getTemp();
	symbolTable.at(contentsOfAReg).setDataType(BOOLEAN);
	pushOperand(contentsOfAReg);

}

void Compiler::emitGreaterThanOrEqualToCode(string operand1, string operand2) { // op2 >= op1

  if (symbolTable.count(operand1) == 0)
	{
		processError("reference to undefined symbol " + operand1);
	}
	
	else if (symbolTable.count(operand2) == 0)
	{
		processError("reference to undefined symbol " + operand2);
	}

	if (symbolTable.at(operand1).getDataType() != symbolTable.at(operand2).getDataType())
	{
		processError("incompatible types");
	}

	if (isTemporary(contentsOfAReg) && contentsOfAReg != symbolTable.at(operand1).getInternalName() && contentsOfAReg != symbolTable.at(operand2).getInternalName())
	{
		emit("","mov", "[" + contentsOfAReg + "],eax", "; deassign AReg");
		symbolTable.at(contentsOfAReg).setAlloc(YES);
		contentsOfAReg = "";
	}
	
	if (!isTemporary(contentsOfAReg) && contentsOfAReg != symbolTable.at(operand1).getInternalName() && contentsOfAReg != symbolTable.at(operand2).getInternalName())
	{
		contentsOfAReg = "";
	}

	if (contentsOfAReg != symbolTable.at(operand1).getInternalName() && contentsOfAReg != symbolTable.at(operand2).getInternalName())
	{
		emit("","mov", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; AReg = " + operand2);
		contentsOfAReg = symbolTable.at(operand2).getInternalName();
	}

	if (contentsOfAReg == symbolTable.at(operand2).getInternalName())
	{
		emit("","cmp", "eax,[" + symbolTable.at(operand1).getInternalName() + "]", "; compare " + operand2 + " and " + operand1);
	}
	
	else if (contentsOfAReg == symbolTable.at(operand1).getInternalName())
	{
		emit("", "mov", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; AReg = " + operand2);
		emit("","cmp", "eax,[" + symbolTable.at(operand1).getInternalName() + "]", "; compare " + operand2 + " and " + operand1);
	}

	string newLabel = getLabel();

	if (contentsOfAReg == symbolTable.at(operand2).getInternalName())
	{
		emit("","jge", "." + newLabel, "; if " + operand2 + " <= " + operand1 + " then jump to set eax to TRUE");
	}
	
	else if (contentsOfAReg == symbolTable.at(operand1).getInternalName())
	{
		emit("","jge", "." + newLabel, "; if " + operand2 + " <= " + operand1 + " then jump to set eax to TRUE");
	}

	emit("", "mov", "eax,[FALSE]", "; else set eax to FALSE");

	if (symbolTable.count("false") == 0)
	{
		insert("false", BOOLEAN, CONSTANT, "0", YES, 1);
	}

	string secondLabel = getLabel();
	emit("","jmp","." + secondLabel, "; unconditionally jump");
	
	emit("." + newLabel + ":");
	emit("", "mov", "eax,[TRUE]", "; set eax to TRUE");

	if (symbolTable.count("true") == 0)
	{
		insert("true", BOOLEAN, CONSTANT, "-1", YES, 1);
	}

	emit("." + secondLabel + ":");

	if (isTemporary(operand1))
	{
		freeTemp();
	}
	
	if (isTemporary(operand2))
	{
		freeTemp();
	}

	contentsOfAReg = getTemp();
	symbolTable.at(contentsOfAReg).setDataType(BOOLEAN);
	pushOperand(contentsOfAReg);

}

// Lexical routines
char Compiler::nextChar() {       // Returns the next character or END_OF_FILE marker
	sourceFile.get(ch);
	static char prevChar = '\n';
	
	if (sourceFile.eof())
		ch = END_OF_FILE;
	else {
		if (prevChar == '\n') {
			++lineNo;
			listingFile << setw(5) << lineNo << '|';
		}
		listingFile << ch;
	}
	prevChar = ch;
	return ch;
}

string Compiler::nextToken() {    // Returns the next token or END_OF_FILE marker
 	token = "";	
	while(token == "") {
		if(ch == '{') { 
			while (nextChar() != END_OF_FILE && ch != '}') {}
			if (ch == END_OF_FILE)
				processError("unexpected end of file: '}' expected");
			// A '}' was found
      else
				nextChar();
		}
		else if (ch == '}')
			processError("'}' cannot begin token");
		else if (isspace(ch))
			nextChar();
		else if (isSpecialSymbol(ch)) {
			token = ch;
			nextChar();
      if(token == ":" && ch == '=') {
        token += ch;
        nextChar();
      }
      else if(token == "<" && (ch == '>' || ch == '=')) {
        token += ch;
        nextChar();
      }
      else if(token == ">" && ch == '=') {
        token += ch;
        nextChar();
      }
		}

		else if (islower(ch)) {
			token = ch;
			while((nextChar() == '_' || isalpha(ch) || isdigit(ch)) && ch != END_OF_FILE)
				token += ch;
			if (ch == END_OF_FILE)
				processError("unexpected end of file");
		}

		else if (isdigit(ch)) {
			token = ch;
			while (isdigit(nextChar()) && ch != END_OF_FILE)
				token += ch;
			if (ch == END_OF_FILE)
				processError("unexpected end of file");
		}

		else if (ch == END_OF_FILE)
			token = ch;
		else
			processError("illegal symbol");
	}	
	token = token.substr(0,15);
	return token;
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
