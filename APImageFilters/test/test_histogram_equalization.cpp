#include "test_utilities.h"

bool testHistogramEqualization() {
    std::cout << "Testing Histogram Equalization..." << std::endl;
    
    // Create a test image with poor contrast
    int width = 100;
    int height = 100;
    int channels = 3;
    Image testImage(width, height, channels);
    unsigned char* data = testImage.getData();
    
    // Create a low-contrast image: most pixels in the middle range (100-150)
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int idx = (y * width + x) * channels;
            // Base value with low contrast
            int baseValue = 100 + (y * 50 / height);
            
            data[idx] = baseValue;
            data[idx + 1] = baseValue;
            data[idx + 2] = baseValue;
        }
    }
    
    // Save the original low-contrast image
    testImage.save("test/images/histeq_original.png");
    
    // Create a copy for comparison
    Image originalCopy(width, height, channels);
    unsigned char* copyData = originalCopy.getData();
    std::memcpy(copyData, data, width * height * channels);
    
    // Apply histogram equalization
    Filter::applyHistogramEqualization(testImage, "HSV");
    
    // Save the equalized image
    testImage.save("test/images/histeq_result.png");
    
    // Verify that histogram equalization improved contrast
    bool contrastImproved = false;
    
    // Calculate standard deviation of original image (measure of contrast)
    double originalSum = 0.0;
    for (int i = 0; i < width * height * channels; i++) {
        originalSum += copyData[i];
    }
    double originalMean = originalSum / (width * height * channels);
    
    double originalVariance = 0.0;
    for (int i = 0; i < width * height * channels; i++) {
        double diff = copyData[i] - originalMean;
        originalVariance += diff * diff;
    }
    originalVariance /= (width * height * channels);
    double originalStdDev = std::sqrt(originalVariance);
    
    // Calculate standard deviation of equalized image
    data = testImage.getData();
    double equalizedSum = 0.0;
    for (int i = 0; i < width * height * channels; i++) {
        equalizedSum += data[i];
    }
    double equalizedMean = equalizedSum / (width * height * channels);
    
    double equalizedVariance = 0.0;
    for (int i = 0; i < width * height * channels; i++) {
        double diff = data[i] - equalizedMean;
        equalizedVariance += diff * diff;
    }
    equalizedVariance /= (width * height * channels);
    double equalizedStdDev = std::sqrt(equalizedVariance);
    
    // Histogram equalization should increase standard deviation (improve contrast)
    std::cout << "Original image standard deviation: " << originalStdDev << std::endl;
    std::cout << "Equalized image standard deviation: " << equalizedStdDev << std::endl;
    
    // Consider it successful if standard deviation increases by at least 10%
    contrastImproved = (equalizedStdDev > originalStdDev * 1.1);
    
    if (contrastImproved) {
        std::cout << "Histogram Equalization test PASSED: Contrast improved" << std::endl;
    } else {
        std::cout << "Histogram Equalization test FAILED: Contrast did not improve significantly" << std::endl;
    }
    
    return contrastImproved;
}