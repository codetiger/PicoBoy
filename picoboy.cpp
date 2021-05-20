#include <iostream>

#include "./gboy/GBoy.h"
 
int main(int argc, char *argv[]){
   std::cout << "Hello World!" << std::endl;
   GBoy *gb = new GBoy();

   while (true) {
      gb->ExecuteFrame();
   }
   
   return 0;
}