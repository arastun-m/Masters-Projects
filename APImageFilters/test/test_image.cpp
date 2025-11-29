#include "test_utilities.h"

// Helper function: compares two unsigned char values allowing a tolerance of 1 unit
bool approxEqual2(unsigned char a, unsigned char b, int tol = 1) {
    return std::abs(a - b) <= tol;
}

// Test 1: RGB -> HSV Conversion for red, black, and white
bool testConvertToHSV() {
    std::cout << "Testing RGB -> HSV Conversion..." << std::endl;
    bool passed = true;
    
    // Test for red: RGB = (255, 0, 0)
    {
        Image img(1, 1, 3);
        unsigned char* data = img.getData();
        data[0] = 255; data[1] = 0; data[2] = 0;
        img.convertToHSV();
        data = img.getData();
        // Expected for pure red: H = 0, S = 255, V = 255
        passed = passed && (data[0] == 0 && data[1] == 255 && data[2] == 255);
    }
    // Test for black: RGB = (0, 0, 0)
    {
        Image img(1, 1, 3);
        unsigned char* data = img.getData();
        data[0] = 0; data[1] = 0; data[2] = 0;
        img.convertToHSV();
        data = img.getData();
        // Expected for black: H = 0, S = 0, V = 0
        passed = passed && (data[0] == 0 && data[1] == 0 && data[2] == 0);
    }
    // Test for white: RGB = (255, 255, 255)
    {
        Image img(1, 1, 3);
        unsigned char* data = img.getData();
        data[0] = 255; data[1] = 255; data[2] = 255;
        img.convertToHSV();
        data = img.getData();
        // Expected for white: H = 0, S = 0, V = 255
        passed = passed && (data[0] == 0 && data[1] == 0 && data[2] == 255);
    }
    
    if (passed) {
        std::cout << "RGB -> HSV Conversion test PASSED." << std::endl;
    } else {
        std::cout << "RGB -> HSV Conversion test FAILED." << std::endl;
    }
    return passed;
}

// Test 2: HSV Round-Trip Conversion (RGB -> HSV -> RGB) for red, black, and white
bool testHSVRoundTrip() {
    std::cout << "Testing HSV Round-Trip Conversion..." << std::endl;
    bool passed = true;
    
    // Test for red
    {
        Image img(1, 1, 3);
        unsigned char* data = img.getData();
        data[0] = 255; data[1] = 0; data[2] = 0;
        img.convertToHSV();
        img.convertToRGBFromHSV();
        data = img.getData();
        passed = passed && (approxEqual2(data[0], 255) &&
                              approxEqual2(data[1], 0) &&
                              approxEqual2(data[2], 0));
    }
    // Test for black
    {
        Image img(1, 1, 3);
        unsigned char* data = img.getData();
        data[0] = 0; data[1] = 0; data[2] = 0;
        img.convertToHSV();
        img.convertToRGBFromHSV();
        data = img.getData();
        passed = passed && (approxEqual2(data[0], 0) &&
                              approxEqual2(data[1], 0) &&
                              approxEqual2(data[2], 0));
    }
    // Test for white
    {
        Image img(1, 1, 3);
        unsigned char* data = img.getData();
        data[0] = 255; data[1] = 255; data[2] = 255;
        img.convertToHSV();
        img.convertToRGBFromHSV();
        data = img.getData();
        passed = passed && (approxEqual2(data[0], 255) &&
                              approxEqual2(data[1], 255) &&
                              approxEqual2(data[2], 255));
    }
    
    if (passed) {
        std::cout << "HSV Round-Trip Conversion test PASSED." << std::endl;
    } else {
        std::cout << "HSV Round-Trip Conversion test FAILED." << std::endl;
    }
    return passed;
}

