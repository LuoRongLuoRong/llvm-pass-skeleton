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

  std::map<std::string, std::map<std::string, std::vector<int>>> mapFileVariable;
  // 读取 json 文件
  for (int i = 0; i < files.size(); ++i) {
//    "LOC": 349,
//    "SV": "m_check_state",
//    "filepath": "http/http_conn.cpp"

//  "SV": "line_status",
//         "column": 82,
//         "filepath": "http/http_conn.cpp",
//         "line": 348,
//         "type": 1

    int line = files[i]["line"].asInt();
    int column = files[i]["column"].asInt();
    int type = files[i]["type"].asInt();
    std::string varname = files[i]["SV"].asString();
    std::string filename = files[i]["filepath"].asString();

    std::map<std::string, std::vector<int>> mapVariableLines;
    if (mapFileVariable.find(filename) != mapFileVariable.end()) {
      mapVariableLines = mapFileVariable[filename];
    }

    std::vector<int> lines;
    if (mapVariableLines.find(varname) != mapVariableLines.end()) {
      lines = mapVariableLines[varname];
    }
    // 更新 lines
    lines.push_back(line);

    // 更新 mapVariableLines
    mapVariableLines[varname] = lines;

    // 更新 mapFileVariable
    mapFileVariable[filename] = mapVariableLines;

    mapSvType[varname] = type;
  }
//  printMapFileVariable(mapFileVariable);
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
bool jsonutil::hasVariable(std::map< std::string, std::map<std::string, std::vector<int>> > mapFileVariable,
                           std::string filename, std::string key) {
  std::map<std::string, std::vector<int>> mapVariableLines = mapFileVariable[filename];

  // 由于 LLVM 的 IR 会在变量的名称后加数字
  std::map<std::string, std::vector<int>> ::iterator it;
  std::map<std::string, std::vector<int>> ::iterator itEnd;
  it = mapVariableLines.begin();
  itEnd = mapVariableLines.end();

  while (it != itEnd){
    std::string savedKey = it->first;
    // key starts with prefix savedKey
    if (key.find(savedKey) == 0) {
      return true;
    }
    ++it;
  }

  return false;
}

// 返回 variable 的原始名称
std::string jsonutil::getVarname(std::map< std::string, std::map<std::string, std::vector<int>> > mapFileVariable,
                                 std::string filename, std::string key) {
  std::map<std::string, std::vector<int>> mapVariableLines = mapFileVariable[filename];

  // 由于 LLVM 的 IR 会在变量的名称后加数字
  std::map<std::string, std::vector<int>> ::iterator it;
  std::map<std::string, std::vector<int>> ::iterator itEnd;
  it = mapVariableLines.begin();
  itEnd = mapVariableLines.end();

  while (it != itEnd){
    std::string savedKey = it->first;
    // key starts with prefix savedKey
    if (key.find(savedKey) == 0) {
      return savedKey;
    }
    ++it;
  }

  return key;
}

bool jsonutil::hasLine(std::vector<int> lines, int line) {
  return std::find(lines.begin(), lines.end(), line) != lines.end();
}

bool jsonutil::hasLine(std::map< std::string, std::map<std::string, std::vector<int>> > mapFileVariable,
                       std::string filename, std::string varname, int line) {
  std::map<std::string, std::vector<int>> mapVariableLines = mapFileVariable[filename];
  std::vector<int> lines = mapVariableLines[varname];
  return std::find(lines.begin(), lines.end(), line) != lines.end();
}

void jsonutil::printMapFileVariable(std::map< std::string, std::map<std::string, std::vector<int>> > mapFileVariable) {
  std::map < std::string, std::map<std::string, std::vector<int>> > ::iterator it;
  std::map < std::string, std::map<std::string, std::vector<int>> > ::iterator itEnd;
  it = mapFileVariable.begin();
  itEnd = mapFileVariable.end();
  while (it != itEnd){
    std::cout << "\n" << it->first << ": ";

    std::map<std::string, std::vector<int>> mapVariableLines = it->second;
    std::map<std::string, std::vector<int>> ::iterator itv;
    std::map<std::string, std::vector<int>> ::iterator itEndv;
    itv = mapVariableLines.begin();
    itEndv = mapVariableLines.end();
    while (itv != itEndv){
      std::cout << "\n\t" << itv->first;
      std::vector<int> lines = itv->second;
      for (auto line : lines)
        std::cout << line << ' ';
      ++itv;
    }

    ++it;
  }
}
