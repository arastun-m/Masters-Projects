#include "Filter.h"

Filter::Filter() {}
Filter::~Filter() {}


/*==============================================================================
 *                             GREYSCALE FILTER
 *============================================================================*/
void Filter::applyGreyscale(Image &img) {
    /**
     * @brief Converts an RGB/RGBA image to greyscale.
     *
     * Uses the luminance formula and reduces the image to 1 channel.
     *
     * @param img The input image (modified in place).
     */

    unsigned char *data = img.getData();
    int width = img.getWidth();
    int height = img.getHeight();
    int channels = img.getChannels();

    if (channels < 3) {
        std::cerr << "Error: Image does not have RGB channels. Greyscale conversion not needed.\n";
        return;
    }

    // Allocate new buffer for greyscale image (1 channel per pixel)
    unsigned char *greyData = new unsigned char[width * height];

    for (int i = 0; i < width * height; ++i) {
        int index = i * channels;
        greyData[i] = static_cast<unsigned char>(0.2126 * data[index] + 0.7152 * data[index + 1] + 0.0722 * data[index + 2]);
    }

    // Replace original image data with greyscale data
    img.setData(greyData);
    img.setChannels(1);
}

/*==============================================================================
 *                             THRESHOLD FILTER
 *============================================================================*/
void Filter::applyThreshold(Image& img, int threshold, const std::string& type) {
    /**
     * @brief Applies a threshold filter to an image.
     *
     * Converts an image to binary form by thresholding intensity values.
     * For grayscale images, values below the threshold are set to 0, and above to 255.
     * For RGB/RGBA images, the threshold is applied to the V channel (HSV) or L channel (HSL).
     *
     * @param img The input image (modified in place).
     * @param threshold The threshold value (0-255).
     * @param type The color space for processing ("HSV" or "HSL", default is "HSV").
     *
     * @note The function modifies only grayscale or RGB/RGBA images.
     *       For RGB/RGBA images, saturation is set to 0 to avoid color tint.
     * @warning If an invalid threshold or unsupported channel format is provided, an error is printed.
     */

    if (threshold < 0 || threshold > 255) {
        std::cerr << "Error: Threshold value must be between 0 and 255.\n";
        return;
    }

    unsigned char *data = img.getData();
    int width = img.getWidth();
    int height = img.getHeight();
    int channels = img.getChannels();

    // Convert type to lowercase for case-insensitive comparison
    std::string typeLower = type;
    std::transform(typeLower.begin(), typeLower.end(), typeLower.begin(), [](unsigned char c) { return std::tolower(c); });

    // Set default type if none provided or invalid
    if (typeLower.empty() || (typeLower != "hsv" && typeLower != "hsl")) {
        typeLower = "hsv"; // Default to "hsv"
    }

    if (channels == 1) {
        // Handle grayscale image directly
        std::cout << "Processing grayscale image with threshold " << threshold << ".\n";
        for (int i = 0; i < width * height; ++i) {
            int index = i * channels;
            unsigned char intensity = data[index];
            unsigned char new_value = (intensity < threshold) ? 0 : 255;
            data[index] = new_value;
        }
    } else if (channels == 3 || channels == 4) {
        // Handle RGB/RGBA images with color space conversion
        if (typeLower == "hsv") {
            img.convertToHSV();
        } else if (typeLower == "hsl") {
            img.convertToHSL();
        }

        data = img.getData(); // Get updated data after conversion

        for (int i = 0; i < width * height; ++i) {
            int index = i * channels;
            unsigned char intensity = data[index + 2]; // V for HSV, L for HSL
            unsigned char new_value = (intensity < threshold) ? 0 : 255;

            // ✅ Modify only the Value (V) channel
            data[index + 2] = new_value;

            // ✅ Force Saturation to 0 (this prevents any tint from appearing)
            data[index + 1] = 0;

            if (channels == 4) {
                data[index + 3] = data[index + 3]; // Preserve alpha
            }
        }

        if (typeLower == "hsv") {
            img.convertToRGBFromHSV();
        } else if (typeLower == "hsl") {
            img.convertToRGBFromHSL();
        }
    } else {
        std::cerr << "Error: Unsupported number of channels (" << channels << ").\n";
        return;
    }

    std::cout << "Applied threshold (" << threshold << ", " << typeLower << ").\n";
}

/*==============================================================================
 *                             BRIGHTNESS FILTER (With Auto Mode)
 *============================================================================*/
void Filter::applyBrightness(Image& img, int brightnessOffset) {
    /**
     * @brief Adjusts image brightness to make the average pixel value 128.
     *
     * If brightnessOffset is given as 666, it calculates a suitable offset
     * to achieve an average pixel intensity of 128.
     *
     * @param img The input image (modified in place).
     * @param brightnessOffset The adjustment value (ignored if 666).
     */

    unsigned char* data = img.getData();
    int width = img.getWidth();
    int height = img.getHeight();
    int channels = img.getChannels();
    int totalPixels = width * height * channels;

    // Compute current mean brightness
    double currentMean = 0.0;
    for (int i = 0; i < totalPixels; ++i) {
        currentMean += data[i];
    }
    currentMean /= totalPixels;

    // Compute the new brightness offset
    int newOffset = (brightnessOffset == 666) ? (128 - static_cast<int>(currentMean)) : brightnessOffset;

    // Apply brightness adjustment
    for (int i = 0; i < totalPixels; ++i) {
        int new_value = data[i] + newOffset;

        // Clamp to [0, 255]
        data[i] = static_cast<unsigned char>(std::max(0, std::min(255, new_value)));
    }
}

/*==============================================================================
 *                             SALT & PEPPER NOISE
 *============================================================================*/
void Filter::applySaltPepperNoise(Image &img, float percentage) {
    /**
     * @brief Adds salt-and-pepper noise to an image.
     *
     * Randomly replaces a percentage of pixels with black (0) or white (255) noise.
     * The noise is applied only to RGB channels, leaving alpha (if present) unchanged.
     *
     * @param img The input image (modified in place).
     * @param percentage The percentage of pixels to be affected (0-100).
     */

    unsigned char *data = img.getData();
    int width = img.getWidth();
    int height = img.getHeight();
    int channels = img.getChannels();

    int totalPixels = width * height;
    int numSaltPepper = static_cast<int>(totalPixels * (percentage / 100.0f)); // Number of pixels to modify

    for (int i = 0; i < numSaltPepper; i++) {
        int randIndex = rand() % totalPixels; // Random pixel index
        int pixelStart = randIndex * channels; // Convert to index in the data array

        float r = static_cast<float>(rand()) / RAND_MAX;
        unsigned char noiseValue = (r < 0.5) ? 0 : 255; // 50% chance of black or white

        // Apply noise only to RGB channels (0, 1, 2)
        for (int j = 0; j < std::min(channels, 3); j++) {
            data[pixelStart + j] = noiseValue;
        }
    }
}

