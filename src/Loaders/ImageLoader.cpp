#include "Loaders/ImageLoader.h"
#include <QDir>
#include <algorithm>

ImageLoader::ImageLoader(const std::string directory)
    : directoryPath(directory) {}

std::vector<std::string> ImageLoader::loadImages() {
    QDir dir(QString::fromStdString(directoryPath));
    QStringList qImageFiles = dir.entryList(QStringList() << "*.jpeg" << "*.jpg" << "*.png", QDir::Files);

    std::vector<std::string> imageFiles;
    for (const QString& filename : qImageFiles) {
        imageFiles.push_back(filename.toStdString());
    }

    // Sort images by the numeric index before the last dot and after the last slash
    std::sort(imageFiles.begin(), imageFiles.end(), [](const std::string& a, const std::string& b) {
        std::string aIndex = a.substr(a.find_last_of('/') + 1, a.find_last_of('.') - a.find_last_of('/') - 1);
        std::string bIndex = b.substr(b.find_last_of('/') + 1, b.find_last_of('.') - b.find_last_of('/') - 1);
        return std::stod(aIndex) < std::stod(bIndex);
    });

    return imageFiles;
}

void ImageLoader::setDirectory(const std::string directory) {
    directoryPath = directory;
}

std::string ImageLoader::getDirectory() const {
    return directoryPath;
}