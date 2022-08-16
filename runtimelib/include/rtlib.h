#include <iostream>
#include <string>
#include <cstring>
#include <ctype.h>
#include <stdio.h>
#include <time.h>
#include <fstream> //操作文件的头文件

// write to file
extern "C" void write2file(char *filename, int line, char *name, int type, const char *state, const char *old_state);
// instrumented function

extern "C" void logint(char *filename, int line, char *name, int type, int state, int old_state);

extern "C" void logbool(char *filename, int line, char *name, int type, bool state, bool old_state);

extern "C" void logchar(char *filename, int line, char *name, int type, char state, char old_state);

extern "C" void logstring(char *filename, int line, char *name, int type, char const *state, char const *old_state);

extern "C" void logchararray(char *filename, int line, char *name, int type, char const *state, char const *old_state);

extern "C" void logcharasterisk(char *filename, int line, char *name, int type, const char *state, char const *old_state);

extern "C" void logdouble(char *filename, int line, char *name, int type, double state, double old_state);

extern "C" void logfloat(char *filename, int line, char *name, int type, float state, float old_state);

extern "C" void loglinevarint(int line, char *name, int state, int old_state);