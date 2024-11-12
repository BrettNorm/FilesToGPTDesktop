cmake_minimum_required(VERSION 3.16)

project(FilesToGPTDesktop LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find the Qt libraries
set(CMAKE_PREFIX_PATH "/Users/brettbax/Qt/6.8.0/macos/lib/cmake")  # Ensure this path is correct
find_package(Qt6 COMPONENTS Widgets REQUIRED)

# Set up the project with standard settings (automoc, autouic, etc.)
set(CMAKE_AUTOMOC ON)

# Add the source files
add_executable(FilesToGPTDesktop
    main.cpp
    mainwindow.cpp
    fileitemwidget.cpp
)

# Link the Qt libraries to your executable
target_link_libraries(FilesToGPTDesktop PRIVATE Qt6::Widgets)
