#include "test_utilities.h"

bool testBoxBlur() {
    std::cout << "Testing Box Blur Filter..." << std::endl;
    
    // Create a test image with patterns that will show blurring effects
    int width = 200;
    int height = 200;
    int channels = 3;
    Image testImage(width, height, channels);
    unsigned char* data = testImage.getData();
    
    // Create a pattern with sharp transitions
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int idx = (y * width + x) * channels;
            
            // Create a checkerboard pattern
            bool isChecked = ((x / 20) % 2 == 0) ^ ((y / 20) % 2 == 0);
            
            if (isChecked) {
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
    testImage.save("test/images/boxblur_original.png");
    
    // Create copies for different kernel sizes
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
    
    // Apply box blur with different kernel sizes
    int smallKernel = 3;
    int mediumKernel = 9;
    int largeKernel = 21;
    int evenKernel = 8; // Should be adjusted to 9 by the function
    
    Filter::applyBoxBlur(smallBlurImage, smallKernel);
    Filter::applyBoxBlur(mediumBlurImage, mediumKernel);
    Filter::applyBoxBlur(largeBlurImage, largeKernel);
    Filter::applyBoxBlur(evenKernelImage, evenKernel);
    
    // Save result images
    smallBlurImage.save("test/images/boxblur_small.png");
    mediumBlurImage.save("test/images/boxblur_medium.png");
    largeBlurImage.save("test/images/boxblur_large.png");
    evenKernelImage.save("test/images/boxblur_even_kernel.png");
    
    // Verify box blur was correctly applied
    bool testPassed = true;
    
    // Calculate average "sharpness" by measuring average local gradient magnitude
    // Helper function to calculate average gradient (measure of sharpness)
    auto calculateAverageGradient = [](unsigned char* data, int width, int height, int channels) -> double {
        double totalGradient = 0.0;
        int gradientCount = 0;
        
        for (int y = 1; y < height - 1; y++) {
            for (int x = 1; x < width - 1; x++) {
                for (int c = 0; c < channels; c++) {
                    int centerIdx = (y * width + x) * channels + c;
                    int rightIdx = (y * width + (x + 1)) * channels + c;
                    int downIdx = ((y + 1) * width + x) * channels + c;
                    
                    // Calculate horizontal and vertical gradients
                    int horizontalGradient = std::abs(data[rightIdx] - data[centerIdx]);
                    int verticalGradient = std::abs(data[downIdx] - data[centerIdx]);
                    
                    // Use magnitude of gradient vector
                    double gradient = std::sqrt(horizontalGradient * horizontalGradient + 
                                               verticalGradient * verticalGradient);
                    
                    totalGradient += gradient;
                    gradientCount++;
                }
            }
        }
        
        return totalGradient / gradientCount;
    };
    
    double smallBlurSharpness = calculateAverageGradient(smallBlurData, width, height, channels);
    double mediumBlurSharpness = calculateAverageGradient(mediumBlurData, width, height, channels);
    double largeBlurSharpness = calculateAverageGradient(largeBlurData, width, height, channels);
    
    std::cout << "Average gradient (measure of sharpness):" << std::endl;
    std::cout << "  Small blur: " << smallBlurSharpness << std::endl;
    std::cout << "  Medium blur: " << mediumBlurSharpness << std::endl;
    std::cout << "  Large blur: " << largeBlurSharpness << std::endl;
    
    // Larger kernel should result in more blurring (less sharpness)
    bool hasBlurGradation = (smallBlurSharpness > mediumBlurSharpness) && 
                           (mediumBlurSharpness > largeBlurSharpness);
    
    // We'll use a tolerance since floating point calculations may lead to small differences
    int tolerance = 2;
    int totalDifference = 0;
    int pixelCount = 0;
    
    for (int i = 0; i < width * height * channels; i++) {
        int diff = std::abs(evenKernelData[i] - mediumBlurData[i]);
        totalDifference += diff;
        pixelCount++;
    }
    
    double avgDifference = static_cast<double>(totalDifference) / pixelCount;
    std::cout << "Average difference between even kernel and equivalent odd kernel: " 
              << avgDifference << std::endl;
    
    // Consider them equal if the average difference is small
    bool evenKernelHandledCorrectly = avgDifference <= tolerance;
    
    // Print sample pixel values for visual verification
    std::cout << "Box Blur Sample Pixels (at checkerboard transition):" << std::endl;
    int sampleX = 20; // At a checkerboard boundary
    int sampleY = height / 2;
    
    std::cout << "Pixel values at transition point (" << sampleX << "," << sampleY << "):" << std::endl;
    
    int idx = (sampleY * width + sampleX) * channels;
    std::cout << "  Original: (" 
              << (int)data[idx] << "," 
              << (int)data[idx+1] << "," 
              << (int)data[idx+2] << ")" << std::endl;
    
    std::cout << "  Small blur (k=" << smallKernel << "): (" 
              << (int)smallBlurData[idx] << "," 
              << (int)smallBlurData[idx+1] << "," 
              << (int)smallBlurData[idx+2] << ")" << std::endl;
    
    std::cout << "  Medium blur (k=" << mediumKernel << "): (" 
              << (int)mediumBlurData[idx] << "," 
              << (int)mediumBlurData[idx+1] << "," 
              << (int)mediumBlurData[idx+2] << ")" << std::endl;
    
    std::cout << "  Large blur (k=" << largeKernel << "): (" 
              << (int)largeBlurData[idx] << "," 
              << (int)largeBlurData[idx+1] << "," 
              << (int)largeBlurData[idx+2] << ")" << std::endl;
    
    testPassed = hasBlurGradation && evenKernelHandledCorrectly;
    
    if (testPassed) {
        std::cout << "Box Blur Filter test PASSED: Blur applied correctly with all kernel sizes" << std::endl;
    } else {
        std::cout << "Box Blur Filter test FAILED:" << std::endl;
        if (!hasBlurGradation) {
            std::cout << "  - Blur effect does not properly increase with kernel size" << std::endl;
        }
        if (!evenKernelHandledCorrectly) {
            std::cout << "  - Even kernel size not handled correctly" << std::endl;
        }
    }
    
    return testPassed;
}