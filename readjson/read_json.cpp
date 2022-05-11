#include "read_json.h"

using namespace std;

// 返回以对应类型的key的vector
vector<string> getFilenames(std::map< std::string, std::map<std::string, std::vector<int>> > mapFileVariable)
{
  vector<string> keys;
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

vector<string> getVariables(std::map<std::string, std::vector<int>> mapVariableLines) {
  vector<string> keys;
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

vector<int> getLines(std::map<std::string, std::vector<int>>mapVariableLines, string key) {
  return mapVariableLines[key];
}

std::map<std::string, std::map<std::string, std::vector<int>>> readfile(std::string jsonPath) {
  ifstream ifs(jsonPath);
  Json::Reader reader;
  Json::Value files;
  reader.parse(ifs, files);
  cout << "obj: " << files << endl;

  std::map<std::string, std::map<std::string, std::vector<int>>> mapFileVariable;
  for (int i = 0; i < files.size(); ++i) {
    cout << files[i]["filename"].asString() << endl;
    std::map<std::string, std::vector<int>> mapVariableLines;
    for (int j = 0; j < files[i]["variables"].size(); ++j) {
      cout << files[i]["variables"][j]["varname"].asString() << endl;
      std::vector<int> lines;
      for (int k = 0; k < files[i]["variables"][j]["lines"].size(); ++k) {
        cout << files[i]["variables"][j]["lines"][k] << endl;
        lines.push_back(files[i]["variables"][j]["lines"][k].asInt());
      }
      mapVariableLines[files[i]["variables"][j]["varname"].asString()] = lines;
    }
    mapFileVariable[files[i]["filename"].asString()] = mapVariableLines;
  }
  return mapFileVariable;
}

bool hasFile(std::map< std::string, std::map<std::string, std::vector<int>> > mapFileVariable, std::string key) {
  return mapFileVariable.end() != mapFileVariable.find(key);
}

bool hasVariable(std::map<std::string, std::vector<int>> mapVariableLines, std::string key) {
  return mapVariableLines.end() != mapVariableLines.find(key);
}

bool isInline() {
  // TODO
  return true;
}

// for test
int main() {
  ifstream ifs("./line_value.json");
  Json::Reader reader;
  Json::Value files;
  reader.parse(ifs, files);
  cout << "obj: " << files << endl;

  std::map<std::string, std::map<std::string, std::vector<int>>> mapFileVariable;
  for (int i = 0; i < files.size(); ++i) {
    cout << files[i]["filename"].asString() << endl;
    std::map<std::string, std::vector<int>> mapVariableLines;
    for (int j = 0; j < files[i]["variables"].size(); ++j) {
      cout << files[i]["variables"][j]["varname"].asString() << endl;
      std::vector<int> lines;
      for (int k = 0; k < files[i]["variables"][j]["lines"].size(); ++k) {
        cout << files[i]["variables"][j]["lines"][k] << endl;
        lines.push_back(files[i]["variables"][j]["lines"][k].asInt());
      }
      mapVariableLines[files[i]["variables"][j]["varname"].asString()] = lines;
    }
    mapFileVariable[files[i]["filename"].asString()] = mapVariableLines;
  }

  cout << "***" << endl;

  for(std::string filename: getFilenames(mapFileVariable)) {
    std::cout << filename << "\n";
    std::cout << hasFile(mapFileVariable, filename) << "\n";
    for (std::string varname: getVariables(mapFileVariable[filename])) {
      std::cout << "\t" << hasVariable(mapFileVariable[filename], "hih") << varname << ": ";
      for (int line: getLines(mapFileVariable[filename], varname)) {
        std::cout << line << " ";
      }
      std::cout << std::endl;
    }
  }

  return 0;
}
