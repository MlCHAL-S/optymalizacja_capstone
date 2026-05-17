cmake_minimum_required(VERSION 3.20)
project(capstone_cplex)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# IBM CPLEX Studio installation folder
set(CPLEX_STUDIO_DIR "/opt/ibm/ILOG/CPLEX_Studio2212")

# Header folders
set(CPLEX_INCLUDE_DIR "${CPLEX_STUDIO_DIR}/cplex/include")
set(CONCERT_INCLUDE_DIR "${CPLEX_STUDIO_DIR}/concert/include")

# Library folders
set(CPLEX_LIB_DIR "${CPLEX_STUDIO_DIR}/cplex/lib/x86-64_linux/static_pic")
set(CONCERT_LIB_DIR "${CPLEX_STUDIO_DIR}/concert/lib/x86-64_linux/static_pic")

add_executable(capstone main.cpp)

target_include_directories(capstone PRIVATE
        "${CPLEX_INCLUDE_DIR}"
        "${CONCERT_INCLUDE_DIR}"
)

target_link_directories(capstone PRIVATE
        "${CPLEX_LIB_DIR}"
        "${CONCERT_LIB_DIR}"
)

# Required for using the standard C++ namespace with Concert
target_compile_definitions(capstone PRIVATE IL_STD)

target_link_libraries(capstone PRIVATE
        ilocplex
        cplex
        concert
        m
        pthread
        dl
)