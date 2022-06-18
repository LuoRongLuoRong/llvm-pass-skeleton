//
// Created by fdse on 5/10/22.
//

#ifndef SKELETON_READ_JSON_H
#define SKELETON_READ_JSON_H

#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include "json.h"
#include "json-forwards.h"
#include "../src/jsoncpp.cpp"


class jsonutil
{
public:
    std::map<std::string, int> mapSvType;
    std::vector<std::string> getFilenames(std::map< std::string, std::map<std::string, std::vector<int>> > mapFileVariable);
    std::vector<std::string> getVariables(std::map<std::string, std::vector<int>> mapVariableLines);
    std::vector<int> getVariableLines(std::map<std::string, std::vector<int>>mapVariableLines, std::string key);
    bool hasVariable(std::map<std::string, std::vector<int>> mapVariableLines, std::string key);
    bool hasVariable(std::map< std::string, std::map<std::string, std::vector<int>> > mapFileVariable, std::string filename, std::string key);
    bool hasFile(std::map< std::string, std::map<std::string, std::vector<int>> > mapFileVariable, std::string key);
    bool hasLine(std::vector<int> lines, int line);
    bool hasLine(std::map< std::string, std::map<std::string, std::vector<int>> > mapFileVariable,
                 std::string filename, std::string varname, int line);
    std::map<std::string, std::map<std::string, std::vector<int>>> readfile(std::string jsonPath);
    std::map<std::string, std::map<std::string, std::vector<int>>> readSVsiteJson(std::string jsonPath);
    void printMapFileVariable(std::map< std::string, std::map<std::string, std::vector<int>> > mapFileVariable);
    std::string getVarname(std::map< std::string, std::map<std::string, std::vector<int>> > mapFileVariable,
                           std::string filename, std::string key);
};

#endif //SKELETON_READ_JSON_H