/*==============================================================================
 *                             HISTORY EQUALIZATION
 *============================================================================*/
void Filter::applyHistogramEqualization(Image &img, std::string type) {
    /**
     * @brief Applies histogram equalization to an image.
     * 
     * Histogram equalization is a method in image processing of contrast adjustment using the image's histogram.
     * It improves the contrast in images by stretching the intensity range.
     * 
     * For grayscale images, the histogram equalization is applied directly.
     * For RGB images, the equalization is applied to the Value (V) channel in HSV color space by default.
     * 
     * Avoids using computationally expensive sorting by using the cumulative distribution function (CDF).
     * 
     * @param img The input image (modified in place).
     * @param type The color space for processing ("HSV" or "HSL", default is "HSV").
     */

    // Get image data and properties
    unsigned char *data = img.getData();
    int width = img.getWidth();
    int height = img.getHeight();
    int channels = img.getChannels();
    int pixelCount = width * height;
    
    // Special case: For grayscale images, apply directly regardless of type parameter
    if (channels == 1) {
        // Calculate histogram - count frequency of each intensity level (0-255)
        int histogram[256] = {0};
        for (int i = 0; i < pixelCount; ++i) {
            histogram[data[i]]++; // Increment count for this intensity value
        }
        
        // Calculate cumulative distribution function (CDF)
        int cdf[256] = {0};
        cdf[0] = histogram[0]; // Start with first bin
        for (int i = 1; i < 256; ++i) {
            cdf[i] = cdf[i-1] + histogram[i]; // Running sum of histogram
        }
        
        // Find minimum non-zero CDF value
        // Required for the histogram equalization formula
        int cdfMin = 0;
        for (int i = 0; i < 256; ++i) {
            if (cdf[i] > 0) {
                cdfMin = cdf[i];
                break; // Found the first non-zero value
            }
        }
        
        // Create mapping lookup table using the equalization formula:
        // newValue = round((cdf(v) - cdfMin) * 255 / (pixelCount - cdfMin))
        unsigned char map[256];
        for (int i = 0; i < 256; ++i) {
            // Apply standard histogram equalization formula
            map[i] = static_cast<unsigned char>(
                std::max(0.0, std::min(255.0, std::round(
                    (cdf[i] - cdfMin) * 255.0 / (pixelCount - cdfMin)
                )))
            );
        }
        
        // Apply the mapping to transform image
        for (int i = 0; i < pixelCount; ++i) {
            data[i] = map[data[i]]; // Replace each pixel with its equalized value
        }
    }
    // For RGB images - work in color space specified by 'type'
    else if (channels >= 3) {
        // HSV equalization - operates on the Value channel
        if (type == "HSV") { // HSV (default)
            // Convert to HSV color space to isolate Value channel
            img.convertToHSV();
            
            // Calculate histogram of V channel only
            int histogram[256] = {0};
            for (int i = 0; i < pixelCount; ++i) {
                int index = i * channels + 2; // V is in the 3rd channel 
                histogram[data[index]]++;
            }
            
            // Calculate the CDF for Value channel
            int cdf[256] = {0};
            cdf[0] = histogram[0];
            for (int i = 1; i < 256; ++i) {
                cdf[i] = cdf[i-1] + histogram[i];
            }
            
            // Find minimum non-zero CDF value
            int cdfMin = 0;
            for (int i = 0; i < 256; ++i) {
                if (cdf[i] > 0) {
                    cdfMin = cdf[i];
                    break;
                }
            }
            
            // Create mapping lookup table - maps original V values to equalized values
            unsigned char map[256];
            for (int i = 0; i < 256; ++i) {
                // Standard histogram equalization formula
                map[i] = static_cast<unsigned char>(
                    std::max(0.0, std::min(255.0, std::round(
                        (cdf[i] - cdfMin) * 255.0 / (pixelCount - cdfMin)
                    )))
                );
            }
            
            // Apply mapping only to V channel 
            for (int i = 0; i < pixelCount; ++i) {
                int index = i * channels + 2; // V is in the 3rd channel 
                data[index] = map[data[index]];
            }
            
            // Convert back to RGB color space for display/output
            img.convertToRGBFromHSV();
        }
        else { // HSL (type == 1) - operates on the Luminance channel
            // Convert to HSL color space to isolate Luminance channel
            img.convertToHSL();
            
            // Calculate histogram of L channel only
            int histogram[256] = {0};
            for (int i = 0; i < pixelCount; ++i) {
                int index = i * channels + 2; // L is in the 3rd channel 
                histogram[data[index]]++;
            }
            
            // Calculate the CDF for Luminance channel
            int cdf[256] = {0};
            cdf[0] = histogram[0];
            for (int i = 1; i < 256; ++i) {
                cdf[i] = cdf[i-1] + histogram[i];
            }
            
            //  Find minimum non-zero CDF value
            int cdfMin = 0;
            for (int i = 0; i < 256; ++i) {
                if (cdf[i] > 0) {
                    cdfMin = cdf[i];
                    break;
                }
            }
            
            // Create mapping lookup table for Luminance values
            unsigned char map[256];
            for (int i = 0; i < 256; ++i) {
                // Standard histogram equalization formula
                map[i] = static_cast<unsigned char>(
                    std::max(0.0, std::min(255.0, std::round(
                        (cdf[i] - cdfMin) * 255.0 / (pixelCount - cdfMin)
                    )))
                );
            }
            
            // Apply mapping only to L channel 
            for (int i = 0; i < pixelCount; ++i) {
                int index = i * channels + 2; // L is in the 3rd channel 
                data[index] = map[data[index]];
            }
            
            // Convert back to RGB color space for display
            img.convertToRGBFromHSL();
        }
    }
}


/*==============================================================================
 *                             BOX BLUR FILTER
 *============================================================================*/
