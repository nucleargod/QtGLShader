project(QtGLShader)
cmake_minimum_required(VERSION 3.1.3)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
#set(CMAKE_AUTOMOC ON)

# set target path
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
#list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

# set sub project names
set(UI_NAME "QtGL")

# Find library
find_package(Qt5Widgets)
find_package(Qt5OpenGL)
if(USE_CV)
  find_package(OpenCV)
  add_definitions("-DUSE_CV")
endif(USE_CV)
find_package(OpenGL REQUIRED)
find_package( OpenCL )
find_package( CUDA )

# get 3rd source and lib
if(USE_CV)
  list(APPEND Third_LIB_LIST ${OpenCV_LIBS})
  list(APPEND Third_DIR_LIST ${OpenCV_INCLUDE_DIRS})
endif(USE_CV)
if(${OpenCL_FOUND})
  list(APPEND Third_LIB_LIST ${OpenCL_LIBRARY})
  list(APPEND Third_DIR_LIST ${OpenCL_INCLUDE_DIR})
endif(${OpenCL_FOUND})
list(APPEND Third_LIB_LIST ${OPENGL_LIBRARIES})
list(APPEND Third_DIR_LIST ${OPENGL_INCLUDE_DIR})

# global ui src
aux_source_directory(${PROJECT_SOURCE_DIR}/${UI_NAME} UI_SRC_LIST)
set(UI_QUI  ${PROJECT_SOURCE_DIR}/${UI_NAME}/mainwindow.ui)
list(APPEND UI_QHDR ${PROJECT_SOURCE_DIR}/${UI_NAME}/mainwindow.h )
list(APPEND UI_QHDR ${PROJECT_SOURCE_DIR}/${UI_NAME}/GLWidget.h )
qt5_wrap_cpp(UI_qhdr_moc ${UI_QHDR})
qt5_wrap_ui (UI_qui_moc  ${UI_QUI})

# optional src
if(${OpenCL_FOUND})
  aux_source_directory(${PROJECT_SOURCE_DIR}/ocl CL_SRC_LIST)
  list(APPEND UI_SRC_LIST ${CL_SRC_LIST})
endif(${OpenCL_FOUND})

# include
include_directories(${PROJECT_SOURCE_DIR} ${Third_DIR_LIST})

# UI target
add_executable(${UI_NAME} ${UI_SRC_LIST} ${UI_qhdr_moc} ${UI_qui_moc})
qt5_use_modules(${UI_NAME} Widgets OpenGL)
target_link_libraries(${UI_NAME} ${Third_LIB_LIST} Qt5::Widgets Qt5::OpenGL)
