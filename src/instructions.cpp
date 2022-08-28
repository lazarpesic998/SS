#include "../inc/assembler.h"
#include "../inc/instructions.h"
#include "../inc/directives.h"


void processHalt(string currentLine){
    regex reg ("([_a-zA-Z][_a-zA-Z0-9]*)");
    smatch matches;

    while(std::regex_search(currentLine, matches, reg)){
        string currSymbol = matches.str(1);
        currentLine = matches.suffix().str();
        if(currSymbol == "halt"){
            halt = true;
            break;
        }
    }
}

void addByteToCode(unsigned char value){
    sectionTable[currentSectionName].code.push_back(value);
    sectionTable[currentSectionName].size++;
    symbolTable[currentSectionName].size++;
    locationCounter++;
}

void addRegistersInfo(string regD, string regS){
    int regDest = atoi(&regD[1]);
    int regSrc = atoi(&regS[1]);
    if(regS == "") regSrc = 0xF;
    char regByte = regSrc & 0xF;
    regByte |= (regDest & 0xF)<<4;
    addByteToCode(regByte);
}

void processJumpOperand(string currentLine){
    //neposredno adresiranje
    if(absoluteJumpLiteral(currentLine)) return;
    if(absoluteJumpSymbol(currentLine)) return;

    //PC relativno adresiranje
    if(pcRelativeJumpSymbol(currentLine)) return;

    //registarsko direktno adresiranje
    if(registerDirectJump(currentLine)) return;

    //memorijsko direktno adresiranje
    if(memoryDirectJumpLiteral(currentLine)) return;
    if(memoryDirectJumpSymbol(currentLine)) return;

    //registrarsko indirektno
    if(registerIndirectJump(currentLine)) return;

    //registrarsko indirektno sa pomerajem
    if(registerIndirectJumpWithOffsetLiteral(currentLine)) return;
    if(registerIndirectJumpWithOffsetSymbol(currentLine)) return;

}

bool absoluteJumpLiteral(string currentLine){
    // regex reg (R"((?:^|\s|,)([+-]?[[:digit:]]+(?:\.[[:digit:]]+)?)(?=$|\s|,))");
    regex reg ("((0x\\w+)|[0-9]+)");

    smatch matches;

    if(std::regex_match(currentLine, matches, reg)){
        //RegsDescr
        addByteToCode(0xFF);
        //AddrMode
        addByteToCode(0x00);

        string literal = matches.str(1);
        addWordToCode(myStoi(literal));
        return true;
    }
    return false;
}

bool absoluteJumpSymbol(string currentLine){

    regex reg (R"((?:^|\s|,)([+-]?[[:w:]]+(?:\.[[:w:]]+)?)(?=$|\s|,))");
    smatch matches;

    if(std::regex_match(currentLine, matches, reg)){
        //RegsDescr
        addByteToCode(0xFF);
        //AddrMode
        addByteToCode(0x00);
        string currVar = matches.str(1);
        handleSymbol(currVar, "abs");
        return true;
    }
    return false;
}

//registarsko direktno sa pomerajem
bool pcRelativeJumpSymbol(string currentLine){
    regex reg ("(%[_a-zA-Z][_a-zA-Z0-9]*)");
    smatch matches;

    if(std::regex_search(currentLine, matches, reg)){
        //RegsDescr
        addByteToCode(0xF7);
        //AddrMode - registarsko direktno sa pomerajem
        addByteToCode(0x05);

        string currVar = matches.str(1);
        currVar.erase(0,1); // removes first character

        handleSymbol(currVar, "PC_REL");
        return true;
    }
    return false;
}

//memorijsko direktno adresiranje
bool memoryDirectJumpLiteral(string currentLine){
    //regex reg (R"((?:^|\s|,)([*][[:digit:]]+(?:\.[[:digit:]]+)?)(?=$|\s|,))");
    regex reg ("\\*((0x\\w+)|[0-9]+)");
    smatch matches;

    if(std::regex_search(currentLine, matches, reg)){
        //RegsDescr
        addByteToCode(0xFF);
        //AddrMode
        addByteToCode(0x04);
        string literal = matches.str(1);
        addWordToCode(myStoi(literal));
        return true;
    }
    return false;
}

bool memoryDirectJumpSymbol(string currentLine){

    regex reg ("(\\*[_a-zA-Z][_a-zA-Z0-9]*)");
    smatch matches;

    if(std::regex_search(currentLine, matches, reg)){
        //RegsDescr
        addByteToCode(0xFF);
        //AddrMode
        addByteToCode(0x04);
        string currVar = matches.str(1);
        handleSymbol(currVar, "abs");
        return true;
    }
    return false;
}

