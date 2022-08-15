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

    //

}

bool absoluteJumpLiteral(string currentLine){
    regex reg (R"((?:^|\s|,)([+-]?[[:digit:]]+(?:\.[[:digit:]]+)?)(?=$|\s|,))");
    smatch matches;

    if(std::regex_search(currentLine, matches, reg)){
        //RegsDescr
        addByteToCode(0xFF);
        //AddrMode
        addByteToCode(0x00);
        addWordToCode(stoi(matches.str(1)));
        return true;
    }
    return false;
}

bool absoluteJumpSymbol(string currentLine){

    regex reg (R"((?:^|\s|,)([+-]?[[:w:]]+(?:\.[[:w:]]+)?)(?=$|\s|,))");
    smatch matches;

    if(std::regex_search(currentLine, matches, reg)){
        //RegsDescr
        addByteToCode(0xFF);
        //AddrMode
        addByteToCode(0x00);

        string currVar = matches.str(1);
        if(symbolTable.find(currVar) != symbolTable.end()) {
            //ako postoji u tabeli simbola i definisan je
            if(symbolTable[currVar].isDefined){
            addWordToCode(symbolTable[currVar].value);
            }else{//ako postoji u tabeli simbola i nije definisan
            symbolTable[currVar].flink.push_back(locationCounter);
            addWordToCode(0);
            }
        }else{ //ako ne postoji u tabeli simbola
            SymbolTableEntry newEntry = SymbolTableEntry(currVar,currentSectionNumber,0,currentSymbolNumber++, false, false,{locationCounter}, -1);
            symbolTable[currVar] = newEntry;
            addWordToCode(0);
        }
        
    }
    return false;
}

void absoluteAddressing(int value){
    //RegsDescr
    addByteToCode(0xFF);
    //AddrMode
    addByteToCode(0x00);
    addWordToCode(value);
}
