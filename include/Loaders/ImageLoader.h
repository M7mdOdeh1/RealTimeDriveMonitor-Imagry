# pragma once

#include <string>
#include <vector>

class ImageLoader {

public:
    ImageLoader(const std::string directory);
    std::vector<std::string> loadImages();
    void setDirectory(const std::string directory);
    std::string getDirectory() const;

private:
    std::string directoryPath;
};


