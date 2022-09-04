#include "../../inc/assembler/assembler.h"
#include "../../inc/assembler/directives.h"
#include "../../inc/assembler/instructions.h"

using namespace std;

//VARIABLES
map <string, SymbolTableEntry> symbolTable;
map <string, SectionTableEntry> sectionTable;
// map <string, list<RelocationTableEntry>> relocationTable;
list<RelocationTableEntry> relocationTable = {};
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
  //push, pop
  {"push", 0xB0},
  {"pop", 0xA0},
 //instrukcija atomicne zamene vrednosti
  {"xchg", 0x60},
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
  SymbolTableEntry* newEntry = new SymbolTableEntry("UND", "UND", 0,0,currentSymbolNumber++, false, false,{}, {}, 0);
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
  if(!halt){processInstruction(myLine); return true;}

  //TODO: change to return false after everything done maybe
  cout << "LINIJA: " << myLine << " NIJE VALIDNA";
  return false;
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
        symbolTable[labelName].sectionName = currentSectionName;
        symbolTable[labelName].sectionNumber = currentSectionNumber;
        symbolTable[labelName].value = locationCounter;
        backpatching(labelName, locationCounter);

      }else{
          SymbolTableEntry newEntry = SymbolTableEntry(labelName, currentSectionName, currentSectionNumber,locationCounter,currentSymbolNumber++, false, true,{}, {}, -1);
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
    string sectionToRapair = symbolTable[labelName].flinkSections.front();
    symbolTable[labelName].flinkSections.pop_front();

    //Ako je simbol globalan ostavi linkeru da razresi
    //TODO: proveri logiku
    if(!symbolTable[labelName].isGlobal){
      int dl = sectionTable[sectionToRapair].code[locationToRepair];
      int dh =  sectionTable[sectionToRapair].code[locationToRepair + 1];
      int newValue = ((dh << 8) | dl) + value;
      sectionTable[sectionToRapair].code[locationToRepair] = (unsigned) newValue & 0xFF; //dataLow
      sectionTable[sectionToRapair].code[locationToRepair + 1] = (unsigned) newValue >> 8; //dataHigh
    }
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

  //LOAD STORE
  //DATA INSTRUCTIONS WITH OPERAND
  std::set<string> dataInstructions = { "ldr", "str" };
    if (dataInstructions.find(instructionName) != dataInstructions.end()) {
    addByteToCode(instructionTable[instructionName]);
    int firstReg = getFirstReg(currentLine);
    processDataOperand(instructionName, firstReg, trim(currentLine.substr(currentLine.find(",") + 1)));

    return true;
  }

  //PUSH
  if (instructionName == "push") {
    addByteToCode(instructionTable[instructionName]);
    processPush(currentLine);
    return true;
  }

  //POP
  if (instructionName == "pop") {
    addByteToCode(instructionTable[instructionName]);
    processPop(currentLine);
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

  printSymbolTableInFile(output);
  printSectionTableInFile(output);

  printSymbolTableForLinker(output);
  printSectionTableForLinker(output);

};

//HELPER FUNCTIONS
void handleSymbol(string symbolName, string relocationType){

  int symbolValue = 0;
  
  //ako postoji simbol u tabeli simbola
  if(symbolTable.find(symbolName) != symbolTable.end()) {

    //ako postoji u tabeli simbola i definisan je
    if(symbolTable[symbolName].isDefined){
      symbolValue = symbolTable[symbolName].value;
    }
    else{
      //ako postoji u tabeli simbola i nije definisan
      symbolTable[symbolName].flink.push_back(locationCounter);
      symbolTable[symbolName].flinkSections.push_back(currentSectionName);
    }
  }//ako ne postoji u tabeli simbola
  else{
      symbolTable[symbolName] = SymbolTableEntry(symbolName, currentSectionName, currentSectionNumber,0,currentSymbolNumber++, false, false,{locationCounter}, {currentSectionName}, -1);
  }

  //create relocation record (kasnije ukoliko je simbol lokalan prepravices na kraju prolaza i sve gde je lokalno stavices ime sekcije u kojoj su - fixRelocations() )
  //ipak ce se znati ranije da li je simbol lokalan ili globalan, za to se brini kada naidjes na .global direktivu ukoliko je u UND sekciji sve ok
  relocationTable.push_back(RelocationTableEntry(currentSectionName,relocationType, locationCounter, symbolName));

  if(relocationType == "PC_REL") symbolValue -= 2; // zato sto do pocetka naredne instrukcije ima jos dva bajta, a ostalo ce linker da sredi
  
  //dodaj symbol u kod
  addWordToCode(symbolValue);
}

int findReg(string reg){
    int regVal=0xF;
    if(reg == "sp") regVal = 0x6;
    else if(reg == "pc") regVal = 0x07;
    else if(reg == "psw") regVal = 0x8;
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

void printSectionTableInFile(string output){

  fstream outputFile;
  outputFile.open("../../obj/"+output,ios::app);  // open a file to perform write operation using file object
  //outputFile.open("../../bin/"+output,ios::out);
  if(outputFile.is_open()) //checking whether the file is open
  {

   for(auto &itr: sectionTable){
      outputFile << endl << endl;
      outputFile << "Section name: " + itr.second.sectionName << "  |   ";
      SectionTableEntry iterSection = sectionTable[itr.second.sectionName];
      outputFile << "Section size: " << iterSection.size << endl;
      outputFile << "Section content: " << endl;

      for(int i=0; i< iterSection.code.size(); i++){
        if(i%8==0) outputFile << endl;
        outputFile << setw(2) << setfill('0') << hex << (unsigned int) iterSection.code[i] << " ";
      }
      outputFile << endl << endl;
      outputFile<< "Relocation table: " <<endl;
      // list<RelocationTableEntry> relocations = relocationTable[itr.second.sectionName];
      outputFile << setw(15) << setfill(' ')<< "RelocationType" 
          << setw(15) << "Offset "  
          << setw(15) << "SymbolTableReference " 
          << endl;
      for(auto &relocation: relocationTable){
        if(relocation.sectionName == itr.second.sectionName){
          outputFile << setw(15) << relocation.type 
            << setw(15) << relocation.offset
            << setw(15) << relocation.symbolTableRef 
            << endl;
        }
      }
   }
    outputFile.close();    //close the file object
  }
}

void printSymbolTableInFile(string output){

  vector<pair<string, SymbolTableEntry>> vec = sortMapToVectorPairs();

  fstream outputFile;
  outputFile.open("../../obj/"+output,ios::out);  // open a file to perform write operation using file object
  if(outputFile.is_open()) //checking whether the file is open
  {

    outputFile << setw(50)<< "Symbol table" << endl;
    outputFile << setw(15) << "symbolNumber " 
                << setw(15) << "value " 
                << setw(15) << "symbolName "  
                << setw(15) << "sectionName "  
                << setw(15) << "isGlobal "  
                << setw(15) << "isDefined " 
                << setw(15) << "size "
                << endl;

    for(auto &itr: vec){
      outputFile << setw(15) << itr.second.symbolNumber
        << setw(15) << itr.second.value 
        << setw(15) << itr.second.symbolName 
        << setw(15) << itr.second.sectionName
        << setw(15) << itr.second.isGlobal
        << setw(15) << itr.second.isDefined
        << setw(15) << itr.second.size
        << endl;
  }
    outputFile.close();    //close the file object
  }
}

void printSymbolTableForLinker(string output){

  vector<pair<string, SymbolTableEntry>> vec = sortMapToVectorPairs();

  fstream outputFile;
  outputFile.open("../../bin/"+output,ios::out);  // open a file to perform write operation using file object
  if(outputFile.is_open()) //checking whether the file is open
  {

    outputFile << "Symbol table" << endl;

    for(auto &itr: vec){
      if(itr.second.isGlobal || itr.second.symbolName == itr.second.sectionName){
        outputFile <<  itr.second.symbolNumber << '|'
                    <<  itr.second.value << '|'
                    <<  itr.second.symbolName << '|'
                    <<  itr.second.sectionName << '|'
                    <<  itr.second.isDefined << '|'
                    <<  itr.second.size
                    << endl;
      }
  }
    outputFile << "###" << endl;
    outputFile.close();    //close the file object
  }
}


void printSectionTableForLinker(string output){

  fstream outputFile;
  outputFile.open("../../bin/"+output,ios::app);  // open a file to perform write operation using file object
  //outputFile.open("../../bin/"+output,ios::out);
  if(outputFile.is_open()) //checking whether the file is open
  {
   for(auto &itr: sectionTable){
      outputFile << itr.second.sectionName << endl;
      SectionTableEntry iterSection = sectionTable[itr.second.sectionName];

      for(int i=0; i< iterSection.code.size(); i++){
        // if(i%8==0) outputFile << endl;
        outputFile << setw(2) << setfill('0') << hex << (unsigned int) iterSection.code[i] << "|";
      }
      outputFile << endl;
      outputFile<< "Relocation table: " <<endl;
      for(auto &relocation: relocationTable){
        if(relocation.sectionName == itr.second.sectionName){
          outputFile << relocation.type << "|"
            << relocation.offset << "|"
            << relocation.symbolTableRef 
            << endl;
        }
      }
      outputFile << "###" << endl;
   }
    outputFile.close();    //close the file object
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

vector<pair<string, SymbolTableEntry>> sortMapToVectorPairs(){
    // create an empty vector of pairs
    std::vector<pair<string, SymbolTableEntry>> vec;
 
    // copy key-value pairs from the map to the vector
    std::copy(symbolTable.begin(),
            symbolTable.end(),
            std::back_inserter<std::vector<pair<string, SymbolTableEntry>>>(vec));

 
    // sort the vector by increasing the order of its pair's second value
    // if the second value is equal, order by the pair's first value
    std::sort(vec.begin(), vec.end(),
            [](const pair<string, SymbolTableEntry> &l, const pair<string, SymbolTableEntry> &r)
            {
                if (l.second.symbolNumber != r.second.symbolNumber) {
                    return l.second.symbolNumber < r.second.symbolNumber;
                }
                return l.first < r.first;
            });
 
    return vec;
}

void fixRelocations(){
  for(auto &relocation: relocationTable){
    string symbolName = relocation.symbolTableRef;
    if(!symbolTable[symbolName].isGlobal){
      relocation.symbolTableRef = symbolTable[symbolName].sectionName;
    }
  }
}
