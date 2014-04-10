project(vtkViewer)
cmake_minimum_required(VERSION 2.8)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

set(MOC mainwindow.h )
set(UI mainwindow.ui)

find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

find_package(Qt4 REQUIRED)
include(${QT_USE_FILE})

qt4_wrap_cpp(MOC_GENERATED ${MOC})
qt4_wrap_ui(UI_GENERATED ${UI})

source_group("Form Files" FILES ${UI} )
source_group("Generated Files" FILES ${MOC_GENERATED}  ${UI_GENERATED})

aux_source_directory(. SRC_LIST)


add_executable(${PROJECT_NAME}  ${SRC_LIST}  ${UI_GENERATED} )

target_link_libraries(${PROJECT_NAME} ${QT_LIBRARIES} QVTK vtkRendering ${VTK_LIBRARIES} )
