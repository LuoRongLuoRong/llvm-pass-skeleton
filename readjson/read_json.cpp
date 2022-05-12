#include "include/read_json.h"

// 返回 filename 的 vector
std::vector<std::string> jsonutil::getFilenames(std::map< std::string, std::map<std::string, std::vector<int>> > mapFileVariable)
{
  std::vector<std::string> keys;
  std::map < std::string, std::map<std::string, std::vector<int>> > ::iterator it;
  std::map < std::string, std::map<std::string, std::vector<int>> > ::iterator itEnd;
  it = mapFileVariable.begin();
  itEnd = mapFileVariable.end();
  while (it != itEnd){
    keys.push_back(it->first);
    ++it;
  }
  return keys;
}

// 返回 variables 的 vector
std::vector<std::string> jsonutil::getVariables(std::map<std::string, std::vector<int>> mapVariableLines) {
  std::vector<std::string> keys;
  std::map<std::string, std::vector<int>> ::iterator it;
  std::map<std::string, std::vector<int>> ::iterator itEnd;
  it = mapVariableLines.begin();
  itEnd = mapVariableLines.end();
  while (it != itEnd){
    keys.push_back(it->first);
    ++it;
  }
  return keys;
}

// 返回 variables 的 lines
std::vector<int> jsonutil::getVariableLines(std::map<std::string, std::vector<int>>mapVariableLines, std::string key) {
  return mapVariableLines[key];
}

// 读取 json，返回 map
std::map<std::string, std::map<std::string, std::vector<int>>> jsonutil::readfile(std::string jsonPath) {
  std::ifstream ifs(jsonPath);
  Json::Reader reader;
  Json::Value files;
  reader.parse(ifs, files);

//  std::cout << files << std::endl;

  std::map<std::string, std::map<std::string, std::vector<int>>> mapFileVariable;
  for (int i = 0; i < files.size(); ++i) {
    std::map<std::string, std::vector<int>> mapVariableLines;
    for (int j = 0; j < files[i]["variables"].size(); ++j) {
      std::vector<int> lines;
      for (int k = 0; k < files[i]["variables"][j]["lines"].size(); ++k) {
        lines.push_back(files[i]["variables"][j]["lines"][k].asInt());
      }
      mapVariableLines[files[i]["variables"][j]["varname"].asString()] = lines;
    }
    mapFileVariable[files[i]["filename"].asString()] = mapVariableLines;
  }
  return mapFileVariable;
}

std::map<std::string, std::map<std::string, std::vector<int>>> jsonutil::readSVsiteJson(std::string jsonPath) {
  std::ifstream ifs(jsonPath);
  Json::Reader reader;
  Json::Value files;
  reader.parse(ifs, files);

  std::cout << files << std::endl;

  std::map<std::string, std::map<std::string, std::vector<int>>> mapFileVariable;
  // 读取 json 文件
  for (int i = 0; i < files.size(); ++i) {
    // {"LOC":"349","SV":"m_check_state","filepath":"http/http_conn.cpp"}
    std::cout << ">>>>>> " << files[i]["LOC"] << std::endl;
    int line = files[i]["LOC"].asInt();
    std::string varname = files[i]["SV"].asString();
    std::string filename = files[i]["filepath"].asString();

    std::map<std::string, std::vector<int>> mapVariableLines;
    if (mapFileVariable.find(filename) != mapFileVariable.end()) {
      mapFileVariable[filename] = mapVariableLines;
    } else {
      mapVariableLines = mapFileVariable[filename];
    }

    std::vector<int> lines;
    if (mapVariableLines.find(varname) != mapVariableLines.end()) {
      mapVariableLines[varname] = lines;
    } else {
      lines = mapVariableLines[varname];
    }
    lines.push_back(line);
  }
  return mapFileVariable;
}

// 判断 filename 是否存在
bool jsonutil::hasFile(std::map< std::string, std::map<std::string, std::vector<int>> > mapFileVariable, std::string key) {
  return mapFileVariable.end() != mapFileVariable.find(key);
}

// 判断 variable 是否存在
bool jsonutil::hasVariable(std::map<std::string, std::vector<int>> mapVariableLines, std::string key) {
  return mapVariableLines.end() != mapVariableLines.find(key);
}

// 判断 variable 是否存在
bool jsonutil::hasVariable(std::map< std::string, std::map<std::string, std::vector<int>> > mapFileVariable, std::string filename, std::string key) {
  std::map<std::string, std::vector<int>> mapVariableLines = mapFileVariable[filename];
  return mapVariableLines.end() != mapVariableLines.find(key);
}


/** writer **/
