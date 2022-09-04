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

void addByteToCode(unsigned char value);
void addRegistersInfo(string regD, string regS);

//jump instructions
void processJumpOperand(string currentLine);
bool absoluteJumpSymbol(string currentLine);
bool absoluteJumpLiteral(string currentLine);
bool pcRelativeJumpSymbol(string currentLine);
bool memoryDirectJumpLiteral(string currentLine);
bool memoryDirectJumpSymbol(string currentLine);
bool registerDirectJump(string currentLine);
bool registerIndirectJump(string currentLine);
bool registerIndirectJumpWithOffsetLiteral(string currentLine);
bool registerIndirectJumpWithOffsetSymbol(string currentLine);

//data instructions
void processDataOperand(string instructionName, int firstReg, string currentLine);
bool absoluteDataSymbol(string instructionName, int firstReg, string currentLine);
bool absoluteDataLiteral(string instructionName, int firstReg, string currentLine);
bool pcRelativeDataSymbol(string instructionName, int firstReg, string currentLine);
bool memoryDirectDataLiteral(string instructionName, int firstReg, string currentLine);
bool memoryDirectDataSymbol(string instructionName, int firstReg, string currentLine);
bool registerDirectData(string instructionName, int firstReg, string currentLine);
bool registerIndirectData(string instructionName, int firstReg, string currentLine);
bool registerIndirectDataWithOffsetLiteral(string instructionName, int firstReg, string currentLine);
bool registerIndirectDataWithOffsetSymbol(string instructionName, int firstReg, string currentLine);
int getFirstReg(string currentLine);
void handleOneRegister(int firstReg);
bool processPush(string currentLine);
bool processPop(string currentLine);
