#include "json_util.h"
#include <stdio.h>
#include <iostream>
#include <unistd.h>

// 最简单的懒汉模式
JsonUtil *JsonUtil::getInstance()
{
    if (instance == NULL)
    {
        instance = new JsonUtil();
        jsonPath = "../SVsite.json";
        // std::string jsonPath = "/home/fdse/luorong/LLVM/test/llvm-pass-skeleton/SVsite.json";
        // std::cout << "请输入 SVsite.json 的相对路径：" << jsonPath;
        // std::cin >> jsonPath;
        // std::cout << "json 的的相对路径：是 " << jsonPath << std::endl;
    }
    return instance;
}

void JsonUtil::save()
{
    if (variables.size() > 0)
    {
        return;
    }
    std::ifstream ifs(jsonPath);
    Json::Reader reader;
    Json::Value files;
    reader.parse(ifs, files);
    // 读取 json 文件
    std::cout << jsonPath << files.size() << std::endl;

    for (int i = 0; i < files.size(); ++i)
    {

        //  "SV": "line_status",
        //         "column": 82,
        //         "filepath": "http/http_conn.cpp",
        //         "line": 348,
        //         "type": 1

        // 从 json 文件中读取内容
        int line = files[i]["line"].asInt();                    // "line": 349,
        std::string sv = files[i]["SV"].asString();             // "SV": "m_check_state",
        std::string filepath = files[i]["filepath"].asString(); // "filepath": "http/http_conn.cpp"
        int type = files[i]["type"].asInt();                    // "type": 0
        int column = files[i]["column"].asInt();

        // 初始化变量，方便后续检查 filepath 和 sv 是否存在
        filepaths.insert(filepath);
        svs.insert(sv);

        // 初始化 var
        Variable var;
        var.line = line;
        var.column = column;
        var.filepath = filepath;
        var.sv = sv;
        var.svl = sv.length();
        var.type = type;

        std::cout << line << ", " << column << ", " << sv << "\n";

        // 在 variables 中记录 variable
        variables.push_back(var);

        // 在 varMap 中记录 variable
        std::string key = getKey(filepath, line, column);
        varMap[key] = var;

        std::string sep = "_";

        // 在 svTypeMap 中记录
        svTypeMap[filepath + sep + sv] = type;
        varnameMap[filepath + sep + sv] = sv;
    }
}

std::string JsonUtil::getKey(std::string filepath, int line, int column)
{
    std::string sep = "_";
    // std::string key = filepath + sep + std::to_string(line) + sep + std::to_string(column);
    std::string key = filepath + sep + std::to_string(line);
    return key;
}

// column 保留变量，实际上并没有用。
bool JsonUtil::hasVar(std::string filepath, int line, int column, std::string varname)
{
    if (inSvs(varname))
    {
        std::string key = getKey(filepath, line, column);
        bool forDebug = varMap.end() != varMap.find(key);
        std::cout << forDebug << filepath << "::" << line << "::" << column << "::" << varname << std::endl;
        return varMap.end() != varMap.find(key);
    }
    else
    {
        // 补丁 is here
        // 将 varname 与每个 variable 进行对比
        for (auto variable : variables)
        {
            int svl = variable.svl;  // true name
            std::string sv = variable.sv;
            std::string varnameTemp = varname.substr(0, svl);
            if (sv.compare(varnameTemp) == 0)
            {
                std::string sep = "_";
                varnameMap[filepath + sep + varname] = sv;
                std::cout << "true" << filepath << "::" << line << "::" << column << "::" << varname << std::endl;
        
                return true;
            }
        }
    }
    std::cout << "False" << filepath << "::" << line << "::" << column << "::" << varname << std::endl; 
    return false;
}

Variable JsonUtil::getVar(std::string filepath, int line, int column, std::string sv)
{
    std::string key = getKey(filepath, line, column);
    return varMap[key];
}

std::string JsonUtil::getVarname(std::string filepath, std::string varname) {
    std::string sep = "_";
    return varnameMap[filepath + sep + varname];
}

int JsonUtil::getType(std::string filepath, std::string sv) {
    std::string sep = "_";
    return svTypeMap[filepath + sep + sv];
}

// 是否存在 file, 是精确匹配
bool JsonUtil::inFilepaths(std::string filepath)
{
    return filepaths.count(filepath) == 1;
}

// 是否存在 variable，是模糊匹配
bool JsonUtil::inSvs(std::string sv)
{
    // there is no guarantee on the；name of LLVM values,
    // it can be arbitrary names depending on how the frontend (in this case, Clang) wants to generate.
    // so the function is actually useless
    return svs.count(sv) == 1;
}

// init static member
JsonUtil *JsonUtil::instance = NULL;
std::string JsonUtil::jsonPath = "";
std::unordered_map<std::string, std::string> JsonUtil::varnameMap = {};
