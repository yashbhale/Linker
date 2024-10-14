#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <string>
#include <unordered_map>

class SymbolTable {
private:
    std::unordered_map<std::string, int> table;

public:
    void addSymbol(const std::string& name, int address);
    int getAddress(const std::string& name);
};

#endif // SYMBOL_TABLE_H
