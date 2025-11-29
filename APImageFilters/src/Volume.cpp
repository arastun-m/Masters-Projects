#include "Volume.h"
#include "stb_image.h"
#include "stb_image_write.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <filesystem>
#include <cstring>  // Add this at the top of Volume.cpp

namespace fs = std::filesystem;


/**
 * @brief Load a 3D volume from a directory of ordered images.
 * 
 * The Volume constructor loads a 3D volume from a directory of images.
 * The images are sorted to ensure they are loaded in the correct order.
 * The images are loaded in order and stored as slices in the volume.
 * 
 * @param directory The directory containing the volume images.
 * @param filenamePrefix The common prefix for the volume image filenames (e.g., vol: vol0.png, vol1.png).
 * @throw std::cerr if no matching images are found in the directory.
 * @throw std::cerr if an image fails to load.
 */
Volume::Volume(const std::string &directory, const std::string &filenamePrefix) : data(nullptr), width(0), height(0), depth(0), channels(0) {
    std::vector<std::string> fileList;
    for (const auto &entry : fs::directory_iterator(directory)) {
        std::string filePath = entry.path().string();
        std::string fileName = entry.path().filename().string();
        
        // Only include files that start with the given prefix
        if (fileName.rfind(filenamePrefix, 0) == 0) {
            fileList.push_back(filePath);
        }
    }
    std::sort(fileList.begin(), fileList.end()); // Ensure proper order

    if (fileList.empty()) {
        std::cerr << "Error: No matching images found in directory " << directory << std::endl;
        return;
    }

    // Load first image to get dimensions
    int imgWidth, imgHeight, imgChannels;
    unsigned char *imgData = stbi_load(fileList[0].c_str(), &imgWidth, &imgHeight, &imgChannels, 0);
    if (!imgData) {
        std::cerr << "Error loading image: " << fileList[0] << std::endl;
        return;
    }

    width = imgWidth;
    height = imgHeight;
    depth = fileList.size();
    channels = imgChannels;
    size_t dataSize = width * height * depth * channels;
    data = new unsigned char[dataSize];

    // Load all slices
    for (size_t i = 0; i < fileList.size(); ++i) {
        imgData = stbi_load(fileList[i].c_str(), &imgWidth, &imgHeight, &imgChannels, 0);
        if (!imgData) {
            std::cerr << "Error loading image: " << fileList[i] << std::endl;
            delete[] data;
            data = nullptr;
            return;
        }
        std::memcpy(data + (i * width * height * channels), imgData, width * height * channels);
        stbi_image_free(imgData);
    }
}

/**
 * @brief Create an empty 3D volume.
 * 
 * The Volume constructor creates an empty 3D volume with the specified dimensions and number of channels.
 * The volume data is initialized to zero.
 * 
 * @param width The width of the volume.
 * @param height The height of the volume.
 * @param depth The depth (number of slices) of the volume.
 * @param channels The number of channels (e.g., RGB = 3).
 */
Volume::Volume(int width, int height, int depth, int channels)
    : width(width), height(height), depth(depth), channels(channels) {
    size_t dataSize = width * height * depth * channels;
    data = new unsigned char[dataSize](); // Initialize to zero
}

/**
 * @brief Copy constructor for a 3D volume.
 * 
 * The Volume copy constructor creates a deep copy of the source volume.
 * The volume data is copied to a new memory location.
 * Used by 3D Gaussian and Median blur filters.
 * 
 * @param other The source volume to copy.
 */
Volume::Volume(const Volume& other)
    : width(other.width), height(other.height), depth(other.depth), channels(other.channels) {
    size_t dataSize = width * height * depth * channels;
    data = new unsigned char[dataSize];
    std::memcpy(data, other.data, dataSize);
}

/**
 * @brief Assignment operator overloading for a 3D volume.
 * 
 * The Volume assignment operator overloading creates a deep copy of the source volume.
 * The volume data is copied to a new memory location.
 * 
 * @param other The source volume to copy.
 * @return A reference to the new volume.
 */
Volume& Volume::operator=(const Volume& other) {
    if (this == &other) { // prevent self-assignment
        return *this;
    }
    delete[] data; // free existing data
    width = other.width;
    height = other.height;
    depth = other.depth;
    channels = other.channels;
    size_t dataSize = width * height * depth * channels;
    data = new unsigned char[dataSize];
    std::memcpy(data, other.data, dataSize);
    return *this;
}

