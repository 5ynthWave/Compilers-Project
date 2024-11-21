// Erik Zuniga && Roberto Lopez
// CS4301 - Compilers Project Stage-0

#include <stage0.h>
#include <string>
using namespace std;

// Constructor
Compiler::Compiler(char **argv) {
 /* open sourceFile using argv[1]
    open listingFile using argv[2]
    open objectFile using argv[3] */
  sourceFile.open(argv[1]);
  listingFile.open(argv[2]);
  objectFile.open(argv[3]);
}
// Destructor
Compiler::~Compiler() {
 /* close all open files */
  sourceFile.close();
  listingFile.close();
  objectFile.close();
}

void Compiler::createListingHeader() {
 /* print "STAGE0:", name(s), DATE, TIME OF DAY
    print "LINE NO:", "SOURCE STATEMENT"
    - line numbers and source statements should be aligned under the headings */
  time_t now = time(null);
  listingFile << "STAGE0: Erik Zuniga and Roberto Lopez       " << ctime(&now) << endl;
  listingFile << "LINE NO.              " << "SOURCE STATEMENT" << endl << endl;
}
void Compiler::parser() {
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
  nextChar();
  if(nextToken() != "program")
    processError("Keyword \'program\'. expected.");
  prog();
}
void Compiler::createListingTrailer() {
 /* print "COMPILATION TERMINATED", "# ERRORS ENCOUNTERED" */
  listingFile << endl << "COMPILATION TERMINATED ";
  listingFile << right << setw(6) << errorCount; << " ERRORS ENCOUNTERED" << endl;
}

// Methods implementing the grammar productions:
// Stage 0, Production 1 -> Token should be "program"
void Compiler::prog() {
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

  if(token != "program")
    processError("Keyword \'program\' expected.");
  progStmt();
  if(token == "const") consts();
  if(token == "var") vars();
  if(token != "begin")
    processError("Keyword \'begin\' expected.");
  beginEndStmt();
  if(token[0] != END_OF_FILE)
    processError("No text may follow \'end\'.");
}
// Stage 0, Production 2 -> Token should be "program"
void Compiler::progStmt() {
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
  
  if(token != "program")
    processError("Keyword \'program\' expected.");
  string x = nextToken();
  if(!isNonKeyId(token))
    processError("Program name expected.");
  if(nextToken() != ';')
    processError("Semicolon \';\' expected.");
  nextToken();
  code("program", x);
  insert(x, PROG_NAME, CONSTANT, x, NO, 0);
}
// Stage 0, Production 3 -> Token should be "const"
void Compiler::consts() {
 /* if (token != "const")
      processError(keyword "const" expected)
    if (nextToken() is not a NON_KEY_ID)
      processError(non-keyword identifier must follow "const")
    constStmts() */

  if(token != "const")
    processError("Keyword \'const\' expected.");
  if(!isNonKeyId(nextToken()))
    processError("Non-keyword identifier must follow \'const\'");
  constStmts();
}
// Stage 0, Production 4 -> Token should be "var"
void Compiler::vars() {
 /* if (token != "var")
      processError(keyword "var" expected)
    if (nextToken() is not a NON_KEY_ID)
      processError(non-keyword identifier must follow "var")
    varStmts() */
  
  if(token != "var")
    processError("Keyword \'var\' expected.");
  if(!isNonKeyId(nextToken()))
    processError("Non-keyword identifier must follow \'var\'.");
  varStmts();
}
// Stage 0, Production 5 -> Token should be "begin"
void Compiler::beginEndStmt() {
 /* if (token != "begin")
      processError(keyword "begin" expected)
    if (nextToken() != "end")
      processError(keyword "end" expected)
    if (nextToken() != ".")
      processError(period expected)
    nextToken()
    code("end", ".") */
  
  if(token != "begin")
    processError("Keyword \'begin\' expected.");
  if(nextToken() != "end")
    processError("Keyword \'end\' expected.");
  if(nextToken() != ".")
    processError("Period \'.\' expected.");
  nextToken();
  code("end", ".");
}
// Stage 0, Production 6 -> Token should be NON_KEY_ID
void Compiler::constStmts() {
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
  
  string x, y;
  if(!isNonKeyId(token))
    processError("Non-keyword identifier expected.");
  x = token;
  if(nextToken() != '=')
    processError("\'=\' expected.");
  y = nextToken();
  if(!(y == '+' || y == '-' || y == "not" || y == "true" || y == "false" || isNonKeyId(y) || isInteger(y)))
    processError("Token to the right of \'=\' is illegal.");
  if(y == '+' || y == '-') {
    if(!isInteger(nextToken()))
      processError("INTEGER expected after sign.");
    y += token;
  }
  if(y == "not") {
    if(!isBoolean(nextToken()))
      processError("BOOLEAN expected after \'not\'.");
    if(token == "true")
      y = "false";
    else
      y = "true";
  }
  if(nextToken() != ';')
    processError("Semicolon \';\' expected.");

  storeTypes type = whichType(y);
  if(!(type != INTEGER || type != BOOLEAN))
    processError("Data type of token on the right-hand side must be INTEGER or BOOLEAN.");
  insert(x, whichType(y), CONSTANT, whichValue(y), YES, 1);

  x = nextToken();
  if(!(x == "begin" || x == "var" || isNonKeyId(x)))
    processError("Non-keyword identifier, \'begin\', or \'var\' expected.");
  if(isNonKeyId(x))
    constStmts();
}
// Stage 0, Production 7 -> Token should be NON_KEY_ID
void Compiler::varStmts() {
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

  string x, y;
  if(!isNonKeyId(token))
    processError("Non-keyword identifier expected.");
  x = ids();
  if(token != ':')
    processError("\':\' expected.");

  nextToken();
  if(!(token == "integer" || token == "boolean"))
    processError("Illegal type follows \':\'.");

  y = token;
  if(nextToken() != ';')
    processError("Semicolon \';\' expected.");

  insert(x, y, VARIABLE, "", YES, 1)
  if(!(nextToken() == "begin" || isNonKeyId(nextToken())))
    processError("Non-keyword identifier or \'begin\' expected.");
  if(isNonKeyId(token))
    varStmts();
}
// Stage 0, Production 8 -> Token should be NON_KEY_ID
string Compiler::ids() {
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

  string temp, tempString;
  if(!isNonKeyId(token))
    processError("Non-keyword identifier expected.");
  tempString = token;
  temp = token;
  if(nextToken() == ',') {
    if(!isNonKeyId(nextToken()))
      processError("Non-keyword identifier expected.");
    tempString = temp + ',' + ids();
  }
  return tempString;
}

