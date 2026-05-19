#include "terminal_utils.hpp"

#include <iostream>

void clearTerminal() { std::cout << "\033[2J\033[H"; }