//registarsko direktno adresiranje - (3B)
bool registerDirectJump(string currentLine){

    regex reg ("\\*(sp|pc|psw|r[0-9])");
    smatch matches;

    if(std::regex_search(currentLine, matches, reg)){

        string currVar = matches.str(1);
        int regD = 0xF;
        int regS = findReg(currVar);
        unsigned char regDescr = createRegByte(regD, regS);

        //RegsDescr
        addByteToCode(regDescr);
        //AddrMode
        addByteToCode(0x01);
        return true;
    }
    return false;
}

//registarsko indirektno adresiranje - (3B)
bool registerIndirectJump(string currentLine){

    regex reg ("\\*\\[(sp|pc|psw|r[0-9])\\]");
    smatch matches;

    if(std::regex_search(currentLine, matches, reg)){

        string currVar = matches.str(1);
        int regD = 0xF;
        int regS = findReg(currVar);
        unsigned char regDescr = createRegByte(regD, regS);

        //RegsDescr
        addByteToCode(regDescr);
        //AddrMode
        addByteToCode(0x02);
        return true;
    }
    return false;
}

//registarsko indirektno adresiranje sa pomerajem - (5B)
bool registerIndirectJumpWithOffsetLiteral(string currentLine){
    //          \*\[(sp|pc|psw|r[0-9])\+(0x\w+|[0-9]+)\]
    regex reg ("(\\*\\[(sp|pc|psw|r[0-9]) \\+ ((0x\\w+)|[0-9]+)\\])");
    smatch matches;

    if(std::regex_search(currentLine, matches, reg)){

        string regSrc = matches.str(2);
        string literal = matches.str(3);
        int regD = 0xF;
        int regS = findReg(regSrc);
        unsigned char regDescr = createRegByte(regD, regS);

        //RegsDescr
        addByteToCode(regDescr);
        //AddrMode
        addByteToCode(0x03);
        addWordToCode(myStoi(literal));
        return true;
    }
    return false;
}