// Helper functions for the Pascallite lexicon:
// Determines if s is a keyword
bool Compiler::isKeyword(string s) const {
  return (s == "program" || s == "begin" || s == "end" || s == "var" || s == "const"
    || s == "integer" || s == "boolean" || s == "true" || s == "false" || s == "not");
}
// Determines if c is a special symbol
bool Compiler::isSpecialSymbol(char c) const {
  return (c == '=' || c == ':' || c == ',' || c == ';' || c == '.' || c == '+' || c == '-');
}
// Determines if s is a non_key_id -> ALPHA | ALPHANUMS
bool Compiler::isNonKeyId(string s) const {
  // Ensure that the token is not in the symbol table,
  // if .find() returns true then .end() was never reached
  if(symbolTable.find(s) != symbolTable.end())
    processError("Multiply named definition.");
  // Ensure token does not start with a capitalized letter
  if(isupper(token[0]))
    processError("Tokens must begin with a lowercase character.");
  // Iterate through token to ensure the rest of the characters are valid
  // ALPHANUMS | '_'
  for(string::iterator iter = s.begin(); iter != s.end(); ++iter) {
    if(!isalnum(*iter) && *iter != '_')
      processError('Invalid character inside token.');
  }
  // Ensure that the token is not a reserved keyword
  if(isKeyword(s))
    processError("Reserved keyword \'" + s + "\' cannot be redefined.");
  // If the token passes all tests, then it is a NON_KEY_ID
  return true;
}
// Determines if s is an integer -> NUM | NUMS
bool Compiler::isInteger(string s) const {
  // Iterate through the token and check if each character is an integer
  for(string::iterator iter = s.begin(); iter != s.end(); ++iter) {
    if(*iter == ';') break;
    if(!isdigit(*iter)) return false;
  }
  return true;
}
// Determines if s is a boolean
bool isBoolean(string s) const {
  return (s == "true" || s == "false");
}
// Determines if s is a literal
bool isLiteral(string s) const {
  return (isInteger(s) || isBoolean(s) || s == "not" || s=='+' || s=='-');
}

