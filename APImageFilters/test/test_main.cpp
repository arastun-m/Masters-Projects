#include "test_utilities.h"

// Function declarations for tests implemented in other files
bool testSharpeningFilter();
bool testEdgeDetection();
bool testHistogramEqualization();
bool testGreyscaleConversion(); 
bool testThresholdFilter();
bool testBrightnessFilter();
bool testSaltPepperNoise();
bool testBoxBlur();
bool testMedianBlur();
bool testGaussianBlur();
bool testProjectionMIP();
bool testProjectionMinIP();
bool testProjectionAIP();
bool testProjectionMedianAIP();
bool testExtractXY();
bool testExtractXZ();
bool testExtractYZ();
bool testSetGetVoxel();
bool testConvertToHSV();
bool testHSVRoundTrip();
bool testConvertToHSL();
bool testHSLRoundTrip();
bool testErrorHandling();
bool testApplyMedianBlur3D();
bool testApplyGaussianBlur3D();

int main() {
    // Run all tests
    bool sharpeningTestPassed = testSharpeningFilter();
    bool edgeDetectionTestPassed = testEdgeDetection();
    bool histogramEqualizationTestPassed = testHistogramEqualization();
    bool greyscaleTestPassed = testGreyscaleConversion();
    bool thresholdTestPassed = testThresholdFilter();
    bool brightnessTestPasses = testBrightnessFilter();
    bool saltpeppernoiseTestPasses = testSaltPepperNoise();
    bool boxblurTestPasses = testBoxBlur();
    bool medianblurTestPasses = testMedianBlur();
    bool gaussianblurTestPasses = testGaussianBlur();
    bool projectionMIPTestPassed = testProjectionMIP();
    bool projectionMinIPTestPassed = testProjectionMinIP();
    bool projectionAIPTestPassed = testProjectionAIP();
    bool projectionMedianAIPTestPassed = testProjectionMedianAIP();
    bool extractXYTestPassed = testExtractXY();
    bool extractXZTestPassed = testExtractXZ();
    bool extractYZTestPassed = testExtractYZ();
    bool setGetVoxelTestPassed = testSetGetVoxel();
    bool convertToHSVTestPassed = testConvertToHSV();
    bool hsvRoundTripTestPassed = testHSVRoundTrip();
    bool convertToHSLTestPassed = testConvertToHSL();
    bool hslRoundTripTestPassed = testHSLRoundTrip();
    bool errorHandlingTestPassed = testErrorHandling();
    bool applyMedianBlur3DTestPassed = testApplyMedianBlur3D();
    bool applyGaussianBlur3DTestPassed = testApplyGaussianBlur3D();



    // Print summary
    std::cout << "\nTest Summary:" << std::endl;
    std::cout << "Sharpening: " << (sharpeningTestPassed ? "PASSED" : "FAILED") << std::endl;
    std::cout << "Edge Detection: " << (edgeDetectionTestPassed ? "PASSED" : "FAILED") << std::endl;
    std::cout << "Histogram Equalization: " << (histogramEqualizationTestPassed ? "PASSED" : "FAILED") << std::endl;
    std::cout << "Greyscale: " << (greyscaleTestPassed ? "PASSED" : "FAILED") << std::endl;
    std::cout << "Threshold: " << (thresholdTestPassed ? "PASSED" : "FAILED") << std::endl;
    std::cout << "Brightness: " << (brightnessTestPasses ? "PASSED" : "FAILED") << std::endl;
    std::cout << "Salt & Pepper Noise: " << (saltpeppernoiseTestPasses ? "PASSED" : "FAILED") << std::endl;
    std::cout << "Box Blur: " << (boxblurTestPasses ? "PASSED" : "FAILED") << std::endl;
    std::cout << "Median Blur: " << (medianblurTestPasses ? "PASSED" : "FAILED") << std::endl;
    std::cout << "Gaussian Blur: " << (gaussianblurTestPasses ? "PASSED" : "FAILED") << std::endl;
    std::cout << "Projection MIP: " << (projectionMIPTestPassed ? "PASSED" : "FAILED") << std::endl;
    std::cout << "Projection MinIP: " << (projectionMinIPTestPassed ? "PASSED" : "FAILED") << std::endl;
    std::cout << "Projection AIP: " << (projectionAIPTestPassed ? "PASSED" : "FAILED") << std::endl;
    std::cout << "Projection MedianAIP: " << (projectionMedianAIPTestPassed ? "PASSED" : "FAILED") << std::endl;
    std::cout << "Extract XY: " << (extractXYTestPassed ? "PASSED" : "FAILED") << std::endl;
    std::cout << "Extract XZ: " << (extractXZTestPassed ? "PASSED" : "FAILED") << std::endl;
    std::cout << "Extract YZ: " << (extractYZTestPassed ? "PASSED" : "FAILED") << std::endl;
    std::cout << "Set/Get Voxel: " << (setGetVoxelTestPassed ? "PASSED" : "FAILED") << std::endl;
    std::cout << "Convert to HSV: " << (convertToHSVTestPassed ? "PASSED" : "FAILED") << std::endl;
    std::cout << "HSV Round-Trip: " << (hsvRoundTripTestPassed ? "PASSED" : "FAILED") << std::endl;
    std::cout << "Convert to HSL: " << (convertToHSLTestPassed ? "PASSED" : "FAILED") << std::endl;
    std::cout << "HSL Round-Trip: " << (hslRoundTripTestPassed ? "PASSED" : "FAILED") << std::endl;
    std::cout << "Error Handling: " << (errorHandlingTestPassed ? "PASSED" : "FAILED") << std::endl;
    std::cout << "Apply Median Blur 3D: " << (applyMedianBlur3DTestPassed ? "PASSED" : "FAILED") << std::endl;
    std::cout << "Apply Gaussian Blur 3D: " << (applyGaussianBlur3DTestPassed ? "PASSED" : "FAILED") << std::endl;


    // Return 0 if all tests passed, 1 otherwise
    return (sharpeningTestPassed && edgeDetectionTestPassed && histogramEqualizationTestPassed && greyscaleTestPassed && thresholdTestPassed
    &&brightnessTestPasses && saltpeppernoiseTestPasses && boxblurTestPasses && medianblurTestPasses &&gaussianblurTestPasses &&projectionMIPTestPassed
    && projectionMinIPTestPassed && projectionAIPTestPassed && projectionMedianAIPTestPassed && extractXYTestPassed && extractXZTestPassed && extractYZTestPassed 
    && setGetVoxelTestPassed && convertToHSVTestPassed && hsvRoundTripTestPassed && convertToHSLTestPassed && hslRoundTripTestPassed && errorHandlingTestPassed 
    && applyMedianBlur3DTestPassed && applyGaussianBlur3DTestPassed) ? 0 : 1;
}