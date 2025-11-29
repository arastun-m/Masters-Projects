#include "test_utilities.h"

bool testEdgeDetection() {
    std::cout << "Testing Edge Detection..." << std::endl;
    
    // Create a test image with a specific pattern that has clear edges
    int width = 100;
    int height = 100;
    int channels = 3;
    Image testImage(width, height, channels);
    unsigned char* data = testImage.getData();
    
    // Create a checkerboard pattern for clear edges
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int idx = (y * width + x) * channels;
            
            // Create a 10x10 checkerboard
            bool isWhite = ((x / 10) % 2 == 0) != ((y / 10) % 2 == 0);
            
            if (isWhite) {
                data[idx] = 255;
                data[idx + 1] = 255;
                data[idx + 2] = 255;
            } else {
                data[idx] = 0;
                data[idx + 1] = 0;
                data[idx + 2] = 0;
            }
        }
    }
    
    // Save the original image
    testImage.save("test/images/edgedetection_original.png");
    
    // Apply the edge detection filter (Sobel)
    Filter::applyEdgeDetection(testImage, "Sobel");
    
    // Save the edge detection result
    testImage.save("test/images/edgedetection_result.png");
    
    // Verify that edge detection worked correctly by checking 
    // that we now have a single-channel image with detected edges
    bool edgesDetected = false;
    data = testImage.getData();
    
    // After edge detection, the image should have 1 channel
    if (testImage.getChannels() != 1) {
        std::cout << "Edge Detection test FAILED: Expected 1 channel, got " 
                  << testImage.getChannels() << std::endl;
        return false;
    }
    
    // Count edge pixels (pixels with high intensity values)
    int edgePixels = 0;
    for (int i = 0; i < width * height; i++) {
        if (data[i] > 100) {  // Threshold to consider a pixel as edge
            edgePixels++;
        }
    }
    
    double edgePercentage = static_cast<double>(edgePixels) / (width * height) * 100.0;
    std::cout << "Percentage of edge pixels detected: " << edgePercentage << "%" << std::endl;
    
    // In a checkerboard pattern, we expect a significant number of edge pixels
    // but not too many (typical range for a 10x10 checkerboard is 15-25%)
    edgesDetected = (edgePercentage > 10.0 && edgePercentage < 35.0);
    
    if (edgesDetected) {
        std::cout << "Edge Detection test PASSED: Edges successfully detected" << std::endl;
    } else {
        std::cout << "Edge Detection test FAILED: Expected edge percentage between 10% and 30%, got " 
                  << edgePercentage << "%" << std::endl;
    }
    
    return edgesDetected;
}