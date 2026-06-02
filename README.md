# Microfluidic Nucleation Software

[![CMake](https://img.shields.io/badge/CMake-3.15+-blue.svg)](https://cmake.org/)
[![C++](https://img.shields.io/badge/C%2B%2B-23-blue.svg)](https://en.wikipedia.org/wiki/C%2B%2B23)
[![OpenCV](https://img.shields.io/badge/OpenCV-4.x-blue.svg)](https://opencv.org/)

A comprehensive software suite for analyzing nucleation processes in microfluidic droplet systems. This project combines computer vision-based droplet detection with statistical analysis to investigate phenomena like the Mpemba effect (anomalous freezing behavior where hot water freezes faster than cold water).

## Features

- **Video-Based Droplet Analysis**: Automatic detection and tracking of microfluidic droplets using YOLO v5 neural networks
- **Real-Time Temperature Correlation**: Links droplet freezing events with precise temperature data
- **Nucleation Rate Calculation**: Computes nucleation rates from experimental data with binned analysis
- **Thermal Simulation**: Models thermal behavior in microfluidic systems
- **Statistical Analysis**: Includes Wilcoxon rank-sum tests to detect anomalous freezing effects
- **Interactive GUI**: Qt5-based application for experiment configuration and data visualization
- **Comprehensive Visualization**: Python scripts for plotting nucleation curves and statistical results

## Table of Contents

- [Getting Started](#getting-started)
  - [Prerequisites](#prerequisites)
  - [Installation](#installation)
- [Usage](#usage)
  - [GUI Application](#gui-application)
  - [Command-Line Tools](#command-line-tools)
  - [Analysis Scripts](#analysis-scripts)
- [Configuration](#configuration)
- [Project Structure](#project-structure)
- [Contributing](#contributing)
- [Support & Troubleshooting](#support--troubleshooting)

## Getting Started

### Prerequisites

**System Requirements:**
- Linux operating system (tested on Ubuntu 24.04)
- CMake >= 3.15
- C++23 compatible compiler (GCC 12+ or Clang 14+)

**Required Libraries:**
- OpenCV 4.x
- Qt5 (Core and Widgets)
- Intel TBB (Threading Building Blocks)
- GNU Scientific Library (GSL)
- ONNX Runtime

**Python (Optional, for analysis scripts):**
- Python 3.8+
- NumPy, SciPy, Matplotlib, scikit-learn

### Installation

#### Ubuntu/Debian

Install system dependencies:

```bash
sudo apt-get update
sudo apt-get install -y \
    build-essential cmake \
    libopencv-dev \
    qtbase5-dev \
    libtbb-dev \
    libgsl-dev \
```

#### Build from Source

```bash
# Clone repository
git clone https://github.com/yourusername/MicrofluidicNucleationSoftware.git
cd MicrofluidicNucleationSoftware

# Create and enter build directory
mkdir build && cd build

# Configure and build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build . -j$(nproc)
```

Executables will be in `build/apps/`:
- `MicrofluidicNucleationApp` - GUI application
- `MpembaPostEval` - Batch processing tool

#### Python Environment (For postprocessing of generated data)

```bash
cd scripts
python3 -m venv venv
source venv/bin/activate
pip install -r requirements.txt
```

## Usage

### GUI Application

Interactive interface for experiment setup and real-time analysis:

```bash
./build/apps/MicrofluidicNucleationApp
```

**Workflow:**
1. Load video file (MP4, AVI, etc.)
2. Configure experiment parameters:
   - Frame rate (fps)
   - Pixel-to-micrometer calibration
   - Temperature settings
3. Select YOLO detection model from `resources/`
4. Run analysis (with optional frame limiting)
5. Export results as CSV

**Key Settings:**
- `parallel`: Thread count (default: 50)
- `frame_stop`: Maximum frames to analyze (0 = all)
- `score_threshold`: Droplet detection confidence (0.0-1.0)

### Command-Line Tools

#### Batch Processing with MpembaPostEval

Process multiple experiments with nucleation rate analysis:

```bash
./build/apps/MpembaPostEval
```

Interactive prompts will guide you through:
- Loading analysis configuration
- Selecting experiment directory
- Specifying bin count for rate calculations

Outputs binned nucleation rates to `-bin-results.csv` files.

#### Configuration Files

Create analysis configs in TOML format (see `examples/analysis_config.cf`):

```toml
[config]
parallel = 50                 # Thread count
frame_stop = 0               # 0 = process all frames
score_threshold = 0.7        # YOLO confidence threshold
nms_threshold = 0.4          # Non-max suppression
confidence_threshold = 0.6   # Detection confidence
net_file = "resources/fixed_50_model.onnx"
calib = 1.0                  # Calibration factor (pixels/mm)
left_border_volume = 5       # Droplet centers left of this pixel value are disregarded for volume calculation
right_border_volume = 10000  # Droplet centers right of this pixel value are disregarded for volume calculation
threshold_count = 20         # Currently unused
```

### Analysis Scripts

Python utilities for post-processing and visualization (in `scripts/`):

```bash
cd scripts && source venv/bin/activate

# Run a statistical test, to check for an Mpemba Effect
python3 detectMpembaEffect.py

# Plot nucleation rate curves
python3 plotNucleationCurve.py

# Visualize nucleation temperature dependency
python3 plotTempRampCurve.py

# Analyze isokinetic curves
python3 plotIsokineticCurves.py
```

**Expected data format:**
- Overview CSV with file paths and temperatures
- Results CSV from MpembaPostEval
- Experiment configuration files

## Configuration

### Experiment Parameters

Define experiments by specifying:
- Video recording path
- Frame rate (fps)
- Pixel calibration factor
- Reference temperatures

## Project Structure

```
MicrofluidicNucleationSoftware/
├── src/                          # Core implementations
│   ├── VideoAnalyzer.cpp         # Frame processing & tracking
│   ├── YoloV5.cpp                # Neural network inference
│   ├── NucleationCalculator.cpp  # Rate calculations
│   ├── ThermalSimulation.cpp     # Thermal modeling
│   └── ...
├── include/                      # Header files
│   ├── MicrofluidicNucleation/  # Core library
│   ├── MicrofluidicNucleationGui/ # GUI components
│   └── MicroCalibrator/          # Calibration tools
├── apps/                         # Executables
│   ├── MpembaGUI.cpp             # Main GUI application
│   └── MpembaPostEval.cpp        # Nucleation Binning Calculator
├── scripts/                      # Python analysis
│   ├── analysis/                 # Analysis modules
│   ├── plotting/                 # Visualization
│   ├── config/                   # Constants
│   └── *.py                      # Individual scripts
├── examples/                     # Sample configs
└── resources/                    # YOLO ONNX models
```

### Core Dependencies

- **OpenCV**: Video I/O and image processing
- **ONNX Runtime**: Neural network inference
- **Intel TBB**: Multi-threaded frame processing
- **Qt5**: GUI framework
- **GSL**: Mathematical functions
- **[spdlog](https://github.com/gabime/spdlog)**: Logging
- **[toml++](https://github.com/marzer/tomlplusplus)**: Configuration parsing

**Version**: 0.1 | **Last Updated**: June 2025
