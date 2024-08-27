#include "Loaders/CSVLoader.h"
#include <iostream>

CSVLoader::CSVLoader(const std::string& filePath) : filePath(filePath) {}

std::vector<std::vector<std::string>> CSVLoader::loadCSV() {
    std::vector<std::vector<std::string>> data;
    std::ifstream file(filePath);

    if (!file.is_open()) {
        throw std::runtime_error("Could not open CSV file: " + filePath);
    }


    std::string line;
    std::getline(file, line);     // skip the first line

    while (std::getline(file, line)) {
        std::stringstream lineStream(line);
        std::string cell;
        std::vector<std::string> row;

        while (std::getline(lineStream, cell, ',')) {
            row.push_back(cell);
        }

        data.push_back(row);
    }

    file.close();
    return data;


}
