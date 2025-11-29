#include "test_utilities.h"


// Helper function: compares two unsigned char values (exact comparison is sufficient here)
bool equalValue1(unsigned char a, unsigned char b) {
    return a == b;
}

// Create a test volume of size 2x2x3 (width=2, height=2, depth=3, channels=1)
// Fill with known values for each slice:
// Slice 0: [10, 20; 30, 40]
// Slice 1: [50, 60; 70, 80]
// Slice 2: [90, 100; 110, 120]
Volume createTestVolume() {
    int width = 2, height = 2, depth = 3, channels = 1;
    Volume vol(width, height, depth, channels);
    for (int z = 0; z < depth; ++z) {
        int base = 10 + z * 40; // For z=0:10, z=1:50, z=2:90
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                int value = 0;
                if (x == 0 && y == 0) value = base;
                if (x == 1 && y == 0) value = base + 10;
                if (x == 0 && y == 1) value = base + 20;
                if (x == 1 && y == 1) value = base + 30;
                vol.setVoxel(x, y, z, static_cast<unsigned char>(value), 0);
            }
        }
    }
    return vol;
}

// Test 1: Maximum Intensity Projection (MIP)
bool testProjectionMIP() {
    std::cout << "Testing Projection MIP..." << std::endl;
    Volume vol = createTestVolume();
    // Use entire volume: user inputs are 1-indexed: minZ=1, maxZ=3.
    auto projected = Projection::orthographicProjection(vol, "MIP", 1, 3);
    unsigned char* projData = projected->getData();
    int width = vol.getWidth();
    
    // Expected values:
    // Pixel (0,0): [10, 50, 90]  -> max = 90
    // Pixel (1,0): [20, 60, 100] -> max = 100
    // Pixel (0,1): [30, 70, 110] -> max = 110
    // Pixel (1,1): [40, 80, 120] -> max = 120
    bool passed = true;
    passed = passed && equalValue1(projData[0], 90);
    passed = passed && equalValue1(projData[1], 100);
    passed = passed && equalValue1(projData[width], 110);
    passed = passed && equalValue1(projData[width+1], 120);
    
    if (passed) {
        std::cout << "Projection MIP test PASSED." << std::endl;
    } else {
        std::cout << "Projection MIP test FAILED." << std::endl;
    }
    return passed;
}

// Test 2: Minimum Intensity Projection (MinIP)
bool testProjectionMinIP() {
    std::cout << "Testing Projection MinIP..." << std::endl;
    Volume vol = createTestVolume();
    auto projected = Projection::orthographicProjection(vol, "MinIP", 1, 3);
    unsigned char* projData = projected->getData();
    int width = vol.getWidth();
    
    // Expected values:
    // Pixel (0,0): [10, 50, 90]  -> min = 10
    // Pixel (1,0): [20, 60, 100] -> min = 20
    // Pixel (0,1): [30, 70, 110] -> min = 30
    // Pixel (1,1): [40, 80, 120] -> min = 40
    bool passed = true;
    passed = passed && equalValue1(projData[0], 10);
    passed = passed && equalValue1(projData[1], 20);
    passed = passed && equalValue1(projData[width], 30);
    passed = passed && equalValue1(projData[width+1], 40);
    
    if (passed) {
        std::cout << "Projection MinIP test PASSED." << std::endl;
    } else {
        std::cout << "Projection MinIP test FAILED." << std::endl;
    }
    return passed;
}

// Test 3: Average Intensity Projection (AIP)
bool testProjectionAIP() {
    std::cout << "Testing Projection AIP..." << std::endl;
    Volume vol = createTestVolume();
    auto projected = Projection::orthographicProjection(vol, "meanAIP", 1, 3);
    unsigned char* projData = projected->getData();
    int width = vol.getWidth();
    
    // Expected values:
    // Pixel (0,0): (10+50+90)/3 = 50
    // Pixel (1,0): (20+60+100)/3 = 60
    // Pixel (0,1): (30+70+110)/3 = 70
    // Pixel (1,1): (40+80+120)/3 = 80
    bool passed = true;
    passed = passed && equalValue1(projData[0], 50);
    passed = passed && equalValue1(projData[1], 60);
    passed = passed && equalValue1(projData[width], 70);
    passed = passed && equalValue1(projData[width+1], 80);
    
    if (passed) {
        std::cout << "Projection AIP test PASSED." << std::endl;
    } else {
        std::cout << "Projection AIP test FAILED." << std::endl;
    }
    return passed;
}

// Test 4: Median Intensity Projection (MedianAIP)
// For an odd depth (3), the median is the middle value.
bool testProjectionMedianAIP() {
    std::cout << "Testing Projection MedianAIP..." << std::endl;
    Volume vol = createTestVolume();
    auto projected = Projection::orthographicProjection(vol, "medianAIP", 1, 3);
    unsigned char* projData = projected->getData();
    int width = vol.getWidth();
    
    // Expected values:
    // Pixel (0,0): [10, 50, 90] -> median = 50
    // Pixel (1,0): [20, 60, 100] -> median = 60
    // Pixel (0,1): [30, 70, 110] -> median = 70
    // Pixel (1,1): [40, 80, 120] -> median = 80
    bool passed = true;
    passed = passed && equalValue1(projData[0], 50);
    passed = passed && equalValue1(projData[1], 60);
    passed = passed && equalValue1(projData[width], 70);
    passed = passed && equalValue1(projData[width+1], 80);
    
    if (passed) {
        std::cout << "Projection MedianAIP test PASSED." << std::endl;
    } else {
        std::cout << "Projection MedianAIP test FAILED." << std::endl;
    }
    return passed;
}

// int main() {
//     bool allPassed = true;
//     allPassed = testProjectionMIP() && allPassed;
//     allPassed = testProjectionMinIP() && allPassed;
//     allPassed = testProjectionAIP() && allPassed;
//     allPassed = testProjectionMedianAIP() && allPassed;
    
//     if (allPassed) {
//         std::cout << "All projection tests PASSED." << std::endl;
//     } else {
//         std::cout << "Some projection tests FAILED." << std::endl;
//     }
//     return 0;
// }
