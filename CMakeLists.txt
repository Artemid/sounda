project( audio )
cmake_minimum_required( VERSION 2.8 )

# remove compiler checks if you are using VS2010, it supports
#   1. auto
#   2. lambda
#   3. const iterator
# and, sadly, it doesn't support `for(item, cotainer)'
include(CheckCXXCompilerFlag)
check_cxx_compiler_flag("-std=c++14"   COMPILER_SUPPORTS_CXX14)
check_cxx_compiler_flag("-std=c++11"   COMPILER_SUPPORTS_CXX11)
if(COMPILER_SUPPORTS_CXX14)
    if(CMAKE_COMPILER_IS_GNUCXX)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++14")
    else()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
    endif()
elseif(COMPILER_SUPPORTS_CXX11)
    if(CMAKE_COMPILER_IS_GNUCXX)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")
    else()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
    endif()
else()
    # MSVC, On by default (if available)
endif()

set(INC_ROOT ${PROJECT_SOURCE_DIR}/inc)
set(INC_THIRDPARTY ${PROJECT_SOURCE_DIR}/thirdparty)

set(INC_DIRECTORIES
    ${INC_ROOT}
    ${INC_THIRDPARTY}/FFT
)

set(SRC_ROOT ${PROJECT_SOURCE_DIR}/src)
set(SRC_THIRDPARTY ${PROJECT_SOURCE_DIR}/thirdparty)

set(SRC_FILES
    ${SRC_ROOT}/main.cpp
    ${SRC_THIRDPARTY}/FFT/fft.cpp
)

set(INC_FILES
    ${SRC_THIRDPARTY}/FFT/fft.h
)

source_group("Core" FILES ${SRC_FILES})

# target_include_directorie(${PROJECT_NAME} ${INC_ROOT})
include_directories(${PROJECT_NAME} ${INC_DIRECTORIES})

add_executable(${PROJECT_NAME} ${SRC_FILES} ${INC_FILES})

target_link_libraries(${PROJECT_NAME} "-lrt -lm -lasound -pthread -lsfml-system -lsfml-window -lsfml-graphics -lsfml-audio")
