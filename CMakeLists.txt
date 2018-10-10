cmake_minimum_required(VERSION 3.12)
project(TensorModel)

set(CMAKE_CXX_STANDARD 14)

add_executable(test
        test/test.cpp
        src/TensorModel.h
        src/HdfWriter.h
        src/TmIndexed.h
        src/TmNamed.h
        src/TmBounds.h
        src/TmObjective.h
        src/TmSet.h
        src/TmVariable.h
        src/TmVector.h
        src/TensorModel.cpp
        src/TmSet.cpp
        src/TmPackedVector.h
        src/TmPackedMatrix.h)

# HDF5 package
find_package(HDF5 REQUIRED COMPONENTS CXX)
INCLUDE_DIRECTORIES(${HDF5_INCLUDE_DIR})

# Boost
find_package(Boost COMPONENTS system filesystem date_time)

# Include H5 libraries to build the flow target (the only one we're building right now)
target_link_libraries(test ${HDF5_CXX_LIBRARIES} ${Boost_LIBRARIES})

# RapidJSON
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/../rapidjson/include)

# COIN-OR Clp
target_link_libraries(test Clp CoinUtils Osi OsiClp)

