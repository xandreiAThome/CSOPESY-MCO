#include "terminal_utils.hpp"

#include <iostream>
#include <fstream>

void clearTerminal() { std::cout << "\033[2J\033[H"; }

void printTextFile(std::string fileName){
    std::ifstream f(fileName);

    if(f.is_open()){
        std::cout << f.rdbuf();
    }
}
