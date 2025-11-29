#include "Slice.h"
#include <iostream>
#include <memory>


/**
 * @brief Extract a 2D XY slice from a 3D volume.
 * 
 * Extracts a 2D XY slice from a 3D volume at the given z-index.
 * The slice is stored as an Image object.
 * 
 * @param volume 3D volume to extract the slice from.
 * @param zIndex Z-index of the slice to extract.
 * @return std::unique_ptr<Image> Image object containing the extracted slice.
 * @throw std::cerr if the z-index is out of range.
 * @see Volume
 */
std::unique_ptr<Image> Slice::extractXY(const Volume& volume, int zIndex) {
    if (zIndex < 0 || zIndex >= volume.getDepth()) {
        std::cerr << "Error: zIndex out of range." << std::endl;
        return std::make_unique<Image>(0, 0, volume.getChannels());
    }

    int width = volume.getWidth();
    int height = volume.getHeight();
    int channels = volume.getChannels();
    auto slice = std::make_unique<Image>(width, height, channels);
    std::cout << "Extracting XY slice at z=" << zIndex << "...\n";

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            for (int c = 0; c < channels; ++c) {
                slice->getData()[(y * width + x) * channels + c] = 
                    volume.getVoxel(x, y, zIndex, c);
            }
        }
    }
    // std::cout << "Finished extracting XY slice." << std::endl;
    return slice;
}

/**
 * @brief Extract a 2D XZ slice from a 3D volume.
 * 
 * Extracts a 2D XZ slice from a 3D volume at the given y-index.
 * The slice is stored as an Image object.
 * 
 * @param volume 3D volume to extract the slice from.
 * @param yIndex Y-index of the slice to extract.
 * @return std::unique_ptr<Image> Image object containing the extracted slice.
 * @throw std::cerr if the y-index is out of range.
 * @see Volume
 */
std::unique_ptr<Image> Slice::extractXZ(const Volume& volume, int yIndex) {
    if (yIndex < 0 || yIndex >= volume.getHeight()) {
        std::cerr << "Error: yIndex out of range." << std::endl;
        return std::make_unique<Image>(0, 0, volume.getChannels());
    }

    int width = volume.getWidth();
    int depth = volume.getDepth();
    int channels = volume.getChannels();
    auto slice = std::make_unique<Image>(width, depth, channels);
    std::cout << "Extracting XZ slice at y=" << yIndex << "...\n";

    for (int z = 0; z < depth; ++z) {
        for (int x = 0; x < width; ++x) {
            for (int c = 0; c < channels; ++c) {
                slice->getData()[(z * width + x) * channels + c] = 
                    volume.getVoxel(x, yIndex, z, c);
            }
        }
    }
    // std::cout << "Finished extracting XZ slice." << std::endl;
    return slice;
}

/**
 * @brief Extract a 2D YZ slice from a 3D volume.
 * 
 * Extracts a 2D YZ slice from a 3D volume at the given x-index.
 * The slice is stored as an Image object.
 * 
 * @param volume 3D volume to extract the slice from.
 * @param xIndex X-index of the slice to extract.
 * @return std::unique_ptr<Image> Image object containing the extracted slice.
 * @throw std::cerr if the x-index is out of range.
 */
std::unique_ptr<Image> Slice::extractYZ(const Volume& volume, int xIndex) {
    if (xIndex < 0 || xIndex >= volume.getWidth()) {
        std::cerr << "Error: xIndex out of range." << std::endl;
        return std::make_unique<Image>(0, 0, volume.getChannels());
    }

    int height = volume.getHeight();
    int depth = volume.getDepth();
    int channels = volume.getChannels();
    auto slice = std::make_unique<Image>(height, depth, channels);
    std::cout << "Extracting YZ slice at x=" << xIndex << "...\n";

    for (int z = 0; z < depth; ++z) {
        for (int y = 0; y < height; ++y) {
            for (int c = 0; c < channels; ++c) {
                slice->getData()[(z * height + y) * channels + c] = 
                    volume.getVoxel(xIndex, y, z, c);
            }
        }
    }
    // std::cout << "Finished extracting YZ slice." << std::endl;
    return slice;
}