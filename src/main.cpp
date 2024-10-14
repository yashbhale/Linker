#include "parser.h"
#include "relocation.h"
#include "linker.h"
#include "platform_detector.h"
#include "symbol_table.h"
#include <iostream>
#include <vector>

int main(int argc, char** argv) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <object file 1> <object file 2> ..." << std::endl;
        return 1;
    }

    PlatformDetector detector;
    Parser parser;
    Relocation relocation;
    Linker linker;
    SymbolTable symbolTable;

    std::vector<std::string> objectFiles;

    for (int i = 1; i < argc; ++i) {
        std::string objectFile = argv[i];
        Platform platform = detector.detectPlatform(objectFile);
        
        switch (platform) {
            case Platform::ELF:
                std::cout << "ELF format detected in " << objectFile << std::endl;
                break;
            case Platform::PE:
                std::cout << "PE format detected in " << objectFile << std::endl;
                break;
            case Platform::MACHO:
                std::cout << "Mach-O format detected in " << objectFile << std::endl;
                break;
            case Platform::COFF:
                std::cout<< "Coff format detected in " << objectFile<<std::endl;
                break;
            default:
                std::cout << "Unknown format in " << objectFile << std::endl;
                continue; // Skip to the next file if format is unknown
        }

        objectFiles.push_back(objectFile);
        parser.parse(objectFile, symbolTable, platform);  
        relocation.applyRelocation(objectFile, platform);
    }

    linker.link(objectFiles);  
    std::cout << "Linking completed." << std::endl;

    return 0;
}
