#include <iostream>
#include <string>
#include <stdio.h>
#include <time.h>
#include <fstream>//操作文件的头文件


// write to file
extern "C" void write2file(char* filename, int line, char* name, int type, int state, int old_state) {
  const char* SEPARATOR = ", ";
  const char* END_LINE = "\n";
  // 定义流对象
  std::ofstream ofs;
  // 以追加写的方式打开文件
  ofs.open("results.txt",std::ios::out|std::ios::app);
  ofs << filename << SEPARATOR
      << line << SEPARATOR
      << name << SEPARATOR
      << type << SEPARATOR
      << old_state << SEPARATOR
      << state << END_LINE;

  ofs.close();
}


// useful function

extern "C" void loglinevarint(int line, char* name, int state, int old_state) {
  //  文件名，行号，变量，时间，前值，后值
  std::cout << "BEFORE: Line " << line << ": " << name << " = " << old_state << "." << std::endl;
  std::cout << "AFTER:  Line " << line << ": " << name << " = " << state << "." << std::endl;
}

extern "C" void logint(char* filename, int line, char* name, int type, int state, int old_state) {
  loglinevarint(line, name, state, old_state);
//  std::ofstream ofs;  // 定义流对象
//  ofs.open("results.txt",std::ios::out|std::ios::app);
//  ofs << "loglinevarint run!\n";
//  ofs.close();
  write2file(filename, line, name, type, state, old_state);
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
