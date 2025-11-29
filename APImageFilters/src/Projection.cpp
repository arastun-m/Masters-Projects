#include "Projection.h"
#include <algorithm>
#include <numeric>
#include <cmath>
#include <iostream>
#include <vector>


// Define an enum to represent the projection type for efficient comparisons.
enum class ProjectionType { MIP, MinIP, AIP, MedianAIP };

// Second veersion, optimised
std::unique_ptr<Image> Projection::orthographicProjection(
    const Volume& volume,
    const std::string& type,
    int minZ,
    int maxZ)
{
    /**
     * @brief Computes an orthographic projection of a 3D volume.
     *
     * Projects a volumetric dataset into a 2D image using various projection methods:
     * - MIP (Maximum Intensity Projection)
     * - MinIP (Minimum Intensity Projection)
     * - meanAIP (Average Intensity Projection)
     * - medianAIP (Median Intensity Projection)
     *
     * @param volume The input 3D volume.
     * @param type The projection type ("MIP", "MinIP", "meanAIP", "medianAIP").
     * @param minZ The starting slice (1-indexed).
     * @param maxZ The ending slice (1-indexed, 0 for full depth).
     * @return A unique pointer to the projected 2D image, or nullptr if input is invalid.
     */

    // Get volume dimensions: width, height, depth, and number of channels.
    int width = volume.getWidth();
    int height = volume.getHeight();
    int depth = volume.getDepth();
    int channels = volume.getChannels();

    // If maxZ is 0, use the entire depth of the volume.
    if (maxZ == 0) {
        maxZ = depth;
    }

    // Validate slab boundaries (user input is 1-indexed).
    if (minZ < 1 || minZ > maxZ || maxZ > depth) {
        std::cerr << "Error: Invalid slab range." << std::endl;
        return nullptr;
    }
    // Convert user-provided slab boundaries (1-indexed) to 0-indexed.
    int startZ = minZ - 1;
    int endZ = maxZ - 1;

    // Calculate the number of slices in the slab.
    int slabCount = endZ - startZ + 1;

    // Create the output image that will store the projection result.
    auto result = std::make_unique<Image>(width, height, channels);
    unsigned char* outData = result->getData();
    const unsigned char* volData = volume.getData();

    // Compute the total number of elements in one slice (width * height * channels).
    int sliceSize = width * height * channels;
    // Total number of output elements (each pixel-channel combination) is the same as one slice.
    int totalElements = sliceSize;

    // Determine the projection type using an enum to avoid repeated string comparisons.
    ProjectionType projType = ProjectionType::MIP;  // default
    if (type == "MIP") {
        projType = ProjectionType::MIP;
    }
    else if (type == "MinIP") {
        projType = ProjectionType::MinIP;
    }
    //added mean behind it. it was just meanAIP
    else if (type == "meanAIP") {
        projType = ProjectionType::AIP;
    }
    //uncapitalized the M in medianAIP
    else if (type == "medianAIP") {
        projType = ProjectionType::MedianAIP;
    }
    else {
        // Error
        std::cerr << "Error: Invalid type." << std::endl;
    }

    // Iterate over every output element (pixel and channel) using a linear index
    for (int idx = 0; idx < totalElements; ++idx) {
        unsigned char projectedValue = 0;
        switch (projType) {
        case ProjectionType::MIP: {
            // Maximum Intensity Projection (MIP): find the maximum value along the z-axis
            unsigned char maxVal = 0;
            for (int z = startZ; z <= endZ; ++z) {
                // Compute the index in the volume for the current z-slice
                int volIndex = z * sliceSize + idx;
                unsigned char val = volData[volIndex];
                if (val > maxVal) {
                    maxVal = val;
                }
            }
            projectedValue = maxVal;
            break;
        }
        case ProjectionType::MinIP: {
            // Minimum Intensity Projection (MinIP): find the minimum value along the z-axis
            unsigned char minVal = 255;
            for (int z = startZ; z <= endZ; ++z) {
                int volIndex = z * sliceSize + idx;
                unsigned char val = volData[volIndex];
                if (val < minVal) {
                    minVal = val;
                }
            }
            projectedValue = minVal;
            break;
        }
        case ProjectionType::AIP: {
            // Average Intensity Projection (AIP): compute the average value along the z-axis
            double sum = 0.0;
            for (int z = startZ; z <= endZ; ++z) {
                int volIndex = z * sliceSize + idx;
                sum += volData[volIndex];
            }
            projectedValue = static_cast<unsigned char>(std::round(sum / slabCount));
            break;
        }
        case ProjectionType::MedianAIP: {
            // Initialize a histogram array for values 0-255 with all counts set to 0
            int hist[256] = { 0 };

            // Build the histogram for the current pixel's slab across the z-axis
            for (int z = startZ; z <= endZ; ++z) {
                int volIndex = z * sliceSize + idx;
                unsigned char val = volData[volIndex];
                hist[val]++;
            }

            int n = slabCount;
            int cumSum = 0;

            // For an odd number of elements, the median is at position n/2 (0-indexed)
            if (n % 2 == 1) {
                int medianPos = n / 2;
                for (int v = 0; v < 256; ++v) {
                    cumSum += hist[v];
                    if (cumSum > medianPos) {
                        projectedValue = static_cast<unsigned char>(v);
                        break;
                    }
                }
            }
            // For an even number of elements, find the two middle values
            else {
                int lowerPos = n / 2 - 1;
                int upperPos = n / 2;
                int lowerMedian = 0, upperMedian = 0;
                bool foundLower = false;
                for (int v = 0; v < 256; ++v) {
                    cumSum += hist[v];
                    if (!foundLower && cumSum > lowerPos) {
                        lowerMedian = v;
                        foundLower = true;
                    }
                    if (cumSum > upperPos) {
                        upperMedian = v;
                        break;
                    }
                }
                // Compute the average of the two median values for even number of elements
                projectedValue = static_cast<unsigned char>(std::round((lowerMedian + upperMedian) / 2.0));
            }
            break;
        }

        default: {
            break;
        }
        }
        // Write the computed projection value to the output image data
        outData[idx] = projectedValue;
    }

    return result;
}

