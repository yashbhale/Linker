#ifndef PLATFORM_DETECTOR_H
#define PLATFORM_DETECTOR_H

#include <string>

enum class Platform {
    UNKNOWN,
    ELF,
    PE,
    MACHO,
    COFF // Add this line
};


class PlatformDetector {
public:
    Platform detectPlatform(const std::string& filePath);
};

std::string platformToString(Platform platform);  // Declaration here

#endif // PLATFORM_DETECTOR_H
