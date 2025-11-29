#include "test_utilities.h"

bool testSaltPepperNoise() {
    std::cout << "Testing Salt & Pepper Noise Filter..." << std::endl;
    
    // Create a test image with uniform values to easily detect noise
    int width = 200;
    int height = 200;
    int channels = 3;
    Image testImage(width, height, channels);
    unsigned char* data = testImage.getData();
    
    // Fill the image with a middle gray color (makes noise more visible)
    for (int i = 0; i < width * height * channels; i++) {
        data[i] = 128;
    }
    
    // Save the original image
    testImage.save("test/images/salt_pepper_original.png");
    
    // Create copies for different noise levels
    Image lowNoiseImage(width, height, channels);
    Image mediumNoiseImage(width, height, channels);
    Image highNoiseImage(width, height, channels);
    
    unsigned char* lowNoiseData = lowNoiseImage.getData();
    unsigned char* mediumNoiseData = mediumNoiseImage.getData();
    unsigned char* highNoiseData = highNoiseImage.getData();
    
    // Copy data to the test images
    std::memcpy(lowNoiseData, data, width * height * channels);
    std::memcpy(mediumNoiseData, data, width * height * channels);
    std::memcpy(highNoiseData, data, width * height * channels);
    
    // Set seed for reproducible testing
    srand(12345);
    
    // Apply noise with different percentages
    float lowPercentage = 1.0f;
    float mediumPercentage = 5.0f;
    float highPercentage = 15.0f;
    
    Filter::applySaltPepperNoise(lowNoiseImage, lowPercentage);
    
    // Reset seed between tests for reproducibility
    srand(12345);
    Filter::applySaltPepperNoise(mediumNoiseImage, mediumPercentage);
    
    srand(12345);
    Filter::applySaltPepperNoise(highNoiseImage, highPercentage);
    
    // Save result images
    lowNoiseImage.save("test/images/salt_pepper_low.png");
    mediumNoiseImage.save("test/images/salt_pepper_medium.png");
    highNoiseImage.save("test/images/salt_pepper_high.png");
    
    // Verify salt and pepper noise was correctly applied
    bool testPassed = true;
    
    // Count altered pixels in each image
    int lowNoiseCount = 0;
    int mediumNoiseCount = 0;
    int highNoiseCount = 0;
    
    for (int i = 0; i < width * height; i++) {
        int pixelIdx = i * channels;
        
        // Check if pixel is salt (white) or pepper (black)
        bool isNoiseLow = (lowNoiseData[pixelIdx] == 0 || lowNoiseData[pixelIdx] == 255) &&
                          (lowNoiseData[pixelIdx] == lowNoiseData[pixelIdx + 1]) &&
                          (lowNoiseData[pixelIdx] == lowNoiseData[pixelIdx + 2]);
        
        bool isNoiseMedium = (mediumNoiseData[pixelIdx] == 0 || mediumNoiseData[pixelIdx] == 255) &&
                             (mediumNoiseData[pixelIdx] == mediumNoiseData[pixelIdx + 1]) &&
                             (mediumNoiseData[pixelIdx] == mediumNoiseData[pixelIdx + 2]);
        
        bool isNoiseHigh = (highNoiseData[pixelIdx] == 0 || highNoiseData[pixelIdx] == 255) &&
                           (highNoiseData[pixelIdx] == highNoiseData[pixelIdx + 1]) &&
                           (highNoiseData[pixelIdx] == highNoiseData[pixelIdx + 2]);
        
        if (isNoiseLow) lowNoiseCount++;
        if (isNoiseMedium) mediumNoiseCount++;
        if (isNoiseHigh) highNoiseCount++;
    }
    
    // Expected number of noisy pixels (+/- 10% tolerance for randomness)
    int totalPixels = width * height;
    int expectedLowCount = static_cast<int>(totalPixels * (lowPercentage / 100.0f));
    int expectedMediumCount = static_cast<int>(totalPixels * (mediumPercentage / 100.0f));
    int expectedHighCount = static_cast<int>(totalPixels * (highPercentage / 100.0f));
    
    float lowCountTolerance = std::abs(lowNoiseCount - expectedLowCount) / static_cast<float>(expectedLowCount);
    float mediumCountTolerance = std::abs(mediumNoiseCount - expectedMediumCount) / static_cast<float>(expectedMediumCount);
    float highCountTolerance = std::abs(highNoiseCount - expectedHighCount) / static_cast<float>(expectedHighCount);
    
    bool lowCountCorrect = lowCountTolerance <= 0.1f;  // Within 10% of expected
    bool mediumCountCorrect = mediumCountTolerance <= 0.1f;
    bool highCountCorrect = highCountTolerance <= 0.1f;
    
    // Print statistics
    std::cout << "Noise Level Statistics:" << std::endl;
    std::cout << "  Low Noise (" << lowPercentage << "%): " 
              << lowNoiseCount << " pixels altered, expected " << expectedLowCount 
              << " (tolerance: " << (lowCountTolerance * 100.0f) << "%)" << std::endl;
    
    std::cout << "  Medium Noise (" << mediumPercentage << "%): " 
              << mediumNoiseCount << " pixels altered, expected " << expectedMediumCount 
              << " (tolerance: " << (mediumCountTolerance * 100.0f) << "%)" << std::endl;
    
    std::cout << "  High Noise (" << highPercentage << "%): " 
              << highNoiseCount << " pixels altered, expected " << expectedHighCount 
              << " (tolerance: " << (highCountTolerance * 100.0f) << "%)" << std::endl;
    
    // Check color distribution (should be roughly 50% black, 50% white)
    int saltCount = 0;  // White pixels (255)
    int pepperCount = 0;  // Black pixels (0)
    
    for (int i = 0; i < width * height; i++) {
        int idx = i * channels;
        if (highNoiseData[idx] == 255 && 
            highNoiseData[idx] == highNoiseData[idx+1] && 
            highNoiseData[idx] == highNoiseData[idx+2]) {
            saltCount++;
        } else if (highNoiseData[idx] == 0 && 
                   highNoiseData[idx] == highNoiseData[idx+1] && 
                   highNoiseData[idx] == highNoiseData[idx+2]) {
            pepperCount++;
        }
    }
    
    float saltRatio = static_cast<float>(saltCount) / (saltCount + pepperCount);
    std::cout << "  Salt/Pepper distribution: " 
              << (saltRatio * 100.0f) << "% salt, " 
              << ((1.0f - saltRatio) * 100.0f) << "% pepper "
              << "(expected close to 50/50)" << std::endl;
    
    bool distributionCorrect = saltRatio >= 0.4f && saltRatio <= 0.6f;  // Within 10% of 50/50
    
    testPassed = lowCountCorrect && mediumCountCorrect && highCountCorrect && distributionCorrect;
    
    if (testPassed) {
        std::cout << "Salt & Pepper Noise Filter test PASSED: Noise applied correctly at all levels" << std::endl;
    } else {
        std::cout << "Salt & Pepper Noise Filter test FAILED:" << std::endl;
        if (!lowCountCorrect) std::cout << "  - Low noise level count incorrect" << std::endl;
        if (!mediumCountCorrect) std::cout << "  - Medium noise level count incorrect" << std::endl;
        if (!highCountCorrect) std::cout << "  - High noise level count incorrect" << std::endl;
        if (!distributionCorrect) std::cout << "  - Salt/pepper distribution not balanced" << std::endl;
    }
    
    return testPassed;
}