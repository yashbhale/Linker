#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <vector>
#include "symbol_table.h"
#include "platform_detector.h"
#include "elf_structures.h"
#include "coff_structures.h"

class Parser {
public:
    void parse(const std::string& objectFile, SymbolTable &symbolTable, Platform platform);
    void parseCOFFHeader(const std::string& filePath, COFFHeader& coffHeader);
    void parseELFHeader(const std::string& filePath, ELFHeader& elfHeader);
    void parseELFSections(const std::string& filePath, std::vector<ELFSectionHeader>& sectionHeaders);
};

#endif // PARSER_H
