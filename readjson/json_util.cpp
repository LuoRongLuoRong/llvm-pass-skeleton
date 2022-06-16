#include "include/json_util.h"

// JsonUtil::JsonUtil() {
//     save();
// }

bool JsonUtil::hasSaved = false;

void JsonUtil::save()
{
    if (hasSaved)
    {
        return;
    }
    hasSaved = true;
    std::ifstream ifs(jsonPath);
    Json::Reader reader;
    Json::Value files;
    reader.parse(ifs, files);
    // 读取 json 文件
    for (int i = 0; i < files.size(); ++i)
    {
        // "SV": "m_check_state",
        // "column": 13,
        // "filepath": "http/http_conn.cpp",
        // "line": 348,
        // "type": 0

        // 从 json 文件中读取内容
        int line = files[i]["line"].asInt();                      // "line": 349,
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
    }
}

std::string JsonUtil::getKey(std::string filepath, int line, int column)
{
    std::string sep = "_";
    std::string key = filepath + sep + std::to_string(line) + sep + std::to_string(column);
    return key;
}

// // useless
// std::string JsonUtil::getKey(std::string filepath, int line, int column, std::string sv) {
//     std::string sep = "_";
//     std::string key = filepath + sep + std::to_string(line) + sep + std::to_string(column) + sep + sv;
//     return key;
// }

///////////////////////////////////

bool JsonUtil::hasVar(std::string filepath, int line, int column)
{
    std::string key = getKey(filepath, line, column);
    return varMap.end() != varMap.find(key);
}

Variable JsonUtil::getVar(std::string filepath, int line, int column)
{
    std::string key = getKey(filepath, line, column);
    return varMap[key];
}

bool JsonUtil::inFilepaths(std::string filepath)
{
    return filepaths.count(filepath) == 1;
}

bool JsonUtil::inSvs(std::string sv)
{
    // there is no guarantee on the name of LLVM values,
    // it can be arbitrary names depending on how the frontend (in this case, Clang) wants to generate.
    // so the function is actually useless
    return svs.count(sv) == 1;
}
