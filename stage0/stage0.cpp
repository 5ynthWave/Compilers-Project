// Erik Zuniga && Roberto Lopez
// CS4301 - stage0

using namespace std;

#include <stage0.h>
#include <string>
#include <cstring>
#include <ctime>
#include <iomanip>
#include <cctype>
#include <map>
#include <iostream>

/* ----- begin CONSTRUCTOR/DESTRUCTOR ----- */
Compiler::Compiler(char **argv) {
  sourceFile.open(argv[1]);
	listingFile.open(argv[2]);
	objectFile.open(argv[3]);
}

Compiler::~Compiler() {
  sourceFile.close();
	listingFile.close();
	objectFile.close();
}
/* ----- end CONSTRUCTOR/DESTRUCTOR ----- */

/* ----- begin OUTPUT FUNCTIONS ----- */
void Compiler::createListingHeader() {
	time_t now = time(NULL);
	listingFile << "STAGE0:  Erik Zuniga and Roberto Lopez       " << ctime(&now) << endl;
	listingFile << "LINE NO.              " << "SOURCE STATEMENT" << endl << endl;
}

void Compiler::parser() {
  nextChar();
  nextToken();
  if(token != "program")
	  processError("keyword \"program\" expected");
  prog();
}

void Compiler::createListingTrailer() {
   listingFile << endl << "COMPILATION TERMINATED ";
   listingFile << right << setw(6) << errorCount;
   listingFile << " ERRORS ENCOUNTERED" << endl;
}

void Compiler::processError(string err) {
	listingFile << endl << "Error: Line " << lineNo << ": " << err << endl;
	errorCount += 1;
	listingFile << "\nCOMPILATION TERMINATED " << errorCount << " ERROR ENCOUNTERED" << endl;
	exit(-1);
}
/* ----- end OUTPUT FUNCTIONS ----- */

/* ----- begin PRODUCTION FUNCTIONS ----- */
// Token should be "program"
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
	if(token[0] != END_OF_FILE)
		processError("no text may follow \"end\"");
}

// Token should be "program"
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

// Token should be "const"
void Compiler::consts() {
	if (token != "const")
		processError("keyword \"const\" expected");
	if (!isNonKeyId(nextToken()))
		processError("non-keyword identifier must follow \"const\"");
	constStmts();
}

// Token should be "var"
void Compiler::vars() {
  if(token != "var")
		processError("keyword \"var\" expected");
	if(!isNonKeyId(nextToken()))
		processError("non-keyword identifier must follow \"var\"");
	varStmts();
}

// Token should be "begin"
void Compiler::beginEndStmt() {
  if (token != "begin")
    processError("keyword \"begin\" expected");
  if (nextToken() != "end")
    processError("keyword \"end\" expected");
  if (nextToken() != ".")
    processError("period expected");
  nextToken();
  code("end", ".");
}

// Token should be NON_KEY_ID
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
  insert(x,whichType(y),CONSTANT,whichValue(y),YES,1);
  x = nextToken();
  if (x != "begin" && x != "var" && !isNonKeyId(x))
	  processError("non-keyword identifier, \"begin\", or \"var\" expected");
  if (isNonKeyId(x))
	  constStmts();
}

// Tokens should be of type NON_KEY_ID
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

// Token should be NON_KEY_ID
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
/* ----- end PRODUCTION FUNCTIONS ----- */

/* ----- begin HELPER FUNCTIONS ----- */
bool Compiler::isKeyword(string s) const {
  return (s == "program" || s == "var" || s == "begin" || s == "end"
    || s == "true" || s == "false" || s == "not" || s == "const"
    || s == "integer" || s == "boolean");
}

bool Compiler::isSpecialSymbol(char c) const {
	return (c == ':' || c == ',' || c == ';' || c == '=' || c == '+'
    || c == '-' || c == '.');
}

bool Compiler::isNonKeyId(string s) const {
	// NON_KEY_ID -> ALPHA ALPHANUMS
	if (isKeyword(s)) return false;
	if (!isalpha(s[0]) || !islower(s[0])) return false;
	if (isdigit(s[0]) || s[0] == '_' ) return false;
	
	for (size_t i = 1; i < s.length(); ++i) {
		if (isSpecialSymbol(s[i]) && s[i] != '_') return false;
		// All lowercase
		if (isalpha(s[i]) && !islower(s[i])) return false;
		// For 049.dat
		if (s[i] == '_' && s[i-1] == '_') return false;
	}
	// For 020.dat
	if (s[s.length() - 1] == '_') return false;
	return true;
}

// Determines if s is an integer -> NUM | NUMS
bool Compiler::isInteger(string s) const {
  // Iterate through the token and check if each character is an integer
	for (uint i = 0; i < s.length(); i++) {
	  if (s[i] == ';') break;
	  if (!isdigit(s[i])) return false;
	}
	return true;   
}

// Determines if s is a boolean
bool Compiler::isBoolean(string s) const {
  return (s == "true" || s == "false");
}

