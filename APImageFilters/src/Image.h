// src/Image.h
#ifndef IMAGE_H
#define IMAGE_H

#include <string>


/**
 * @class Image
 * @brief Load and save images, convert between color spaces.
 * 
 * The Image class provides methods to load and save the images.
 * The Image class also provides methods to convert images between RGB, HSV, and HSL color spaces.
 * It uses the stb_image and stb_image_write libraries for image I/O.
 * 
 * Image data is stored as unsigned char values in row-major order.
 * The data is stored in the format: [R, G, B, R, G, B, ...].
 * 
 * Example usage:
 * @code
 * Image img("input.png");
 * img.convertToHSV();
 * img.save("output.png");
 * @endcode
 */
class Image {
public:
    Image(const std::string &filename);
    ~Image();

    void save(const std::string &filename);
    unsigned char* getData() const;
    void setData(unsigned char *newData);
    int getWidth() const;
    int getHeight() const;
    int getChannels() const;
    void setChannels(int newChannels);

    void convertToHSV();
    void convertToRGBFromHSV();
    void convertToHSL();
    void convertToRGBFromHSL();

    Image(int width, int height, int channels); // New constructor

private:
    int width;
    int height;
    int channels;
    unsigned char* data;
};

#endif