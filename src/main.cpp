#include <iostream>
#include <string>
#include <fstream>

#include "../inc/assembler.h"

using namespace std;
  
int main(int argc, char** argv)
{
   if(argc != 4){
      cout << "Wrong arguments number.";
      return -1;
   }
   string input = argv[3];
   string output = argv[2];
   fstream inputFile;

   setupAssembler();
   
   inputFile.open(input, ios::in); //open a file to perform read operation using file object
   if (inputFile.is_open()){   //checking whether the file is open
      
      string currentLine;
      while(getline(inputFile, currentLine)){ //read data from file object and put it into string.
         // cout << currentLine << "\n"; //print the data of the string

         if(!processLine(currentLine)) break;;
      }
      inputFile.close(); //close the file object.
   }
   
   fixRelocations();
   generateOutput(output);

   return 0;
}