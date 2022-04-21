#include <iostream>
#include <string>

extern "C" void logop(int i) {
    std::cout << "computed:  " << i << std::endl;
}

extern "C" void logvar(int i, std::string name) {
    std::cout << "Num: " << i << "; Name: " << name << std::endl;
}