void Filter::applyBoxBlur(Image& img, int kernelSize) {
    /**
     * Optimised Separable Box Blur filter using two passes (horizontal and vertical).
     * Also uses a sliding window to compute the average pixel value.
     * Which avoids redundant calculations by reusing intermediate results.
     * 
     * Applying the kernel per each pixel has time complexity of O(2k) where k is the kernel size.
     * Unoptimised version has a complexity of O(k^2), much slower for large kernels.
     * 
     * @param img The input image to be blurred
     * @param kernelSize The size of the kernel (odd number)
     * NOTE: The kernel size must be an odd number.
     */

    if (kernelSize % 2 == 0) {
        std::cerr << "Warning: Kernel size is even, adjusting to " << kernelSize + 1 << "." << std::endl;
        kernelSize++;
    }

    int width = img.getWidth();
    int height = img.getHeight();
    int channels = img.getChannels();
    unsigned char* data = img.getData();
    // Temporary buffers for horizontal and vertical passes
    unsigned char* tempData = new unsigned char[width * height * channels];

    // Horizontal pass
    horizontalBoxBlur(data, tempData, width, height, channels, kernelSize);
    // Vertical pass
    verticalBoxBlur(tempData, data, width, height, channels, kernelSize);

    delete[] tempData;
}

/**
 * @brief Apply a horizontal box blur filter to an image.
 * 
 * The horizontal box blur filter applies a 1D box blur kernel to each row of the image.
 * Helper function for the 2D box blur filter.
 * 
 * @param input The input image data.
 * @param output The output image data.
 * @param width The width of the image.
 * @param height The height of the image.
 * @param channels The number of channels in the image.
 * @param kernelSize The size of the kernel (odd number).
 */
void Filter::horizontalBoxBlur(unsigned char* input, unsigned char* output, int width, int height, int channels, int kernelSize) {
    int halfKernel = kernelSize / 2;
    for (int y = 0; y < height; ++y) {
        for (int c = 0; c < channels; ++c) {
            int sum = 0;
            int count = 0;

            // Initialize the sum for the first window
            for (int kx = -halfKernel; kx <= halfKernel; ++kx) {
                int x = kx;
                if (x >= 0 && x < width) {
                    int index = (y * width + x) * channels + c;
                    sum += input[index];
                    count++;
                }
            }

            // Apply the sliding window
            for (int x = 0; x < width; ++x) {
                int index = (y * width + x) * channels + c;
                output[index] = static_cast<unsigned char>(sum / count);

                // Slide the window: subtract the leftmost pixel and add the rightmost pixel
                int leftX = x - halfKernel - 1;
                if (leftX >= 0) {
                    int leftIndex = (y * width + leftX) * channels + c;
                    sum -= input[leftIndex];
                    count--;
                }

                int rightX = x + halfKernel + 1;
                if (rightX < width) {
                    int rightIndex = (y * width + rightX) * channels + c;
                    sum += input[rightIndex];
                    count++;
                }
            }
        }
    }
}

/**
 * @brief Apply a vertical box blur filter to an image.
 * 
 * The vertical box blur filter applies a 1D box blur kernel to each column of the image.
 * Helper function for the 2D box blur filter.
 * 
 * @param input The input image data.
 * @param output The output image data.
 * @param width The width of the image.
 * @param height The height of the image.
 * @param channels The number of channels in the image.
 * @param kernelSize The size of the kernel (odd number).
 */
void Filter::verticalBoxBlur(unsigned char* input, unsigned char* output, int width, int height, int channels, int kernelSize) {
    int halfKernel = kernelSize / 2;
    for (int x = 0; x < width; ++x) {
        for (int c = 0; c < channels; ++c) {
            int sum = 0;
            int count = 0;

            // Initialize the sum for the first window
            for (int ky = -halfKernel; ky <= halfKernel; ++ky) {
                int y = ky;
                if (y >= 0 && y < height) {
                    int index = (y * width + x) * channels + c;
                    sum += input[index];
                    count++;
                }
            }

            // Apply the sliding window
            for (int y = 0; y < height; ++y) {
                int index = (y * width + x) * channels + c;
                output[index] = static_cast<unsigned char>(sum / count);

                // Slide the window: subtract the top pixel and add the bottom pixel
                int topY = y - halfKernel - 1;
                if (topY >= 0) {
                    int topIndex = (topY * width + x) * channels + c;
                    sum -= input[topIndex];
                    count--;
                }

                int bottomY = y + halfKernel + 1;
                if (bottomY < height) {
                    int bottomIndex = (bottomY * width + x) * channels + c;
                    sum += input[bottomIndex];
                    count++;
                }
            }
        }
    }
}


/*==============================================================================
 *                             MEDIAN BLUR FILTER
 *============================================================================*/
void Filter::applyMedianBlur(Image& img, int kernelSize) {
    /**
     * Median Blur filter using a sliding window approach.
     * The median value is calculated from the histogram of pixel intensities to avoid sorting.
     * The window slides over the image, updating the histogram efficiently.
     * 
     * @param img The input image to be blurred
     * @param kernelSize The size of the kernel (odd number)
     * NOTE: The kernel size must be an odd number.
     */

    if (kernelSize < 1) { // kernel size check
        std::cerr << "Error: Kernel size must be positive." << std::endl;
        return;
    }
    if (kernelSize % 2 == 0) { // kernel size must be an odd nubmer
        std::cerr << "Warning: Kernel size is even, adjusting to " << kernelSize + 1 << "." << std::endl;
        kernelSize++;
    }
    int width = img.getWidth();
    int height = img.getHeight();
    int channels = img.getChannels();
    unsigned char* inputData = img.getData();

    // temporary buffer for output
    unsigned char* tempData = new unsigned char[width * height * channels];

    int halfKernel = kernelSize / 2;
    std::vector<int> histogram(256, 0); // Histogram for 8-bit pixel values
    int count = 0;

    // Iterate over each channel
    for (int c = 0; c < channels; ++c) {
        // Iterate over each row
        for (int y = 0; y < height; ++y) {
            // Initialize the histogram for the first window in the row
            initializeHistogram(inputData, histogram, count, width, height, channels, c, y, 0, kernelSize);

            // Process each pixel in the row
            for (int x = 0; x < width; ++x) {
                // Find the median from the histogram
                unsigned char medianValue = findMedian(histogram, count);

                // Store the median value
                int index = (y * width + x) * channels + c;
                tempData[index] = medianValue;

                // Slide the window: update the histogram
                if (x + halfKernel + 1 < width) {
                    updateHistogram(inputData, histogram, count, width, height, channels, c, y, x, kernelSize);
                }
            }
        }
    }

    // Copy the result back to the original image data
    std::memcpy(inputData, tempData, width * height * channels);
    delete[] tempData;
}

