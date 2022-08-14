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

bool isGlobalDirective(string currentLine);
void processGlobalDirective(string currentLine);
bool isExtern(string currentLine);
void processExtern(string currentLine);
bool isSection(string currentLine);
void processSection(string currentLine);
bool isWord(string currentLine);
void processWord(string currentLine);
void processWordLiteral(string currentLine);
void processWordSymbolList(string currentLine);
bool isSkip(string currentLine);
void processSkip(string currentLine);
bool isEnd(string currentLine);
void processEnd();
void addIntToCode(int value);