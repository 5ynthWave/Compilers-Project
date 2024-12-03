// Erik Zuniga && Roberto Lopez
// CS4301 - stage1

// Constructor
Compiler(char **argv) {
}
// Destructor
~Compiler() {

}

void createListingHeader() {

}
void parser() {

}
void createListingTrailer() {

}

// Methods implementing the grammar productions
// Stage 0, Production 1 ..
void prog() {

}
void progStmt() {

}
void consts() {

}
void vars() {

}
void beginEndStmt() {

}
void constStmts() {

}
void varStmts() {

}
// .. Stage 0, Production 8
string ids() {

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

}
void processError(string err) {

}
void freeTemp() {

}
string getTemp() {

}
string getLabel() {

}
bool isTemporary(string s) {        // Determines if s represents a temporary

}
