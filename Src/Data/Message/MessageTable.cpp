#include <fstream>
#include <sstream>
#include <iostream>
#include <unordered_map>
#include <string>
#include "../../Utility/Utility.h"
#include "MessageTable.h"

MessageTable::MessageTable(void) {

}

MessageTable::~MessageTable(void) {
	
}


void MessageTable::Init(void) {
	std::string path = "Data/Data/Message/MessageTable.txt";
	std::ifstream ifs(path);
	std::string buf;
	while (std::getline(ifs, buf)) {
		messageTable_[
			std::stoi(buf.substr(0, buf.find(":")))
		] = 
			Utility::StringToWString(buf.substr(buf.find(":") + 1, buf.length() - buf.find(":")));
	}
}

const std::wstring& MessageTable::GetMessage_(int id) const {
	return messageTable_.at(id);
}