/**
 * @brief Initialize the histogram for the first window in a row.
 * 
 * Histogram is computed for the pixel intensities in the window.
 * The count is the total number of pixels in the window.
 * 
 * @param inputData The input image data.
 * @param histogram The histogram to be initialized.
 * @param count The total number of pixels in the window.
 * @param width The width of the image.
 * @param height The height of the image.
 * @param channels The number of channels in the image.
 * @param c The channel index.
 * @param y The row index.
 * @param xStart The starting column index.
 * @param kernelSize The size of the kernel (odd number).
 */
void Filter::initializeHistogram(const unsigned char* inputData, std::vector<int>& histogram, int& count,
    int width, int height, int channels, int c, int y, int xStart, int kernelSize) {
    int halfKernel = kernelSize / 2;
    count = 0;
    std::fill(histogram.begin(), histogram.end(), 0); // Reset histogram

    for (int ky = -halfKernel; ky <= halfKernel; ++ky) {
        for (int kx = -halfKernel; kx <= halfKernel; ++kx) {
            int nx = xStart + kx;
            int ny = y + ky;

            if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
                int idx = (ny * width + nx) * channels + c;
                histogram[inputData[idx]]++;
                count++;
            }
        }
    }
}

/**
 * @brief Update the histogram as the window slides.
 * 
 * The histogram is updated by removing the leftmost column and adding the rightmost column.
 * 
 * @param inputData The input image data.
 * @param histogram The histogram to be updated.
 * @param count The total number of pixels in the window.
 * @param width The width of the image.
 * @param height The height of the image.
 * @param channels The number of channels in the image.
 * @param c The channel index.
 * @param y The row index.
 * @param x The column index.
 * @param kernelSize The size of the kernel (odd number).
 */
void Filter::updateHistogram(const unsigned char* inputData, std::vector<int>& histogram, int& count,
int width, int height, int channels, int c, int y, int x, int kernelSize) {
    int halfKernel = kernelSize / 2;

    // Remove the leftmost column
    for (int ky = -halfKernel; ky <= halfKernel; ++ky) {
        int ny = y + ky;
        if (ny >= 0 && ny < height) {
            int leftX = x - halfKernel; // removed -1
            
            if (leftX >= 0) {
                int leftIdx = (ny * width + leftX) * channels + c;
                histogram[inputData[leftIdx]]--;
                count--;
            }
        }
    }

    // Add the rightmost column
    for (int ky = -halfKernel; ky <= halfKernel; ++ky) {
        int ny = y + ky;
        if (ny >= 0 && ny < height) {
        int rightX = x + halfKernel + 1; // added +1
            if (rightX < width) {
                int rightIdx = (ny * width + rightX) * channels + c;
                histogram[inputData[rightIdx]]++;
                count++;
            }
        }
    }
}

/**
 * @brief Find the median value from the histogram.
 * 
 * The median value is the intensity level that splits the histogram into two equal parts.
 * The histogram is used to find the median efficiently without sorting the pixel values.
 * 
 * @param histogram The histogram of pixel intensities.
 * @param count The total number of pixels in the window.
 * @return The median intensity value.
 */
unsigned char Filter::findMedian(const std::vector<int>& histogram, int count) {
    int medianIndex = count / 2;
    int sum = 0;
    for (int i = 0; i < 256; ++i) {
        sum += histogram[i];
        if (sum > medianIndex) {
            return static_cast<unsigned char>(i);
        }
    }
    return 0; // Default value (should not be reached)
}


/*==============================================================================
 *                             GAUSSIAN BLUR FILTER
 *============================================================================*/
void Filter::applyGaussianBlur(Image& img, int kernelSize, float stdev) {
    /**
     * Gaussian Blur filter using two 1D passes (horizontal and vertical).
     * The 1D Gaussian kernel is created and applied along each row and column.
     * Also uses sliding window approach to compute the weighted average efficiently.
     * 
     * @param img The input image to be blurred
     * @param kernelSize The size of the kernel (odd number)
     * @param stdev The standard deviation of the Gaussian distribution
     * NOTE: The kernel size must be an odd number.
     */

    if (kernelSize % 2 == 0) {
        std::cerr << "Warning: Kernel size is even, adjusting to " << kernelSize + 1 << "." << std::endl;
        kernelSize++;
    }

    int width = img.getWidth();
    int height = img.getHeight();
    int channels = img.getChannels();
    unsigned char* inputData = img.getData();

    // Create the 1D Gaussian kernel
    std::vector<float> kernel = createGaussianKernel1D(kernelSize, stdev);

    // Temporary buffers for horizontal and vertical passes
    unsigned char* tempData = new unsigned char[width * height * channels];

    // Apply horizontal pass
    applyGaussianBlur1DHorizontal(inputData, tempData, width, height, channels, kernel);
    // Apply vertical pass
    applyGaussianBlur1DVertical(tempData, inputData, width, height, channels, kernel);
    delete[] tempData;
}

/**
 * @brief Create a 1D Gaussian kernel.
 * 
 * The Gaussian kernel is created using the standard deviation.
 * The kernel is normalized to ensure the sum of weights is 1.
 * 
 * @param kernelSize The size of the kernel (odd number).
 * @param stdev The standard deviation of the Gaussian distribution.
 * @return The 1D Gaussian kernel.
 */
std::vector<float> Filter::createGaussianKernel1D(int kernelSize, float stdev) {
    std::vector<float> kernel(kernelSize);
    float sum = 0.0f;
    int halfKernel = kernelSize / 2;

    for (int i = 0; i < kernelSize; ++i) {
        float x = static_cast<float>(i - halfKernel);
        kernel[i] = std::exp(-0.5f * (x * x) / (stdev * stdev));
        sum += kernel[i];
    }

    // Normalize the kernel
    for (int i = 0; i < kernelSize; ++i) {
        kernel[i] /= sum;
    }

    return kernel;
}

/**
 * @brief Apply a 1D Gaussian blur filter along the rows of an image.
 * 
 * The 1D Gaussian kernel is applied to each row of the image.
 * The result is stored in the output buffer.
 * 
 * @param input The input image data.
 * @param output The output image data.
 * @param width The width of the image.
 * @param height The height of the image.
 * @param channels The number of channels in the image.
 * @param kernel The 1D Gaussian kernel (size must be an odd number).
 */
