#include <iostream>
#include <string>

extern "C" void logop(int i) {
    std::cout << "computed:  " << i << std::endl;
}

extern "C" void logvar(int i, std::string name) {
    std::cout << "Num: " << i << "; Name: " << name << std::endl;
}

extern "C" void logif(int i, float f) {
    std::cout << "Num: " << i << "; Float: " << f << std::endl;
}

extern "C" void testArgs(int i, float f, std::string name) {
    std::cout << "Num: " << i << "; " << f << "; Name: " << name << std::endl;
}
