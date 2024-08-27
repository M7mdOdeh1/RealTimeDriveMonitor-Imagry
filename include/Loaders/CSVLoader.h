#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <sstream>

class CSVLoader {
public:
    CSVLoader(const std::string& filePath);
    std::vector<std::vector<std::string>> loadCSV();

private:
    std::string filePath;
};

