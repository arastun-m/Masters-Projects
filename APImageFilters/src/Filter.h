#ifndef FILTER_H
#define FILTER_H

#include "Image.h"
#include "Volume.h"
#include <iostream>
#include <algorithm>  // For std::max and std::min
#include <cstdlib>    // For rand()
#include <cstring>    // For memcpy
#include <cmath>      // For math functions
#include <vector>     // For Median Blur


/**
 * @class Filter
 * @brief A collection of image processing filters.
 * 
 * The Filter class provides a set of static methods for applying various filters to images.
 * Filters include greyscale conversion, thresholding, brightness adjustment, noise addition,
 * histogram equalization, blurring, edge detection, and more.
 * 
 * The class also includes 3D filters for volume data; 3D median and Gaussian blurring.
 * 
 * Example usage:
 * @code
 * Image img("input.png");
 * Filter::applyGreyscale(img);
 * Filter::applyThreshold(img, 128);
 * img.save("output.png");
 * @endcode
 */
class Filter {
public:
    // Constructor & Destructor
    Filter();
    ~Filter();

    // Greyscale filter
    static void applyGreyscale(Image &img);

    // Threshold filter
    static void applyThreshold(Image &img, int threshold, const std::string& type= "HSV");

    // Brightness adjustment
    static void applyBrightness(Image& img, int brightnessOffset);
    static void applySharpening(Image& img);

    // Salt & Pepper Noise
    static void applySaltPepperNoise(Image &img, float percentage);

    // Histogram Equalization with color space option
    // type: 0 = HSV value channel (default), 1 = HSL luminance channel
    static void applyHistogramEqualization(Image &img, std::string type="HSV");

    // Box Blur filter
    static void applyBoxBlur(Image& img, int kernelSize);

    // Median Blur filter
    static void applyMedianBlur(Image& img, int kernelSize);
    
    // Gaussian Blur filter
    static void applyGaussianBlur(Image& img, int kernelSize, float stdev);

    // Edge detection filter
    static void applyEdgeDetection(Image &img, std::string type);

    // 3D Median Blur filter
    static void applyMedianBlur3D(Volume& vol, int kernelSize);

    // 3D Gaussian Blur filter
    static void applyGaussianBlur3D(Volume& vol, int kernelSize, float stdev);

private:
    // Helper Functions for 2D Box Blur
    static void horizontalBoxBlur(unsigned char* input, unsigned char* output, int width, int height, int channels, int kernelSize);
    static void verticalBoxBlur(unsigned char* input, unsigned char* output, int width, int height, int channels, int kernelSize);

    // Helper Functions for 2D Median Blur
    static void initializeHistogram(const unsigned char* inputData, std::vector<int>& histogram, int& count,
        int width, int height, int channels, int c, int y, int xStart, int kernelSize);
    static void updateHistogram(const unsigned char* inputData, std::vector<int>& histogram, int& count,
                     int width, int height, int channels, int c, int y, int x, int kernelSize);
    static unsigned char findMedian(const std::vector<int>& histogram, int count);

    // Helper Functions for 2D Gaussian Blur
    static std::vector<float> createGaussianKernel1D(int kernelSize, float stdev);
    static void applyGaussianBlur1DHorizontal(const unsigned char* input, unsigned char* output,
        int width, int height, int channels,
        const std::vector<float>& kernel);
    static void applyGaussianBlur1DVertical(const unsigned char* input, unsigned char* output,
        int width, int height, int channels,
        const std::vector<float>& kernel);

    // Helper Functions for 3D Median Blur
    static void initializeHistogram3D(const Volume& vol, std::vector<int>& histogram, int& count,
        int width, int height, int depth, int channels, int c, int z, int y, int xStart, int kernelSize);
    static void updateHistogram3D(const Volume& vol, std::vector<int>& histogram, int& count,
        int width, int height, int depth, int channels, int c, int z, int y, int x, int kernelSize);
    
    // Helper Functions for 3D Gaussian Blur
    static std::vector<float> create3DGaussianKernel1D(int kernelSize, float stdev);
    static void applyGaussianBlur1DX(Volume& vol, Volume& tempVol, const std::vector<float>& kernel);
    static void applyGaussianBlur1DY(Volume& vol, Volume& tempVol, const std::vector<float>& kernel);
    static void applyGaussianBlur1DZ(Volume& vol, Volume& tempVol, const std::vector<float>& kernel);
};

#endif