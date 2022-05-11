#include <iostream>
#include <string>

// useful function

extern "C" void loglinevarint(int line, char* name, int state) {
    std::cout << "Line " << line << ": " << name << " = " << state << "." << std::endl;
}

extern "C" void loglinevarbool(int line, char* name, bool state) {
    std::cout << "Line " << line << ": " << name << " = " << state << "." << std::endl;
}

extern "C" void loglinevarchar(int line, char* name, char state) {
    if (state == 1 || state == 0) {
        loglinevarbool(line, name, state);
        return;
    }
    std::cout << "Line " << line << ": " << name << " = " << state << "." << std::endl;
}

extern "C" void loglinevarstring(int line, char* name, char* state) {
    std::cout << "Line " << line << ": " << name << " = " << state << "." << std::endl;
}

// useless function

extern "C" void logop(int i) {
    std::cout << "computed:  " << i << std::endl;
}

extern "C" void logvar(int i, char* name) {
    std::cout << "Num: " << i << "; Name: " << name << std::endl;
}

extern "C" void logif(int i, float f) {
    std::cout << "Num: " << i << "; Float: " << f << std::endl;
}

extern "C" void testArgs(int i, float f, std::string name) {
    std::cout << "Num: " << i << "; " << f << "; Name: " << name << std::endl;
}
