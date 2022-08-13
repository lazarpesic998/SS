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

bool isHalt(string currentLine);
void processHalt(string currentLine);
bool isInt(string currentLine);
void procesInt(string currentLine);
bool isIret(string currentLine);
void processIret(string currentLine);
bool isCall(string currentLine);
void processCall(string currentLine);
bool isRet(string currentLine);
void processRet(string currentLine);
//naredbe skoka
bool isJmp(string currentLine);
void processJmp(string currentLine);
bool isJeq(string currentLine);
void processJeq(string currentLine);
bool isJne(string currentLine);
void processJne(string currentLine);
bool isJgt(string currentLine);
void processJgt(string currentLine);
//push i pop
bool isPush(string currentLine);
void processPush(string currentLine);
bool isPop(string currentLine);
void processPop(string currentLine);
//artitmeticke naredbe
bool isXchg(string currentLine);
void processXchg(string currentLine);
bool isAdd(string currentLine);
void processAdd(string currentLine);
bool isSub(string currentLine);
void processSub(string currentLine);
bool isMul(string currentLine);
void processMul(string currentLine);
bool isDiv(string currentLine);
void processDiv(string currentLine);
//logicke naredbe
bool isCmp(string currentLine);
void processCmp(string currentLine);
bool isNot(string currentLine);
void processNot(string currentLine);
bool isAnd(string currentLine);
void processAnd(string currentLine);
bool isOr(string currentLine);
void processOr(string currentLine);
bool isXor(string currentLine);
void processXor(string currentLine);
bool isTest(string currentLine);
void processTest(string currentLine);
bool isShl(string currentLine);
void processShl(string currentLine);
bool isShr(string currentLine);
void processShr(string currentLine);
//load i store
bool isLdr(string currentLine);
void processLdr(string currentLine);
bool isStr(string currentLine);
void processStr(string currentLine);