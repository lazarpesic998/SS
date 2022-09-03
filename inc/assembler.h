#pragma once

using namespace std;
#include <iostream>
#include <string>
#include <fstream>
#include <regex>
#include <list>
#include <map>
#include <iomanip>
#include <locale>
#include <set>
#include <algorithm>
#include <experimental/filesystem>


struct RelocationTableEntry{
  string sectionName;
  string type;//0-apsolutno adresiranje, 1 - pc relativno adresiranje
  int offset = 0;
  string symbolTableRef;

  RelocationTableEntry(string sectionName, string type, int offset, string symbolTableRef){
    this->sectionName = sectionName;
    this->type = type;
    this->offset = offset;
    this->symbolTableRef = symbolTableRef;
  };
  RelocationTableEntry(){};
};

struct SymbolTableEntry{

  string symbolName;
  int sectionNumber = 0;
  string sectionName;
  int value = 0;
  int symbolNumber;
  bool isGlobal = false;
  bool isDefined = false;
  list<int> flink = {};
  list<string> flinkSections = {};
  int size = -1;

  SymbolTableEntry(string symbolName, string sectionName, int sectionNumber, int value, int symbolNumber, bool isGlobal, bool isDefined, list<int> flink, list<string> flinkSections, int size){
    this->symbolName = symbolName;
    this->sectionName = sectionName;
    this->sectionNumber = sectionNumber;
    this->value = value;
    this->symbolNumber = symbolNumber;
    this->isGlobal = isGlobal;
    this->isDefined = isDefined;
    this->flink = flink;
    this->flinkSections = flinkSections;
    this->size = size;
  }
  SymbolTableEntry(){};
};

struct SectionTableEntry{
  string sectionName;
  int sectionNumber = 0;
  long startingAddress = 0;
  long size = 0;
  vector<unsigned char> code;

  SectionTableEntry(string sectionName, int sectionNumber){
    this->sectionName = sectionName;
    this->sectionNumber = sectionNumber;
  }
  SectionTableEntry(){};
};

extern map <string, SymbolTableEntry> symbolTable;
extern map <string, SectionTableEntry> sectionTable;
extern int currentSymbolNumber;
extern int currentSectionNumber;
extern string currentSectionName;
extern SymbolTableEntry currentSection;
extern int locationCounter;
extern bool halt;
extern map<string, int> instructionTable;
// extern map <string, list<RelocationTableEntry>> relocationTable;
extern list<RelocationTableEntry> relocationTable;

void setupAssembler();
bool processLine(string currentLine);
string trimComments(string currentLine);
string trim(const string &s);
bool processDirective(string currentLine);
bool processInstruction(string currentLine);
bool isLabel(string currentLine);
string processLabel(string currentLine);

void backpatching(string labelName, int value);
void handleSymbol(string symbolName, string relocationType);
int findReg(string reg);
char createRegByte(int regD, int regS);
int myStoi(string literal);

void printSymbolTable();
void printSectionTable();
void printSymbolTableInFile(string output);
void printSectionTableInFile(string output);
void fixRelocations();
void generateOutput(string outputFile);
vector<pair<string, SymbolTableEntry>> sortMapToVectorPairs();


