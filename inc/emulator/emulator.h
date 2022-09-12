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
#include <bitset>

void resetProcessor();
void emulate();

void pushStack(short value);
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

void processCALL();
void processJMP();
void processJEQ();
void processJNE();
void processJGT();
void processLDR();
void processSTR();


//FETCHING INSTRUCTIONS
void fetchRegisters();
string fetchAddrMode();
short fetchTwoBytesFromMemory(int location);
short fetchOperand();
void storeToMemory(short value, int location);

 //HELPER FUNCTIONS
 void setFlag(short flag, bool isFlag);
 void generateOutput();
