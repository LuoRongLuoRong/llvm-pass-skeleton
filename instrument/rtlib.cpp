#include <iostream>
#include <string>
#include <stdio.h>
#include <time.h>
#include <fstream>//操作文件的头文件


// write to file
extern "C" void write2file(char* filename, int line,
    char* name, int state, int old_state) {
  const char* SEPARATOR = ", ";
  const char* END_LINE = "\n";
  // 定义流对象
  std::ofstream ofs;
  // 以追加写的方式打开文件
  ofs.open("results.txt",std::ios::out|std::ios::app);
  ofs << filename << SEPARATOR
      << line << SEPARATOR
      << name << SEPARATOR
      << old_state << SEPARATOR
      << state << END_LINE;

  ofs.close();
}


// instrumented function

extern "C" void loglinevarint(int line, char* name, int state, int old_state) {
  //  文件名，行号，变量，时间，前值，后值
  std::cout << "BEFORE: Line " << line << ": " << name << " = " << old_state << "." << std::endl;
  std::cout << "AFTER:  Line " << line << ": " << name << " = " << state << "." << std::endl;
}

extern "C" void logint(char* filename, int line, char* name, int state, int old_state) {
//  loglinevarint(line, name, state, old_state);
  write2file(filename, line, name, state, old_state);
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
