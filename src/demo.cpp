#include <iostream>

int foo_int() {
    int state_int = 0;
    // ...
    state_int = 1;
    // ...
    std::cin >> state_int;
    return state_int;
}

bool foo_bool() {
    bool state_bool = true;
    // ...
    state_bool = false;
    // ...
    std::cin >> state_bool;
    return state_bool;
}

char foo_char() {
    char state_char = 'l';
    // ...
    state_char = 'r';
    // ...
    std::cin >> state_char;
    return state_char;
}

std::string foo_string() {
    std::string state_string = "ILoveLLVM";
    // ...
    state_string = "IDon'tLoveLLVM";
    // ...
    std::cin >> state_string;
    return state_string;
}

float foo_float() {
    float state_float = 1.23;
    // ...
    state_float = -1.23;
    // ...
    std::cin >> state_float;
    return state_float;
}

double foo_double() {
    double state_double = 1.23;
    // ...
    state_double = -1.23;
    // ...
    std::cin >> state_double;
    return state_double;
}

int main() {
    // int state_int = foo_int();
    bool state_bool = foo_bool();
    char state_char = foo_char();
    // std::string state_string = foo_string();
}
