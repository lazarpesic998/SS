#include "../inc/assembler.h"
#include "../inc/directives.h"
#include "../inc/instructions.h"

using namespace std;

//VARIABLES
map <string, SymbolTableEntry> symbolTable;
map <string, SectionTableEntry> sectionTable;
int currentSymbolNumber = 0;
int currentSectionNumber = 0;
string currentSectionName = "UND";
SymbolTableEntry currentSection;
int locationCounter = 0;
bool halt = false;

map<string, int> instructionTable = {
  {"halt", 0x00}, //zaustavaljanje procesora
  {"int",  0x10}, //instrukcija softverskog prekida
  {"iret", 0x20}, //instrukcija povratka iz prekidne rutine
  {"call", 0x30}, //instrukcija poziva potprograma
  {"ret",  0x40}, //instrukcija povratka iz potprograma
  //skokovi
  {"jmp",  0x50},
  {"jeq",  0x51},
  {"jne",  0x52},
  {"jgt",  0x53},
  //TODO: push i pop
  // {"push", 0b0000},
  // {"pop", 0b0000},
  {"xchg", 0x60}, //instrukcija atomicne zamene vrednosti
  //aritmeticke operacije
  {"add",  0x70}, 
  {"sub",  0x71},
  {"mul",  0x72},
  {"div",  0x73},
  {"cmp",  0x74},
  //logicke operacije
  {"not",  0x80},
  {"and",  0x81},
  {"or",   0x82},
  {"xor",  0x83},
  {"test", 0x84},
  //pomeracke operacije
  {"shl",  0x90},
  {"shr",  0x91},
  //load i store
  {"ldr", 0xA0},
  {"str", 0xB0}
};


//FUNCTIONS
void setupAssembler(){
  SymbolTableEntry* newEntry = new SymbolTableEntry("UND",0,0,currentSymbolNumber++, false, false,{}, 0);
  symbolTable["UND"] = *newEntry;
}

//returns false is .end found - dont process lines anymore
bool processLine(string currentLine){

  //trimming line
  string myLine = trimComments(currentLine);
  myLine = trim(myLine);
  
  //ako naidjes na labelu procesiraj je
  myLine = processLabel(myLine);
  if (myLine.size() == 0 || myLine == ":") return true; //vrati ako nemas vise sta da obradis

  if(isEnd(myLine)) {processEnd(); return false;}
  if (processDirective(myLine)) return true;
  if(!halt){processInstruction(currentLine); return true;}

  //TODO: change to return false after everything done maybe
  return true;
}

string processLabel(string currentLine){
    regex reg ("(.*:)");
    smatch matches;

    while(std::regex_search(currentLine, matches, reg)){
        // std::cout << "Nadjena labela" << "\n";
        string labelName = matches.str(1);
        labelName = labelName.substr(0, labelName.size()-1);
      // Eliminate the previous match and create
      // a new string to search
      currentLine = matches.suffix().str();
      //obrada labele
      if(symbolTable.find(labelName) != symbolTable.end()) {
        //simbol vec postoji
        symbolTable[labelName].isDefined = true;
        symbolTable[labelName].sectionNumber = currentSectionNumber;
        symbolTable[labelName].value = locationCounter;
        backpatching(labelName, locationCounter);

      }else{
          SymbolTableEntry newEntry = SymbolTableEntry(labelName,currentSectionNumber,locationCounter,currentSymbolNumber++, false, true,{}, -1);
          symbolTable[labelName] = newEntry;
      }
    }
    return currentLine;
}

void backpatching(string labelName, int value){

//reapair symbols in code which were not defined before
  for(int i=0; i<symbolTable[labelName].flink.size(); i++){
    int locationToRepair = symbolTable[labelName].flink.front();
    symbolTable[labelName].flink.pop_front();
    //TODO: proveri da li treba samo jedan bajt da prepravis ili 2!!!
    sectionTable[currentSectionName].code[locationToRepair] += value;
  }

}

bool processDirective(string currentLine){

  regex reg ("(\\.?[_a-zA-Z][_a-zA-Z0-9]*)");
  smatch matches;
  std::regex_search(currentLine, matches, reg);
  string directiveName = matches.str(1);
  currentLine = matches.suffix().str();

  if(directiveName == ".global"){processGlobalDirective(currentLine); return true;}
  if(directiveName == ".extern"){processExtern(currentLine); return true;}
  if(directiveName == ".section"){processSection(currentLine); return true;}
  if(directiveName == ".word"){processWord(currentLine); return true;}
  if(directiveName == ".skip"){processSkip(currentLine); return true;}
  return false;
}

