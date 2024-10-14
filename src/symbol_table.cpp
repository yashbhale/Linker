#include "symbol_table.h"

void SymbolTable::addSymbol(const std::string& name, int address) {
    table[name] = address; 
}

int SymbolTable::getAddress(const std::string& name) {
    auto it = table.find(name); 
    if (it != table.end()) {
        return it->second; 
    }
    return -1; 
}
