#include <iostream>
#include <string>


int main() {
    // float f1 = 2.0; 
    // f1 = f1 + 1; 
    // std::cin >> f1;
    // f1 = f1 + 2.2;
    // std::cout << f1 << " is 3.2"<< std::endl;

    // double d1 = 9999.0; 
    // d1 = d1 + 8.9; 
    // std::cin >> d1;
    // d1 = d1 + 8.9;
    // std::cout << d1 << " is 9.9" << std::endl;

    // int i1 = 2; 
    // i1 = i1 + 5; 
    // std::cin >> i1; 
    // int i2 = i1;
    // i1 = i1 + 5; 
    // i2 = i1 + 9; 
    // std::cout << i1 << " is 6"<< std::endl;

    // std::string s1 = "qwe";  
    // std::cin >> s1;
    // s1 = s1 + "123";
    // std::cout << s1 << std::endl;


    char s2[4] = "asd"; 
    // std::cin >> s2;
    s2[0] = 'b';
    // std::cout << s2 << std::endl;

    char* s3 = s2;
    s3[1] = 'q';
    
    return 0;
}