/**
 * @brief Destructor for a 3D volume.
 * 
 * The Volume destructor frees the memory allocated for the volume data.
 */
Volume::~Volume() {
    if (data) {
        delete[] data;
        data = nullptr; // prevent double free
    }
}

/**
 * @brief Save the 3D volume to a directory of images.
 * 
 * The save method saves the 3D volume to a directory of images.
 * Each slice of the volume is saved as a separate image.
 * The images are saved in the format: vol0.png, vol1.png, ...
 * 
 * @param directory The directory to save the volume images.
 */
void Volume::save(const std::string &directory) const {
    fs::create_directories(directory);
    for (int z = 0; z < depth; ++z) {
        std::string filename = directory + "/vol" + std::to_string(z) + ".png";
        stbi_write_png(filename.c_str(), width, height, channels,
                       data + (z * width * height * channels), width * channels);
    }
}

// Accessors
unsigned char* Volume::getData() const { return data; }
void Volume::setData(unsigned char *newData) {
    std::memcpy(data, newData, width * height * depth * channels);
}
int Volume::getWidth() const { return width; }
int Volume::getHeight() const { return height; }
int Volume::getDepth() const { return depth; }
int Volume::getChannels() const { return channels; }
void Volume::setChannels(int newChannels) { channels = newChannels; }

/**
 * @brief Get the voxel value at the specified position.
 * 
 * The getVoxel method returns the voxel value at the specified position.
 * If the position is outside the volume bounds, it returns 0.
 * 
 * @param x The x-coordinate of the voxel.
 * @param y The y-coordinate of the voxel.
 * @param z The z-coordinate of the voxel.
 * @param channel The channel index (e.g., 0 for red, 1 for green, 2 for blue).
 * @return The voxel value at the specified position.
 */
unsigned char Volume::getVoxel(int x, int y, int z, int channel) const {
    if (x < 0 || x >= width || y < 0 || y >= height || z < 0 || z >= depth || channel < 0 || channel >= channels) {
        return 0;
    }
    return data[(z * width * height + y * width + x) * channels + channel];
}

/**
 * @brief Set the voxel value at the specified position.
 * 
 * The setVoxel method sets the voxel value at the specified position.
 * If the position is outside the volume bounds, it does nothing.
 * 
 * @param x The x-coordinate of the voxel.
 * @param y The y-coordinate of the voxel.
 * @param z The z-coordinate of the voxel.
 * @param value The new value for the voxel.
 * @param channel The channel index (e.g., 0 for red, 1 for green, 2 for blue).
 */
void Volume::setVoxel(int x, int y, int z, unsigned char value, int channel) {
    if (x < 0 || x >= width || y < 0 || y >= height || z < 0 || z >= depth || channel < 0 || channel >= channels) {
        return;
    }
    data[(z * width * height + y * width + x) * channels + channel] = value;
}

/**
 * @brief Extract a sub-volume (thin slab) between minZ and maxZ.
 * 
 * The sub-volume is a copy of the original volume with only the specified depth range.
 * 
 * @param minZ The minimum depth index of the sub-volume.
 * @param maxZ The maximum depth index of the sub-volume.
 * @return The sub-volume between minZ and maxZ.
 */
Volume Volume::extractSubVolume(int minZ, int maxZ) const {
    // Ensure bounds are within the valid depth range
    if (minZ < 0) minZ = 0;
    if (maxZ >= depth) maxZ = depth - 1;
    if (minZ > maxZ) {
        std::cerr << "Error: Invalid slab range (" << minZ << " to " << maxZ << ").\n";
        return Volume(width, height, 0, channels); // Return an empty volume
    }

    int newDepth = maxZ - minZ + 1;
    Volume subVolume(width, height, newDepth, channels);

    for (int z = minZ; z <= maxZ; ++z) {
        int srcOffset = z * width * height * channels;
        int destOffset = (z - minZ) * width * height * channels;
        std::memcpy(subVolume.getData() + destOffset, data + srcOffset, width * height * channels);
    }

    return subVolume;
}