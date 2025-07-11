cmake_minimum_required(VERSION 3.10)
project(BudgieCProject C)

# Use C99
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Set default build type to Debug if unspecified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING
        "Choose the type of build (Debug, Release, Testing)" FORCE)
endif()

# === Define custom Testing build flags ===
set(CMAKE_C_FLAGS_TESTING "-O1 -g -DTESTING" CACHE STRING "Flags for Testing build")
set(CMAKE_EXE_LINKER_FLAGS_TESTING "" CACHE STRING "Linker flags for Testing")

# Optional float support
option(USE_FLOAT "Use float instead of double for buReal" ON)
if(USE_FLOAT)
    add_compile_definitions(USE_FLOAT)
    message(STATUS "Using float precision")
else()
    message(STATUS "Using double precision")
endif()

# === Source folders ===
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(TEST_DIR ${CMAKE_CURRENT_SOURCE_DIR}/tests)
set(DEMO_DIR ${SRC_DIR}/demos)

# === Main app ===
set(SOURCES
    ${SRC_DIR}/main.c
    ${SRC_DIR}/core.c
    ${SRC_DIR}/particle.c
)
add_executable(budgie ${SOURCES})
target_include_directories(budgie PRIVATE ${SRC_DIR})
target_link_libraries(budgie m)

# === Core test runner ===
add_executable(run_tests_core
    ${TEST_DIR}/test_core.c
    ${TEST_DIR}/unity/src/unity.c
    ${SRC_DIR}/core.c
)
target_include_directories(run_tests_core PRIVATE ${SRC_DIR} ${TEST_DIR}/unity/src)
target_link_libraries(run_tests_core m)
add_test(NAME BudgieCoreTests COMMAND run_tests_core)

# === Particle test runner ===
add_executable(run_tests_particle
    ${TEST_DIR}/test_particle.c
    ${TEST_DIR}/unity/src/unity.c
    ${SRC_DIR}/core.c
    ${SRC_DIR}/cparticle.c
)
target_include_directories(run_tests_particle PRIVATE ${SRC_DIR} ${TEST_DIR}/unity/src)
target_link_libraries(run_tests_particle m)
add_test(NAME BudgieParticleTests COMMAND run_tests_particle)

# === Define ballistic demo target ===
set(DEMO_DIR ${SRC_DIR}/demos)
set(BALLISTIC_DIR ${SRC_DIR}/demos/ballistic)

file(GLOB BALLISTIC_DEMO_SOURCES
    ${DEMO_DIR}/*.c
    ${BALLISTIC_DIR}/*.c
)

# Only include specific core sources (not main.c)
set(CORE_SOURCES
    ${SRC_DIR}/core.c
    ${SRC_DIR}/particle.c
    ${SRC_DIR}/cparticle.c
)

add_executable(demo_ballistic ${BALLISTIC_DEMO_SOURCES} ${CORE_SOURCES})

# Raylib + OpenMP linkage
find_package(raylib REQUIRED)
target_link_libraries(demo_ballistic raylib m)

find_package(OpenMP)
if(OpenMP_C_FOUND)
    target_link_libraries(demo_ballistic OpenMP::OpenMP_C)
    message(STATUS "OpenMP found and linked for demo_ballistic")
else()
    message(WARNING "OpenMP not found; demo_ballistic may fail if OpenMP is used")
endif()



# === Define fireworks demo target ===
set(DEMO_DIR ${SRC_DIR}/demos)
set(FIREWORKS_DIR ${SRC_DIR}/demos/fireworks)

file(GLOB FIREWORKS_DEMO_SOURCES
    ${DEMO_DIR}/*.c
    ${FIREWORKS_DIR}/*.c
)

# Only include specific core sources (not main.c)
set(CORE_SOURCES
    ${SRC_DIR}/core.c
    ${SRC_DIR}/particle.c
    ${SRC_DIR}/random.c
)

add_executable(demo_fireworks ${FIREWORKS_DEMO_SOURCES} ${CORE_SOURCES})

# Raylib + OpenMP linkage
find_package(raylib REQUIRED)
target_link_libraries(demo_fireworks raylib m)

find_package(OpenMP)
if(OpenMP_C_FOUND)
    target_link_libraries(demo_fireworks OpenMP::OpenMP_C)
    message(STATUS "OpenMP found and linked for demo_fireworks")
else()
    message(WARNING "OpenMP not found; demo_fireworks may fail if OpenMP is used")
endif()




# === Enable testing framework ===
enable_testing()

# === Show helpful usage when `make` is run with no target ===
add_custom_target(usage
    COMMAND ${CMAKE_COMMAND} -E echo ""
    COMMAND ${CMAKE_COMMAND} -E echo "Available build targets:"
    COMMAND ${CMAKE_COMMAND} -E echo "  make budgie               # Build the main application"
    COMMAND ${CMAKE_COMMAND} -E echo "  make run_tests_core       # Build core unit tests"
    COMMAND ${CMAKE_COMMAND} -E echo "  make run_tests_particle   # Build particle unit tests"
    COMMAND ${CMAKE_COMMAND} -E echo "  make demo_ballistic          # Build Raylib demo"
    COMMAND ${CMAKE_COMMAND} -E echo "  ctest                     # Run all tests using CTest"
    COMMAND ${CMAKE_COMMAND} -E echo ""
    VERBATIM
)
