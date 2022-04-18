#include <iostream>
#include <string>

void logop(int i);
void logvar(int i, std::string name);


void logop(int i) {
    std::cout << "computed:  " << i << std::endl;
}

void logvar(int i, std::string name) {
    std::cout << "Num: " << i << "; Name: " << name << std::endl;
}
