#ifndef PROJECT_H
#define PROJECT_H

#include <string>
#include <memory>
#include "Volume.h"
#include "Image.h"


/**
 * @class Projection
 * @brief Creating orthographic projections of a volume.
 * 
 * The class provides a static method for creating orthographic projections of a volume.
 * 
 * Example usage:
 * @code
 * auto result = Projection::orthographicProjection(vol, "MIP", 1, 100);
 * @endcode
 */
class Projection {
public:
    static std::unique_ptr<Image> orthographicProjection(const Volume& volume, const std::string& type, int minZ, int maxZ);
};

#endif
