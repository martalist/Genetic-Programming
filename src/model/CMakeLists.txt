add_library(model 
    INode.h
    Variable.cpp
    Function.cpp
    FunctionFactory.cpp
    Operators.cpp
)
set_target_properties(model PROPERTIES LINKER_LANGUAGE CXX)