void Filter::applyGaussianBlur1DHorizontal(const unsigned char* input, unsigned char* output,
                                   int width, int height, int channels,
                                   const std::vector<float>& kernel) {
    int halfKernel = kernel.size() / 2;

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            for (int c = 0; c < channels; ++c) {
                float sum = 0.0f;

                // Apply the 1D Gaussian kernel along the row
                for (int kx = -halfKernel; kx <= halfKernel; ++kx) {
                    int nx = x + kx;

                    if (nx >= 0 && nx < width) {
                        int idx = (y * width + nx) * channels + c;
                        sum += input[idx] * kernel[kx + halfKernel];
                    }
                }

                // Store the result in the output buffer
                int index = (y * width + x) * channels + c;
                output[index] = static_cast<unsigned char>(sum);
            }
        }
    }
}

/**
 * @brief Apply a 1D Gaussian blur filter along the columns of an image.
 * 
 * The 1D Gaussian kernel is applied to each column of the image.
 * The result is stored in the output buffer.
 * 
 * @param input The input image data.
 * @param output The output image data.
 * @param width The width of the image.
 * @param height The height of the image.
 * @param channels The number of channels in the image.
 * @param kernel The 1D Gaussian kernel (size must be an odd number).
 */
void Filter::applyGaussianBlur1DVertical(const unsigned char* input, unsigned char* output,
                                 int width, int height, int channels,
                                 const std::vector<float>& kernel) {
    int halfKernel = kernel.size() / 2;

    for (int x = 0; x < width; ++x) {
        for (int y = 0; y < height; ++y) {
            for (int c = 0; c < channels; ++c) {
                float sum = 0.0f;

                // Apply the 1D Gaussian kernel along the column
                for (int ky = -halfKernel; ky <= halfKernel; ++ky) {
                    int ny = y + ky;

                    if (ny >= 0 && ny < height) {
                        int idx = (ny * width + x) * channels + c;
                        sum += input[idx] * kernel[ky + halfKernel];
                    }
                }

                // Store the result in the output buffer
                int index = (y * width + x) * channels + c;
                output[index] = static_cast<unsigned char>(sum);
            }
        }
    }
}


/*==============================================================================
 *                             EDGE DETECTION FILTERS
 *============================================================================*/
 /**
  * @brief Compute the mirror index for boundary reflection.
  * 
  * @param index The current index.
  * @param maxVal The maximum value for the index.
  * @return The mirror index.
  */
 inline int mirrorIndex(int index, int maxVal) {
    if (index < 0)
        return -index;               // mirror at left/top border
    if (index >= maxVal)
        return 2 * maxVal - index - 2; // mirror at right/bottom border
    return index;
}

void Filter::applyEdgeDetection(Image& img, std::string type) {
    /**
     * @brief Applies an edge detection filter to an image.
     *
     * Supports Sobel, Prewitt, Scharr, and Roberts Cross operators to detect edges.
     * Converts the image to grayscale before applying the filter.
     *
     * @param img The input image (modified in place).
     * @param type The edge detection type ("Sobel", "Prewitt", "Scharr", "RobertsCross").
     *
     * @note The output is a single-channel grayscale image representing edge magnitudes.
     * @warning Prints an error if an invalid type is provided.
     */

    std::cout << "Applying edge detection filter: " << type << std::endl;

    int kernelSize = 3;  // default for 3x3 kernels
    int offset = 1;      // center index for 3x3 kernels
    int Gx[3][3] = { 0 };
    int Gy[3][3] = { 0 };

    if (type == "Sobel") {
        int tempGx[3][3] = {
            { -1,  0,  1 },
            { -2,  0,  2 },
            { -1,  0,  1 }
        };
        int tempGy[3][3] = {
            { -1, -2, -1 },
            {  0,  0,  0 },
            {  1,  2,  1 }
        };
        for (int i = 0; i < 3; ++i)
            for (int j = 0; j < 3; ++j) {
                Gx[i][j] = tempGx[i][j];
                Gy[i][j] = tempGy[i][j];
            }
    }
    else if (type == "Prewitt") {
        int tempGx[3][3] = {
            { -1,  0,  1 },
            { -1,  0,  1 },
            { -1,  0,  1 }
        };
        int tempGy[3][3] = {
            { -1, -1, -1 },
            {  0,  0,  0 },
            {  1,  1,  1 }
        };
        for (int i = 0; i < 3; ++i)
            for (int j = 0; j < 3; ++j) {
                Gx[i][j] = tempGx[i][j];
                Gy[i][j] = tempGy[i][j];
            }
    }
    else if (type == "Scharr") {
        int tempGx[3][3] = {
            { -3,   0,  3 },
            { -10,  0, 10 },
            { -3,   0,  3 }
        };
        int tempGy[3][3] = {
            { -3, -10, -3 },
            {  0,   0,  0 },
            {  3,  10,  3 }
        };
        for (int i = 0; i < 3; ++i)
            for (int j = 0; j < 3; ++j) {
                Gx[i][j] = tempGx[i][j];
                Gy[i][j] = tempGy[i][j];
            }
    }
    else if (type == "RobertsCross") {
        kernelSize = 2;  // Roberts uses a 2x2 kernel
        offset = 0;
        int tempGx[2][2] = {
            { 1,  0 },
            { 0, -1 }
        };
        int tempGy[2][2] = {
            { 0,  1 },
            { -1, 0 }
        };
        // Copy the 2x2 kernel values into Gx and Gy.
        for (int i = 0; i < 2; ++i)
            for (int j = 0; j < 2; ++j) {
                Gx[i][j] = tempGx[i][j];
                Gy[i][j] = tempGy[i][j];
            }
    }
    else {
        std::cerr << "Error: Invalid edge detection type.\n";
        return;
    }

    // Convert image to grayscale.
    Filter::applyGreyscale(img);

    unsigned char* data = img.getData();
    int width = img.getWidth();
    int height = img.getHeight();
    int channels = img.getChannels(); // Even though grayscale, channels might still be > 1

    // Allocate output array for single channel edge data.
    unsigned char* edgeData = new unsigned char[width * height];

    // Precompute mirror indices for each row and column.
    std::vector<int> rowAbove(height), rowCenter(height), rowBelow(height);
    for (int i = 0; i < height; ++i) {
        rowAbove[i] = mirrorIndex(i - 1, height);
        rowCenter[i] = i;
        rowBelow[i] = mirrorIndex(i + 1, height);
    }
    std::vector<int> colLeft(width), colCenter(width), colRight(width);
    for (int j = 0; j < width; ++j) {
        colLeft[j] = mirrorIndex(j - 1, width);
        colCenter[j] = j;
        colRight[j] = mirrorIndex(j + 1, width);
    }

    if (kernelSize == 3) {
        // Process each pixel using the precomputed mirror indices.
        for (int i = 0; i < height; ++i) {
            int iAbove = rowAbove[i];
            int iCenter = rowCenter[i];
            int iBelow = rowBelow[i];
            for (int j = 0; j < width; ++j) {
                int jLeft = colLeft[j];
                int jCenter = colCenter[j];
                int jRight = colRight[j];

                int sumX = 0, sumY = 0;
                // Access the 3x3 neighborhood without additional if-checks:
                sumX += data[(iAbove * width + jLeft) * channels] * Gx[0][0];
                sumY += data[(iAbove * width + jLeft) * channels] * Gy[0][0];

                sumX += data[(iAbove * width + jCenter) * channels] * Gx[0][1];
                sumY += data[(iAbove * width + jCenter) * channels] * Gy[0][1];

                sumX += data[(iAbove * width + jRight) * channels] * Gx[0][2];
                sumY += data[(iAbove * width + jRight) * channels] * Gy[0][2];

                sumX += data[(iCenter * width + jLeft) * channels] * Gx[1][0];
                sumY += data[(iCenter * width + jLeft) * channels] * Gy[1][0];

                sumX += data[(iCenter * width + jCenter) * channels] * Gx[1][1];
                sumY += data[(iCenter * width + jCenter) * channels] * Gy[1][1];

                sumX += data[(iCenter * width + jRight) * channels] * Gx[1][2];
                sumY += data[(iCenter * width + jRight) * channels] * Gy[1][2];

                sumX += data[(iBelow * width + jLeft) * channels] * Gx[2][0];
                sumY += data[(iBelow * width + jLeft) * channels] * Gy[2][0];

                sumX += data[(iBelow * width + jCenter) * channels] * Gx[2][1];
                sumY += data[(iBelow * width + jCenter) * channels] * Gy[2][1];

                sumX += data[(iBelow * width + jRight) * channels] * Gx[2][2];
                sumY += data[(iBelow * width + jRight) * channels] * Gy[2][2];

                int magnitude = static_cast<int>(std::sqrt(sumX * sumX + sumY * sumY));
                // Clamp the magnitude to [0, 255]
                magnitude = (magnitude > 255 ? 255 : magnitude);
                edgeData[i * width + j] = static_cast<unsigned char>(magnitude);
            }
        }
    }
    else if (kernelSize == 2) {
        // Similar improvements can be applied to the 2x2 branch for Roberts Cross.
        for (int i = 0; i < height; ++i) {
            int iCenter = mirrorIndex(i, height);
            int iBelow = mirrorIndex(i + 1, height);
            for (int j = 0; j < width; ++j) {
                int jCenter = mirrorIndex(j, width);
                int jRight = mirrorIndex(j + 1, width);
                int sumX = 0, sumY = 0;
                sumX += data[(iCenter * width + jCenter) * channels] * Gx[0][0];
                sumY += data[(iCenter * width + jCenter) * channels] * Gy[0][0];

                sumX += data[(iCenter * width + jRight) * channels] * Gx[0][1];
                sumY += data[(iCenter * width + jRight) * channels] * Gy[0][1];

                sumX += data[(iBelow * width + jCenter) * channels] * Gx[1][0];
                sumY += data[(iBelow * width + jCenter) * channels] * Gy[1][0];

                sumX += data[(iBelow * width + jRight) * channels] * Gx[1][1];
                sumY += data[(iBelow * width + jRight) * channels] * Gy[1][1];

                int magnitude = static_cast<int>(std::sqrt(sumX * sumX + sumY * sumY));
                magnitude = (magnitude > 255 ? 255 : magnitude);
                edgeData[i * width + j] = static_cast<unsigned char>(magnitude);
            }
        }
    }
    // Update the image to have one channel and use the new edge data.
    img.setChannels(1);
    img.setData(edgeData);
}

