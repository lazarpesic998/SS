#include "../../inc/assembler/assembler.h"
#include "../../inc/assembler/directives.h"

bool isGlobalDirective(string currentLine){
    regex reg ("(.global)");
    smatch matches;

    while(std::regex_search(currentLine, matches, reg)){
      //std::cout << "Nadjena global direktiva  " << "\n";
      return matches.ready();
    }
    return false;

}

void processGlobalDirective(string currentLine){

  regex reg ("([_a-zA-Z][_a-zA-Z0-9]*)");
  smatch matches;

  while(std::regex_search(currentLine, matches, reg)){
    // Get the first match
      string currVar = matches.str(1);
  
      // Eliminate the previous match and create
      // a new string to search
      currentLine = matches.suffix().str();
      if(currVar == "global") continue;

      if(symbolTable.find(currVar) != symbolTable.end()) {
        cout<< "***ERROR!*** Symbol already defined!" << endl;
        exit(-1);
      }
      SymbolTableEntry newEntry = SymbolTableEntry(currVar, "UND", -1 ,-1,currentSymbolNumber++, true, false,{}, {}, -1);
      symbolTable[currVar] = newEntry;
  }
  // printSymbolTable();
}

bool isExtern(string currentLine){
    regex reg ("(.extern)");
    smatch matches;

    while(std::regex_search(currentLine, matches, reg)){
      //std::cout << "Nadjena extern direktiva  " << "\n";
      return matches.ready();
    }
    return false;
}

void processExtern(string currentLine){
  regex reg ("([_a-zA-Z][_a-zA-Z0-9]*)");
  smatch matches;

  while(std::regex_search(currentLine, matches, reg)){
    // Get the first match
    string currVar = matches.str(1);

    // Eliminate the previous match and create
    // a new string to search
    currentLine = matches.suffix().str();
    if(currVar == "extern") continue;

    if(symbolTable.find(currVar) != symbolTable.end()) {
      cout<< "***ERROR!*** Symbol already defined!" << endl;
      exit(-1);
    }
    SymbolTableEntry newEntry = SymbolTableEntry(currVar,"UND", 0,0,currentSymbolNumber++, true, false,{}, {}, -1);
    symbolTable[currVar] = newEntry;
  }
}

bool isSection(string currentLine){
  regex reg ("(.section)");
    smatch matches;

    while(std::regex_search(currentLine, matches, reg)){
      // std::cout << "Nadjena .section direktiva  " << "\n";
      return matches.ready();
    }
    return false;
}

void processSection(string currentLine){

  //extract section name from line
  regex reg ("([_a-zA-Z][_a-zA-Z0-9]*)");
  smatch matches;
  regex_search(currentLine, matches, reg);
  // currentLine = matches.suffix().str();
  // regex_search(currentLine, matches, reg);
  
  // if(matches.size()!= 2) {
  //   cout<< "***ERROR!*** Too many params for section directive!" << endl;
  //   exit(-1);
  // }
  string sectionName = matches.str(1);

  //procces previous section
  currentSection = symbolTable.find(currentSectionName)->second;
  currentSection.size = locationCounter;

//adding new section
  // for sections symbolNumber will be equal to sectionNumber
  currentSectionNumber = currentSymbolNumber;
  currentSectionName = sectionName;

  SymbolTableEntry newEntry = SymbolTableEntry(sectionName, currentSectionName, currentSectionNumber,0,currentSymbolNumber++, false, true,{}, {}, 0);
  SectionTableEntry sectionTableEntry = SectionTableEntry(currentSectionName, currentSectionNumber);

  //if section exists
  if(symbolTable.find(sectionName) != symbolTable.end()) {
    currentSection = symbolTable.find(sectionName)->second;
    locationCounter = currentSection.size;
  }else{ //if section does not exist yet
    currentSection = newEntry;
    symbolTable[sectionName] = newEntry;
    sectionTable[sectionName] = sectionTableEntry;
    locationCounter = 0;
    // relocationTable[currentSectionName] = {};
  }
}

bool isWord(string currentLine){
    regex reg ("(.word)");
    smatch matches;

    while(std::regex_search(currentLine, matches, reg)){
        // std::cout << "Nadjena .word direktiva  " << "\n";
        return matches.ready();
    }
    return false;
}

void processWord(string currentLine){
  if(processWordSymbolList(currentLine)) return;
  if(processWordLiteral(currentLine)) return;
  
}

bool processWordLiteral(string currentLine){
  //regex reg (R"((?:^|\s|,)([+-]?[[:digit:]]+(?:\.[[:digit:]]+)?)(?=$|\s|,))");
  regex reg ("((0x\\w+)|[0-9]+)");
  smatch matches;

  while(std::regex_search(currentLine, matches, reg)){
      string currVal = matches.str(1);
      currentLine = matches.suffix().str();

      int intVal = myStoi(currVal);
      addWordToCode(intVal);
      return true;
  }
  return false;
}

bool processWordSymbolList(string currentLine){
  regex reg ("([_a-zA-Z][_a-zA-Z0-9]*)");
  smatch matches;
  bool retVal = false;

  while(std::regex_search(currentLine, matches, reg)){
      string currVar = matches.str(1);
      currentLine = matches.suffix().str();
      handleSymbol(currVar, "ABS");
      retVal = true;
  }
  return retVal;
}

bool isSkip(string currentLine){
    regex reg ("(.skip)");
    smatch matches;
    while(std::regex_search(currentLine, matches, reg)){
        // std::cout << "Nadjena .skip direktiva  " << "\n";
        return matches.ready();
    }
    return false;
}

void processSkip(string currentLine){
  regex reg ("([0-9]+)");
  smatch matches;
  while(std::regex_search(currentLine, matches, reg)){
    string currVal = matches.str(1);
    currentLine = matches.suffix().str();

    int intVal = stoi(currVal);
    for(int i=0; i<intVal; i++){
      sectionTable[currentSectionName].code.push_back(0);
      sectionTable[currentSectionName].size++;
      symbolTable[currentSectionName].size++;
      locationCounter++;
    }
  }
}

bool isEnd(string currentLine){
  regex reg ("(.end)");
  smatch matches;
  while(std::regex_search(currentLine, matches, reg)){
    // std::cout << "Nadjena .end direktiva  " << "\n";
    return matches.ready();
  }
  return false;
}

void processEnd(){
  currentSection = symbolTable.find(currentSectionName)->second;
  currentSection.size = locationCounter;
}

//HELPERS
void addWordToCode(int value){
  unsigned char dataHigh = (unsigned) value >> 8;
  unsigned char dataLow = (unsigned) value & 0xFF;
  sectionTable[currentSectionName].code.push_back(dataLow);
  sectionTable[currentSectionName].code.push_back(dataHigh);

  sectionTable[currentSectionName].size += 2;
  symbolTable[currentSectionName].size +=2;
  locationCounter += 2;
}
