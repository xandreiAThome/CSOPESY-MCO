#include "terminal_utils.hpp"

#include <iostream>
#include <fstream>

#ifdef _WIN32
#  define NOMINMAX
#  include <windows.h>
#endif

void setConsoleOutputUtf8() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif
}

void clearTerminal() { 
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif

    printTextFile("assets/introText.txt");
}

void printTextFile(std::string fileName){
    std::ifstream f(fileName);

    if(f.is_open()){
        std::cout << f.rdbuf();
    } else {
        std::cerr << "Unable to open text file: " << fileName << '\n';
    }
}
