#include "../../inc/emulator/emulator.h"


unsigned char memory[65536] = {0};
short r[9] = {0};
string currentInstruction = 0;

int regD;
int regS;

map<short, string> instructionMap = {
  {0x00, "halt"}, //zaustavaljanje procesora
  {0x10, "int"}, //instrukcija softverskog prekida
  {0x20, "iret"}, //instrukcija povratka iz prekidne rutine
  {0x30, "call"}, //instrukcija poziva potprograma
  {0x40, "ret"}, //instrukcija povratka iz potprograma
  //skokovi
  {0x50, "jmp"},
  {0x51, "jeq"},
  {0x52, "jne"},
  {0x53, "jgt"},
  //push, pop
//   {0xB0, "push"},
//   {0xA0, "pop"},
 //instrukcija atomicne zamene vrednosti
  {0x60, "xchg"},
  //aritmeticke operacije
  {0x70, "add"}, 
  {0x71, "sub"},
  {0x72, "mul"},
  {0x73, "div"},
  {0x74, "cmp"},
  //logicke operacije
  {0x80, "not"},
  {0x81, "and"},
  {0x82, "or"},
  {0x83, "xor"},
  {0x84, "test"},
  //pomeracke operacije
  {0x90, "shl"},
  {0x91, "shr"},
  //load i store
  {0xA0, "ldr"},
  {0xB0, "str"}
};

short flagZ = 1;
short flagO = 1 << 1;
short flagC = 1 << 2;
short flagN = 1 << 3;

  
int main(int argc, char** argv)
{
   if(argc != 2){
      cout << "Wrong arguments number.";
      return -1;
   }
   string input = argv[1];
   fstream inputFile;

   inputFile.open("../../bin/" + input, ios::in); //open a file to perform read operation using file object
   if (inputFile.is_open()){   //checking whether the file is open
      
      string currentLine;
      int position = 0;
      while(getline(inputFile, currentLine)){ //read data from file object and put it into string.
         std::string delimiter = ": ";
         size_t pos = currentLine.find(delimiter);
         currentLine.erase(0, pos + delimiter.length());

         delimiter = " ";
         std::string token;
         unsigned char byte;
         while ((pos = currentLine.find(delimiter)) != std::string::npos) {
            token = currentLine.substr(0, pos);
            byte =  stoi(token, 0, 16);
            currentLine.erase(0, pos + delimiter.length());
            memory[position++] = byte;
         }
      }

      // resetProcessor();
      // emulate();

      inputFile.close(); //close the file object.
   }
   return 0;
}

void resetProcessor(){
   
   unsigned char pcLow = memory[0];
   unsigned char pcHigh = memory[1];

   r[8] = 0; //psw
   r[7] = (pcHigh << 8) | (pcLow & 0xFF); //pc
   r[6] = 0xFFFF - 1; //sp
}


void emulate(){

   while(true){
      //fetchInstruction
      currentInstruction = instructionMap[r[7]];
      r[7]++;

      //NO OPERANDS INSTRUCTIONS
      if(currentInstruction == "halt"){
         break;
      }
      if(currentInstruction == "iret"){
         processIRET();
         continue;
      }
      if(currentInstruction == "ret"){
         processRET();
         continue;
      }


      //INSTRUCTIONS WITH REGISTARS ONLY (2B)

      if(currentInstruction == "int"){ processINT(); continue;}
      if(currentInstruction == "xchg"){ processXCHG(); continue;}
      if(currentInstruction == "add"){ processADD(); continue; }
      if(currentInstruction == "sub"){ processSUB(); continue; }
      if(currentInstruction == "mul"){ processMUL(); continue; }
      if(currentInstruction == "div"){ processDIV(); continue; }
      if(currentInstruction == "cmp"){ processCMP(); continue; }
      if(currentInstruction == "not"){ processNOT(); continue; }
      if(currentInstruction == "and"){ processAND(); continue; }
      if(currentInstruction == "or"){ processOR(); continue; }
      if(currentInstruction == "xor"){ processXOR(); continue; }
      if(currentInstruction == "test"){ processTEST(); continue; }
      if(currentInstruction == "shl"){ processSHL(); continue; }
      if(currentInstruction == "shr"){ processSHR(); continue; }



   }
}

