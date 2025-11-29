#ifndef SLICE_H
#define SLICE_H

#include "Volume.h"
#include "Image.h"
#include <memory>


/**
 * @class Slice
 * @brief Extracting 2D slices from a 3D volume.
 * 
 * The Slice class provides static methods for extracting 2D slices from a 3D volume.
 * The extracted slices can be in the XY, XZ, or YZ plane.
 */
class Slice {
public:
    // Extract a 2D slice from a 3D volume
    static std::unique_ptr<Image> extractXY(const Volume& volume, int zIndex);
    static std::unique_ptr<Image> extractXZ(const Volume& volume, int yIndex);
    static std::unique_ptr<Image> extractYZ(const Volume& volume, int xIndex);
};

#endif