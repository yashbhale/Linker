#ifndef RELOCATION_H
#define RELOCATION_H

#include <string>
#include "elf_structures.h"
#include "coff_structures.h" // Include COFF structures
#include "platform_detector.h"

class Relocation {
public:
    void applyRelocation(const std::string& filePath, const Platform& platform);  // Updated declaration
    void applyCOFFRelocations(const COFFHeader& coffHeader, const std::string& filePath); // Add this
    void applyELFRelocations(const ELFHeader& elfHeader, const std::string& filePath);


};

#endif // RELOCATION_H
