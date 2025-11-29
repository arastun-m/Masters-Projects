#include "test_utilities.h"

bool testBrightnessFilter() {
    std::cout << "Testing Brightness Filter..." << std::endl;
    
    // Create a test image with known values
    int width = 100;
    int height = 100;
    int channels = 3;
    Image testImage(width, height, channels);
    unsigned char* data = testImage.getData();
    
    // Fill the image with a pattern
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int idx = (y * width + x) * channels;
            // Create pattern: horizontal gradient from 50 to 200
            int value = 50 + (x * 150 / width);
            data[idx] = value;
            data[idx + 1] = value;
            data[idx + 2] = value;
        }
    }
    
    // Save the original image
    testImage.save("test/images/brightness_original.png");
    
    // Create copies for different brightness adjustments
    Image increasedImage(width, height, channels);
    Image decreasedImage(width, height, channels);
    Image extremeIncreaseImage(width, height, channels);
    Image extremeDecreaseImage(width, height, channels);
    
    unsigned char* increasedData = increasedImage.getData();
    unsigned char* decreasedData = decreasedImage.getData();
    unsigned char* extremeIncreaseData = extremeIncreaseImage.getData();
    unsigned char* extremeDecreaseData = extremeDecreaseImage.getData();
    
    // Copy data to the test images
    std::memcpy(increasedData, data, width * height * channels);
    std::memcpy(decreasedData, data, width * height * channels);
    std::memcpy(extremeIncreaseData, data, width * height * channels);
    std::memcpy(extremeDecreaseData, data, width * height * channels);
    
    // Apply brightness with different offsets
    int increaseOffset = 50;
    int decreaseOffset = -30;
    int extremeIncreaseOffset = 300;  // Should clamp to 255
    int extremeDecreaseOffset = -300; // Should clamp to 0
    
    Filter::applyBrightness(increasedImage, increaseOffset);
    Filter::applyBrightness(decreasedImage, decreaseOffset);
    Filter::applyBrightness(extremeIncreaseImage, extremeIncreaseOffset);
    Filter::applyBrightness(extremeDecreaseImage, extremeDecreaseOffset);
    
    // Save result images
    increasedImage.save("test/images/brightness_increased.png");
    decreasedImage.save("test/images/brightness_decreased.png");
    extremeIncreaseImage.save("test/images/brightness_extreme_increase.png");
    extremeDecreaseImage.save("test/images/brightness_extreme_decrease.png");
    
    // Verify brightness was correctly applied
    bool testPassed = true;
    bool increaseCorrect = true;
    bool decreaseCorrect = true;
    bool extremeIncreaseCorrect = true;
    bool extremeDecreaseCorrect = true;
    
    // Check increased brightness
    for (int i = 0; i < width * height * channels; i++) {
        int expectedValue = std::min(255, data[i] + increaseOffset);
        if (increasedData[i] != expectedValue) {
            increaseCorrect = false;
            std::cout << "Increase test failed at index " << i 
                      << ": expected " << expectedValue 
                      << ", got " << (int)increasedData[i] << std::endl;
            break;
        }
    }
    
    // Check decreased brightness
    for (int i = 0; i < width * height * channels; i++) {
        int expectedValue = std::max(0, data[i] + decreaseOffset);
        if (decreasedData[i] != expectedValue) {
            decreaseCorrect = false;
            std::cout << "Decrease test failed at index " << i 
                      << ": expected " << expectedValue 
                      << ", got " << (int)decreasedData[i] << std::endl;
            break;
        }
    }
    
    // Check extreme increase (should clamp to 255)
    for (int i = 0; i < width * height * channels; i++) {
        int expectedValue = std::min(255, data[i] + extremeIncreaseOffset);
        if (extremeIncreaseData[i] != expectedValue) {
            extremeIncreaseCorrect = false;
            std::cout << "Extreme increase test failed at index " << i 
                      << ": expected " << expectedValue 
                      << ", got " << (int)extremeIncreaseData[i] << std::endl;
            break;
        }
    }
    
    // Check extreme decrease (should clamp to 0)
    for (int i = 0; i < width * height * channels; i++) {
        int expectedValue = std::max(0, data[i] + extremeDecreaseOffset);
        if (extremeDecreaseData[i] != expectedValue) {
            extremeDecreaseCorrect = false;
            std::cout << "Extreme decrease test failed at index " << i 
                      << ": expected " << expectedValue 
                      << ", got " << (int)extremeDecreaseData[i] << std::endl;
            break;
        }
    }
    
    // Sample a few pixels for display
    std::cout << "Brightness Adjustment Sample Pixels:" << std::endl;
    for (int i = 0; i < 5; i++) {
        int x = width / 6 * (i + 1);
        int y = height / 2;
        int idx = (y * width + x) * channels;
        
        std::cout << "Pixel (" << x << "," << y << ") R channel:" << std::endl
                  << "  Original: " << (int)data[idx] << std::endl
                  << "  +50 brightness: " << (int)increasedData[idx] << std::endl
                  << "  -30 brightness: " << (int)decreasedData[idx] << std::endl
                  << "  +300 brightness: " << (int)extremeIncreaseData[idx] << std::endl
                  << "  -300 brightness: " << (int)extremeDecreaseData[idx] << std::endl;
    }
    
    testPassed = increaseCorrect && decreaseCorrect && 
                 extremeIncreaseCorrect && extremeDecreaseCorrect;
    
    if (testPassed) {
        std::cout << "Brightness Filter test PASSED: All adjustments applied correctly" << std::endl;
    } else {
        std::cout << "Brightness Filter test FAILED:" << std::endl;
        if (!increaseCorrect) std::cout << "  - Brightness increase test failed" << std::endl;
        if (!decreaseCorrect) std::cout << "  - Brightness decrease test failed" << std::endl;
        if (!extremeIncreaseCorrect) std::cout << "  - Extreme brightness increase test failed" << std::endl;
        if (!extremeDecreaseCorrect) std::cout << "  - Extreme brightness decrease test failed" << std::endl;
    }
    
    return testPassed;
}