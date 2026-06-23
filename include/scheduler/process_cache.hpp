#pragma once

struct ProcessCache{
    int pid;
    std::string name;
    int coreId;
    int executedInstructions;
    int totalInstructions;
    std::vector<std::string> logs;
};