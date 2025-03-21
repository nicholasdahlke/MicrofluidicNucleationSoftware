set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(MFN_HEADER_LIST
        "${PROJECT_SOURCE_DIR}/include/MicrofluidicNucleation/Vector2D.h"
        "${PROJECT_SOURCE_DIR}/include/MicrofluidicNucleation/Detection.h"
        "${PROJECT_SOURCE_DIR}/include/MicrofluidicNucleation/Droplet.h"
        "${PROJECT_SOURCE_DIR}/include/MicrofluidicNucleation/Frame.h"
        "${PROJECT_SOURCE_DIR}/include/MicrofluidicNucleation/VideoAnalyzer.h"
        "${PROJECT_SOURCE_DIR}/include/MicrofluidicNucleation/Experiment.h"
        "${PROJECT_SOURCE_DIR}/include/MicrofluidicNucleation/AnalysisConfig.h"
        "${PROJECT_SOURCE_DIR}/include/MicrofluidicNucleation/YoloV5.h"
        "${PROJECT_SOURCE_DIR}/include/MicrofluidicNucleation/ResultsWriter.h"
        "${PROJECT_SOURCE_DIR}/include/MicrofluidicNucleation/TemperatureReader.h"
        "${PROJECT_SOURCE_DIR}/include/MicrofluidicNucleation/DropletResult.h"
        "${PROJECT_SOURCE_DIR}/include/MicrofluidicNucleation/CsvWriter.h"
        "${PROJECT_SOURCE_DIR}/include/MicrofluidicNucleation/NucleationCalculator.h"
        "${PROJECT_SOURCE_DIR}/include/MicrofluidicNucleation/ThermalSimulation.h"
        "${PROJECT_SOURCE_DIR}/include/spdlog/spdlog.h"
        "${PROJECT_SOURCE_DIR}/include/toml++/toml.hpp"
)
set(MFN_SOURCE_LIST
        Vector2D.cpp
        Detection.cpp
        Droplet.cpp
        Frame.cpp
        VideoAnalyzer.cpp
        Experiment.cpp
        YoloV5.cpp
        ResultsWriter.cpp
        TemperatureReader.cpp
        DropletResult.cpp
        AnalysisConfig.cpp
        NucleationCalculator.cpp
        ThermalSimulation.cpp
)


set(MFNGUI_HEADER_LIST
        "${PROJECT_SOURCE_DIR}/include/MicrofluidicNucleationGui/MicrofluidicNucleationGui.h"
        "${PROJECT_SOURCE_DIR}/include/MicrofluidicNucleationGui/MainWindow.h"
        "${PROJECT_SOURCE_DIR}/include/spdlog/spdlog.h"

)
SET(MFNGUI_SOURCE_LIST
        MicrofluidicNucleationGui.cpp
        MainWindow.cpp
)


find_package(OpenCV REQUIRED)
find_package(TBB REQUIRED)
find_package(Qt5 REQUIRED COMPONENTS Widgets)
find_package(GSL REQUIRED)

add_library(microfluidic_nucleation ${MFN_SOURCE_LIST} ${MFN_HEADER_LIST})
target_include_directories(microfluidic_nucleation PUBLIC ../include)
target_link_libraries(microfluidic_nucleation ${OpenCV_LIBS} TBB::tbb GSL::gsl)
set($ENV{OPENCV_OCL4DNN_CONFIG_PATH} "")
target_compile_features(microfluidic_nucleation PUBLIC cxx_std_23)

add_library(microcalibrator
        MicroCalibrator.cpp
        "${PROJECT_SOURCE_DIR}/include/MicroCalibrator/MicroCalibrator.h"
        "${PROJECT_SOURCE_DIR}/include/spdlog/spdlog.h"
)
target_include_directories(microcalibrator PUBLIC ../include)
target_link_libraries(microcalibrator ${OpenCV_LIBS} Qt5::Widgets)
target_compile_features(microcalibrator PUBLIC cxx_std_23)


add_library(microfluidic_nucleation_gui ${MFNGUI_SOURCE_LIST} ${MFNGUI_HEADER_LIST})
target_include_directories(microfluidic_nucleation_gui PUBLIC ../include)
target_link_libraries(microfluidic_nucleation_gui Qt5::Widgets microfluidic_nucleation microcalibrator)
target_compile_features(microfluidic_nucleation_gui PUBLIC cxx_std_23)

source_group(
        TREE "${PROJECT_SOURCE_DIR}/include"
        PREFIX "Header Files"
        FILES ${HEADER_LIST}
)