#include "../../inc/emulator/emulator.h"

using namespace std;
  
int main(int argc, char** argv)
{
   if(argc != 2){
      cout << "Wrong arguments number.";
      return -1;
   }
   string input = argv[2];
   fstream inputFile;

   
   inputFile.open(input, ios::in); //open a file to perform read operation using file object
   if (inputFile.is_open()){   //checking whether the file is open
      
      string currentLine;
      while(getline(inputFile, currentLine)){ //read data from file object and put it into string.
         // cout << currentLine << "\n"; //print the data of the string

      }
      inputFile.close(); //close the file object.
   }
   

   return 0;
}