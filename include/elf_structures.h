#ifndef ELF_STRUCTURES_H
#define ELF_STRUCTURES_H

#include <cstdint>

// ELF Header
struct ELFHeader {
    unsigned char e_ident[16]; // ELF identification
    uint16_t e_type;           // Object file type
    uint16_t e_machine;        // Architecture
    uint32_t e_version;        // Object file version
    uint64_t e_entry;          // Entry point address
    uint64_t e_phoff;          // Program header table file offset
    uint64_t e_shoff;          // Section header table file offset
    uint32_t e_flags;          // Processor-specific flags
    uint16_t e_ehsize;         // ELF header size
    uint16_t e_phentsize;      // Size of program header table entry
    uint16_t e_phnum;          // Number of program header table entries
    uint16_t e_shentsize;      // Size of section header table entry
    uint16_t e_shnum;          // Number of section header table entries
    uint16_t e_shstrndx;       // Section header string table index
};

// ELF Section Header
struct ELFSectionHeader {
    uint32_t sh_name;       // Section name (index into string table)
    uint32_t sh_type;       // Section type
    uint64_t sh_flags;      // Section flags
    uint64_t sh_addr;       // Virtual address in memory
    uint64_t sh_offset;     // Offset in file
    uint64_t sh_size;       // Size of section
    uint32_t sh_link;       // Link to another section
    uint32_t sh_info;       // Additional section information
    uint64_t sh_addralign;  // Section alignment
    uint64_t sh_entsize;    // Entry size if section holds a table
};

// ELF Symbol Entry
struct Elf64_Sym {
    uint32_t st_name;  // Symbol name (index into string table)
    uint8_t st_info;   // Type and binding attributes
    uint8_t st_other;  // Reserved (not used)
    uint16_t st_shndx; // Section index
    uint64_t st_value; // Value of the symbol
    uint64_t st_size;  // Size of the symbol
};

// ELF Relocation Entry (without addend)
struct ELFRelocation {
    uint64_t r_offset;      // Offset in the section to apply relocation
    uint64_t r_info;        // Symbol index and type of relocation
};

// ELF Relocation Entry with addend (RELA type)
struct ELFRelocationA {
    uint64_t r_offset;      // Offset in the section to apply relocation
    uint64_t r_info;        // Symbol index and type of relocation
    int64_t r_addend;       // Constant addend used to compute value
};

// Relocation type macros (for simplicity, we're focusing on 64-bit ELF)
#define ELF64_R_SYM(i)    ((i) >> 32)          // Extract symbol index
#define ELF64_R_TYPE(i)   ((i) & 0xFFFFFFFF)   // Extract relocation type

// Define relocation types for ELF64 (e.g., x86_64 architecture)
#define R_X86_64_32       10  // Direct 32-bit relocation
#define R_X86_64_PC32     2   // PC-relative 32-bit relocation

#endif // ELF_STRUCTURES_H
