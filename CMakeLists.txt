cmake_minimum_required(VERSION 2.8.11)
project(SimRt)

# enable c++17 features
set(CMAKE_CXX_STANDARD 17)

# set paths
set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)
set(EXTERNAL_DIR ${CMAKE_SOURCE_DIR}/extern)

#add openmp support if found
find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

# collect source files
file(GLOB_RECURSE SRC_FILES src/*.cpp src/*.h)

# stb
file(GLOB STB_INCLUDE ${EXTERNAL_DIR}/stb/*.h)

# include directories
include_directories(
    ${SRC_DIR}
    ${EXTERNAL_DIR}/stb
)

# keep folder structure in visual studio
function(assign_source_group)
    foreach(_source IN ITEMS ${ARGN})
        if (IS_ABSOLUTE "${_source}")
            file(RELATIVE_PATH _source_rel "${CMAKE_CURRENT_SOURCE_DIR}" "${_source}")
        else()
            set(_source_rel "${_source}")
        endif()
        get_filename_component(_source_path "${_source_rel}" PATH)
        string(REPLACE "/" "\\" _source_path_msvc "${_source_path}")
        source_group("${_source_path_msvc}" FILES "${_source}")
    endforeach()
endfunction(assign_source_group)
if(MSVC)
assign_source_group(${SRC_FILES})
endif(MSVC)

add_executable(Raytracing ${SRC_FILES} ${STB_INCLUDE})