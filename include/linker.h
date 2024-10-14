#ifndef LINKER_H
#define LINKER_H

#include <vector>
#include <string>

class Linker {
public:
    void link(const std::vector<std::string>& objectFiles);
};

#endif // LINKER_H
