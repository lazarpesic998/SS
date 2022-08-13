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
  if (myLine.size() == 0) return true;

  if(isEnd(myLine)) {processEnd(); return false;}
  if (processDirective(myLine)) return true;

  //TODO: mozda je HALT kao posebna naredba
  if(processInstruction(currentLine)) return false;

  //TODO: change to return false after everything done maybe
  return true;
}

bool processDirective(string currentLine){

  if(isGlobalDirective(currentLine)){processGlobalDirective(currentLine); return true;}
  if(isExtern(currentLine)){processExtern(currentLine); return true;}
  if(isSection(currentLine)){processSection(currentLine); return true;}
  if(isWord(currentLine)){processWord(currentLine); return true;}
  if(isSkip(currentLine)){processSkip(currentLine); return true;}
  return false;
}

bool processIntstruction(string currentLine){

  if(isHalt(currentLine)){processHalt(currentLine); return true;}
  if(isInt(currentLine)){procesInt(currentLine); return true;}
  if(isIret(currentLine)){processIret(currentLine); return true;}
  if(isCall(currentLine)){processCall(currentLine); return true;}
  if(isRet(currentLine)){processRet(currentLine); return true;}
  //naredbe skoka
  if(isJmp(currentLine)){processJmp(currentLine); return true;}
  if(isJeq(currentLine)){processJeq(currentLine); return true;}
  if(isJne(currentLine)){processJne(currentLine); return true;}
  if(isJgt(currentLine)){processJgt(currentLine); return true;}
  //push i pop
  if(isPush(currentLine)){processPush(currentLine); return true;}
  if(isPop(currentLine)){processPop(currentLine); return true;}
  //artitmeticke naredbe
  if(isXchg(currentLine)){processXchg(currentLine); return true;}
  if(isAdd(currentLine)){processAdd(currentLine); return true;}
  if(isSub(currentLine)){processSub(currentLine); return true;}
  if(isMul(currentLine)){processMul(currentLine); return true;}
  if(isDiv(currentLine)){processDiv(currentLine); return true;}
  //logicke naredbe
  if(isCmp(currentLine)){processCmp(currentLine); return true;}
  if(isNot(currentLine)){processNot(currentLine); return true;}
  if(isAnd(currentLine)){processAnd(currentLine); return true;}
  if(isOr(currentLine)){processOr(currentLine); return true;}
  if(isXor(currentLine)){processXor(currentLine); return true;}
  if(isTest(currentLine)){processTest(currentLine); return true;}
  if(isShl(currentLine)){processShl(currentLine); return true;}
  if(isShr(currentLine)){processShr(currentLine); return true;}
  //load i store
  if(isLdr(currentLine)){processLdr(currentLine); return true;}
  if(isStr(currentLine)){processStr(currentLine); return true;}


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