// Test 3: RGB -> HSL Conversion for red, black, and white
bool testConvertToHSL() {
    std::cout << "Testing RGB -> HSL Conversion..." << std::endl;
    bool passed = true;
    
    // Test for red: RGB = (255, 0, 0)
    {
        Image img(1, 1, 3);
        unsigned char* data = img.getData();
        data[0] = 255; data[1] = 0; data[2] = 0;
        img.convertToHSL();
        data = img.getData();
        // Expected for pure red: H = 0, S = 255, L ≈ 127 (0.5 * 255)
        passed = passed && (data[0] == 0 &&
                              data[1] == 255 &&
                              approxEqual2(data[2], 127));
    }
    // Test for black: RGB = (0, 0, 0)
    {
        Image img(1, 1, 3);
        unsigned char* data = img.getData();
        data[0] = 0; data[1] = 0; data[2] = 0;
        img.convertToHSL();
        data = img.getData();
        // Expected for black: H = 0, S = 0, L = 0
        passed = passed && (data[0] == 0 && data[1] == 0 && data[2] == 0);
    }
    // Test for white: RGB = (255, 255, 255)
    {
        Image img(1, 1, 3);
        unsigned char* data = img.getData();
        data[0] = 255; data[1] = 255; data[2] = 255;
        img.convertToHSL();
        data = img.getData();
        // Expected for white: H = 0, S = 0, L = 255
        passed = passed && (data[0] == 0 && data[1] == 0 && data[2] == 255);
    }
    
    if (passed) {
        std::cout << "RGB -> HSL Conversion test PASSED." << std::endl;
    } else {
        std::cout << "RGB -> HSL Conversion test FAILED." << std::endl;
    }
    return passed;
}

// Test 4: HSL Round-Trip Conversion (RGB -> HSL -> RGB) for red, black, and white
bool testHSLRoundTrip() {
    std::cout << "Testing HSL Round-Trip Conversion..." << std::endl;
    bool passed = true;
    
    // Test for red
    {
        Image img(1, 1, 3);
        unsigned char* data = img.getData();
        data[0] = 255; data[1] = 0; data[2] = 0;
        img.convertToHSL();
        img.convertToRGBFromHSL();
        data = img.getData();
        passed = passed && (approxEqual2(data[0], 255) &&
                              approxEqual2(data[1], 0) &&
                              approxEqual2(data[2], 0));
    }
    // Test for black
    {
        Image img(1, 1, 3);
        unsigned char* data = img.getData();
        data[0] = 0; data[1] = 0; data[2] = 0;
        img.convertToHSL();
        img.convertToRGBFromHSL();
        data = img.getData();
        passed = passed && (approxEqual2(data[0], 0) &&
                              approxEqual2(data[1], 0) &&
                              approxEqual2(data[2], 0));
    }
    // Test for white
    {
        Image img(1, 1, 3);
        unsigned char* data = img.getData();
        data[0] = 255; data[1] = 255; data[2] = 255;
        img.convertToHSL();
        img.convertToRGBFromHSL();
        data = img.getData();
        passed = passed && (approxEqual2(data[0], 255) &&
                              approxEqual2(data[1], 255) &&
                              approxEqual2(data[2], 255));
    }
    
    if (passed) {
        std::cout << "HSL Round-Trip Conversion test PASSED." << std::endl;
    } else {
        std::cout << "HSL Round-Trip Conversion test FAILED." << std::endl;
    }
    return passed;
}

// Test 5: Error Handling Test
// When the image has only 1 channel, calling the conversion functions should output an error message
// and leave the data unchanged.
bool testErrorHandling() {
    std::cout << "Testing Error Handling for 1-channel image..." << std::endl;
    bool passed = true;
    
    Image img(1, 1, 1);
    unsigned char* data = img.getData();
    data[0] = 123;  // Arbitrary initial value
    img.convertToHSV();
    img.convertToHSL();
    unsigned char* newData = img.getData();
    passed = passed && (newData[0] == 123);
    
    if (passed) {
        std::cout << "Error Handling test PASSED." << std::endl;
    } else {
        std::cout << "Error Handling test FAILED." << std::endl;
    }
    return passed;
}

// int main() {
//     bool allPassed = true;
//     allPassed = testConvertToHSV() && allPassed;
//     allPassed = testHSVRoundTrip() && allPassed;
//     allPassed = testConvertToHSL() && allPassed;
//     allPassed = testHSLRoundTrip() && allPassed;
//     allPassed = testErrorHandling() && allPassed;
    
//     if (allPassed) {
//         std::cout << "All tests PASSED." << std::endl;
//     } else {
//         std::cout << "Some tests FAILED." << std::endl;
//     }
//     return 0;
// }
