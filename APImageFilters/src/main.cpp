#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <iostream>
#include <unordered_map>
#include <vector>
#include <sstream>
#include "Image.h"
#include "Volume.h"
#include "Filter.h"
#include "Projection.h"
#include "Slice.h"


/**
 * @class APImageFilters
 * @brief Main class for the APImageFilters application.
 * 
 * Handles the parsing of command line arguments and the processing of images and volumes.
 * 
 * Example usage:
 * @code
 * ./APImageFilters -i input.png -r Gaussian 5 2.0 output.png
 * ./APImageFilters -i input.png -g -b 50 -r Gaussian 5 2.0 -h HSV output.png
 * ./APImageFilters -d volume.vol -r Gaussian 5 2.0 -s XY 50 -p MIP output.png
 * @endcode
 */
class APImageFilters {
public:
    void process(int argc, char* argv[]);

private:
    std::unordered_map<std::string, std::vector<std::string>> options;
    std::string input_file, output_file;
    std::string volume_directory, volume_filename;
    std::string processed_volume_dir = "ProcessedVolume";
    bool is_volume = false;
    int first_index = 1, last_index = -1; // Thin slab default values

    void parseArguments(int argc, char* argv[]);
    void processImage();
    void processVolume();
};

/**
 * @brief Parse command line arguments and store them in the options map.
 * 
 * The options map stores the command line arguments in the format:
 * - Key: Option flag (e.g. "-r", "--blur")
 * - Value: Vector of arguments for that option (e.g. ["Gaussian", "5", "2.0"])
 * 
 * @param argc Number of arguments.
 * @param argv Array of arguments.
 * @throw EXIT_FAILURE if the number of arguments is less than 4.
 */
void APImageFilters::parseArguments(int argc, char* argv[]) {
    if (argc < 4) {
        std::cerr << "Usage: ./APImageFilters (-i <image> | -d <volume>) [options] <output_image>" << std::endl;
        exit(EXIT_FAILURE);
    }

    // Map long options to short equivalents
    std::unordered_map<std::string, std::string> alias_map = {
        {"--first", "-f"},
        {"--last", "-l"},
        {"--projection", "-p"},
        {"--slice", "-s"},
        {"--blur", "-r"},
        {"--threshold", "-t"},
        {"--brightness", "-b"},
        {"--greyscale", "-g"},
        {"--histogram", "-h"},
        {"--edge", "-e"},
        {"--sharpen", "-p"},
        {"--saltpepper", "-n"}
    };

    for (int i = 1; i < argc - 1; ++i) {
        std::string arg = argv[i];

        // Normalize long options to their short equivalents
        if (alias_map.count(arg)) {
            arg = alias_map[arg];
        }

        if (arg == "-i" && i + 1 < argc) {
            input_file = argv[++i];
        } else if (arg == "-d" && i + 1 < argc) {
            std::string full_path = argv[++i];
            size_t last_slash = full_path.find_last_of("/");
            if (last_slash != std::string::npos) {
                volume_directory = full_path.substr(0, last_slash + 1); // Keep the directory path
                volume_filename = full_path.substr(last_slash + 1); // Store the base filename
            } else {
                volume_directory = full_path; // If no slash, assume it's just the directory
                volume_filename = "vol"; // Default name if unspecified
            }
            is_volume = true;
        } else if (arg == "-f") {
            if (i + 1 < argc) first_index = std::stoi(argv[++i]);
        } else if (arg == "-l") {
            if (i + 1 < argc) last_index = std::stoi(argv[++i]);
        } else if (arg[0] == '-') {
            std::vector<std::string> values;
            while (i + 1 < argc - 1 &&
                   (argv[i + 1][0] != '-' || 
                   (std::isdigit(argv[i + 1][1]) || argv[i + 1][1] == '.' || 
                   (argv[i + 1][0] == '-' && std::isdigit(argv[i + 1][1]))))) {
                values.push_back(argv[++i]);
            }
            options[arg] = values;
        }
    }

    output_file = argv[argc - 1];
}

/**
 * @brief Process the image or volume based on the command line arguments.
 */
void APImageFilters::process(int argc, char* argv[]) {
    parseArguments(argc, argv);
    if (is_volume) {
        processVolume();
    } else {
        processImage();
    }
}

/**
 * @brief Process the image based on the command line arguments.
 * 
 * The image is loaded from the input file and processed based on the options provided.
 * The processed image is saved to the output file.
 * 
 * @throw EXIT_FAILURE if the image fails to load.
 */
