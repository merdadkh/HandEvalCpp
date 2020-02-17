cmake_minimum_required (VERSION 3.12)

# The project name
project(PokerAI VERSION 0.1)

find_package(pybind11 REQUIRED)

set(ProbEvalSrcs 
        Hand_Tables.cpp
        HandEval.cpp
        HandIter.cpp
        ProbEval.cpp    
    )
    
pybind11_add_module(PokerProbEval pybinding.cpp ${ProbEvalSrcs})


add_executable( HandEval
                Driver.cpp
                TestCases.cpp
                ${ProbEvalSrcs}
                )



target_include_directories(HandEval PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
