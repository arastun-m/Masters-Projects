#include "test_utilities.h"

bool testSharpeningFilter() {
    std::cout << "Testing Sharpening Filter..." << std::endl;
    
    // Create a test image with a specific pattern
    int width = 100;
    int height = 100;
    int channels = 3;
    Image testImage(width, height, channels);
    unsigned char* data = testImage.getData();
    
    // Create a simple pattern: a gradient with a central square
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int idx = (y * width + x) * channels;
            // Create gradient background
            data[idx] = static_cast<unsigned char>(x * 255 / width);
            data[idx + 1] = static_cast<unsigned char>(y * 255 / height);
            data[idx + 2] = 128;
            
            // Add a center square
            if (x >= 40 && x < 60 && y >= 40 && y < 60) {
                data[idx] = 200;
                data[idx + 1] = 50;
                data[idx + 2] = 50;
            }
        }
    }
    
    // Save the original image for comparison
    testImage.save("test/images/sharpening_original.png");
    
    // Create a copy for comparison after sharpening
    Image originalCopy(width, height, channels);
    unsigned char* copyData = originalCopy.getData();
    std::memcpy(copyData, data, width * height * channels);
    
    // Apply the sharpening filter
    Filter::applySharpening(testImage);
    
    // Save the sharpened image
    testImage.save("test/images/sharpening_result.png");
    
    // Verify that sharpening increased contrast along edges
    bool hasSharpened = false;
    int differentPixels = 0;
    for (int i = 0; i < width * height * channels; i++) {
        if (data[i] != copyData[i]) {
            differentPixels++;
        }
    }
    
    // Check if at least some percentage of pixels changed (sharpening should affect edge pixels)
    double percentChanged = static_cast<double>(differentPixels) / (width * height * channels) * 100.0;
    std::cout << "Percentage of pixels changed: " << percentChanged << "%" << std::endl;
    
    // If at least 1% of pixels changed, consider it successful
    hasSharpened = (percentChanged > 1.0);
    
    if (hasSharpened) {
        std::cout << "Sharpening test PASSED: Image was successfully sharpened" << std::endl;
    } else {
        std::cout << "Sharpening test FAILED: Image was not properly sharpened" << std::endl;
    }
    
    return hasSharpened;
}