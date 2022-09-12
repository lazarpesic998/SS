#include "../../inc/emulator/emulator.h"

unsigned char memory[65536] = {0};
unsigned short r[9] = {0};
string currentInstruction = "NO_INSTR";

int regD;
int regS;
string addrMode;
short operand;

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

map<short, string> addrModeMap = {
   {0x00, "ABS"},
   {0x01, "REG_DIR"},
   {0x05, "PC_REL"}, //registarsko direktno sa pomerajem
   {0x02, "REG_INDIR"},
   {0x03, "REG_INDIR_OFFSET"},
   {0x04, "MEM_DIR"},
   {0x12, "PUSH"},
   {0x42, "POP"}
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

      resetProcessor();
      emulate();
      generateOutput();
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
      currentInstruction = instructionMap[memory[ r[7] ] ];
      // generateOutput();
      // cout << currentInstruction << endl;
      
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


      //JUMP INSTRUCTIONS WITH OPERAND
      std::set<string> operandInstructions = { "call", "jmp", "jeq", "jne", "jgt", "ldr", "str"};
      if (operandInstructions.find(currentInstruction) != operandInstructions.end()) {
         fetchRegisters();
         addrMode = fetchAddrMode();
         if(currentInstruction != "str") operand = fetchOperand();

         if(addrMode == "PUSH") { pushStack(r[regD]); continue; }
         if(addrMode == "POP") { r[regD] = popStack(); continue; }

         if(currentInstruction == "call"){processCALL(); continue; }
         if(currentInstruction == "jmp"){ processJMP(); continue; }
         if(currentInstruction == "jeq"){ processJEQ(); continue; }
         if(currentInstruction == "jne"){ processJNE(); continue; }
         if(currentInstruction == "jgt"){ processJGT(); continue; }
         if(currentInstruction == "ldr"){ processLDR(); continue; }
         if(currentInstruction == "str"){ processSTR(); continue; }
      }else{
         cout << "INSTRUCTION DOES NOT EXIST: " << currentInstruction << r[7] << endl;
         exit(1);
      }


   }
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

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

   r[6] -= 2;  
   memory[ r[6] + 1 ] = (value >> 8) & 0xFF; //data high
   memory[ r[6] ] = value & 0xFF; //data low

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

//INSTRUCTIONS WITH REGISTERS ONLY
void processINT(){

   fetchRegisters();
   //push pc
   pushStack(r[7]);
   //push psw
   pushStack(r[8]);
   //pc <= mem16[(regD mod 8)*2]; 
   // TODO: check maybe r[regD]
   r[7] = fetchTwoBytesFromMemory((r[regD] % 8) * 2);
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

//OPERATIONS WITH OPERANDS
void processCALL(){
   //push pc; pc <= operand;
   pushStack(r[7]);
   r[7] = operand;
}
void processJMP(){
   //pc <= operand;
   r[7] = operand;
}
void processJEQ(){
   //if (equal) pc <= operand
   if(r[8] & flagZ){
      r[7] = operand;
   }
}
void processJNE(){
   //if (not equal) pc <= operand; 
   if( !(r[8] & flagZ) ){
      r[7] = operand;
   }
}
void processJGT(){
   //if (signed greater) pc <= operand; 
   if( !(r[8] & flagZ) && !(r[8] & flagN) ){
      r[7] = operand;
   }
}
void processLDR(){
   //regD <= operand;
   r[regD] = operand;
}

//FETCHING INSTRUCTIONS
 void fetchRegisters(){
   regD = (memory[r[7]] >> 4) & 0xF;
   regS = memory[r[7]] & 0xF;
   r[7]++;
 }

 string fetchAddrMode(){
   return addrModeMap[ memory[r[7]++] ];
 }

 short fetchOperand(){
   //5B operand
   if(addrMode == "ABS") {
      operand = fetchTwoBytesFromMemory(r[7]);
      r[7] += 2; //pc = pc + 2
   }
   if(addrMode == "MEM_DIR"){
      short offset = fetchTwoBytesFromMemory(r[7]);
      r[7] += 2; //pc = pc + 2
      operand = fetchTwoBytesFromMemory(offset); 
   } 
   if(addrMode == "PC_REL"){
      operand = fetchTwoBytesFromMemory(r[7]);
      r[7] += 2; //pc = pc + 2
      operand += r[7];
   }
   if(addrMode == "REG_INDIR_OFFSET"){
      short offset = fetchTwoBytesFromMemory(r[7]);
      r[7] += 2; //pc = pc + 2
      operand = fetchTwoBytesFromMemory(r[regS] + offset); //regS from assembler
   }
   //3B operand
   if(addrMode == "REG_DIR"){
      operand = r[regS];
   }
   if(addrMode == "REG_INDIR"){
      operand = fetchTwoBytesFromMemory(r[regS]);
   }

   return operand;
 }

 short fetchTwoBytesFromMemory(int location){

   int high = (int8_t)  memory[location+1];
   int low = (int8_t)memory[location] & 0xFF;
   int codeValue = (high << 8) | (low);
   return (high << 8) | (low & 0xFF);
 }

  void processSTR(){
   // //5B operand
   if(addrMode == "ABS") {
      cout << "STORE WITH ABSOLUTE ADRESSING NOT ALOWED" << endl;
      exit(1);
   }
   if(addrMode == "MEM_DIR"){
      short offset = fetchTwoBytesFromMemory(r[7]);
      r[7] += 2; //pc = pc + 2
      storeToMemory(r[regD], offset);
   } 
   if(addrMode == "PC_REL"){
      short offset = fetchTwoBytesFromMemory(r[7]);
      r[7] += 2; //pc = pc + 2
      storeToMemory(r[regD], r[7] + offset);
   }
   if(addrMode == "REG_INDIR_OFFSET"){
      short offset = fetchTwoBytesFromMemory(r[7]);
      r[7] += 2; //pc = pc + 2
      storeToMemory(r[regD], r[regS] + offset);
   }
   // //3B operand
   if(addrMode == "REG_DIR"){
      r[regS] = r[regD];
   }
   if(addrMode == "REG_INDIR"){
      storeToMemory(r[regD], r[regS]);
   }

   // return operand;
 }

 void storeToMemory(short value, int location){
   unsigned char dataHigh = (unsigned) value >> 8;
   unsigned char dataLow = (unsigned) value & 0xFF;
   memory[location] = dataLow;
   memory[location + 1] = dataHigh;
 }

 //HELPER FUNCTIONS
 void setFlag(short flag, bool isFlag){
   if(isFlag) r[8] |= flag;
   else r[8] &= ~flag;
 }


void generateOutput(){
   cout << "------------------------------------------------" << endl;
   cout << "Emulated processor executed halt instruction" << endl;
   cout << "Emulated processor state: psw=0b" << std::bitset<16>(r[8]) << endl;
   cout<< hex << " r0=0x" << setw(4) << setfill('0') << r[0] << " r1=0x" << setw(4) << setfill('0') << r[1] << " r2=0x" << setw(4) << setfill('0') <<  r[2] << " r3=0x" << setw(4) << setfill('0') <<  r[3] << endl;
   cout<< hex << " r4=0x" << setw(4) << setfill('0') << r[4] << " r5=0x" << setw(4) << setfill('0') << r[5] << " r6=0x" << setw(4) << setfill('0') << r[6] << " r7=0x" << setw(4) << setfill('0') << r[7] << endl;
}