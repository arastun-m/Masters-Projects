#include "test_utilities.h"

bool testMedianBlur() {
    std::cout << "Testing Median Blur Filter..." << std::endl;
    
    // Create a test image with patterns that will show the median blur effect
    int width = 200;
    int height = 200;
    int channels = 3;
    Image testImage(width, height, channels);
    unsigned char* data = testImage.getData();
    
    // Create a pattern with salt and pepper noise
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int idx = (y * width + x) * channels;
            
            // Create a gradient background
            int baseValue = (x + y) % 200;
            data[idx] = baseValue;
            data[idx + 1] = baseValue;
            data[idx + 2] = baseValue;
            
            // Add random salt and pepper noise (10% probability)
            if (rand() % 100 < 10) {
                // 50% salt, 50% pepper
                unsigned char noiseValue = (rand() % 2 == 0) ? 0 : 255;
                data[idx] = noiseValue;
                data[idx + 1] = noiseValue;
                data[idx + 2] = noiseValue;
            }
        }
    }
    
    // Save the original noisy image
    testImage.save("test/images/median_blur_original.png");
    
    // Create copies for different kernel sizes
    Image smallKernelImage(width, height, channels);
    Image mediumKernelImage(width, height, channels);
    Image largeKernelImage(width, height, channels);
    Image evenKernelImage(width, height, channels); // For testing even kernel size handling
    Image negativeKernelImage(width, height, channels); // For testing negative kernel size handling
    
    unsigned char* smallKernelData = smallKernelImage.getData();
    unsigned char* mediumKernelData = mediumKernelImage.getData();
    unsigned char* largeKernelData = largeKernelImage.getData();
    unsigned char* evenKernelData = evenKernelImage.getData();
    unsigned char* negativeKernelData = negativeKernelImage.getData();
    
    // Copy data to the test images
    std::memcpy(smallKernelData, data, width * height * channels);
    std::memcpy(mediumKernelData, data, width * height * channels);
    std::memcpy(largeKernelData, data, width * height * channels);
    std::memcpy(evenKernelData, data, width * height * channels);
    std::memcpy(negativeKernelData, data, width * height * channels);
    
    // Apply median blur with different kernel sizes
    int smallKernel = 3;
    int mediumKernel = 5;
    int largeKernel = 7;
    int evenKernel = 4; // Should be adjusted to 5
    int negativeKernel = -1; // Should be handled with error message
    
    Filter::applyMedianBlur(smallKernelImage, smallKernel);
    Filter::applyMedianBlur(mediumKernelImage, mediumKernel);
    Filter::applyMedianBlur(largeKernelImage, largeKernel);
    Filter::applyMedianBlur(evenKernelImage, evenKernel);
    Filter::applyMedianBlur(negativeKernelImage, negativeKernel);
    
    // Save result images
    smallKernelImage.save("test/images/median_blur_small.png");
    mediumKernelImage.save("test/images/median_blur_medium.png");
    largeKernelImage.save("test/images/median_blur_large.png");
    evenKernelImage.save("test/images/median_blur_even_kernel.png");
    negativeKernelImage.save("test/images/median_blur_negative_kernel.png");
    
    // Verify median blur was correctly applied
    bool testPassed = true;
    
    // Lambda function to count noise pixels
    auto countNoisePixels = [](unsigned char* imgData, int width, int height, int channels) -> int {
        int noiseCount = 0;
        for (int i = 0; i < width * height; i++) {
            int idx = i * channels;
            // Check if pixel is pure black or pure white (noise)
            if ((imgData[idx] == 0 && imgData[idx+1] == 0 && imgData[idx+2] == 0) ||
                (imgData[idx] == 255 && imgData[idx+1] == 255 && imgData[idx+2] == 255)) {
                noiseCount++;
            }
        }
        return noiseCount;
    };
    
    // Count noise pixels in original and filtered images
    int originalNoiseCount = countNoisePixels(data, width, height, channels);
    int smallKernelNoiseCount = countNoisePixels(smallKernelData, width, height, channels);
    int mediumKernelNoiseCount = countNoisePixels(mediumKernelData, width, height, channels);
    int largeKernelNoiseCount = countNoisePixels(largeKernelData, width, height, channels);
    int evenKernelNoiseCount = countNoisePixels(evenKernelData, width, height, channels);
    int negativeKernelNoiseCount = countNoisePixels(negativeKernelData, width, height, channels);
    
    std::cout << "Noise pixel counts:" << std::endl;
    std::cout << "  Original: " << originalNoiseCount << std::endl;
    std::cout << "  Small kernel (k=" << smallKernel << "): " << smallKernelNoiseCount << std::endl;
    std::cout << "  Medium kernel (k=" << mediumKernel << "): " << mediumKernelNoiseCount << std::endl;
    std::cout << "  Large kernel (k=" << largeKernel << "): " << largeKernelNoiseCount << std::endl;
    std::cout << "  Even kernel (k=" << evenKernel << " -> " << evenKernel+1 << "): " << evenKernelNoiseCount << std::endl;
    std::cout << "  Negative kernel (k=" << negativeKernel << "): " << negativeKernelNoiseCount << std::endl;
    
    // Verify noise reduction improves with larger kernels
    bool noiseReductionWorks = 
        (smallKernelNoiseCount < originalNoiseCount) && 
        (mediumKernelNoiseCount <= smallKernelNoiseCount) && 
        (largeKernelNoiseCount <= mediumKernelNoiseCount);
    
    // Verify even kernel size is correctly adjusted to odd
    bool evenKernelHandledCorrectly = (evenKernelNoiseCount == mediumKernelNoiseCount);
    
    // Verify negative kernel size is handled correctly (should not modify the image)
    bool negativeKernelHandledCorrectly = (negativeKernelNoiseCount == originalNoiseCount);
    
    // Sample a few pixels to visually verify median behavior
    std::cout << "Median Blur Sample Pixels:" << std::endl;
    
    // Create a region with deterministic noise for testing
    int testX = width / 4;
    int testY = height / 4;
    int testWindowSize = 5;
    
    // Create a test area with specific values
    for (int y = testY; y < testY + testWindowSize; y++) {
        for (int x = testX; x < testX + testWindowSize; x++) {
            if (x < width && y < height) {
                int idx = (y * width + x) * channels;
                unsigned char pixelValue = 0;
                
                // Create a pattern: corners are 0, center is 255, rest are 128
                if ((x == testX && y == testY) || 
                    (x == testX && y == testY + testWindowSize - 1) ||
                    (x == testX + testWindowSize - 1 && y == testY) ||
                    (x == testX + testWindowSize - 1 && y == testY + testWindowSize - 1)) {
                    pixelValue = 0; // Corners
                } else if (x == testX + testWindowSize/2 && y == testY + testWindowSize/2) {
                    pixelValue = 255; // Center
                } else {
                    pixelValue = 128; // Others
                }
                
                data[idx] = pixelValue;
                data[idx + 1] = pixelValue;
                data[idx + 2] = pixelValue;
                
                // Copy to small kernel image for testing
                smallKernelData[idx] = pixelValue;
                smallKernelData[idx + 1] = pixelValue;
                smallKernelData[idx + 2] = pixelValue;
            }
        }
    }
    
    // Apply median blur to just this test region
    Filter::applyMedianBlur(smallKernelImage, 3);
    
    // Display the center pixel before and after
    int centerIdx = ((testY + testWindowSize/2) * width + (testX + testWindowSize/2)) * channels;
    std::cout << "Test region center pixel:" << std::endl;
    std::cout << "  Original: " << (int)data[centerIdx] << " (should be 255)" << std::endl;
    std::cout << "  After median blur: " << (int)smallKernelData[centerIdx] << " (should be 128)" << std::endl;
    
    // The median of [0,0,0,0,128,128,128,128,255] should be 128
    bool medianCalculationCorrect = (smallKernelData[centerIdx] == 128);
    
    testPassed = noiseReductionWorks && evenKernelHandledCorrectly && 
                 negativeKernelHandledCorrectly && medianCalculationCorrect;
    
    if (testPassed) {
        std::cout << "Median Blur Filter test PASSED: Noise reduction and median calculation work correctly" << std::endl;
    } else {
        std::cout << "Median Blur Filter test FAILED:" << std::endl;
        if (!noiseReductionWorks) {
            std::cout << "  - Noise reduction does not improve with larger kernels" << std::endl;
        }
        if (!evenKernelHandledCorrectly) {
            std::cout << "  - Even kernel size not adjusted correctly" << std::endl;
        }
        if (!negativeKernelHandledCorrectly) {
            std::cout << "  - Negative kernel size not handled correctly" << std::endl;
        }
        if (!medianCalculationCorrect) {
            std::cout << "  - Median calculation is incorrect" << std::endl;
        }
    }
    
    return testPassed;
}