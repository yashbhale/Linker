#include "platform_detector.h"
#include <fstream>
#include <iostream>
using namespace std;

std::string platformToString(Platform platform) {
    switch (platform) {
        case Platform::ELF: return "ELF";
        case Platform::PE: return "PE";
        case Platform::MACHO: return "Mach-O";
        case Platform::COFF: return "COFF"; // Add this line
        default: return "Unknown";
    }
}


Platform PlatformDetector::detectPlatform(const std::string& filePath) {
    std::ifstream file(filePath, std::ios::binary);
    
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filePath << std::endl;

        return Platform::UNKNOWN;
    }

    char buffer[4];
    file.read(buffer, 4);

    // Print the first 4 bytes of the file for debugging
    std::cout << "Magic bytes in " << filePath << ": ";
    for (int i = 0; i < 4; i++) {
        std::cout << std::hex << (int)(unsigned char)buffer[i] << " ";
    }
    std::cout << std::dec << std::endl;  // Switch back to decimal

    if (file.gcount() < 4) {
        std::cerr << "Error reading file: " << filePath << std::endl;
        return Platform::UNKNOWN;
    }

    // Check for ELF file
    if (buffer[0] == 0x7F && buffer[1] == 'E' && buffer[2] == 'L' && buffer[3] == 'F') {
        return Platform::ELF;
    }

    // Check for PE file (Windows Executable)
    if (buffer[0] == 'M' && buffer[1] == 'Z') {
        return Platform::PE;
    }

    // Check for COFF file
unsigned char coffMagic[4];
file.seekg(0); // Go back to the start of the file
file.read(reinterpret_cast<char*>(coffMagic), 4);

// Print the magic bytes for debugging
std::cout << "Magic bytes in COFF file: ";
for (int i = 0; i < 4; i++) {
    std::cout << std::hex << (int)(unsigned char)coffMagic[i] << " ";
}
std::cout << std::dec << std::endl;

std::cout << "Magic bytes read: ";
for (int i = 0; i < 4; i++) {
    std::cout << std::hex << (int)(unsigned char)coffMagic[i] << " ";
}
std::cout << std::dec << std::endl;


// Check known COFF magic bytes
if ((coffMagic[0] == 0x64 && coffMagic[1] == 0x86 && coffMagic[2] == 0x06 && coffMagic[3] == 0x00) ||
    (coffMagic[0] == 0x14 && coffMagic[1] == 0x00) || 
    (coffMagic[0] == 0x4D && coffMagic[1] == 0x5A && coffMagic[2] == 0x90 && coffMagic[3] == 0x00)) {  // Added more checks
    return Platform::COFF;
}

    // Check for Mach-O file
    if ((unsigned char)buffer[0] == 0xFE && (unsigned char)buffer[1] == 0xED) {
        return Platform::MACHO;
    }

    cout<<"Hello"<<endl;
    return Platform::UNKNOWN;
}
