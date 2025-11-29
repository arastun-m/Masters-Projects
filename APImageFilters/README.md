# APImageFilters: Image Filters, Projections, and Slices

**APImageFilters** is a C++ project that implements a variety of image and volume filtering techniques. It supports both 2D images and 3D volumes (stacked images) and provides a range of filters, including color correction, blurring, sharpening, edge detection, and 3D-specific operations like slicing and orthographic projections.

This project was developed by the **Runge-Kutta** team as part of the Advanced Programming coursework.

## Team Members
- Yixuan Yan
- Chenwei Yin
- Yiding Hao
- Tingting Chen
- Arastun Mammadli
- Hazem Bakhshwain
- Adityee Jain

## Project Management
To streamline task allocation, issue tracking, and code reviews, we utilized **GitHub Projects** ([View Project Board](https://github.com/orgs/ese-ada-lovelace-2024/projects/53/views/1)).


## Table of Contents
- [Installation Instructions](#installation-instructions)
- [Usage Instructions](#usage-instructions)
- [Tests](#tests)
- [Documentation](#documentation)
- [More Information](#more-information)
- [References](#references)
- [License](#license)

---

## Installation Instructions

### MacOS or Linux
Make sure to have cmake and a C++ compiler installed on your device:
```bash
brew install cmake
brew install gcc
```

Clone the project repository:
```bash
git clone https://github.com/ese-ada-lovelace-2024/advanced-programming-group-runge-kutta.git
```

Then, create a build directory and run cmake:
```bash
cmake -S . -B build
```

Finally, compile the project:

```bash
cmake --build build
```

### Windows with MSVC
If you don't already have them installed on Windows (not WSL), you will need to install CMake and git. You can download CMake from [here](https://cmake.org/download/) and git from [here](https://git-scm.com/download/win).

First, in Windows Powershell, clone the repository:
```pwsh
git clone https://github.com/ese-ada-lovelace-2024/advanced-programming-group-runge-kutta.git
```

Then, create a build directory and run cmake (Visual Studio 17 2022 or any other appropriate version):
```pwsh
cmake -S . -B build -G "Visual Studio 17 2022"
```

Open the generated solution file (.sln) in Visual Studio:
```pwsh
start build\APImageFilters.sln
```

Finally, compile the project in Visual Studio.


## Usage instructions
Before running the commands ensure you are within the `build` directory and that it contains `APImageFilters` executable.
```
+-- build
|   +-- CMakeFiles/
|   +-- Testing/
|   +-- APImageFilters
|   ...
```


### Inputs
The project supports 2 inputs types:
1. Input Image:  `-i <input_image>`
2. Input Volume: `-d <input_volume_dir`>

When processing images, the `-i` flag should be used along with a path to the input image.

When processing volumes (3D; stacks of images), the `-d` flag should be used along with a  path to the directory containing all volume images. The volume directory should only contain the stacked images. Ideally, images should follow a sorted naming convention (`vol1.png`, `vol2.png`, and etc).


### Outputs
Based on input instructions, the program can be provided with 2 output types.
1. Output Image: `<output_image>` (must have extension .png or .jpg)
2. Output Directory: `<output_dir>` (stores all stacked images in this directory)


### 2D Image Input Flags
Follows the format: `./APImageFilters -i input.png <insert_flag> output.png`
The `<insert_flag>` options are defined below:

**Colour Correction:**
- Greyscale: `-g`
- Brightness: `-b <brightness_value>` (-256 to 256)
- Histogram Equalisation: `-h <HSV/HSL>` (default ot HSV)
- Threshold: `-t <treshold_val> <HSV/HSL>` (default ot HSV)
- Salt and Pepper Noise: `-n <noise_per>` (0 to 100)

**Convolutional Filters for Image Blur:**
- Box Blur: `-r Box <kernel_size>`
- Median Blur: `-r Median <kernel_size>`
- Gaussian Blur: `-r Gaussian <kernel_size> <std_dev>` (std_dev default to 2.0)

**Convolutional Filters for Laplacian Image Sharpening:**
- `--sharpen`

**Convolutional Filters for Edge Detection:**
- `--edge <type_name>` (supported types: Sobel, Prewitt, Scharr, Roberts Cross)


### 3D Volume Input Flags
Follows the format: `./APImageFilters -d input_volume_dir <insert_flag> output_dir`
The `<insert_flag>` options are defined below:

- 3D Median Blur: `-r Median <kernel_size>`
- 3D Gaussian Blur: `-r Gaussian <kernel_size> <std_dev>` (std_dev default to 2.0)
- Volume Projection: `-p <projection_type>` (projection types: MIP, MinIP, AIP)
- Slicing: `-s <slice_type>` (slice types: XY, YZ, XZ) 


### Combining Commands
The flags can also be combined to apply multiple actions at once.

For example; applying brightness and gaussian blur to the input.png and storing the result at output.png:
```bash
./APImageFilters -i input.png -b 100 -r Gaussian 5 2.0 output.png
```


## Tests
The tests are located in the following [test](test) directory.

Some of the covered features include:
- **Filter Accuracy:** Ensures that each filter produces the expected output for given inputs.
- **Boundary Handling:** Validates that filters handle edge cases (e.g., image boundaries) correctly.
- **Performance:** Ensures that filters operate efficiently, especially for large kernels or volumes.
- **Data Integrity:** Verifies that image and volume data is correctly loaded, processed, and saved without corruption.
- **Parameter Validation:** Tests that filters handle invalid parameters (e.g., even kernel sizes) gracefully.

### Running Tests
Go to the project directory, and build the project:
```bash
cmake --build build
```

Make sure your build directory contains the `test/images/` folders:
```
+-- build
|   +-- CMakeFiles/
|   +-- Testing/
|   +-- APImageFilters
|   +-- test/
|   |   +-- images/
```

To run the pleriminary tests:
```bash
cd build
ctest
```

To run the general tests:
```bash
cd build
./RunAllTests
```


## Documentation
The documentation for this project was generated using [Doxygen](https://www.doxygen.nl/index.html).

To access the documentation:
```bash
open docs/html/index.html
```


## More Information

For more information on the project, see the [report](report.pdf) document.

For more information on analysis and timing test comparisons checks see the [Visual Tests Notebook](vis-test.ipynb).

For more details on the commands, see [command line options](command_line_options.md) file.

To experiment with some data, you can download CT Scan datasets [here](https://imperiallondon-my.sharepoint.com/:u:/g/personal/tmd02_ic_ac_uk/EafXMuNsbcNGnRpa8K62FjkBvIKvCswl1riz7hPDHpHdSQ).


## References

See the [REFERENCES](REFERENCES.md) file. 

## License

This project is licensed under the GNU General Public License v3.0. You can freely use, modify, and distribute the code, provided that any derivative works are also licensed under the same terms. See the [LICENSE](LICENSE) file for more details.
