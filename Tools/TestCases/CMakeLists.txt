if(NOT ("cxx_std_17" IN_LIST CMAKE_CXX_COMPILE_FEATURES))
    message(WARNING test-cases requires C++17 support)
    return()
endif()

add_library(test-cases STATIC
    FileContents.h
    FileContents.cpp
    Main.h
    Main.cpp
    TestCases.h
    TestCases.cpp
    ToolResults.h
    ToolResults.cpp
)
set_target_properties(test-cases PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    FOLDER Tools/Libs
)
target_include_directories(test-cases PUBLIC .)
