set(HEADER_LIST
        "${PROJECT_SOURCE_DIR}/include/MicrofluidicNucleation/Vector2D.h"
        "${PROJECT_SOURCE_DIR}/include/MicrofluidicNucleation/Detection.h"
        "${PROJECT_SOURCE_DIR}/include/MicrofluidicNucleation/Droplet.h"
        "${PROJECT_SOURCE_DIR}/include/MicrofluidicNucleation/Frame.h"
        "${PROJECT_SOURCE_DIR}/include/MicrofluidicNucleation/VideoAnalyzer.h"
        "${PROJECT_SOURCE_DIR}/include/MicrofluidicNucleation/Experiment.h"
        "${PROJECT_SOURCE_DIR}/include/MicrofluidicNucleation/AnalysisConfig.h"
        "${PROJECT_SOURCE_DIR}/include/spdlog/spdlog.h"
        "${PROJECT_SOURCE_DIR}/include/MicrofluidicNucleation/YoloV5.h"
        "${PROJECT_SOURCE_DIR}/include/MicrofluidicNucleation/ResultsWriter.h"
        "${PROJECT_SOURCE_DIR}/include/MicrofluidicNucleation/TemperatureReader.h"
)

set(SOURCE_LIST
        Vector2D.cpp
        Detection.cpp
        Droplet.cpp
        Frame.cpp
        VideoAnalyzer.cpp
        Experiment.cpp
        YoloV5.cpp
        ResultsWriter.cpp
        TemperatureReadder.cpp
)
add_library(microfluidic_nucleation ${SOURCE_LIST} ${HEADER_LIST})

target_include_directories(microfluidic_nucleation PUBLIC ../include)

find_package(OpenCV REQUIRED)
find_package(TBB REQUIRED)
target_link_libraries(microfluidic_nucleation ${OpenCV_LIBS} TBB::tbb)
set($ENV{OPENCV_OCL4DNN_CONFIG_PATH} "")
target_compile_features(microfluidic_nucleation PUBLIC cxx_std_23)

source_group(
        TREE "${PROJECT_SOURCE_DIR}/include"
        PREFIX "Header Files"
        FILES ${HEADER_LIST}
)