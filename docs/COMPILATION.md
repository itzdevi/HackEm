# Compilation Guide

This project uses CMake as its build system. Follow the instructions below to configure and compile the project.

### Prerequisites

Make sure the following tools are installed on your system:

-   **CMake** (version ≥ `3.20` recommended)

-   **C++ Compiler**:

    -   Linux/macOS: `g++` or `clang++`

    -   Windows: Visual Studio (MSVC)

-   **Make** (or Ninja) on Linux/macOS if building from the command line

-   Git (optional, if cloning the repository)

#

### Step 1: Clone the repository

```bash
git clone https://github.com/itzdevi/HackEm.git
```

#

### Step 2: Create a build directory

```bash
mkdir build
cd build
```

#

### Step 3: Configure the project

Run CMake to configure the build system:

```bash
cmake ..
```

Optional configuration options:

-   Debug build (with symbols and no optimization):

```bash
cmake -DCMAKE_BUILD_TYPE=Debug ..
```

-   Release build (optimized for performance):

```bash
cmake -DCMAKE_BUILD_TYPE=Release ..
```

-   Specify a custom install path:

```bash
cmake -DCMAKE_INSTALL_PREFIX=/custom/path ..
```

#

### Step 4: Build the project

Once configured, compile the project:

```bash
cmake --build . --config Release
```

Or for multi-core builds:

```bash
cmake --build . --config Release -- -j$(nproc)
```

_(Replace Release with Debug if building a debug version.)_

#

### Step 5: Install the project (optional)

```bash
cmake --install .
```

This will copy binaries, headers, and other resources to the install directory (default: /usr/local on Linux/macOS).

#

### Troubleshooting

-   CMake not found: Ensure CMake is installed and in your PATH.