/*==============================================================================
 *                             3D MEDIAN BLUR FILTER
 *============================================================================*/
void Filter::applyMedianBlur3D(Volume& vol, int kernelSize) {
    /**
     * 3D Median Blur Filter using a sliding window approach.
     * Histograms are used to find the median value for each voxel.
     * The window slides over the volume, updating the histogram efficiently.
     * 
     * @param vol The input volume to be blurred
     * @param kernelSize The size of the kernel (odd number)
     * NOTE: The kernel size must be an odd number.
     */

    // Check kernel size
    if (kernelSize < 1) {
        std::cerr << "Error: Kernel size must be positive." << std::endl;
        return;
    }
    // If kernel size is even, adjust to next odd number
    if (kernelSize % 2 == 0) {
        std::cerr << "Warning: Kernel size is even, adjusting to " << kernelSize + 1 << "." << std::endl;
        kernelSize++;
    }
    int halfKernel = kernelSize / 2;

    int width = vol.getWidth();
    int height = vol.getHeight();
    int depth = vol.getDepth();
    int channels = vol.getChannels();

    // Temporary volume for storing the blurred output
    Volume tempVol = vol;

    // Iterate over each channel
    for (int c = 0; c < channels; ++c) {
        // Iterate over each depth slice
        for (int z = 0; z < depth; ++z) {
            // Iterate over each row
            for (int y = 0; y < height; ++y) {
                // Initialize the histogram for the first window in the row
                std::vector<int> histogram(256, 0); // Histogram for 8-bit voxel values
                int count = 0;
                initializeHistogram3D(vol, histogram, count, width, height, depth, channels, c, z, y, 0, kernelSize);

                // Process each voxel in the row
                for (int x = 0; x < width; ++x) {
                    // Find the median from the histogram
                    unsigned char medianValue = findMedian(histogram, count);

                    // Store the median value in the temporary volume
                    tempVol.setVoxel(z, y, x, medianValue, c);

                    // Slide the window: update the histogram
                    if (x + halfKernel + 1 < width) {
                        updateHistogram3D(vol, histogram, count, width, height, depth, channels, c, z, y, x, kernelSize);
                    }
                }
            }
        }
    }

    // Copy the result back to the original volume
    vol = tempVol;
}

/**
 * @brief Initialize the histogram for the first window in a row.
 * 
 * Histogram is computed for the voxel intensities in the window.
 * 
 * @param vol The input volume.
 * @param histogram The histogram to be initialized.
 * @param count The total number of voxels in the window.
 * @param width The width of the volume.
 * @param height The height of the volume.
 * @param depth The depth of the volume.
 * @param channels The number of channels in the volume.
 * @param c The channel index.
 * @param z The depth index.
 * @param y The row index.
 * @param xStart The starting column index.
 * @param kernelSize The size of the kernel (odd number). 
 */
