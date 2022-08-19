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
void processDataOperand(string currentLine);
bool absoluteDataSymbol(string currentLine);
bool absoluteDataLiteral(string currentLine);
bool pcRelativeDataSymbol(string currentLine);
bool memoryDirectDataLiteral(string currentLine);
bool memoryDirectDataSymbol(string currentLine);
bool registerDirectData(string currentLine);
bool registerIndirectData(string currentLine);
bool registerIndirectDataWithOffsetLiteral(string currentLine);
bool registerIndirectDataWithOffsetSymbol(string currentLine);