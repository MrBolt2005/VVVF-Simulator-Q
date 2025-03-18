cmake_minimum_required(VERSION 3.16)

# Set the project name and version
project(VvvfSimulator VERSION 1.0 LANGUAGES CXX)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable AUTOMOC
set(CMAKE_AUTOMOC ON)

# Set CMAKE_PREFIX_PATH to include the Qt6 installation directory
set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} "D:/Qt/6.8.1/msvc2022_64" "C:/dev/vcpkg/installed/x64-windows")

# Set the CMAKE_TOOLCHAIN_FILE to use vcpkg
set(CMAKE_TOOLCHAIN_FILE "C:/dev/vcpkg/scripts/buildsystems/vcpkg.cmake")

# Add the /Zc:__cplusplus flag for MSVC
if (MSVC)
    add_compile_options(/Zc:__cplusplus)
endif()

# Find the required Qt packages
find_package(Qt6 REQUIRED COMPONENTS Core Quick QuickControls2)

# Ensure QML import paths are set up properly
set(QT_QML_GENERATE_QMLLS_INI ON)

# Add the executable
add_executable(VvvfSimulator
    src/main.cpp
    src/VvvfSimulator/GUI/MainWindow.qml
)

# Link the Qt libraries
target_link_libraries(VvvfSimulator PRIVATE Qt6::Core Qt6::Quick Qt6::QuickControls2)

# Add QML import paths
qt6_add_qml_module(VvvfSimulator
    URI VvvfSimulator
    VERSION 1.0
    QML_FILES
        src/VvvfSimulator/GUI/MainWindow.qml
)

# Add resources
qt6_add_resources(VvvfSimulator "resources"
    PREFIX "/"
    FILES
        src/VvvfSimulator/GUI/MainWindow.qml
)

# Find and link additional libraries
# avcpp
find_package(avcpp REQUIRED)
target_link_libraries(VvvfSimulator PRIVATE avcpp::avcpp)

# FFmpeg
find_package(FFmpeg REQUIRED)
target_link_libraries(VvvfSimulator PRIVATE FFmpeg::FFmpeg)

# libremidi
find_package(libremidi REQUIRED)
target_link_libraries(VvvfSimulator PRIVATE libremidi)

# reflect-cpp
find_package(reflectcpp REQUIRED)
target_link_libraries(VvvfSimulator PRIVATE reflectcpp::reflectcpp)

# yaml-cpp
find_package(yaml-cpp REQUIRED)
target_link_libraries(VvvfSimulator PRIVATE yaml-cpp::yaml-cpp)