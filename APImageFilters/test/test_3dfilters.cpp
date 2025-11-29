#include "test_utilities.h"

// Note: It is assumed that Volume has methods such as getWidth(), getHeight(), getDepth(), getChannels(),
// setVoxel(x, y, z, value, channel) and getVoxel(x, y, z, channel). Adjust parameter order if necessary.

// Test for applyMedianBlur3D
bool testApplyMedianBlur3D() {
    std::cout << "Testing applyMedianBlur3D..." << std::endl;
    
    // Create a small volume of size 3x3x3 with one channel.
    int width = 3, height = 3, depth = 3, channels = 1;
    Volume vol(width, height, depth, channels);
    
    // Fill the volume with a constant value of 100.
    for (int z = 0; z < depth; ++z) {
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                vol.setVoxel(x, y, z, 100, 0);
            }
        }
    }
    // Introduce outliers:
    // Set center voxel (1,1,1) to 0 and one corner (0,0,0) to 200.
    vol.setVoxel(1, 1, 1, 0, 0);
    vol.setVoxel(0, 0, 0, 200, 0);
    
    // Apply 3D median blur with a kernel size of 3.
    Filter::applyMedianBlur3D(vol, 3);
    
    // In this nearly uniform volume the median should be 100 in most locations.
    // Check the center voxel (1,1,1) and a border voxel (1,1,0)
    bool passed = true;
    unsigned char centerValue = vol.getVoxel(1, 1, 1, 0);
    unsigned char borderValue = vol.getVoxel(1, 1, 0, 0);
    passed = passed && (centerValue == 100) && (borderValue == 100);
    
    if (passed)
        std::cout << "applyMedianBlur3D test PASSED." << std::endl;
    else
        std::cout << "applyMedianBlur3D test FAILED." << std::endl;
    
    return passed;
}

// Test for applyGaussianBlur3D
bool testApplyGaussianBlur3D() {
    std::cout << "Testing applyGaussianBlur3D..." << std::endl;
    
    // Create a constant volume of size 4x4x4 with one channel.
    int width = 4, height = 4, depth = 4, channels = 1;
    Volume vol(width, height, depth, channels);
    for (int z = 0; z < depth; ++z) {
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                vol.setVoxel(x, y, z, 150, 0);
            }
        }
    }
    
    // For a constant volume, a Gaussian blur should leave the values unchanged (apart from rounding).
    Filter::applyGaussianBlur3D(vol, 3, 1.0f);
    
    bool passed = true;
    for (int z = 0; z < depth; ++z) {
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                unsigned char blurredValue = vol.getVoxel(x, y, z, 0);
                passed = passed && (blurredValue == 150);
            }
        }
    }
    
    if (passed)
        std::cout << "applyGaussianBlur3D test PASSED." << std::endl;
    else
        std::cout << "applyGaussianBlur3D test FAILED." << std::endl;
    
    return passed;
}

// int main() {
//     bool allPassed = true;
//     allPassed = testApplyMedianBlur3D() && allPassed;
//     allPassed = testApplyGaussianBlur3D() && allPassed;
    
//     if (allPassed)
//         std::cout << "All 3D blur filter tests PASSED." << std::endl;
//     else
//         std::cout << "Some 3D blur filter tests FAILED." << std::endl;
    
//     return 0;
// }
