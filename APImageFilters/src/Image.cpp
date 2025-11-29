// src/Image.cpp
#include "Image.h"
#include <iostream>
#include "stb_image.h"
#include "stb_image_write.h"
#include <cmath> // Add this for fmod and fabs
#include <cstring>  // Add this at the top of Volume.cpp


/**
 * @brief Construct a new Image object from the given filename.
 * 
 * The image data is loaded using the stb_image library.
 * 
 * @param filename The filename of the image to load.
 * @throw std::cerr if the image fails to load.
 */
Image::Image(const std::string &filename) {
    unsigned char* loadedData = stbi_load(filename.c_str(), &width, &height, &channels, 0);
    if (!loadedData) {
        std::cerr << "Failed to load image: " << filename << std::endl;
        data = nullptr;
        return;
    }

    data = new unsigned char[width * height * channels]; // Allocate memory
    std::memcpy(data, loadedData, width * height * channels); // Copy loaded data
    stbi_image_free(loadedData); // Free stb_image memory
}

/**
 * @brief Destroy the Image object.
 * 
 * The image data is freed using the stb_image library.
 */
Image::~Image() {
    if (data) {
        // std::cout << "Freeing Image data at " << static_cast<void*>(data) << std::endl;
        stbi_image_free(data);
        data = nullptr; // Prevent double free
    }
}

/**
 * @brief Save the image to the given filename.
 * 
 * The image data is saved using the stb_image_write library.
 * 
 * @param filename The filename to save the image to.
 * @throw std::cerr if there is no image data to save.
 */
void Image::save(const std::string &filename) {
    if (!data) {
        std::cerr << "No image data to save." << std::endl;
        return;
    }
    stbi_write_png(filename.c_str(), width, height, channels, data, width * channels);
}

/**
 * @brief Get the image data.
 * 
 * @return The image data.
 * @throw std::cerr if there is no image data.
 * @see setData
 */
unsigned char* Image::getData() const {
    return data;
}

/**
 * @brief Set the image data.
 * 
 * The existing image data is freed before setting the new data.
 * Avoids freeing the same pointer by checking if the new data is the same as the existing data.
 * 
 * @param newData The new image data.
 * @see getData
 */
void Image::setData(unsigned char *newData) {
    if (data && data != newData) { // Avoid freeing the same pointer
        // std::cout << "Warning: Overwriting existing Image data at " 
        //           << static_cast<void*>(data) << " with new data at "
        //           << static_cast<void*>(newData) << std::endl;
        delete[] data;
    }
    data = newData;
}

int Image::getWidth() const {
    return width;
}

int Image::getHeight() const {
    return height;
}

int Image::getChannels() const {
    return channels;
}

void Image::setChannels(int newChannels) {
    channels = newChannels;
}

// Helper functions for min/max
float min3(float a, float b, float c) { return std::min(a, std::min(b, c)); }
float max3(float a, float b, float c) { return std::max(a, std::max(b, c)); }

/**
 * @brief Convert RGB to HSV.
 * 
 * The RGB values are converted to HSV values and stored in the image data.
 * 
 * @throw std::cerr if the image does not have RGB channels.
 */
void Image::convertToHSV() {
    if (channels < 3) {
        std::cerr << "Error: Image does not have RGB channels.\n";
        return;
    }

    for (int i = 0; i < width * height; ++i) {
        int index = i * channels;
        float r = data[index] / 255.0f;
        float g = data[index + 1] / 255.0f;
        float b = data[index + 2] / 255.0f;

        float maxVal = max3(r, g, b);
        float minVal = min3(r, g, b);
        float delta = maxVal - minVal;

        float h = 0, s = 0, v = maxVal;

        if (delta > 0) {
            s = delta / maxVal;

            if (maxVal == r) {
                h = 60 * std::fmod(((g - b) / delta), 6); // Use std::fmod
            } else if (maxVal == g) {
                h = 60 * (((b - r) / delta) + 2);
            } else {
                h = 60 * (((r - g) / delta) + 4);
            }

            if (h < 0) h += 360;
        }

        // Store HSV (scaled for unsigned char storage)
        data[index] = static_cast<unsigned char>(h / 360.0 * 255);
        data[index + 1] = static_cast<unsigned char>(s * 255);
        data[index + 2] = static_cast<unsigned char>(v * 255);
    }
}

/**
 * @brief Convert HSV back to RGB.
 * 
 * The HSV values are converted back to RGB values and stored in the image data.
 * 
 * @throw std::cerr if the image does not have RGB channels.
 */
