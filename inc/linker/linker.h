#pragma once
using namespace std;
#include <iostream>
#include <string>
#include <fstream>
#include <map>
#include <list>
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

struct SymbolTableEntry{

  string symbolName;
  string sectionName;
  int value = 0;
  int symbolNumber;
  bool isDefined = false;
  int size = -1;
  int currentFileNumber = -1;

  SymbolTableEntry(int symbolNumber, int value, string symbolName, string sectionName, bool isDefined, int size, int currentFileNumber){
    this->symbolName = symbolName;
    this->sectionName = sectionName;
    this->value = value;
    this->symbolNumber = symbolNumber;
    this->isDefined = isDefined;
    this->size = size;
    this->currentFileNumber = currentFileNumber;
  }
  SymbolTableEntry(){};
};

struct RelocationTableEntry{
  string type; //ABS or PC_REL
  int offset = 0;
  string symbolTableRef;

  RelocationTableEntry(string type, int offset, string symbolTableRef){
    this->type = type;
    this->offset = offset;
    this->symbolTableRef = symbolTableRef;
  };
  RelocationTableEntry(){};
};

struct SectionInfoEntry{

    vector<unsigned char> code;
    int startingAddress = -1;
    vector <RelocationTableEntry> relocations;

};

void generateLinkerOutput(string output);
void processLine(string currentLine);
void processSymbolTable(string currentLine);
void processCode(string currentLine);
void processRelocation(string currentLine);
void linkFiles();
void resolveRelocations();
void checkForUndefinedSymbols();
void patchSymbolValues();