void APImageFilters::processImage() {
    Image img(input_file);
    if (!img.getData()) {
        std::cerr << "Failed to load image: " << input_file << std::endl;
        return;
    }

    for (const auto& opt : options) {
        if (opt.first == "-g" || opt.first == "--greyscale") {
            Filter::applyGreyscale(img);
        } else if (opt.first == "-b" || opt.first == "--brightness") {
            int brightnessOffset = (opt.second.empty()) ? 666 : std::stoi(opt.second[0]);
            Filter::applyBrightness(img, brightnessOffset);
        } else if (opt.first == "-h" || opt.first == "--histogram") {
            std::string type = (opt.second.empty()) ? "HSV" : opt.second[0];
            Filter::applyHistogramEqualization(img, type);
        } else if (opt.first == "-r" || opt.first == "--blur") {
            if (opt.second.size() >= 2) {
                std::string type = opt.second[0];
                int size = std::stoi(opt.second[1]);
                float stdev = (opt.second.size() == 3) ? std::stof(opt.second[2]) : 1.0f;
                if (type == "Gaussian") {
                    Filter::applyGaussianBlur(img, size, stdev);
                } else if (type == "Box") {
                    Filter::applyBoxBlur(img, size);
                } else if (type == "Median") {
                    Filter::applyMedianBlur(img, size);
                }
            }
        } else if (opt.first == "-e" || opt.first == "--edge") {
            Filter::applyEdgeDetection(img, opt.second[0]);
        } else if (opt.first == "-p" || opt.first == "--sharpen") {
            Filter::applySharpening(img);
        } else if (opt.first == "-n" || opt.first == "--saltpepper") {
            Filter::applySaltPepperNoise(img, std::stof(opt.second[0]));
        } else if (opt.first == "-t" || opt.first == "--threshold") {
            int threshold_value = std::stoi(opt.second[0]);
            std::string type = (opt.second.size() > 1) ? opt.second[1] : "HSV";
            Filter::applyThreshold(img, threshold_value, type);
        }
    }

    img.save(output_file);
}

/**
 * @brief Process the volume based on the command line arguments.
 * 
 * The volume is loaded from the volume directory and filename and processed based on the options provided.
 * The processed volume is saved to the processed volume directory.
 * The processed volume can be further sliced or projected and saved to the output file.
 * 
 * @throw EXIT_FAILURE if the volume fails to load.
 */
void APImageFilters::processVolume() {
    std::string processed_volume_path = processed_volume_dir + "/processed_volume";
    Volume vol(volume_directory, volume_filename);
    if (!vol.getData()) {
        std::cerr << "Failed to load volume: " << volume_directory << std::endl;
        return;
    }
    if (last_index == -1) last_index = vol.getDepth(); // Default to full volume

    if (options.count("-r") || options.count("--blur")) {
        auto blurArgs = options["-r"];
        if (blurArgs.size() >= 2) {
            std::string type = blurArgs[0];
            int size = std::stoi(blurArgs[1]);
            float stdev = 2.0f; // Default value for stdev
            
            if (type == "Gaussian") {
                if (blurArgs.size() >= 3) {
                    stdev = std::stof(blurArgs[2]);
                }
                Filter::applyGaussianBlur3D(vol, size, stdev);
            } else if (type == "Median") {
                Filter::applyMedianBlur3D(vol, size);
            }
            
            vol.save(processed_volume_path);
            std::cout << "Saved blurred volume to " << processed_volume_path << "\n";
        }
    }


    std::unique_ptr<Image> result;
    if (options.count("-s") || options.count("--slice")) {
        auto sliceArgs = options["-s"];
        if (sliceArgs.size() == 2) {
            std::string plane = sliceArgs[0];
            int index = std::stoi(sliceArgs[1]);

            // Create a sub-volume with only the thin slab range
            Volume subVol = vol.extractSubVolume(first_index - 1, last_index - 1);

            if (plane == "XY") {
                result = Slice::extractXY(subVol, index);
            } else if (plane == "XZ") {
                result = Slice::extractXZ(subVol, index);
            } else if (plane == "YZ") {
                result = Slice::extractYZ(subVol, index);
            }
        }
    }
    if (options.count("-p") || options.count("--projection")) {
        auto projArgs = options["-p"];
        if (!projArgs.empty()) {
            std::string type = projArgs[0];
            result = Projection::orthographicProjection(vol, type, first_index, last_index);
        }
    }
    
    if (result && result->getData()) {
        result->save(output_file);
        std::cout << "Saved volume result to " << output_file << "\n";
    }
}

/**
 * @brief Main function for the APImageFilters application.
 */
int main(int argc, char* argv[]) {
    APImageFilters app;
    app.process(argc, argv);
    return 0;
}