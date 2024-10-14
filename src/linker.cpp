#include "linker.h"
#include "parser.h"
#include "symbol_table.h"
#include "relocation.h"
#include "platform_detector.h"
#include "platform_utils.h"
#include "coff_structures.h"
#include "elf_structures.h" // Add ELF structures
#include <iostream>
#include <fstream>

void parseELF(const std::string& filePath, SymbolTable& symbolTable) {
    std::ifstream file(filePath, std::ios::binary);

    if (!file.is_open()) {
        std::cerr << "Error opening ELF file: " << filePath << std::endl;
        return;
    }

    ELFHeader elfHeader;
    file.read(reinterpret_cast<char*>(&elfHeader), sizeof(elfHeader));

    if (elfHeader.e_ident[0] != 0x7F || elfHeader.e_ident[1] != 'E' ||
        elfHeader.e_ident[2] != 'L' || elfHeader.e_ident[3] != 'F') {
        std::cerr << "Not a valid ELF file: " << filePath << std::endl;
        return;
    }

    std::cout << "ELF File: " << filePath << std::endl;

    // Read section headers
    std::vector<ELFSectionHeader> sectionHeaders(elfHeader.e_shnum);
    file.seekg(elfHeader.e_shoff, std::ios::beg);
    file.read(reinterpret_cast<char*>(&sectionHeaders[0]), elfHeader.e_shnum * sizeof(ELFSectionHeader));

    // Find symbol table and string table
    ELFSectionHeader* symTabSection = nullptr;
    ELFSectionHeader* strTabSection = nullptr;

    for (const auto& shdr : sectionHeaders) {
        if (shdr.sh_type == 2) {  // SHT_SYMTAB
            symTabSection = const_cast<ELFSectionHeader*>(&shdr);
        }
        if (shdr.sh_type == 3) {  // SHT_STRTAB
            strTabSection = const_cast<ELFSectionHeader*>(&shdr);
        }
    }

    if (!symTabSection || !strTabSection) {
        std::cerr << "Symbol table or string table not found in ELF file." << std::endl;
        return;
    }

    // Read the symbol table
    std::vector<Elf64_Sym> symbols(symTabSection->sh_size / sizeof(Elf64_Sym));
    file.seekg(symTabSection->sh_offset, std::ios::beg);
    file.read(reinterpret_cast<char*>(&symbols[0]), symbols.size() * sizeof(Elf64_Sym));

    // Read the string table
    std::vector<char> stringTable(strTabSection->sh_size);
    file.seekg(strTabSection->sh_offset, std::ios::beg);
    file.read(&stringTable[0], strTabSection->sh_size);

    // Add symbols to the symbol table
    for (const auto& sym : symbols) {
        std::string symName(&stringTable[sym.st_name]);
        symbolTable.addSymbol(symName, sym.st_value);
    }
}


void parseCOFF(const std::string& filePath, SymbolTable& symbolTable) {
    std::ifstream file(filePath, std::ios::binary);
    
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filePath << std::endl;
        return;
    }

    COFFHeader header;
    file.read(reinterpret_cast<char*>(&header), sizeof(header));

    // Check for symbol table and extract it
    if (header.NumberOfSymbols == 0) {
        std::cerr << "No symbols found in COFF file: " << filePath << std::endl;
        return;
    }

    // Move to the symbol table position
    file.seekg(header.PointerToSymbolTable, std::ios::beg);

    // Parse symbols
    for (uint32_t i = 0; i < header.NumberOfSymbols; ++i) {
        COFFSymbol symbol;
        file.read(reinterpret_cast<char*>(&symbol), sizeof(symbol));

        std::string symbolName(symbol.Name, 8);  // Assuming symbol names are simple
        symbolTable.addSymbol(symbolName, symbol.Value);
    }
}


void Linker::link(const std::vector<std::string>& objectFiles) {
    Relocation relocation;
    Parser parser;
    SymbolTable symbolTable;
    PlatformDetector detector;

    for (const auto& objectFile : objectFiles) {
        // Detect the platform
        Platform platform = detector.detectPlatform(objectFile);
        std::cout << "Linking object file: " << objectFile << " (" << platformToString(platform) << ")" << std::endl;

        // Call the parse function with platform-specific logic
        parser.parse(objectFile, symbolTable, platform);

        // Apply relocations based on the platform
        switch (platform) {
            case Platform::COFF:  // COFF files
                COFFHeader coffHeader;
                parser.parseCOFFHeader(objectFile, coffHeader);
                relocation.applyCOFFRelocations(coffHeader, objectFile);
                break;

            case Platform::ELF:  // ELF files
                ELFHeader elfHeader;  // Use ELFHeader instead of ELFHeader
                parser.parseELFHeader(objectFile, elfHeader);
                relocation.applyELFRelocations(elfHeader, objectFile);
                break;

            default:
                std::cerr << "Unknown platform. Skipping relocation." << std::endl;
                break;
        }
    }

    std::cout << "Cross-platform linking completed." << std::endl;
}
