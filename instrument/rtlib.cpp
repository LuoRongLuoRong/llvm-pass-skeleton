#include <iostream>
#include <string>
#include <cstring>
#include <ctype.h>
#include <stdio.h>
#include <time.h>
#include <fstream> //操作文件的头文件

#define MAX_INT (((unsigned int)(-1)) >> 1)

// write to file
extern "C" void write2file(char *filename, int line, char *name, int type, const char *state, const char *old_state)
{
  const char *SEPARATOR = ", ";
  const char *END_LINE = "\n";

  // 定义流对象
  std::ofstream ofs;
  // 以追加写的方式打开文件
  ofs.open("results.txt", std::ios::out | std::ios::app);

  ofs << filename << SEPARATOR
      << line << SEPARATOR
      << name << SEPARATOR
      << type << SEPARATOR
      // << old_state << SEPARATOR
      << state << END_LINE;

  ofs.close();
}

// instrumented function

extern "C" void logint1(int i) {
  std::cout << "GOOD" << std::endl;
}

extern "C" void logint(char *filename, int line, char *name, int type, int state, int old_state)
{
  std::cout << "Line " << line << ": " << name << " = " << state << "." << std::endl;
  //  loglinevarint(line, name, state, old_state);
  std::string s = std::to_string(state);
  const char *pchar = s.c_str();
  write2file(filename, line, name, type, pchar, pchar);
}

extern "C" void logbool(char *filename, int line, char *name, int type, bool state, bool old_state)
{
  std::cout << "Line " << line << ": " << name << " = " << state << "." << std::endl;
  char const *pchar = state ? "1" : "0";
  write2file(filename, line, name, type, pchar, pchar);
}

extern "C" void logchar(char *filename, int line, char *name, int type, char state, char old_state)
{
  if (state == 1 || state == 0)
  {
    logbool(filename, line, name, type, state, old_state);
    return;
  }
  char pchar[] = "";
  pchar[0] = state;

  write2file(filename, line, name, type, pchar, pchar);
}

extern "C" void logstring(char *filename, int line, char *name, int type, char const *state, char const *old_state)
{
  std::cout << "Line " << line << ": " << name << " = " << state << "." << std::endl;
  //    write2file(filename, line, name, state, old_state);
}

extern "C" void logchararray(char *filename, int line, char *name, int type, char const *state, char const *old_state)
{
  std::cout << "Line " << line << ": " << name << " = " << state << "." << std::endl;
  write2file(filename, line, name, type, state, old_state);
}

extern "C" void logcharasterisk(char *filename, int line, char *name, int type, const char *state, char const *old_state)
{
  std::cout << "Line " << line << ": " << name << " = " << state << "." << strlen(state) << std::endl;
  // 32~126号 是可打印字符
  for (int i = 0; i < strlen(state); i++)
  {
    if (!isprint(state[i]))
    {
      const char *pchar = "";
      write2file(filename, line, name, type, pchar, pchar);
      return;
    }
  }
  write2file(filename, line, name, type, state, state);
}

extern "C" void logdouble(char *filename, int line, char *name, int type, double state, double old_state)
{
  std::string s = std::to_string(state);
  // std::cout << "测试 double " << state << std::endl;
  char const *pchar = s.c_str();
  write2file(filename, line, name, type, pchar, pchar);
}

extern "C" void logfloat(char *filename, int line, char *name, int type, float state, float old_state)
{
  std::string s = std::to_string(state);
  // std::cout << "测试 float " << state << std::endl;
  char const *pchar = s.c_str();
  write2file(filename, line, name, type, pchar, pchar);
}

extern "C" void loglinevarint(int line, char *name, int state, int old_state)
{
  //  文件名，行号，变量，时间，前值，后值
  std::cout << "BEFORE: Line " << line << ": " << name << " = " << old_state << "." << std::endl;
  std::cout << "AFTER:  Line " << line << ": " << name << " = " << state << "." << std::endl;
}