bool registerIndirectJumpWithOffsetSymbol(string currentLine){
    regex reg ("(\\*\\[(sp|pc|psw|r[0-9]) \\+ ([_a-zA-Z][_a-zA-Z0-9]*)\\])");
    smatch matches;

    if(std::regex_search(currentLine, matches, reg)){

        string regSrc = matches.str(2);
        string symbol = matches.str(3);
        int regD = 0xF;
        int regS = findReg(regSrc);
        unsigned char regDescr = createRegByte(regD, regS);

        //RegsDescr
        addByteToCode(regDescr);
        //AddrMode
        addByteToCode(0x03);
        handleSymbol(symbol, "abs");
        return true;
    }
    return false;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//DATA OPERAND LD AND STR
void processDataOperand(string currentLine){

    //neposredno adresiranje
    if(absoluteDataLiteral(currentLine)) return;
    if(absoluteDataSymbol(currentLine)) return;

    //PC relativno adresiranje
    if(pcRelativeDataSymbol(currentLine)) return;

    //registarsko direktno adresiranje
    if(registerDirectData(currentLine)) return;

    //memorijsko direktno adresiranje
    if(memoryDirectDataLiteral(currentLine)) return;
    if(memoryDirectDataSymbol(currentLine)) return;

    //registrarsko indirektno
    if(registerIndirectData(currentLine)) return;

    //registrarsko indirektno sa pomerajem
    if(registerIndirectDataWithOffsetLiteral(currentLine)) return;
    if(registerIndirectDataWithOffsetSymbol(currentLine)) return;
}


bool absoluteDataLiteral(string currentLine){
    // regex reg (R"((?:^|\s|,)([+-]?[[:digit:]]+(?:\.[[:digit:]]+)?)(?=$|\s|,))");
    regex reg ("$((0x\\w+)|[0-9]+)");

    smatch matches;

    if(std::regex_match(currentLine, matches, reg)){
        //RegsDescr
        addByteToCode(0xFF);
        //AddrMode
        addByteToCode(0x00);

        string literal = matches.str(1);
        addWordToCode(myStoi(literal));
        return true;
    }
    return false;
}

bool absoluteDataSymbol(string currentLine){

    // regex reg (R"((?:^|\s|,)([+-]?[[:w:]]+(?:\.[[:w:]]+)?)(?=$|\s|,))");
    regex reg ("$([_a-zA-Z][_a-zA-Z0-9]*)");
    smatch matches;

    if(std::regex_match(currentLine, matches, reg)){
        //RegsDescr
        addByteToCode(0xFF);
        //AddrMode
        addByteToCode(0x00);
        string currVar = matches.str(1);
        handleSymbol(currVar, "abs");
        return true;
    }
    return false;
}

//memorijsko direktno adresiranje
bool memoryDirectDataLiteral(string currentLine){
    //regex reg (R"((?:^|\s|,)([*][[:digit:]]+(?:\.[[:digit:]]+)?)(?=$|\s|,))");
    regex reg ("((0x\\w+)|[0-9]+)");
    smatch matches;

    if(std::regex_search(currentLine, matches, reg)){
        //RegsDescr
        addByteToCode(0xFF);
        //AddrMode
        addByteToCode(0x04);
        string literal = matches.str(1);
        addWordToCode(myStoi(literal));
        return true;
    }
    return false;
}

bool memoryDirectDataSymbol(string currentLine){

    regex reg ("([_a-zA-Z][_a-zA-Z0-9]*)");
    smatch matches;

    if(std::regex_search(currentLine, matches, reg)){
        //RegsDescr
        addByteToCode(0xFF);
        //AddrMode
        addByteToCode(0x04);
        string currVar = matches.str(1);
        handleSymbol(currVar, "abs");
        return true;
    }
    return false;
}

//registarsko direktno sa pomerajem
bool pcRelativeDataSymbol(string currentLine){
    regex reg ("(%[_a-zA-Z][_a-zA-Z0-9]*)");
    smatch matches;

    if(std::regex_search(currentLine, matches, reg)){
        //RegsDescr
        addByteToCode(0xF7);
        //AddrMode - registarsko indirektno sa pomerajem
        addByteToCode(0x03);

        string currVar = matches.str(1);
        currVar.erase(0,1); // removes first character

        handleSymbol(currVar, "PC_REL");
        return true;
    }
    return false;
}

//registarsko direktno adresiranje - (3B)
bool registerDirectData(string currentLine){

    regex reg ("(sp|pc|psw|r[0-9])");
    smatch matches;

    if(std::regex_search(currentLine, matches, reg)){

        string currVar = matches.str(1);
        int regD = 0xF;
        int regS = findReg(currVar);
        unsigned char regDescr = createRegByte(regD, regS);

        //RegsDescr
        addByteToCode(regDescr);
        //AddrMode
        addByteToCode(0x01);
        return true;
    }
    return false;
}

//registarsko indirektno adresiranje - (3B)
bool registerIndirectData(string currentLine){

    regex reg ("\\[(sp|pc|psw|r[0-9])\\]");
    smatch matches;

    if(std::regex_search(currentLine, matches, reg)){

        string currVar = matches.str(1);
        int regD = 0xF;
        int regS = findReg(currVar);
        unsigned char regDescr = createRegByte(regD, regS);

        //RegsDescr
        addByteToCode(regDescr);
        //AddrMode
        addByteToCode(0x02);
        return true;
    }
    return false;
}

//registarsko indirektno adresiranje sa pomerajem - (5B)
bool registerIndirectDataWithOffsetLiteral(string currentLine){
    //          \*\[(sp|pc|psw|r[0-9])\+(0x\w+|[0-9]+)\]
    regex reg ("(\\[(sp|pc|psw|r[0-9]) \\+ ((0x\\w+)|[0-9]+)\\])");
    smatch matches;

    if(std::regex_search(currentLine, matches, reg)){

        string regSrc = matches.str(2);
        string literal = matches.str(3);
        int regD = 0xF;
        int regS = findReg(regSrc);
        unsigned char regDescr = createRegByte(regD, regS);

        //RegsDescr
        addByteToCode(regDescr);
        //AddrMode
        addByteToCode(0x03);
        addWordToCode(myStoi(literal));
        return true;
    }
    return false;
}

bool registerIndirectDataWithOffsetSymbol(string currentLine){
    regex reg ("(\\[(sp|pc|psw|r[0-9]) \\+ ([_a-zA-Z][_a-zA-Z0-9]*)\\])");
    smatch matches;

    if(std::regex_search(currentLine, matches, reg)){

        string regSrc = matches.str(2);
        string symbol = matches.str(3);
        int regD = 0xF;
        int regS = findReg(regSrc);
        unsigned char regDescr = createRegByte(regD, regS);

        //RegsDescr
        addByteToCode(regDescr);
        //AddrMode
        addByteToCode(0x03);
        handleSymbol(symbol, "abs");
        return true;
    }
    return false;
}


//PUSH AND POP


bool processPush(string currentLine){

    regex reg ("(sp|pc|psw|r[0-9])");
    smatch matches;

    if(std::regex_search(currentLine, matches, reg)){

        string currVar = matches.str(1);
        int regD = findReg(currVar);
        int regS = 0x07; //SP
        unsigned char regDescr = createRegByte(regD, regS);

        //RegsDescr
        addByteToCode(regDescr);
        //AddrMode
        addByteToCode(0x12);
        return true;
    }
    return false;
}

bool processPop(string currentLine){

    regex reg ("(sp|pc|psw|r[0-9])");
    smatch matches;

    if(std::regex_search(currentLine, matches, reg)){

        string currVar = matches.str(1);
        int regD = 0x07; //SP
        int regS = findReg(currVar);
        unsigned char regDescr = createRegByte(regD, regS);

        //RegsDescr
        addByteToCode(regDescr);
        //AddrMode
        addByteToCode(0x42);
        return true;
    }
    return false;
}