// Determines if s is a literal
bool Compiler::isLiteral(string s) const {
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
/* ----- end HELPER FUNCTIONS ----- */

/* ----- begin ACTION ROUTINES ----- */
void Compiler::insert(string externalName, storeTypes inType, modes inMode, string inValue, allocation inAlloc, int inUnits) {
	 // Create symbol table entry for each identifier in list of external names
	 // Multiply inserted names are illegal
	string name;
	string::iterator iter = externalName.begin();		
	
	while (iter < externalName.end()) {
		name = "";
		while (iter < externalName.end() && *iter != ',' ) {
			name = name + *iter;
			++iter;
		}

		if (!name.empty()) {
			if (symbolTable.count(name) > 0)
				processError("multiple name definition");
			else if (isKeyword(name))
				processError("illegal use of keyword");
      // Create table entry
			else {
        // If it is an internal name
				if (isupper(name[0])) {
					symbolTable.insert(pair<string, SymbolTableEntry>(name.substr(0, 15), // Key
					SymbolTableEntry(name, inType, inMode, inValue, inAlloc, inUnits)));  // Value
				}
        // If it is an external name then we must create an internal name
				else {
					symbolTable.insert(pair<string, SymbolTableEntry>(name.substr(0, 15),
					SymbolTableEntry(genInternalName(inType), inType, inMode, inValue, inAlloc, inUnits)));
				}
				if (symbolTable.size() > 256)
					processError("Cannot hold over 256 entries");
			}
		}
    // Increment iter to get past comma and prevent an infinite loop
		++iter;
	}
}

/* ----- WHICHTYPE/VALUE FUNCTIONS ----- */
// Tells which type a name has
storeTypes Compiler::whichType(string name) {
	// Create an iterator to symbolTable map starting at name's address
	map<string,SymbolTableEntry>::iterator iter = symbolTable.find(name);

	storeTypes type;
  // Initially check if the name is a literal
	if (isLiteral(name)) {
    // If it is a proper literal, then check if it's a boolean,
		if (isBoolean(name))
			type = BOOLEAN;
    // otherwise it is an integer
		else
			type = INTEGER;
	}
  // Name is an identifier and hopefully a constant
	else {
    // If name is in the symbolTable, then grab its data-type
	  if(symbolTable.count(name) > 0)
	  	type = iter->second.getDataType();
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

// Tells which value a name has
string Compiler::whichValue(string name) {
	map<string,SymbolTableEntry>::iterator iter = symbolTable.find(name);
	string value;
	if(isLiteral(name))
		value = name;
	else {
		if(iter->second.getValue() != "" && symbolTable.count(name) > 0)
			value = iter->second.getValue();
		else
			processError("reference to undefined constant");
	}
	return value;
}

/* ----- CODE FUNCTION ----- */
void Compiler::code(string op, string operand1 , string operand2) {
  if( op == "program")
  	emitPrologue(operand1);
  else if (op == "end")
  	emitEpilogue();
  else
  	processError("compiler error since function code should not be called with illegal arguments");
}

/* ----- EMIT FUNCTIONS ----- */
// Output the individual lines
void Compiler::emit(string label, string instruction, string operands, string comment) {
	objectFile.setf(ios_base::left);
	objectFile << left << setw(8) << label;              
	objectFile << left << setw(8) << instruction;  
	objectFile << left << setw(24) << operands;
	objectFile << comment << endl;
}

// Output the beginning of .asm file
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

// Output the ending of .asm file
void Compiler::emitEpilogue(string progName, string operand2) {
  emit("","Exit", "{0}");
  objectFile << endl;
  emitStorage();
}

// Output the storage of individual lines
void Compiler::emitStorage() {
  map<string,SymbolTableEntry>::iterator iter = symbolTable.begin();
  emit("SECTION", ".data");
  for (iter = symbolTable.begin(); iter != symbolTable.end(); ++iter) {
  	if ((iter->second.getAlloc()) == YES && (iter->second.getMode() == CONSTANT)) {
		  string value = iter->second.getValue();
		  if (iter->second.getValue() == "false")
		  	value = "0";
		  else if (iter->second.getValue() == "true")
		  	value = "-1";
      emit(iter->second.getInternalName(), "dd", value, "; " + iter->first);
    }
  }
  
	objectFile << "\n";
	emit("SECTION", ".bss");
 
	for (iter = symbolTable.begin(); iter != symbolTable.end(); ++iter) {                                                              
		if (iter->second.getAlloc() == YES && iter->second.getMode() == VARIABLE)
			emit(iter->second.getInternalName(), "resd", "1", "; " + iter->first);
	}
}
/* ----- end ACTION ROUTINES ----- */

/* ----- start LEXICAL SCANNER ----- */
// Returns the next character or end of file marker
char Compiler::nextChar() {	
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

// Returns the next token or end of file marker
string Compiler::nextToken() {
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
		}
		else if (islower(ch)) {
			token = ch;
			while((nextChar() == '_' || isalpha(ch) || isdigit(ch)) && ch != END_OF_FILE)
				token = token + ch;
			if (ch == END_OF_FILE)
				processError("unexpected end of file");
		}
		
		else if (isdigit(ch)) {
			token = ch;
			while (isdigit(nextChar()) && ch != END_OF_FILE)
				token = token + ch;
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
/* ----- end LEXICAL SCANNER ----- */

/* ----- begin OTHER ROUTINES ----- */
string Compiler::genInternalName(storeTypes stype) const {
	string name;
	static int numsI = 0, numsB = 0; 
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
	}
	return name;
}
/* ----- end OTHER ROUTINES ----- */