#include <iostream>
#include <fstream>
#include <json/json.h>

using namespace std;

int main() {
	ifstream ifs("./line_value.json");
	Json::Reader reader;
	Json:Value obj;
	reader.parse(ifs, obj);
	cout << "lastname: " << obj["lastname"].asString() << endl;
	return 0;
}