void Filter::initializeHistogram3D(const Volume& vol, std::vector<int>& histogram, int& count,
                                   int width, int height, int depth, int channels, int c, int z, int y, int xStart, int kernelSize) {
    int halfKernel = kernelSize / 2;
    count = 0;
    std::fill(histogram.begin(), histogram.end(), 0); // Reset histogram

    for (int kz = -halfKernel; kz <= halfKernel; ++kz) {
        for (int ky = -halfKernel; ky <= halfKernel; ++ky) {
            for (int kx = -halfKernel; kx <= halfKernel; ++kx) {
                int nx = xStart + kx;
                int ny = y + ky;
                int nz = z + kz;

                if (nx >= 0 && nx < width && ny >= 0 && ny < height && nz >= 0 && nz < depth) {
                    unsigned char voxelValue = vol.getVoxel(nz, ny, nx, c);
                    histogram[voxelValue]++;
                    count++;
                }
            }
        }
    }
}

/**
 * @brief Update the histogram as the window slides.
 * 
 * The histogram is updated by removing the leftmost column and adding the rightmost column.
 * 
 * @param vol The input volume.
 * @param histogram The histogram to be updated.
 * @param count The total number of voxels in the window.
 * @param width The width of the volume.
 * @param height The height of the volume.
 * @param depth The depth of the volume.
 * @param channels The number of channels in the volume.
 * @param c The channel index.
 * @param z The depth index.
 * @param y The row index.
 * @param x The column index.
 * @param kernelSize The size of the kernel (odd number).
 */
void Filter::updateHistogram3D(const Volume& vol, std::vector<int>& histogram, int& count,
                               int width, int height, int depth, int channels, int c, int z, int y, int x, int kernelSize) {
    int halfKernel = kernelSize / 2;

    // Remove the leftmost column
    for (int kz = -halfKernel; kz <= halfKernel; ++kz) {
        for (int ky = -halfKernel; ky <= halfKernel; ++ky) {
            int nz = z + kz;
            int ny = y + ky;

            if (nz >= 0 && nz < depth && ny >= 0 && ny < height) {
                int leftX = x - halfKernel; // removed -1 (results match opencv)
                if (leftX >= 0) {
                    unsigned char leftVoxel = vol.getVoxel(nz, ny, leftX, c);
                    histogram[leftVoxel]--;
                    count--;
                }
            }
        }
    }

    // Add the rightmost column
    for (int kz = -halfKernel; kz <= halfKernel; ++kz) {
        for (int ky = -halfKernel; ky <= halfKernel; ++ky) {
            int nz = z + kz;
            int ny = y + ky;

            if (nz >= 0 && nz < depth && ny >= 0 && ny < height) {
                int rightX = x + halfKernel + 1;
                if (rightX < width) {
                    unsigned char rightVoxel = vol.getVoxel(nz, ny, rightX, c);
                    histogram[rightVoxel]++;
                    count++;
                }
            }
        }
    }
}

/*==============================================================================
 *                             3D GAUSSIAN BLUR FILTER
 *============================================================================*/
void Filter::applyGaussianBlur3D(Volume& vol, int kernelSize, float stdev) {
    /**
     * @brief 3D Gaussian Blur Filter using three 1D passes (X, Y, Z).
     * 
     * The 1D Gaussian kernel is created and applied along each dimension.
     * The volume is blurred in 3D using three passes for each dimension.
     * 
     * @param vol The input volume to be blurred
     * @param kernelSize The size of the kernel (odd number)
     * @param stdev The standard deviation of the Gaussian distribution
     * @throw std::cerr if kernel size is even
     */

    if (kernelSize % 2 == 0) {
        std::cerr << "Warning: Kernel size is even, adjusting to " << kernelSize + 1 << "." << std::endl;
        kernelSize++;
    }

    std::vector<float> kernel = create3DGaussianKernel1D(kernelSize, stdev);

    Volume tempVol1 = vol; // For X and Y passes
    Volume tempVol2 = vol; // For Z pass

    applyGaussianBlur1DX(vol, tempVol1, kernel);
    applyGaussianBlur1DY(tempVol1, tempVol2, kernel);
    applyGaussianBlur1DZ(tempVol2, vol, kernel);
}

/**
 * @brief Create a 1D Gaussian kernel.
 * 
 * The Gaussian kernel is created using the standard deviation.
 * The kernel is normalized to ensure the sum of weights is 1.
 * 
 * @param kernelSize The size of the kernel (odd number).
 * @param stdev The standard deviation of the Gaussian distribution.
 */
std::vector<float> Filter::create3DGaussianKernel1D(int kernelSize, float stdev) {
    std::vector<float> kernel(kernelSize);
    float sum = 0.0f;
    int halfKernel = kernelSize / 2;

    for (int i = 0; i < kernelSize; ++i) {
        float x = static_cast<float>(i - halfKernel);
        kernel[i] = std::exp(-0.5f * (x * x) / (stdev * stdev));
        sum += kernel[i];
    }

    for (int i = 0; i < kernelSize; ++i) {
        kernel[i] /= sum;
    }

    return kernel;
}

/**
 * @brief Apply a 1D Gaussian blur filter along the X dimension of a volume.
 * 
 * The 1D Gaussian kernel is applied to each row of each depth slice.
 * The result is stored in the output buffer.
 * 
 * @param vol The input volume data.
 * @param tempVol The temporary volume data.
 * @param kernel The 1D Gaussian kernel (size must be an odd number).
 */
void Filter::applyGaussianBlur1DX(Volume& vol, Volume& tempVol, const std::vector<float>& kernel) {
    int width = vol.getWidth();
    int height = vol.getHeight();
    int depth = vol.getDepth();
    int channels = vol.getChannels();
    int halfKernel = kernel.size() / 2;

    for (int z = 0; z < depth; ++z) {
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                for (int c = 0; c < channels; ++c) {
                    float sum = 0.0f;

                    for (int kx = -halfKernel; kx <= halfKernel; ++kx) {
                        int nx = x + kx;
                        // Extend Padding (Clamp)
                        if (nx < 0) nx = 0;
                        if (nx >= width) nx = width - 1;

                        unsigned char voxelValue = vol.getVoxel(z, y, nx, c);
                        sum += voxelValue * kernel[kx + halfKernel];
                    }

                    tempVol.setVoxel(z, y, x, static_cast<unsigned char>(sum), c);
                }
            }
        }
    }
}

