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

struct SymbolTableEntry{

  string symbolName;
  int sectionNumber = 0;
  int value = 0;
  int symbolNumber;
  bool isGlobal = false;
  bool isDefined = false;
  list<int> flink = {};
  int size = -1;

  SymbolTableEntry(string symbolName, int sectionNumber, int value, int symbolNumber, bool isGlobal, bool isDefined, list<int> flink, int size){
    this->symbolName = symbolName;
    this->sectionNumber = sectionNumber;
    this->value = value;
    this->symbolNumber = symbolNumber;
    this->isGlobal = isGlobal;
    this->isDefined = isDefined;
    this->flink = flink;
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

void setupAssembler();
bool processLine(string currentLine);
string trimComments(string currentLine);
string trim(const string &s);
bool processDirective(string currentLine);
bool processInstruction(string currentLine);
bool isLabel(string currentLine);
string processLabel(string currentLine);
void backpatching(string labelName, int value);

void printSymbolTable();
void printSectionTable();
void generateOutput(string outputFile);

