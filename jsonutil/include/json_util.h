//
// Created by fdse on 6/15/22.
//

#ifndef JSON_UTIL_H
#define JSON_UTIL_H

#include <iostream>
#include <fstream>
#include <unordered_map>
#include <set>
#include <vector>
#include "jsoncpp.cpp"
#include "json-forwards.h"

typedef struct
{
    std::string filepath; // filepath
    std::string sv;       // SV
    int svl;              // sv 的长度。其实这算是一个补丁，因为 LLVM 编译时在某个变量的名字的后面添加一些数字之类的。
    int type;             // 0: state variable; 1: key variable
    int line;             // line
    int column;           // column
} Variable;               // 将结构体 struct 重新定义为 Variable，从而不需要在命名时书写 struct 。

class JsonUtil
{
private:
    std::set<std::string> filepaths;                  // filepaths of variables
    std::set<std::string> svs;                        // name of variable
    std::vector<Variable> variables;                  // what we read from SVSite.json
    std::unordered_map<std::string, Variable> varMap; // key: filepath+SV; value: variable
    std::unordered_map<std::string, int> svTypeMap;   // key: filepath+SV; value: type

    static JsonUtil *instance;
    static std::string jsonPath;
    static std::unordered_map<std::string, std::string> varnameMap;  // key: filepath+varname; value: SV

    JsonUtil(){};
    ~JsonUtil(){};
    JsonUtil(const JsonUtil &);
    JsonUtil &operator=(const JsonUtil &);

public:
    static JsonUtil *getInstance();

    // 读取文件
    void save();
    //
    std::string getKey(std::string filepath, int line, int column);
    // std::string getKey(std::string filepath, int line, int column, std::string sv);

    bool hasVar(std::string filepath, int line, int column, std::string sv);
    Variable getVar(std::string filepath, int line, int column, std::string sv);
    std::string getVarname(std::string filepath, std::string varname);
    int getType(std::string filepath, std::string sv);

    bool inFilepaths(std::string filepath);
    bool inSvs(std::string sv);

    void dealWithFileSeparatror(std::string & filePath);
};

#endif // JSON_UTIL_H
