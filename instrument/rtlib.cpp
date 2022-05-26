#include <iostream>
#include <string>
#include <stdio.h>
#include <time.h>
#include <fstream>//操作文件的头文件


// write to file
extern "C" void write2file(char* filename, int line,
    char* name, int state, int old_state, int mode) {
  const char* SEPARATOR = ", ";
  const char* END_LINE = "\n";

  // 定义流对象
  std::ofstream ofs;
  // 以追加写的方式打开文件
  ofs.open("results.txt",std::ios::out|std::ios::app);

  // 区分不同类型
  switch (mode) {
    case 0:
      ofs << filename << SEPARATOR
          << line << SEPARATOR
          << name << SEPARATOR
          << old_state << SEPARATOR
          << state << END_LINE;
      break;
    case 1:
      ofs << filename << SEPARATOR
          << line << SEPARATOR
          << name << SEPARATOR
          << (bool)old_state << SEPARATOR
          << (bool)state << END_LINE;
      break;
    case 2:
      ofs << filename << SEPARATOR
          << line << SEPARATOR
          << name << SEPARATOR
          << (char)old_state << SEPARATOR
          << (char)state << END_LINE;
      break;
    default:
      break;
  }

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
    write2file(filename, line, name, state, old_state, 0);
}

extern "C" void logbool(char* filename, int line, char* name, bool state, bool old_state) {
    std::cout << "Line " << line << ": " << name << " = " << state << "." << std::endl;
    write2file(filename, line, name, state, old_state, 1);
}

extern "C" void logchar(char* filename, int line, char* name, char state, char old_state) {
    if (state == 1 || state == 0) {
        logbool(filename, line, name, state, old_state);
        return;
    }
    write2file(filename, line, name, state, old_state, 2);
}

extern "C" void logstring(char* filename, int line, char* name, char* state, char* old_state) {
    std::cout << "Line " << line << ": " << name << " = " << state << "." << std::endl;
//    write2file(filename, line, name, state, old_state, 3);
}
