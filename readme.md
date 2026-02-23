# Dependencies
## CMake 
CMake 3.31.0+
### Download
https://cmake.org/download/
### Via APT (Ubuntu)
https://askubuntu.com/questions/355565/how-do-i-install-the-latest-version-of-cmake-from-the-command-line 

## Boost 
Boost 1.74+ (Header-Only Libraries)
### Python
Python 3.12
### Unix
https://www.boost.org/doc/libs/1_86_0/more/getting_started/unix-variants.html
### Windows
Note - Do not install, just for information.
https://www.boost.org/doc/libs/1_86_0/more/getting_started/windows.html
### Windows - vcpkg
At SCT we find it easier to use vcpkg to manage our boost installation. If you install vcpkg, boost will automatically be installed by vcpkg during the cmake configuration process described later on in this document.
The vcpkg.json file for this project has already been created. To use vcpkg just: install vcpkg, run the boostrap script, and add VCPKG_ROOT to your SYSTEM Variables - PATH. 
See all these steps and more here: https://learn.microsoft.com/en-us/vcpkg/get_started/get-started?pivots=shell-powershell
When building with vcpkg, it is recommended to also install "Build Tools for Visual Studio 2022 or 2026", found at https://visualstudio.microsoft.com/downloads/.  This allows for selection of the appropriate "Visual Studio xx 202x" generator

# CMake Information
## Select Generator
CMake will automatically search your system for available generators. To see what cmake has selected as the default generator, issue the cmake command with the --help flag and look for the generator tagged with '*'.
To override the default generator during cmake configuration use the cmake flag -G, or alternatively, you can use/modify one of the CMakeUserPresets, which allow for the generator to be specified explicitly.
Generally Unix systems will have a suitable generator and compiler installed, for Windows, installing the newest Visual Studio build tools will include both: https://visualstudio.microsoft.com/downloads/
## CMake Presets
### User Presets
We have defined quite a few cmake presets, however you may make your own by creating and populating a user presets file (CMakeUserPresets.json).
In your user presets file you can use the "inherit" field to copy settings from the existing presets.
For more preset details see: https://cmake.org/cmake/help/latest/manual/cmake-presets.7.html
### Configuration Presets
To list available configuration presets, from the project directory use the cmake flag --list-presets: cmake --list-presets
- vcpkg standard (release/debug) - Windows only, requires vcpkg. Shared build output. Target architecture will be x64.
- vcpkg static (release/debug) - Windows only, requires vcpkg. Static build output. Target architecture will be x64.
- vcpkg standard 32-bit (release/debug) - Windows only, requires vcpkg. Shared build output. Target architecture will be x86.
- vcpkg python (release/debug) - Windows only, requires vcpkg. Shared build output, will include python compatible output. Target architecture will be x64.
- standard (release/debug) - Shared build output. Target system architecture will match your system.
- static (release/debug) - Static build output. Target system architecture will match your system.
- python (release/debug) - Shared build output, will include python compatible output. Target system architecture will match your system.
### Build Presets
For each of the configuration presets there is a matching build preset. Please use matching configuration and build presets.
To list available build presets, from the project directory use the cmake flags --build and --list-presets: cmake --build --list-presets

# Build Steps
Run all steps from the project root, where the CMakePresets.json file lives.
## 1. CMake Configuration
Select a configuration preset and run the cmake configuration. 
Notes:
- If vcpkg is installing boost for the first time, it will take quite a while.
- Override the generator with -G or with a custom user preset
- If switching between certain presets, --fresh may be required
### Command
cmake --preset SELECTED_PRESET_HERE
### Example
cmake --preset vcpkg-standard-release
## 2. Cmake Build
Select the matching build preset to the configuration preset used above.
### Command
cmake --build ./build --preset SELECTED_PRESET_HERE
### Example
cmake --build ./build --preset vcpkg-release

## 3. Python Virtual Environments
After executing a python build, a wheel will be placed in the Output-py/Debug or Output-py/Release subfolder.
Python activation scripts (.bat, .ps1, .sh) are provided to create a VENV virtual environment that imports the supplied wheel.

# Examples
Example programs are provided for each C, C++ and Python language, based on the source code in the examples folder.
C and C++ executables can be executed directly, and the python example can be executed using the VENV python virtual environment described above.
s
# Output
Outputs can be found in the build folder under Output, Output-py or Output-static depending on the build type.

# Licenses
See LICENSE.txt for this project's license





This project includes third‑party components under separate licenses.
See the LICENSES/third-party directory for details.



