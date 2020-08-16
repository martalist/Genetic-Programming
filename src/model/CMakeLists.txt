add_library(model 
    INode.h
    Function.cpp
    FunctionFactory.cpp)
set_target_properties(model PROPERTIES LINKER_LANGUAGE CXX)