short popStack(){

   if(r[6] + 1 >= 0xFFFF) {
      cout << "ERROR! Stack empty!" <<endl;
      exit(-1);
   }
   short retVal = ( memory[r[6]+1] << 8 ) | ( memory[r[6]] & 0xFF );
   r[6] +=2;
   return retVal;
}

void pushStack(short value){
  memory[ r[6] - 1 ] = (value >> 8) & 0xFF; //data high
  memory[ r[6] - 2 ] = value & 0xFF; //data low
  r[6] -= 2;
}

//NO OPERANDS INSTRUCTIONS
void processIRET(){
   //pop psw
   r[8] = popStack();
   //pop pc
   r[7] = popStack();
}

void processRET(){
   //pop pc
   r[7] = popStack();
}

//INSTRUCTIONS WITH REGISTARS ONLY
void processINT(){

   fetchRegisters();
   //push psw
   pushStack(r[8]);
   //pc <= mem16[(regD mod 8)*2]; 
   // TODO: check maybe r[regD]
   r[7] = memory[ (r[regD] % 8) * 2 ];
}

void processXCHG(){
   fetchRegisters();
   // temp <= regD; regD <= regS; regS <= temp;
   short temp = r[regD];
   r[regD] = r[regS];
   r[regS] = temp;
}

void processADD(){
   fetchRegisters();
   //regD <= regD + regS; 
   r[regD] += r[regS];
}
void processSUB(){
   fetchRegisters();
   //regD <= regD - regS; 
   r[regD] -= r[regS];
}
void processMUL(){
   fetchRegisters();
   //regD <= regD * regS; 
   r[regD] *= r[regS];
}
void processDIV(){
   fetchRegisters();
   //regD <= regD / regS; 
   r[regD] /= r[regS];
}
void processCMP(){
   fetchRegisters();
   //temp <= regD - regS; 
   int temp = r[regD] - r[regS];
   
   //FLAGS
   if(temp == 0) setFlag(flagZ, true);
   else setFlag(flagZ, false);

   if(temp < 0) setFlag(flagN, true);
   else setFlag(flagN, false);

   if(temp < -32768 || temp > 32767) setFlag(flagO, true);
   else setFlag(flagO, false);

   //TODO: check carry flag
   if(temp < -32768 || temp > 32767) setFlag(flagC, true);
   else setFlag(flagC, false);
}
void processNOT(){
   fetchRegisters();
   //regD <= ~regD  
   r[regD] = ~r[regD];
}
void processAND(){
   fetchRegisters();
   //regD <= regD & regS; 
   r[regD] &= r[regS];
}
void processOR(){
   fetchRegisters();
   //regD <= regD | regS; 
   r[regD] |= r[regS];
}
void processXOR(){
   fetchRegisters();
   //regD <= regD ^ regS; 
   r[regD] ^= r[regS];
}
void processTEST(){
   fetchRegisters();
   //temp <= regD & regS;  | Z N
   int temp = r[regD] & r[regS];

   if(temp == 0) setFlag(flagZ, true);
   else setFlag(flagZ, false);

   if(temp < 0) setFlag(flagN, true);
   else setFlag(flagN, false);
}
void processSHL(){
   fetchRegisters();
   //regD <= regD << regS; | Z C N 
   r[regD] = r[regD] << r[regS];

   if(r[regD] == 0) setFlag(flagZ, true);
   else setFlag(flagZ, false);

   if(r[regD] < 0) setFlag(flagN, true);
   else setFlag(flagN, false);

   //TODO: check carry flag
   if(r[regS] > 16) setFlag(flagC, true);
   else setFlag(flagC, false);

}
void processSHR(){
   fetchRegisters();
   //regD <= regD >> regS; | Z C N 
   r[regD] = r[regD] >> r[regS];

   if(r[regD] == 0) setFlag(flagZ, true);
   else setFlag(flagZ, false);

   if(r[regD] < 0) setFlag(flagN, true);
   else setFlag(flagN, false);

   //TODO: check carry flag
   if(r[regS] > 16) setFlag(flagC, true);
   else setFlag(flagC, false);
}



//FETCHING INSTRUCTIONS
 void fetchRegisters(){
   regD = (memory[r[7]] >> 4) & 0xF;
   regS = memory[r[7]] & 0xF;
   r[7]++;
 }

 //HELPER FUNCTIONS
 void setFlag(short flag, bool isFlag){
   if(isFlag) r[8] |= flag;
   else r[8] &= ~flag;
 }