void Image::convertToRGBFromHSV() {
    if (channels < 3) {
        std::cerr << "Error: Image does not have RGB channels.\n";
        return;
    }

    for (int i = 0; i < width * height; ++i) {
        int index = i * channels;
        float h = (data[index] / 255.0f) * 360.0f;
        float s = data[index + 1] / 255.0f;
        float v = data[index + 2] / 255.0f;

        float c = v * s;
        float x = c * (1 - std::fabs(std::fmod(h / 60.0, 2) - 1)); // Use std::fabs and std::fmod
        float m = v - c;

        float r = 0, g = 0, b = 0;

        if (0 <= h && h < 60) { r = c; g = x; b = 0; }
        else if (60 <= h && h < 120) { r = x; g = c; b = 0; }
        else if (120 <= h && h < 180) { r = 0; g = c; b = x; }
        else if (180 <= h && h < 240) { r = 0; g = x; b = c; }
        else if (240 <= h && h < 300) { r = x; g = 0; b = c; }
        else { r = c; g = 0; b = x; }

        data[index] = static_cast<unsigned char>((r + m) * 255);
        data[index + 1] = static_cast<unsigned char>((g + m) * 255);
        data[index + 2] = static_cast<unsigned char>((b + m) * 255);
    }
}

/**
 * @brief Convert RGB to HSL.
 * 
 * The RGB values are converted to HSL values and stored in the image data.
 * 
 * @throw std::cerr if the image does not have RGB channels.
 */
void Image::convertToHSL() {
    if (channels < 3) {
        std::cerr << "Error: Image does not have RGB channels.\n";
        return;
    }

    for (int i = 0; i < width * height; ++i) {
        int index = i * channels;
        float r = data[index] / 255.0f;
        float g = data[index + 1] / 255.0f;
        float b = data[index + 2] / 255.0f;

        float maxVal = max3(r, g, b);
        float minVal = min3(r, g, b);
        float delta = maxVal - minVal;

        float h = 0, s = 0, l = (maxVal + minVal) / 2;

        if (delta > 0) {
            s = (l > 0.5) ? delta / (2 - maxVal - minVal) : delta / (maxVal + minVal);

            if (maxVal == r) {
                h = 60 * std::fmod(((g - b) / delta), 6); // Use std::fmod
            } else if (maxVal == g) {
                h = 60 * (((b - r) / delta) + 2);
            } else {
                h = 60 * (((r - g) / delta) + 4);
            }

            if (h < 0) h += 360;
        }

        data[index] = static_cast<unsigned char>(h / 360.0 * 255);
        data[index + 1] = static_cast<unsigned char>(s * 255);
        data[index + 2] = static_cast<unsigned char>(l * 255);
    }
}

/**
 * @brief Convert HSL back to RGB.
 * 
 * The HSL values are converted back to RGB values and stored in the image data.
 * 
 * @throw std::cerr if the image does not have RGB channels.
 */
void Image::convertToRGBFromHSL() {
    if (channels < 3) {
        std::cerr << "Error: Image does not have RGB channels.\n";
        return;
    }

    for (int i = 0; i < width * height; ++i) {
        int index = i * channels;
        float h = (data[index] / 255.0f) * 360.0f;
        float s = data[index + 1] / 255.0f;
        float l = data[index + 2] / 255.0f;

        float c = (1 - std::fabs(2 * l - 1)) * s; // Use std::fabs
        float x = c * (1 - std::fabs(std::fmod(h / 60.0, 2) - 1)); // Use std::fabs and std::fmod
        float m = l - c / 2;

        float r = 0, g = 0, b = 0;

        if (0 <= h && h < 60) { r = c; g = x; b = 0; }
        else if (60 <= h && h < 120) { r = x; g = c; b = 0; }
        else if (120 <= h && h < 180) { r = 0; g = c; b = x; }
        else if (180 <= h && h < 240) { r = 0; g = x; b = c; }
        else if (240 <= h && h < 300) { r = x; g = 0; b = c; }
        else { r = c; g = 0; b = x; }

        data[index] = static_cast<unsigned char>((r + m) * 255);
        data[index + 1] = static_cast<unsigned char>((g + m) * 255);
        data[index + 2] = static_cast<unsigned char>((b + m) * 255);
    }
}

#include <cstring> // For memset

/**
 * @brief Construct a new Image object with the given width, height, and channels.
 * 
 * The image data is initialized with zeros.
 */
Image::Image(int width, int height, int channels)
    : width(width), height(height), channels(channels) {
    data = new unsigned char[width * height * channels](); // Initialize with zeros
}