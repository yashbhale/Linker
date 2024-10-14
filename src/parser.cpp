#include "parser.h"
#include <iostream>
#include <fstream>
#include "coff_structures.h"
#include "elf_structures.h"

void Parser::parseCOFFHeader(const std::string& filePath, COFFHeader& coffHeader) {
    std::ifstream file(filePath, std::ios::binary);
    
    if (!file.is_open()) {
        std::cerr << "Error opening COFF file: " << filePath << std::endl;
        return;
    }

    file.read(reinterpret_cast<char*>(&coffHeader), sizeof(COFFHeader));

    if (file.fail()) {
        std::cerr << "Error reading COFF header from file: " << filePath << std::endl;
    } else {
        std::cout << "Successfully parsed COFF header from: " << filePath << std::endl;
    }
}

void Parser::parseELFHeader(const std::string& filePath, ELFHeader& elfHeader) {
    std::ifstream file(filePath, std::ios::binary);
    
    if (!file.is_open()) {
        std::cerr << "Error opening ELF file: " << filePath << std::endl;
        return;
    }

    file.read(reinterpret_cast<char*>(&elfHeader), sizeof(ELFHeader));

    if (file.fail()) {
        std::cerr << "Error reading ELF header from file: " << filePath << std::endl;
        return;
    }

    if (elfHeader.e_ident[0] != 0x7F || elfHeader.e_ident[1] != 'E' || elfHeader.e_ident[2] != 'L' || elfHeader.e_ident[3] != 'F') {
        std::cerr << "Not a valid ELF file: " << filePath << std::endl;
        return;
    }

    std::cout << "Successfully parsed ELF header from: " << filePath << std::endl;
}

void Parser::parseELFSections(const std::string& filePath, std::vector<ELFSectionHeader>& sectionHeaders) {
    std::ifstream file(filePath, std::ios::binary);
    
    if (!file.is_open()) {
        std::cerr << "Error opening ELF file for section parsing: " << filePath << std::endl;
        return;
    }

    ELFHeader elfHeader;
    parseELFHeader(filePath, elfHeader);

    sectionHeaders.resize(elfHeader.e_shnum);

    file.seekg(elfHeader.e_shoff, std::ios::beg);
    file.read(reinterpret_cast<char*>(sectionHeaders.data()), elfHeader.e_shnum * sizeof(ELFSectionHeader));

    if (file.fail()) {
        std::cerr << "Error reading section headers from file: " << filePath << std::endl;
    } else {
        std::cout << "Successfully parsed " << elfHeader.e_shnum << " section headers from: " << filePath << std::endl;
    }
}

void Parser::parse(const std::string& objectFile, SymbolTable& symbolTable, Platform platform) {
    std::cout << "Parsing object file: " << objectFile << " (Platform: " << platformToString(platform) << ")" << std::endl;

    switch (platform) {
        case Platform::ELF: {
            std::cout << "Handling ELF-specific parsing for: " << objectFile << std::endl;
            ELFHeader elfHeader;
            parseELFHeader(objectFile, elfHeader);

            std::vector<ELFSectionHeader> sectionHeaders;
            parseELFSections(objectFile, sectionHeaders);
            break;
        }
        case Platform::COFF: {
            std::cout << "Handling COFF (PE) specific parsing for: " << objectFile << std::endl;
            COFFHeader coffHeader;
            parseCOFFHeader(objectFile, coffHeader);
            break;
        }
        case Platform::MACHO:
            std::cout << "Handling Mach-O-specific parsing for: " << objectFile << std::endl;
            break;
        default:
            std::cout << "Unknown platform. Skipping platform-specific parsing." << std::endl;
            break;
    }

    symbolTable.addSymbol("example_symbol", 0x1000);
}