bool processInstruction(string currentLine){
  
  regex reg ("([_a-zA-Z][_a-zA-Z0-9]*)");
  smatch matches;
  std::regex_search(currentLine, matches, reg);
  string instructionName = matches.str(1);
  currentLine = matches.suffix().str();

  //NO OPERANDS INSTRUCTIONS
  if(instructionName == "halt"){
    halt = true;
    addByteToCode(instructionTable[instructionName]);
    return true;
  }
  if(instructionName == "iret" || instructionName == "ret"){
    addByteToCode(instructionTable[instructionName]);
    return true;
  }

  //INSTRUCTIONS WITH REGISTARS ONLY
  std::set<string> instructionsWithReg = { "int", "xchg", "add", "sub", 
                                      "mul", "div", "cmp", "not",
                                      "and", "or", "xor", "test",
                                      "shl", "shr"};
  if (instructionsWithReg.find(instructionName) != instructionsWithReg.end()) {
    
    addByteToCode(instructionTable[instructionName]);
    std::regex_search(currentLine, matches, reg);
    string regD = matches.str(1);
    currentLine = matches.suffix().str(); std::regex_search(currentLine, matches, reg);
    string regS = matches.str(1);
    addRegistersInfo(regD, regS);
    return true;
  }

  //DATA INSTRUCTIONS WITH OPERAND
  std::set<string> jumpInstructions = { "ld", "str" };
    if (jumpInstructions.find(instructionName) != jumpInstructions.end()) {
    addByteToCode(instructionTable[instructionName]);
    processDataOperand(currentLine);

    return true;
  }


  //JUMP INSTRUCTIONS WITH OPERAND
  std::set<string> jumpInstructions = { "call", "jmp", "jeq", "jne", "jgt"};
  if (jumpInstructions.find(instructionName) != jumpInstructions.end()) {
    addByteToCode(instructionTable[instructionName]);
    processJumpOperand(currentLine);

    return true;
  }
  
  return false;
}

void generateOutput(string output){
  printSymbolTable();
  printSectionTable();
  // cout << "IDALJE NEMA IZLAZNOG FAJLA";
  // fstream outputFile;
  //  outputFile.open(output,ios::out);  // open a file to perform write operation using file object
  //  if(outputFile.is_open()) //checking whether the file is open
  //  {
  //     outputFile<<"Tutorials point \n";   //inserting text
  //     outputFile.close();    //close the file object
  //  }
};


//HELPER FUNCTIONS
void handleSymbol(string symbolName, int addend){

    if(symbolTable.find(symbolName) != symbolTable.end()) {

      //ako postoji u tabeli simbola i definisan je
      if(symbolTable[symbolName].isDefined){
        addWordToCode(symbolTable[symbolName].value);
      }
      else{
        //ako postoji u tabeli simbola i nije definisan
        symbolTable[symbolName].flink.push_back(locationCounter);
        addWordToCode(addend);
      }
  }
  else{ //ako ne postoji u tabeli simbola
      SymbolTableEntry newEntry = SymbolTableEntry(symbolName,currentSectionNumber,0,currentSymbolNumber++, false, false,{locationCounter}, -1);
      symbolTable[symbolName] = newEntry;
      addWordToCode(addend);
  }
}

int findReg(string reg){
    int regVal=0xF;
    if(reg == "sp") regVal = 0x6;
    else if(reg == "pc") regVal = 0x07;
    else if(reg == "psw") regVal = 0xF;
    else{
        regVal = stoi(reg.erase(0,1));
    }
    return regVal;
}

char createRegByte(int regD, int regS){
    return ((regD & 0xF) << 4 | (regS & 0xF));
}

int myStoi(string literal){
  if (literal.rfind("0x", 0) == 0) return stoi(literal, 0, 16);
  else return stoi(literal);
}


void printSectionTable(){

  cout << "Section table" << endl;

   for(auto &itr: sectionTable){
      cout << "Section name: " + itr.second.sectionName << endl;
      SectionTableEntry iterSection = sectionTable[itr.second.sectionName];
      cout << "Section size: " << iterSection.size << endl;
      cout << "Section content: " << endl;

      for(int i=0; i< iterSection.code.size(); i++){
        cout << (unsigned int) iterSection.code[i] << "  ";
      }
      cout << endl;
   }

}

void printSymbolTable(){

cout << setw(50)<< "Symbol table" << endl;
cout << setw(15) << "symbolName " 
    << setw(15) << "sectionNumber "  
    << setw(15) << "value " 
    << setw(15) << "symbolNumber "  
    << setw(15) << "isGlobal "  
    << setw(15) << "isDefined " 
    << setw(15) << "size "
    << endl;

 for(auto &itr: symbolTable){
  cout << setw(15) << itr.second.symbolName 
    << setw(15) << itr.second.sectionNumber
    << setw(15) << itr.second.value
    << setw(15) << itr.second.symbolNumber 
    << setw(15) << itr.second.isGlobal
    << setw(15) << itr.second.isDefined
    << setw(15) << itr.second.size
    << endl;
  }
}

string trimComments(string currentLine){

  return currentLine.substr(0, currentLine.find('#'));

}

const std::string WHITESPACE = " \n\r\t\f\v";

string ltrim(const string &s)
{
    size_t start = s.find_first_not_of(WHITESPACE);
    return (start == string::npos) ? "" : s.substr(start);
}
 
string rtrim(const string &s)
{
    size_t end = s.find_last_not_of(WHITESPACE);
    return (end == string::npos) ? "" : s.substr(0, end + 1);
}
 
string trim(const string &s) {
    return rtrim(ltrim(s));
}