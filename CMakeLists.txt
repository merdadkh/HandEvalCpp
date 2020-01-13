cmake_minimum_required (VERSION 3.12)

# The project name
project(PokerAI VERSION 0.1)

# set(CMAKE_CXX_COMPILER ${CMAKE_CXX_COMPILER} CACHE FILEPATH "Compiler")
# set(CMAKE_C_COMPILER ${CMAKE_C_COMPILER} CACHE FILEPATH "Compiler")
# set(CMAKE_CXX_STANDARD 11)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated")

# add_subdirectory(Simulator)
##########################################################
# GRM
add_executable( HandEval
                Driver.cpp
                Hand_Tables.cpp
                HandEval.cpp
                HandIter.cpp
                TestCases.cpp
                ProbEval.cpp
                )
target_include_directories(HandEval PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
