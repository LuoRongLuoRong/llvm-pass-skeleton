#include "include/json_util.h"

// JsonUtil::JsonUtil() {
//     save();
// }

void JsonUtil::save()
{
    std::ifstream ifs(jsonPath);
    Json::Reader reader;
    Json::Value files;
    reader.parse(ifs, files);
    // 读取 json 文件
    for (int i = 0; i < files.size(); ++i)
    {
        //    "LOC": 349,
        //    "SV": "m_check_state",
        //    "filepath": "http/http_conn.cpp"
        //    "type": 0

        int loc = files[i]["LOC"].asInt();                      // "LOC": 349,
        std::string sv = files[i]["SV"].asString();             // "SV": "m_check_state",
        std::string filepath = files[i]["filepath"].asString(); // "filepath": "http/http_conn.cpp"
        int type = files[i]["type"].asInt();                    // "type": 0

        // Variable 是否存在
        if (hasVar(filepath, sv))
        {
            // 将 loc 补充进去即可。
            varMap[filepath + sv].lines.push_back(loc);
        }
        else
        {
            filepaths.insert(filepath);
            svs.insert(sv);

            std::vector<int> lines;
            lines.push_back(loc);

            // 初始化 var
            Variable var;        
            var.lines = lines;
            var.filepath = filepath;
            var.sv = sv;
            var.type = type;

            // 在 variables 中记录 variable
            variables.push_back(var);
            // 在 varMap 中记录 variable
            varMap[filepath + sv] = var;
        }
    }
}

bool JsonUtil::hasVar(std::string filepath, std::string sv)
{
    return hasVar(filepath + sv);
}

bool JsonUtil::hasVar(std::string key)
{
    return varMap.end() != varMap.find(key);
}

Variable JsonUtil::getVar(std::string filepath, std::string sv)
{
    return varMap[filepath + sv];
}

bool JsonUtil::inFilepaths(std::string filepath) {
    return filepaths.count(filepath) == 1;
}

bool JsonUtil::inSvs(std::string sv) {
    return svs.count(sv) == 1;
}