/**
 * @brief Apply a 1D Gaussian blur filter along the Y dimension of a volume.
 * 
 * The 1D Gaussian kernel is applied to each column of each depth slice.
 * The result is stored in the output buffer.
 * 
 * @param vol The input volume data.
 * @param tempVol The temporary volume data.
 * @param kernel The 1D Gaussian kernel (size must be an odd number).
 */
void Filter::applyGaussianBlur1DY(Volume& vol, Volume& tempVol, const std::vector<float>& kernel) {
    int width = vol.getWidth();
    int height = vol.getHeight();
    int depth = vol.getDepth();
    int channels = vol.getChannels();
    int halfKernel = kernel.size() / 2;

    for (int z = 0; z < depth; ++z) {
        for (int x = 0; x < width; ++x) {
            for (int y = 0; y < height; ++y) {
                for (int c = 0; c < channels; ++c) {
                    float sum = 0.0f;

                    for (int ky = -halfKernel; ky <= halfKernel; ++ky) {
                        int ny = y + ky;
                        // Extend Padding (Clamp)
                        if (ny < 0) ny = 0;
                        if (ny >= height) ny = height - 1;

                        unsigned char voxelValue = vol.getVoxel(z, ny, x, c);
                        sum += voxelValue * kernel[ky + halfKernel];
                    }

                    tempVol.setVoxel(z, y, x, static_cast<unsigned char>(sum), c);
                }
            }
        }
    }
}

/**
 * @brief Apply a 1D Gaussian blur filter along the Z dimension of a volume.
 * 
 * The 1D Gaussian kernel is applied to each depth slice.
 * 
 * @param vol The input volume data.
 * @param tempVol The temporary volume data.
 * @param kernel The 1D Gaussian kernel (size must be an odd number).
 */
void Filter::applyGaussianBlur1DZ(Volume& vol, Volume& tempVol, const std::vector<float>& kernel) {
    int width = vol.getWidth();
    int height = vol.getHeight();
    int depth = vol.getDepth();
    int channels = vol.getChannels();
    int halfKernel = kernel.size() / 2;

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            for (int z = 0; z < depth; ++z) {
                for (int c = 0; c < channels; ++c) {
                    float sum = 0.0f;

                    for (int kz = -halfKernel; kz <= halfKernel; ++kz) {
                        int nz = z + kz;
                        // Extend Padding (Clamp)
                        if (nz < 0) nz = 0;
                        if (nz >= depth) nz = depth - 1;

                        unsigned char voxelValue = vol.getVoxel(nz, y, x, c);
                        sum += voxelValue * kernel[kz + halfKernel];
                    }

                    tempVol.setVoxel(z, y, x, static_cast<unsigned char>(sum), c);
                }
            }
        }
    }
}


/*==============================================================================
 *                             IMAGE SHARPENING FILTER
 *============================================================================*/
void Filter::applySharpening(Image& img) {
    /**
     * @brief Applies a sharpening filter to an image using a 3x3 Laplacian kernel.
     *
     * Enhances edges by applying a convolution-based sharpening filter.
     * Supports grayscale (1-channel), RGB (3-channel), and RGBA (4-channel) images.
     * The alpha channel (if present) remains unchanged.
     *
     * @param img The input image to be sharpened (modified in place).
     * @note Only supports 1, 3, or 4 channels. Prints an error if unsupported.
     */

    // Get image information
    unsigned char* data = img.getData();
    int width = img.getWidth();
    int height = img.getHeight();
    int channels = img.getChannels();

    // Check the channel1, 3, 4
    if (channels != 1 && channels != 3 && channels != 4) {
        std::cerr << "Error: Unsupported number of channels (" << channels << "). Sharpening not applicable.\n";
        return;
    }

    std::vector<unsigned char> newData(width * height * channels);

    // 3x3 Laplacian Kernel
    const int kernel[9] = {
         0, -1,  0,
        -1,  4, -1,
         0, -1,  0
    };

    // Iterate over all pixels
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int centerIdx = (y * width + x) * channels;

            for (int c = 0; c < channels; ++c) {
                // Keep Alpha channel
                if (channels == 4 && c == 3) {
                    newData[centerIdx + c] = data[centerIdx + c];
                    continue;
                }

                int sum = 0;

                // Apply the 3x3 convolution kernel
                for (int i = 0; i < 9; ++i) {
                    int neighborX = x + (i % 3 - 1);
                    int neighborY = y + (i / 3 - 1);

                    // Clamp neighbor coordinates to image bounds (extend mode)
                    int clampedX = std::clamp(neighborX, 0, width - 1);
                    int clampedY = std::clamp(neighborY, 0, height - 1);

                    int neighborIdx = (clampedY * width + clampedX) * channels + c;
                    sum += data[neighborIdx] * kernel[i];
                }

                // I_sharp = I_original + G
                int sharpValue = data[centerIdx + c] + sum;

                // Clamp range to 0-255
                newData[centerIdx + c] = static_cast<unsigned char>(std::max(0, std::min(255, sharpValue)));
            }
        }
    }

    // Update the original image data
    std::copy(newData.begin(), newData.end(), data);
}

/**
 * @brief Compute the mirror index for boundary reflection.
 * 
 * A helper function that computes the mirror index.
 * For example, if index < 0, it returns -index; if index >= max, it returns 2*max - index - 2.
 * 
 * @param index The current index.
 * @param max The maximum value for the index.
 */
inline int mirror(int index, int max) {
    if (index < 0)
        return -index;
    if (index >= max)
        return 2 * max - index - 2;
    return index;
}

/**
 * @brief Compute the median of 9 unsigned char values.
 * 
 * A self-implemented median function for 9 unsigned char values.
 * It uses a simple bubble sort for the small fixed-size array.
 * 
 * @param a0-a8 The 9 unsigned char values.
 * @return The median of the 9 values.
 */
unsigned char median9(unsigned char a0, unsigned char a1, unsigned char a2,
    unsigned char a3, unsigned char a4, unsigned char a5,
    unsigned char a6, unsigned char a7, unsigned char a8) {
    unsigned char arr[9] = { a0, a1, a2, a3, a4, a5, a6, a7, a8 };
    for (int i = 0; i < 8; ++i) {
        for (int j = i + 1; j < 9; ++j) {
            if (arr[j] < arr[i]) {
                unsigned char temp = arr[i];
                arr[i] = arr[j];
                arr[j] = temp;
            }
        }
    }
    // The median of 9 elements is at index 4 after sorting.
    return arr[4];
}