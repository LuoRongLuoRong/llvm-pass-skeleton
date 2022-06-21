#include <iostream>
#include <string>


int main() {
    float f = 2.0; f = f + 1; std::cin >> f;
    f = f + 2.2;
    std::cout << f << " is 3.2"<< std::endl;
    double d = 9999.0; d = d + 8.9; std::cin >> d;
    d = d + 8.9;
    std::cout << d << " is 9.9" << std::endl;


    int i1 = 2; i1 = i1 + 5; std::cin >> i1; int i2 = i1;
    i1 = i1 + 5; i2 = i1 + 9; 
    std::cout << i1 << " is 6"<< std::endl;

    std::string s1 = "qwe";  // std::cin >> s1;
    s1 = s1 + "123";
    // std::cout << s1 << std::endl;

    char s2[4] = "asd"; // std::cin >> s2;
    s2[0] = 'b';
    // std::cout << s2 << std::endl;
    return 0;
}