// Action routines:
// Create symbol table entry for each identigier in list of external names
// multiply inserted names (several at once) are illegal
void Compiler::insert(string externalName, storeTypes inType, modes inMode,
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
	string name;
	string::iterator itr = externalName.begin();		
	
	while (itr < externalName.end())
  {
		name = "";
		while (itr < externalName.end() && *itr != ',' ){
			name = name + *itr;
			//cout << name << '\n'; 
			++itr;
			//cout << "Still in the second loop...\n"; 
		}
		// infinite loop
		// cout << "Exited inner while loop!\n"; // 
		if (!name.empty()) {
			if (symbolTable.count(name) > 0) {
				processError("Multiple name definition.");
			} else if (isKeyword(name)){
				processError("Illegal use of keyword.");
			} else { // Create table entry 
				if (isupper(name[0])) { // Is internal name
					symbolTable.insert(pair<string, SymbolTableEntry>(name.substr(0, 15), // Key
					SymbolTableEntry(name, inType, inMode, inValue, inAlloc, inUnits))); // Value
        } else { // Is an external name, must to create an internal name
					symbolTable.insert(pair<string, SymbolTableEntry>(name.substr(0, 15),
					SymbolTableEntry(genInternalName(inType), inType, inMode, inValue, inAlloc, inUnits)));
        }
				// adding output of SymbolTable for verification
				// ------------------------------------------------------------------
				// check for entries in symbolTable map
				/*
				cout << "Contents of symbolTable:\n"; 
				cout << left << setw(15) << "Key" << "|";
				cout << setw(15) << "Internal Name" << "|";
				cout << setw(9) << "Data Type" << "|";
				cout << setw(8) << "Mode" << "|";
				cout << setw(15) << "Value" << "|";
				cout << setw(5) << "Alloc" << "|";
				cout << "Units" << '\n';
				cout << setw(78) << setfill('_') << "_" << '\n';
				cout << setfill(' ');
				
				for (auto it = symbolTable.begin(); it != symbolTable.end(); ++it) {
			        // Access the key and value using the iterator
			        string key = it->first;
			        SymbolTableEntry value = it->second;
			        // Print the key-value pair
			        cout << setw(15) << it->first << "|";
							cout << setw(15) << it->second.getInternalName() << '|';
							cout << setw(9) << it->second.getDataType() << '|';
							cout << setw(8) << it->second.getMode() << '|';
							cout << setw(15) << it->second.getValue() << '|';
							cout << setw(5) << it->second.getAlloc() << '|';
							cout << it->second.getUnits() << '\n';	
				if (symbolTable.size() > 256)
				{
					processError("Cannot hold over 256 entries");
				}
			  }
			  cout << '\n';
				// ------------------------------------------------------------------
				*/
				
				if (symbolTable.size() > 256)
					processError("Cannot hold over 256 entries");
			}
		}
		++itr; // Need to increment itr to get past comma (prevents infinite loop)
	}
  
}
// Tells which data type a name has
storeTypes Compiler::whichType(string name) {
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
string Compiler::whichValue(string name) {
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
void Compiler::code(string op, string operand1 = "", string operand2 = "") {
 /* if (op == "program")
      emitPrologue(operand1)
    else if (op == "end")
      emitEpilogue()
    else
      processError(compiler error since function code should not be called with illegal arguments) */
  
  if(op == "program")
    emitPrologue(operand1);
  else if(op == "end")
    emitEpilogue();
  else
    processError("Compiler error: function code should not be called with illegal arguments.");
}

// Emit functions:
void Compiler::emit(string label = "", string instruction = "", string operands = "",
          string comment = "") {
 /* Turn on left justification in objectFile
    Output label in a field of width 8
    Output instruction in a field of width 8
    Output the operands in a field of width 24
    Output the comment */
  
  objectFile.setf(ios_base::left);
  objectFile << left << setw(8) << label;
  objectFile << left << setw(8) << instruction;
  objectFile << left << setw(24) << operands;
  objectFile << comment << endl;
}
void Compiler::emitPrologue(string progName, string = "") {
 /* Output identifying comments at beginning of objectFile
    Output the %INCLUDE directives
    emit("SECTION", ".text")
    emit("global", "_start", "", "; program " + progName)
    emit("_start:") */
  
  time_t current = time(0);
  char* time = ctime(&current);
  objectFile << "; Erik Zuniga and Roberto Lopez       " << right << setw(6) << time;
  objectFile << "%INCLUDE \"Along32.inc\"\n" << "%INCLUDE \"Macros_Along.inc\"\n" << endl;
  emit("SECTION", ".text");
  emit("global", "_start", "", "; program " + progName);
  emit("_start");
}
void Compiler::emitEpilogue(string = "", string = "") {
 /* emit("","Exit", "{0}")
    emitStorage(); */
  
  emit("", "Exit", "{0}");
  objectFile << endl;
  emitStorage();
}
void Compiler::emitStorage() {
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
  
  // Output the "SECTION .data" section
  emit("SECTION", ".data");
  for(iter = symbolTable.begin(); iter != symbolTable.end(); ++iter) {
    // For entries in the symbolTable with allocation=YES and storage_mode=CONSTANT
    if(iter->second.getAlloc() == YES && iter->second.getMode() == CONSTANT) {
      // Check boolean values
      string value = iter->second.getValue();
      if(iter->second.getValue() == "false")
        value = "0";
      if(iter->second.getValue() == "true")
        value = "1";
      // Emit the output line to objectFile
      emit(iter->second.getInternalName(), "dd", value, "; " + iter->first);
    }
  }

  // Output the "SECTION .bss" section
  objectFile << "\n";
  emit("SECTION", ".bss");
  for(iter = symbolTable.begin(); iter != symbolTable.end(); ++iter) {
    if(iter->second.getAlloc() == YES && iter->second.getMode() == VARIABLE) {
      // Emit the output line to objectFile
      emit(iter->second.getInternalName(), "resd", "1", "; ", + iter->first);
    }
  }
}

// Lexical routines:
// Returns the next character or END_OF_FILE marker
char Compiler::nextChar() {
 /* read in next character
    if (end of file) 
      ch = END_OF_FILE // Use a special character to designate end of file
                       // END_OF_FILE is defined in stage0.h
    else {
      ch = next character
      print to listing file (starting new line if necessary)
    }
    return ch */

  sourceFile.get(ch);
  static char prevChar = '\n';
  if(sourceFile.eof())
    ch = END_OF_FILE;
  else {
    if(prevChar == '\n') {
      ++lineNo;
      listingFile << setw(5) << lineNo << '|';
    }
    listingFile << ch;
  }
  prevChar = ch;
  return ch;
}
// Returns the next token or END_OF_FILE marker
string Compiler::nextToken() {
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
  
  token = "";
  while(token == "") {
    if(ch == '{') {
      // Stay on a loop to simply read a comment, do nothing
      while(nextChar() != END_OF_FILE && ch != '}') {}
      if(ch == END_OF_FILE)
        processError("Unexpected end of file marker: \'}\' expected.");
      // A '}' was found.
      else nextChar();
    }
    else if(ch == '}')
      processError("\'}\' cannot begin token.");
    else if(isspace(ch))
      nextChar();
    else if(isSpecialSymbol(ch)) {
      token = ch;
      nextChar();
    }
    else if(islower(ch)) {
      token = ch;
      while(nextChar() == '_' || isalpha(ch) || isdigit(ch) && ch != END_OF_FILE)
        token += ch;
      if(ch == END_OF_FILE)
        processError("Unexpected end of file marker.");
    }
    else if(isdigit(ch)) {
      token = ch;
      while(isdigit(nextChar()) && ch != END_OF_FILE)
        token += ch;
      if(ch == END_OF_FILE)
        processError("Unexpected end of file marker.");
    }
    else if(ch == END_OF_FILE)
      token = ch;
    else
      processError("Illegal symbol.");
  }
  // Token sizes are maximum of length 15
  token = token.substr(0, 15);
  return token;
}

// Other routines:
string Compiler::genInternalName(storeTypes stype) const {
  string name;
  // Integers to count the number of I's and B's
  // in the assembler file
  static int numsI, numsB = 0;
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
void Compiler::processError(string err) {
 /* Output err to listingFile
    Call exit(EXIT_FAILURE) to terminate program */
  listingFile << endl << "Error: line " << lineNo << ": " << err << endl;
  ++errorCount;
  listingFile << "\nCOMPILATION TERMINATED " << errorCount << " ERROR ENCOUNTERED" << endl;
  exit(-1);
}