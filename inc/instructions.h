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
void processJumpOperand(string currentLine);
bool absoluteJumpSymbol(string currentLine);
bool absoluteJumpLiteral(string currentLine);