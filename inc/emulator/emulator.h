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

void resetProcessor();
void emulate();

short popStack();

//NO OPERANDS INSTRUCTIONS
void processIRET();
void processRET();

//INSTRUCTIONS WITH REGISTARS ONLY
void processINT();
void processXCHG();
void processADD();
void processSUB();
void processMUL();
void processDIV();
void processCMP();
void processNOT();
void processAND();
void processOR();
void processXOR();
void processTEST();
void processSHL();
void processSHR();


//FETCHING INSTRUCTIONS
 void fetchRegisters();

 //HELPER FUNCTIONS
 void setFlag(short flag, bool isFlag);
