#include "relocation.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip> // For hex formatting
#include "coff_structures.h"
#include "elf_structures.h"
#include "platform_detector.h"

void Relocation::applyRelocation(const std::string& filePath, const Platform& platform) {
    if (platform == Platform::ELF) {
        ELFHeader elfHeader; // Assume this is parsed earlier
        applyELFRelocations(elfHeader, filePath);
    } else if (platform == Platform::COFF) {
        COFFHeader coffHeader; // Assume this is parsed earlier
        applyCOFFRelocations(coffHeader, filePath);
    } else {
        std::cerr << "Unsupported format for relocation in file: " << filePath << std::endl;
    }
}

// This function applies COFF relocations to the object file
// This function applies COFF relocations to the object file
void Relocation::applyCOFFRelocations(const COFFHeader& coffHeader, const std::string& filePath) {
    std::fstream file(filePath, std::ios::in | std::ios::out | std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error opening file for COFF relocations: " << filePath << std::endl;
        return;
    }

    for (int sectionIndex = 0; sectionIndex < coffHeader.NumberOfSections; ++sectionIndex) {
        COFFSectionHeader sectionHeader;
        file.seekg(sizeof(COFFHeader) + sectionIndex * sizeof(COFFSectionHeader), std::ios::beg);
        file.read(reinterpret_cast<char*>(&sectionHeader), sizeof(sectionHeader));

        // Move to the relocation table for this section
        file.seekg(sectionHeader.PointerToRelocations, std::ios::beg);

        for (int relocationIndex = 0; relocationIndex < sectionHeader.NumberOfRelocations; ++relocationIndex) {
            COFFRelocation relocation;
            file.read(reinterpret_cast<char*>(&relocation), sizeof(relocation));

            COFFSymbol symbol;
            std::streampos symbolTablePos = coffHeader.PointerToSymbolTable + relocation.SymbolTableIndex * sizeof(COFFSymbol);
            file.seekg(symbolTablePos, std::ios::beg);
            file.read(reinterpret_cast<char*>(&symbol), sizeof(symbol));

            // Extract the symbol name
            std::string symbolName(symbol.Name, 8);
            std::cout << "Raw relocation type (before endian check): " << std::hex << relocation.Type << std::endl;


            // Handle different relocation types
            switch (relocation.Type) {
                case IMAGE_REL_I386_DIR32: {
                    uint32_t offset = relocation.VirtualAddress;
                    uint32_t symbolValue = symbol.Value;
                    file.seekg(offset, std::ios::beg);
                    uint32_t value;
                    file.read(reinterpret_cast<char*>(&value), sizeof(value));
                    value += symbolValue; // Apply 32-bit absolute relocation
                    file.seekp(offset, std::ios::beg);
                    file.write(reinterpret_cast<char*>(&value), sizeof(value));
                    std::cout << "Applied 32-bit absolute relocation for symbol: " << symbolName << std::endl;
                    break;
                }
                case IMAGE_REL_I386_REL32: {
                    uint32_t offset = relocation.VirtualAddress;
                    file.seekg(offset, std::ios::beg);
                    uint32_t value;
                    file.read(reinterpret_cast<char*>(&value), sizeof(value));
                    value += symbol.Value - offset - 4; // Apply 32-bit PC-relative relocation
                    file.seekp(offset, std::ios::beg);
                    file.write(reinterpret_cast<char*>(&value), sizeof(value));
                    std::cout << "Applied 32-bit relative relocation for symbol: " << symbolName << std::endl;
                    break;
                }
                default:
                    std::cerr << "Unknown or unsupported relocation type: " 
                              << relocation.Type << " for symbol: " << symbolName 
                              << " at address: " << std::hex << relocation.VirtualAddress << std::endl;
                    break;
            }
        }
    }
}



// This function applies ELF relocations to the object file
void Relocation::applyELFRelocations(const ELFHeader& elfHeader, const std::string& filePath) {
    std::fstream file(filePath, std::ios::in | std::ios::out | std::ios::binary);

    if (!file.is_open()) {
        std::cerr << "Error opening ELF file for relocations: " << filePath << std::endl;
        return;
    }

    std::vector<ELFSectionHeader> sectionHeaders(elfHeader.e_shnum);
    file.seekg(elfHeader.e_shoff, std::ios::beg);
    file.read(reinterpret_cast<char*>(sectionHeaders.data()), elfHeader.e_shnum * sizeof(ELFSectionHeader));

    for (const auto& section : sectionHeaders) {
        if (section.sh_type == 4 || section.sh_type == 9) {  // SHT_RELA or SHT_REL
            file.seekg(section.sh_offset, std::ios::beg);

            for (uint64_t i = 0; i < section.sh_size / section.sh_entsize; ++i) {
                ELFRelocation rela;
                file.read(reinterpret_cast<char*>(&rela), sizeof(rela));

                uint32_t symbolIndex = ELF64_R_SYM(rela.r_info);
                uint32_t relocationType = ELF64_R_TYPE(rela.r_info);

                std::cout << "Applying relocation type " << relocationType << " at offset "
                          << std::hex << rela.r_offset << std::endl;

                switch (relocationType) {
                    case R_X86_64_32: {
                        uint32_t value;
                        file.seekg(rela.r_offset, std::ios::beg);
                        file.read(reinterpret_cast<char*>(&value), sizeof(value));
                        file.seekg(section.sh_offset + (symbolIndex * sizeof(Elf64_Sym)), std::ios::beg);
                        Elf64_Sym symbol;
                        file.read(reinterpret_cast<char*>(&symbol), sizeof(symbol));
                        value += static_cast<uint32_t>(symbol.st_value); // Apply relocation
                        file.seekp(rela.r_offset, std::ios::beg);
                        file.write(reinterpret_cast<char*>(&value), sizeof(value));
                        std::cout << "Applied R_X86_64_32 relocation at offset: " << std::hex << rela.r_offset << std::endl;
                        break;
                    }
                    case R_X86_64_PC32: {
                        uint32_t value;
                        file.seekg(rela.r_offset, std::ios::beg);
                        file.read(reinterpret_cast<char*>(&value), sizeof(value));
                        file.seekg(section.sh_offset + (symbolIndex * sizeof(Elf64_Sym)), std::ios::beg);
                        Elf64_Sym symbol;
                        file.read(reinterpret_cast<char*>(&symbol), sizeof(symbol));
                        value += static_cast<uint32_t>(symbol.st_value - rela.r_offset); // Apply relative relocation
                        file.seekp(rela.r_offset, std::ios::beg);
                        file.write(reinterpret_cast<char*>(&value), sizeof(value));
                        std::cout << "Applied R_X86_64_PC32 relocation at offset: " << std::hex << rela.r_offset << std::endl;
                        break;
                    }
                    default:
                        std::cerr << "Unsupported relocation type: " << relocationType << std::endl;
                        break;
                }
            }
        }
    }
}
