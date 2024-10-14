#ifndef COFF_STRUCTURES_H
#define COFF_STRUCTURES_H
// Relocation types (you can extend this list as needed)
#define IMAGE_REL_I386_DIR32 0x0006  // Direct 32-bit relocation
#define IMAGE_REL_I386_REL32 0x0014  // Relative 32-bit relocation


#include <cstdint>

#pragma pack(push, 1)

struct COFFRelocation {
    uint32_t VirtualAddress;  // Offset within the section to be relocated
    uint32_t SymbolTableIndex; // Index of the symbol in the symbol table
    uint16_t Type;            // Type of relocation (e.g., absolute, relative)
};


struct COFFHeader {
    uint16_t Machine;
    uint16_t NumberOfSections;
    uint32_t TimeDateStamp;
    uint32_t PointerToSymbolTable;
    uint32_t NumberOfSymbols;
    uint16_t SizeOfOptionalHeader;
    uint16_t Characteristics;
};

struct COFFSectionHeader {
    char Name[8];           // Section name
    uint32_t PhysicalAddress;
    uint32_t VirtualSize;
    uint32_t VirtualAddress;
    uint32_t SizeOfRawData;
    uint32_t PointerToRawData;
    uint32_t PointerToRelocations;
    uint32_t PointerToLinenumbers;
    uint16_t NumberOfRelocations;
    uint16_t NumberOfLinenumbers;
    uint32_t Characteristics;
};

// Define the structure for COFF Symbol Table entry
struct COFFSymbol {
    union {
        char Name[8];           // Short name (8 bytes)
        struct {
            uint32_t Zeroes;    // If Name is not used
            uint32_t Offset;    // Offset into the string table
        };
    };
    uint32_t Value;             // Value of the symbol (section offset or absolute value)
    int16_t SectionNumber;      // Section number or special values (e.g., -1 for undefined)
    uint16_t Type;              // Type of the symbol (e.g., function, object)
    uint8_t StorageClass;       // Storage class (e.g., external, static)
    uint8_t NumberOfAuxSymbols; // Number of auxiliary symbols that follow this symbol
};

#pragma pack(pop)

#endif // COFF_STRUCTURES_H
