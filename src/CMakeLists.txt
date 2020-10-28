# add_subdirectory(utils)
add_subdirectory(model)

# will make a foobar.o file and pack it into a library so that
# when hello.h is included the foobar.o library is available.
add_library(prog 
    Program.cpp 
    Population.cpp 
    ConfigParser.cpp
)

# add_executable(${PROJECT_NAME} main.cpp) # builds an executable called ./${PROJECT_NAME} from main.cpp

# target_link_libraries(${PROJECT_NAME} # allows executable to know about the libraries 
    # prog
    # # utils
    # model
    # Eigen3::Eigen
    # ${Boost_LIBRARIES}
# )

