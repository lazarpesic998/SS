#include "../../inc/linker/linker.h"


//VARIABLES
map <string, SymbolTableEntry> symbolTable;
string currentPhase = "symbolTable";
string currentSectionName = "UND";

vector< map <string, SectionInfoEntry> > filesInfoList;
vector < vector<string> > fileSectionInfo;
int currentFileNumber = -1;

vector<unsigned char> code;


int main(int argc, char** argv)
{
   if(argc <= 2){
      cout << "LINKER: Wrong arguments number." << endl;
      return -1;
   }

   string option = argv[1];
   string output = argv[3];
   fstream inputFile;

   //setupLinker();

    fstream outputFile;
    outputFile.open("../../bin/"+output,ios::out);  // open a file to perform write operation using file object
    if(outputFile.is_open()) //checking whether the file is open
    {

        for(int i=4; i<argc; i++){
            string input = argv[i];
            inputFile.open("../../bin/"+input, ios::in); //open a file to perform read operation using file object
            if (inputFile.is_open()){   //checking whether the file is open
                
                //create new entry in fileInfoList
                std::map<string, SectionInfoEntry> m;
                filesInfoList.push_back(m);
                currentFileNumber++;

                //initialize fileSectionInfo to empty vector 
                vector<string> v = {};
                fileSectionInfo.push_back(v);

                string currentLine;
                while(getline(inputFile, currentLine)){
                    //cout << currentLine << endl; //read data from file object and put it into string.
                    //outputFile << currentLine << endl; //print the data of the string
                    processLine(currentLine);
                }
                inputFile.close(); //close the file object.
            }
        }

        linkFiles();
        resolveRelocations();

        outputFile.close();    //close the file object
    }
    return 0;
}

void setupLinker(string output){

}

void processLine(string currentLine){
    if(currentLine == "Symbol table") {currentPhase = "symbolTable"; return;}
    if(currentLine == "###") {currentPhase = "sectionName"; return;}
    if(currentLine == "Relocation table: ") {currentPhase = "relocations"; return;}

    
    if(currentPhase == "symbolTable") {processSymbolTable(currentLine); return;}
    if(currentPhase == "sectionName") {
        currentPhase = "code";
        currentSectionName = currentLine;
        fileSectionInfo.at(currentFileNumber).push_back(currentSectionName);
        return;
    }
    if(currentPhase == "code") {processCode(currentLine);  return;}
    if(currentPhase == "relocations") { processRelocation(currentLine); return;}

}

void processSymbolTable(string currentLine){
    std::string delimiter = "|";

    int symbolNumber = stoi(currentLine.substr(0, currentLine.find(delimiter)));
    // cout << symbolNumber << endl;
    currentLine.erase(0, currentLine.find(delimiter) + delimiter.length()); 

    int symbolValue = stoi(currentLine.substr(0, currentLine.find(delimiter)));
    // cout << symbolValue << endl;
    currentLine.erase(0, currentLine.find(delimiter) + delimiter.length());  

    string symbolName = currentLine.substr(0, currentLine.find(delimiter));
    // cout << symbolName << endl;
    currentLine.erase(0, currentLine.find(delimiter) + delimiter.length());  

    string sectionName = currentLine.substr(0, currentLine.find(delimiter));
    currentLine.erase(0, currentLine.find(delimiter) + delimiter.length());  

    int isDefined = stoi(currentLine.substr(0, currentLine.find(delimiter)));
    currentLine.erase(0, currentLine.find(delimiter) + delimiter.length());  

    int size = stoi(currentLine.substr(0, currentLine.find(delimiter)));
    currentLine.erase(0, currentLine.find(delimiter) + delimiter.length());  

     if(symbolTable.find(symbolName) != symbolTable.end() && symbolName != sectionName) {

        if(symbolTable[symbolName].isDefined && isDefined){
            cout<< "***ERROR!*** MULTIPLE SYMBOL DEFINITION!" << endl;
            exit(-1);
        }else if(isDefined){
            symbolTable[symbolName].value = symbolValue;
            symbolTable[symbolName].sectionName = sectionName;
            symbolTable[symbolName].isDefined = isDefined;
        }
    }

    SymbolTableEntry newEntry = SymbolTableEntry(symbolNumber, symbolValue, symbolName, sectionName, isDefined, size);
    symbolTable[symbolName] = newEntry;

}
void processCode(string currentLine){
    std::string delimiter = "|";

    size_t pos = 0;
    std::string token;
    unsigned char byte;
    while ((pos = currentLine.find(delimiter)) != std::string::npos) {
        token = currentLine.substr(0, pos);
        byte =  stoi(token, 0, 16);
        currentLine.erase(0, pos + delimiter.length());

        filesInfoList.at(currentFileNumber)[currentSectionName].code.push_back(byte);
    }
}
void processRelocation(string currentLine){
    std::string delimiter = "|";

    string type = currentLine.substr(0, currentLine.find(delimiter));
    currentLine.erase(0, currentLine.find(delimiter) + delimiter.length()); 

    int offset = stoi(currentLine.substr(0, currentLine.find(delimiter)), 0, 16);
    currentLine.erase(0, currentLine.find(delimiter) + delimiter.length());  

    string symbolName = currentLine.substr(0, currentLine.find(delimiter));
    currentLine.erase(0, currentLine.find(delimiter) + delimiter.length());

    RelocationTableEntry newRelocation = RelocationTableEntry(type, offset, symbolName);
    filesInfoList.at(currentFileNumber)[currentSectionName].relocations.push_back(newRelocation);


}

void linkFiles(){

    for(int i=0; i < filesInfoList.size(); i++){
        while( fileSectionInfo.at(i).size() != 0){
            string currSection = fileSectionInfo.at(i).at(0);
            fileSectionInfo.at(i).erase(fileSectionInfo.at(i).begin());

            //ako vec nije kopiran kod
            if(filesInfoList.at(i)[currSection].startingAddress == -1){
                //copy code and set starting address for section
                filesInfoList.at(i)[currSection].startingAddress = code.size();
                for( int j=0; j < filesInfoList.at(i)[currSection].code.size(); j++){
                    code.push_back(filesInfoList.at(i)[currSection].code.at(j));
                }

                //check if in any other file is section with the same name
                for(int k=i; k < filesInfoList.size(); k++){
                    //ako postoji kopiraj njen sadrzaj u nastavku, proveri da li je vec kopirano ranije
                    if(filesInfoList.at(k).find(currSection) != filesInfoList.at(k).end())
                    {
                        if(filesInfoList.at(k)[currSection].startingAddress == -1){ //za svaki slucaj provera da li je vec kopiran kod
                            filesInfoList.at(k)[currSection].startingAddress = code.size();
                            for( int j=0; j < filesInfoList.at(k)[currSection].code.size(); j++){
                                code.push_back(filesInfoList.at(k)[currSection].code.at(j));
                            }
                        }
                    }
                }
            }
            
        }
    }

}

void resolveRelocations(){

}

void generateLinkerOutput(string output){
    ofstream myfile;
    myfile.open (output, fstream::app);
    myfile << output;
    myfile.close();
}