cmake_minimum_required(VERSION 3.12.3)

project(WebScanner)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(project_ui
    mainwindow.ui
    listitem.ui)

set(project_headers
    mainwindow.h
    WebScannerHandler.h
    listitem.h
    ListWidget.h
    AsyncProcessor.h
    URLStatus.h
    ScanningConfiguration.h
    ButtonsState.h)

set(project_sources
    main.cpp
    mainwindow.cpp
    WebScannerHandler.cpp
    listitem.cpp
    ListWidget.cpp
    AsyncProcessor.cpp)

find_package(Qt5 REQUIRED COMPONENTS Widgets Core GUI Network)

qt5_wrap_ui(project_headers_wrapped ${project_ui})
qt5_wrap_cpp(project_sources_moc ${project_headers})

add_executable(${PROJECT_NAME} ${project_headers} ${project_sources} ${project_sources_moc} ${project_headers_wrapped})

target_link_libraries(${PROJECT_NAME}
PRIVATE
Qt5::Core
Qt5::Widgets
Qt5::Gui
Qt5::Network)
