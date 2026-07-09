cmake_minimum_required(VERSION 3.16)
project(NewStepMeasurementControl VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 QUIET COMPONENTS Widgets Charts)
if(Qt6_FOUND)
    set(QT_PACKAGE Qt6)
else()
    find_package(Qt5 REQUIRED COMPONENTS Widgets)
    set(QT_PACKAGE Qt5)
endif()

add_executable(newstep-mct
    src/main.cpp
    src/mainwindow.cpp
    src/mainwindow.h
    src/testengine.cpp
    src/testengine.h
)

target_link_libraries(newstep-mct PRIVATE ${QT_PACKAGE}::Widgets)

install(TARGETS newstep-mct
    RUNTIME DESTINATION bin
    BUNDLE DESTINATION .
)

set(CPACK_PACKAGE_NAME "NewStep-MCT")
set(CPACK_PACKAGE_VENDOR "NewStep")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Qt/C++ measurement and control test framework")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_GENERATOR "TGZ")
include(CPack)
