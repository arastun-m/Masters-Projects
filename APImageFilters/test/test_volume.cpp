#include "test_utilities.h"


// Set a specific voxel value and then read it back to verify correctness.
bool testSetGetVoxel() {
    int width = 4, height = 4, depth = 4, channels = 3;
    Volume vol(width, height, depth, channels);
    
    // Set voxel at (1,2,3) channel 0 to 123
    vol.setVoxel(1, 2, 3, 123, 0);
    // Set voxel at (1,2,3) channel 1 to 45
    vol.setVoxel(1, 2, 3, 45, 1);
    // Set voxel at (1,2,3) channel 2 to 200
    vol.setVoxel(1, 2, 3, 200, 2);
    
    unsigned char v0 = vol.getVoxel(1, 2, 3, 0);
    unsigned char v1 = vol.getVoxel(1, 2, 3, 1);
    unsigned char v2 = vol.getVoxel(1, 2, 3, 2);
    
    assert(v0 == 123);
    assert(v1 == 45);
    assert(v2 == 200);
    
    std::cout << "TestSetGetVoxel: PASSED." << std::endl;
    return true;
}
// int main() {

//     testSetGetVoxel();
  
    
//     std::cout << "All Volume tests passed." << std::endl;
//     return 0;
// }