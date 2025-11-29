#include "test_utilities.h"

bool testGaussianBlur() {
    std::cout << "Testing Gaussian Blur Filter..." << std::endl;
    
    // Create a test image with patterns that will show blurring effects
    int width = 200;
    int height = 200;
    int channels = 3;
    Image testImage(width, height, channels);
    unsigned char* data = testImage.getData();
    
    // Create a pattern with sharp edges for testing blur
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int idx = (y * width + x) * channels;
            
            // Create a pattern of squares
            bool isSquare = ((x / 40) % 2 == 0) ^ ((y / 40) % 2 == 0);
            
            if (isSquare) {
                data[idx] = 255;     // R
                data[idx + 1] = 255; // G
                data[idx + 2] = 255; // B
            } else {
                data[idx] = 0;       // R
                data[idx + 1] = 0;   // G
                data[idx + 2] = 0;   // B
            }
        }
    }
    
    // Save the original image
    testImage.save("test/images/gaussian_blur_original.png");
    
    // Create copies for different kernel sizes and standard deviations
    Image smallBlurImage(width, height, channels);
    Image mediumBlurImage(width, height, channels);
    Image largeBlurImage(width, height, channels);
    Image evenKernelImage(width, height, channels); // For testing even kernel size handling
    
    unsigned char* smallBlurData = smallBlurImage.getData();
    unsigned char* mediumBlurData = mediumBlurImage.getData();
    unsigned char* largeBlurData = largeBlurImage.getData();
    unsigned char* evenKernelData = evenKernelImage.getData();
    
    // Copy data to the test images
    std::memcpy(smallBlurData, data, width * height * channels);
    std::memcpy(mediumBlurData, data, width * height * channels);
    std::memcpy(largeBlurData, data, width * height * channels);
    std::memcpy(evenKernelData, data, width * height * channels);
    
    // Apply Gaussian blur with different kernel sizes and standard deviations
    int smallKernel = 3;
    float smallStdev = 1.0f;
    
    int mediumKernel = 7;
    float mediumStdev = 2.0f;
    
    int largeKernel = 15;
    float largeStdev = 4.0f;
    
    int evenKernel = 6; // Should be adjusted to 7
    float evenStdev = 2.0f;
    
    Filter::applyGaussianBlur(smallBlurImage, smallKernel, smallStdev);
    Filter::applyGaussianBlur(mediumBlurImage, mediumKernel, mediumStdev);
    Filter::applyGaussianBlur(largeBlurImage, largeKernel, largeStdev);
    Filter::applyGaussianBlur(evenKernelImage, evenKernel, evenStdev);
    
    // Save result images
    smallBlurImage.save("test/images/gaussian_blur_small.png");
    mediumBlurImage.save("test/images/gaussian_blur_medium.png");
    largeBlurImage.save("test/images/gaussian_blur_large.png");
    evenKernelImage.save("test/images/gaussian_blur_even_kernel.png");
    
    // Verify Gaussian blur was correctly applied
    bool testPassed = true;
    
    // Helper function to calculate average edge strength (as a measure of blur)
    auto calculateEdgeStrength = [](unsigned char* imgData, int width, int height, int channels) -> double {
        double totalEdgeStrength = 0.0;
        int edgeCount = 0;
        
        for (int y = 1; y < height - 1; y++) {
            for (int x = 1; x < width - 1; x++) {
                for (int c = 0; c < channels; c++) {
                    int centerIdx = (y * width + x) * channels + c;
                    int rightIdx = (y * width + (x + 1)) * channels + c;
                    int downIdx = ((y + 1) * width + x) * channels + c;
                    
                    // Calculate horizontal and vertical gradients
                    int horizontalGradient = std::abs(imgData[rightIdx] - imgData[centerIdx]);
                    int verticalGradient = std::abs(imgData[downIdx] - imgData[centerIdx]);
                    
                    // Use magnitude of gradient vector
                    double gradient = std::sqrt(horizontalGradient * horizontalGradient + 
                                              verticalGradient * verticalGradient);
                    
                    totalEdgeStrength += gradient;
                    edgeCount++;
                }
            }
        }
        
        return totalEdgeStrength / edgeCount;
    };
    
    // Calculate edge strength for all images
    double originalEdgeStrength = calculateEdgeStrength(data, width, height, channels);
    double smallBlurEdgeStrength = calculateEdgeStrength(smallBlurData, width, height, channels);
    double mediumBlurEdgeStrength = calculateEdgeStrength(mediumBlurData, width, height, channels);
    double largeBlurEdgeStrength = calculateEdgeStrength(largeBlurData, width, height, channels);
    double evenKernelEdgeStrength = calculateEdgeStrength(evenKernelData, width, height, channels);
    
    std::cout << "Edge strength measurements:" << std::endl;
    std::cout << "  Original: " << originalEdgeStrength << std::endl;
    std::cout << "  Small blur (k=" << smallKernel << ", σ=" << smallStdev << "): " 
              << smallBlurEdgeStrength << std::endl;
    std::cout << "  Medium blur (k=" << mediumKernel << ", σ=" << mediumStdev << "): " 
              << mediumBlurEdgeStrength << std::endl;
    std::cout << "  Large blur (k=" << largeKernel << ", σ=" << largeStdev << "): " 
              << largeBlurEdgeStrength << std::endl;
    std::cout << "  Even kernel (k=" << evenKernel << " → " << evenKernel+1 
              << ", σ=" << evenStdev << "): " << evenKernelEdgeStrength << std::endl;
    
    // Verify blur effect increases with kernel size and standard deviation
    // Allow a small tolerance for the first comparison
    double relativeSmallDifference = std::abs(originalEdgeStrength - smallBlurEdgeStrength) / originalEdgeStrength;
    bool blurEffectWorks = (relativeSmallDifference < 0.01 || originalEdgeStrength > smallBlurEdgeStrength) && 
                          (smallBlurEdgeStrength > mediumBlurEdgeStrength) && 
                          (mediumBlurEdgeStrength > largeBlurEdgeStrength);
    
    // Verify even kernel size is correctly adjusted to odd
    // The edge strength should be approximately equal to the medium blur
    // Use a 5% tolerance for floating point comparisons
    double relativeDifference = std::abs(evenKernelEdgeStrength - mediumBlurEdgeStrength) / mediumBlurEdgeStrength;
    bool evenKernelHandledCorrectly = relativeDifference < 0.05;
    
    // Check sample pixels at edge transitions
    std::cout << "Gaussian Blur Sample Pixels:" << std::endl;
    
    // Sample at a border between black and white regions
    int sampleX = 40; // At a square boundary
    int sampleY = height / 2;
    int idx = (sampleY * width + sampleX) * channels;
    
    std::cout << "Sample pixel values at edge transition (" << sampleX << "," << sampleY << "):" << std::endl;
    std::cout << "  Original: (" 
              << (int)data[idx] << "," 
              << (int)data[idx+1] << "," 
              << (int)data[idx+2] << ")" << std::endl;
    
    std::cout << "  Small blur: (" 
              << (int)smallBlurData[idx] << "," 
              << (int)smallBlurData[idx+1] << "," 
              << (int)smallBlurData[idx+2] << ")" << std::endl;
    
    std::cout << "  Medium blur: (" 
              << (int)mediumBlurData[idx] << "," 
              << (int)mediumBlurData[idx+1] << "," 
              << (int)mediumBlurData[idx+2] << ")" << std::endl;
    
    std::cout << "  Large blur: (" 
              << (int)largeBlurData[idx] << "," 
              << (int)largeBlurData[idx+1] << "," 
              << (int)largeBlurData[idx+2] << ")" << std::endl;
    
    // Edge transitions should be smoothed (not pure black or white)
    // For a proper Gaussian blur, edge pixels should have intermediate values
    bool smoothEdgeTransition = (smallBlurData[idx] > 0 && smallBlurData[idx] < 255) || 
                               (mediumBlurData[idx] > 0 && mediumBlurData[idx] < 255) || 
                               (largeBlurData[idx] > 0 && largeBlurData[idx] < 255);
    
    // Check kernel size warning message
    // This is harder to test directly, but we've included the test case
    
    testPassed = blurEffectWorks && evenKernelHandledCorrectly && smoothEdgeTransition;
    
    if (testPassed) {
        std::cout << "Gaussian Blur Filter test PASSED: Blur effect increases correctly with kernel size and σ" << std::endl;
    } else {
        std::cout << "Gaussian Blur Filter test FAILED:" << std::endl;
        if (!blurEffectWorks) {
            std::cout << "  - Blur effect does not increase properly with kernel size and σ" << std::endl;
        }
        if (!evenKernelHandledCorrectly) {
            std::cout << "  - Even kernel size not handled correctly" << std::endl;
        }
        if (!smoothEdgeTransition) {
            std::cout << "  - Edge transitions not properly smoothed" << std::endl;
        }
    }
    
    return testPassed;
}