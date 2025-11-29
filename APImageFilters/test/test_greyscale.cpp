#include "test_utilities.h"


bool testGreyscaleConversion() {
    std::cout << "Testing Greyscale Conversion..." << std::endl;
    
    // Create a test image with known RGB values
    int width = 100;
    int height = 100;
    int channels = 3;
    Image testImage(width, height, channels);
    unsigned char* data = testImage.getData();
    
    // Fill the image with different colors for testing
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int idx = (y * width + x) * channels;
            // Create a pattern with different R, G, B values
            data[idx] = (x % 255);                // R
            data[idx + 1] = (y % 255);            // G
            data[idx + 2] = ((x + y) % 255);      // B
        }
    }
    
    // Save the original color image
    testImage.save("test/images/greyscale_original.png");
    
    // Create a copy for verification
    Image originalCopy(width, height, channels);
    unsigned char* copyData = originalCopy.getData();
    std::memcpy(copyData, data, width * height * channels);
    
    // Apply greyscale conversion
    Filter::applyGreyscale(testImage);
    
    // Save the greyscale image
    testImage.save("test/images/greyscale_result.png");
    
    // Verify the greyscale conversion
    bool conversionCorrect = true;
    
    // Check if channels is now 1
    if (testImage.getChannels() != 1) {
        std::cout << "Greyscale Conversion test FAILED: Image should have 1 channel but has " 
                  << testImage.getChannels() << std::endl;
        return false;
    }
    
    // Verify pixel values - each greyscale pixel should match the expected formula
    data = testImage.getData();
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int originalIdx = (y * width + x) * channels;
            int greyscaleIdx = y * width + x;
            
            // Calculate expected greyscale value using the same formula
            unsigned char expectedGrey = static_cast<unsigned char>(
                0.2126 * copyData[originalIdx] + 
                0.7152 * copyData[originalIdx + 1] + 
                0.0722 * copyData[originalIdx + 2]
            );
            
            // Allow for a small rounding difference (1 unit) due to floating point calculations
            if (abs(data[greyscaleIdx] - expectedGrey) > 1) {
                std::cout << "Greyscale Conversion test FAILED: Incorrect conversion at (" 
                          << x << "," << y << "). Expected: " << (int)expectedGrey 
                          << " Got: " << (int)data[greyscaleIdx] << std::endl;
                conversionCorrect = false;
                break;
            }
        }
        if (!conversionCorrect) break;
    }
    
    // Sample a few pixels for display in the output
    std::cout << "Checking sample pixels:" << std::endl;
    for (int i = 0; i < 5; i++) {
        int x = width / 6 * (i + 1);
        int y = height / 2;
        
        int originalIdx = (y * width + x) * channels;
        int greyscaleIdx = y * width + x;
        
        unsigned char expectedGrey = static_cast<unsigned char>(
            0.2126 * copyData[originalIdx] + 
            0.7152 * copyData[originalIdx + 1] + 
            0.0722 * copyData[originalIdx + 2]
        );
        
        std::cout << "Sample pixel (" << x << "," << y << "): "
                  << "Original RGB: (" << (int)copyData[originalIdx] << ","
                  << (int)copyData[originalIdx + 1] << "," << (int)copyData[originalIdx + 2] << ") "
                  << "Greyscale: " << (int)data[greyscaleIdx] << " "
                  << "Expected: " << (int)expectedGrey << std::endl;
    }
    
    if (conversionCorrect) {
        std::cout << "Greyscale Conversion test PASSED: All pixels correctly converted" << std::endl;
    } else {
        std::cout << "Greyscale Conversion test FAILED: Some pixels have incorrect values" << std::endl;
    }
    
    return conversionCorrect;
}