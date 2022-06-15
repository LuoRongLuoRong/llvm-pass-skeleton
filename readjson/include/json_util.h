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
#include "json.h"
#include "json-forwards.h"
#include "../src/jsoncpp.cpp"

typedef struct Variable {
   std::string filepath;  // filepath
   std::string sv;  // SV
   int type;  // 0: state variable; 1: key variable
   std::vector<int> lines;   // LOC
} Variable;  // 将结构体 struct 重新定义为 Variable，从而不需要在命名时书写 struct 。

class JsonUtil {
    private:
        std::string jsonPath = "SVsite.json";
        std::set<std::string> filepaths;  // filepaths of variables
        std::set<std::string> svs;  // name of variable
        std::vector<Variable> variables;  // what we read from SVSite.json
        std::unordered_map<std::string, Variable> varMap;  // key: filepath+SV; value: variable 
    public:

        void save();
        bool hasVar(std::string filepath, std::string sv);
        bool hasVar(std::string key);
        Variable getVar(std::string filepath, std::string sv);

        bool inFilepaths(std::string filepath);
        bool inSvs(std::string sv);
};

#endif // JSON_UTIL_H
