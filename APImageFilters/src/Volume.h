#ifndef VOLUME_H
#define VOLUME_H

#include <string>


/**
 * @class Volume
 * @brief Load, save, and process 3D volumes.
 * 
 * The Volume class provides methods to load and save 3D volumes.
 * It provides methods to access and modify voxel values in the volume.
 * The Volume class also provides methods to extract sub-volumes and apply 3D filters.
 * 
 * The volume data is stored as unsigned char values in row-major order.
 * The data is stored in the format: [R, G, B, R, G, B, ...].
 * It consists of stacked 2D images in the format: [slice0, slice1, slice2, ...].
 * 
 * Example usage:
 * @code
 * Volume vol("volume_directory/", "vol");
 * vol.save("output_directory/");
 * @endcode
 */
class Volume {
public:
    // Constructors & Destructor
    Volume(const std::string &filename, const std::string &filenamePrefix); // Load from file
    Volume(int width, int height, int depth, int channels); // Create empty volume
    Volume(const Volume& other); // Copy constructor
    ~Volume();

     // Assignment Operator Overloading
     Volume& operator=(const Volume& other);

    // Save volume data
    void save(const std::string &filename) const;
    
    // Accessors
    unsigned char* getData() const;
    void setData(unsigned char *newData);
    int getWidth() const;
    int getHeight() const;
    int getDepth() const;
    int getChannels() const;
    void setChannels(int newChannels);
    
    // Voxel access
    unsigned char getVoxel(int x, int y, int z, int channel = 0) const;
    void setVoxel(int x, int y, int z, unsigned char value, int channel = 0);

    // Extract a sub-volume (thin slab) between minZ and maxZ
    Volume extractSubVolume(int minZ, int maxZ) const;

private:
    int width;
    int height;
    int depth;
    int channels;
    unsigned char* data;
};

#endif
