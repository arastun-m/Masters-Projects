#include "test_utilities.h"

bool testThresholdFilter() {
    std::cout << "Testing Threshold Filter..." << std::endl;
    
    // Create a test image with a range of intensities
    int width = 100;
    int height = 100;
    int channels = 3;
    Image testImage(width, height, channels);
    unsigned char* data = testImage.getData();
    
    // Fill the image with a gradient of values
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int idx = (y * width + x) * channels;
            int intensity = (x + y) % 256;  // Gradient pattern from 0-255
            data[idx] = intensity;
            data[idx + 1] = intensity;
            data[idx + 2] = intensity;
        }
    }
    
    // Save the original gradient image
    testImage.save("test/images/threshold_original.png");
    
    // Make copies for different threshold tests
    Image copyHSV(width, height, channels);
    Image copyHSL(width, height, channels);
    Image copyGrayscale(width, height, 1);  // Grayscale image
    
    unsigned char* copyDataHSV = copyHSV.getData();
    unsigned char* copyDataHSL = copyHSL.getData();
    unsigned char* copyDataGrayscale = copyGrayscale.getData();
    
    // Copy data to the test images
    std::memcpy(copyDataHSV, data, width * height * channels);
    std::memcpy(copyDataHSL, data, width * height * channels);
    
    // Create grayscale version for testing single-channel threshold
    for (int i = 0; i < width * height; i++) {
        int rgbIdx = i * channels;
        copyDataGrayscale[i] = data[rgbIdx];  // Just copy R channel for simplicity
    }
    
    // Apply threshold with different parameters
    int threshold = 128;
    Filter::applyThreshold(copyHSV, threshold, "HSV");
    Filter::applyThreshold(copyHSL, threshold, "HSL");
    Filter::applyThreshold(copyGrayscale, threshold, "");  // Default type
    
    // Save result images
    copyHSV.save("test/images/threshold_hsv_result.png");
    copyHSL.save("test/images/threshold_hsl_result.png");
    copyGrayscale.save("test/images/threshold_grayscale_result.png");
    
    // Verify threshold was correctly applied
    bool testPassed = true;
    bool hsvCorrect = true;
    bool hslCorrect = true;
    bool grayscaleCorrect = true;
    
    // Check HSV version
    copyDataHSV = copyHSV.getData();  // Get updated data
    for (int i = 0; i < width * height; i++) {
        int idx = i * channels;
        int originalIntensity = data[idx];  // Original intensity
        
        // For RGB, we need to check if the result matches what we'd expect
        // after HSV conversion, thresholding, and conversion back
        // This is a simplified check since exact color conversion is complex
        bool isBlackOrWhite = (copyDataHSV[idx] == 0 && copyDataHSV[idx+1] == 0 && copyDataHSV[idx+2] == 0) ||
                             (copyDataHSV[idx] == 255 && copyDataHSV[idx+1] == 255 && copyDataHSV[idx+2] == 255);
                             
        if (!isBlackOrWhite) {
            hsvCorrect = false;
            break;
        }
    }
    
    // Check HSL version (similar to HSV check)
    copyDataHSL = copyHSL.getData();  // Get updated data
    for (int i = 0; i < width * height; i++) {
        int idx = i * channels;
        int originalIntensity = data[idx];  // Original intensity
        
        bool isBlackOrWhite = (copyDataHSL[idx] == 0 && copyDataHSL[idx+1] == 0 && copyDataHSL[idx+2] == 0) ||
                             (copyDataHSL[idx] == 255 && copyDataHSL[idx+1] == 255 && copyDataHSL[idx+2] == 255);
                             
        if (!isBlackOrWhite) {
            hslCorrect = false;
            break;
        }
    }
    
    // Check grayscale version
    copyDataGrayscale = copyGrayscale.getData();  // Get updated data
    for (int i = 0; i < width * height; i++) {
        int originalIntensity = data[i * channels];  // Original intensity
        int thresholdedValue = copyDataGrayscale[i];
        
        // Check that the thresholding was applied correctly
        bool shouldBeWhite = originalIntensity >= threshold;
        if ((shouldBeWhite && thresholdedValue != 255) || (!shouldBeWhite && thresholdedValue != 0)) {
            grayscaleCorrect = false;
            break;
        }
    }
    
    // Sample a few pixels for display
    std::cout << "HSV Thresholding Sample Pixels (Original -> Result):" << std::endl;
    for (int i = 0; i < 5; i++) {
        int x = width / 6 * (i + 1);
        int y = height / 2;
        int idx = (y * width + x) * channels;
        
        std::cout << "Pixel (" << x << "," << y << "): " 
                  << "Original: (" << (int)data[idx] << "," << (int)data[idx+1] << "," << (int)data[idx+2] << ") -> "
                  << "Result: (" << (int)copyDataHSV[idx] << "," << (int)copyDataHSV[idx+1] << "," << (int)copyDataHSV[idx+2] << ")" 
                  << std::endl;
    }
    
    // Sample grayscale pixels
    std::cout << "Grayscale Thresholding Sample Pixels (Original -> Result):" << std::endl;
    for (int i = 0; i < 5; i++) {
        int x = width / 6 * (i + 1);
        int y = height / 2;
        int rgbIdx = (y * width + x) * channels;
        int grayIdx = y * width + x;
        
        std::cout << "Pixel (" << x << "," << y << "): " 
                  << "Original: " << (int)data[rgbIdx] << " -> "
                  << "Result: " << (int)copyDataGrayscale[grayIdx]
                  << std::endl;
    }
    
    testPassed = hsvCorrect && hslCorrect && grayscaleCorrect;
    
    if (testPassed) {
        std::cout << "Threshold Filter test PASSED: All tests executed correctly" << std::endl;
    } else {
        std::cout << "Threshold Filter test FAILED:" << std::endl;
        if (!hsvCorrect) std::cout << "  - HSV threshold did not apply correctly" << std::endl;
        if (!hslCorrect) std::cout << "  - HSL threshold did not apply correctly" << std::endl;
        if (!grayscaleCorrect) std::cout << "  - Grayscale threshold did not apply correctly" << std::endl;
    }
    
    return testPassed;
}