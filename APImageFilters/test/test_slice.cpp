#include "test_utilities.h"

// Helper function to compare two values (for single-channel data, exact comparison is sufficient)
bool equalValue(unsigned char a, unsigned char b) {
    return a == b;
}

// Test 1: Extract XY Slice
// Create a 2x2x3 volume (width=2, height=2, depth=3) with one channel.
// Fill slice 0 with value 10, slice 1 with value 20, and slice 2 with value 30.
// Then extract the XY slice at zIndex = 1 and verify that all pixels are 20.
bool testExtractXY() {
    std::cout << "Testing XY Slice Extraction..." << std::endl;
    bool passed = true;
    
    int width = 2, height = 2, depth = 3, channels = 1;
    Volume vol(width, height, depth, channels);
    // Fill each slice with a constant value:
    // Slice 0: value 10, Slice 1: value 20, Slice 2: value 30.
    for (int z = 0; z < depth; ++z) {
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                // Set voxel value to 10 + 10 * z
                vol.setVoxel(x, y, z, static_cast<unsigned char>(10 + 10 * z), 0);
            }
        }
    }
    
    auto sliceXY = Slice::extractXY(vol, 1);
    unsigned char* sliceData = sliceXY->getData();
    // The extracted XY slice (at zIndex = 1) should be 2x2, and every pixel should be 20.
    for (int i = 0; i < width * height * channels; ++i) {
        passed = passed && equalValue(sliceData[i], 20);
    }
    
    if (passed)
        std::cout << "XY Slice Extraction test PASSED." << std::endl;
    else
        std::cout << "XY Slice Extraction test FAILED." << std::endl;
    
    return passed;
}

// Test 2: Extract XZ Slice
// Create a 2x3x4 volume (width=2, height=3, depth=4) with one channel.
// For each voxel, set its value = y*10 + z. Then extract the XZ slice at yIndex = 1.
// The resulting image should have size 2 x 4 and each pixel value should equal (1*10 + z).
bool testExtractXZ() {
    std::cout << "Testing XZ Slice Extraction..." << std::endl;
    bool passed = true;
    
    int width = 2, height = 3, depth = 4, channels = 1;
    Volume vol(width, height, depth, channels);
    // Fill the volume: value = y*10 + z
    for (int z = 0; z < depth; ++z) {
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                vol.setVoxel(x, y, z, static_cast<unsigned char>(y * 10 + z), 0);
            }
        }
    }
    
    auto sliceXZ = Slice::extractXZ(vol, 1); // Extract XZ slice at yIndex = 1
    unsigned char* sliceData = sliceXZ->getData();
    // The extracted XZ slice should be 2 (width) x 4 (depth).
    // Expected: For each z from 0 to 3, pixel value = 1*10 + z.
    for (int z = 0; z < depth; ++z) {
        for (int x = 0; x < width; ++x) {
            int idx = (z * width + x) * channels;
            unsigned char expected = static_cast<unsigned char>(10 + z);
            passed = passed && equalValue(sliceData[idx], expected);
        }
    }
    
    if (passed)
        std::cout << "XZ Slice Extraction test PASSED." << std::endl;
    else
        std::cout << "XZ Slice Extraction test FAILED." << std::endl;
    
    return passed;
}

// Test 3: Extract YZ Slice
// Create a 3x2x4 volume (width=3, height=2, depth=4) with one channel.
// For each voxel, set its value = x*100 + y*10 + z. Then extract the YZ slice at xIndex = 2.
// The resulting image should have size 2 x 4 and each pixel value should equal (2*100 + y*10 + z).
bool testExtractYZ() {
    std::cout << "Testing YZ Slice Extraction..." << std::endl;
    bool passed = true;
    
    int width = 3, height = 2, depth = 4, channels = 1;
    Volume vol(width, height, depth, channels);
    // Fill the volume: value = x*100 + y*10 + z
    for (int z = 0; z < depth; ++z) {
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                vol.setVoxel(x, y, z, static_cast<unsigned char>(x * 100 + y * 10 + z), 0);
            }
        }
    }
    
    auto sliceYZ = Slice::extractYZ(vol, 2); // Extract YZ slice at xIndex = 2
    unsigned char* sliceData = sliceYZ->getData();
    // The extracted YZ slice should be 2 (height) x 4 (depth).
    // Expected: For each (y, z), value = 2*100 + y*10 + z.
    for (int z = 0; z < depth; ++z) {
        for (int y = 0; y < height; ++y) {
            int idx = (z * height + y) * channels;
            unsigned char expected = static_cast<unsigned char>(200 + y * 10 + z);
            passed = passed && equalValue(sliceData[idx], expected);
        }
    }
    
    if (passed)
        std::cout << "YZ Slice Extraction test PASSED." << std::endl;
    else
        std::cout << "YZ Slice Extraction test FAILED." << std::endl;
    
    return passed;
}

// int main() {
//     bool allPassed = true;
//     allPassed = testExtractXY() && allPassed;
//     allPassed = testExtractXZ() && allPassed;
//     allPassed = testExtractYZ() && allPassed;
    
//     if (allPassed)
//         std::cout << "All slice extraction tests PASSED." << std::endl;
//     else
//         std::cout << "Some slice extraction tests FAILED." << std::endl;
    
//     return 0;
// }
