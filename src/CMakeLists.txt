# add_subdirectory(utils)
add_subdirectory(model)

# will make a foobar.o file and pack it into a library so that
# when hello.h is included the foobar.o library is available.
add_library(prog Program.cpp Population.cpp)

add_executable(integenetics main.cpp) # builds an executable called ./integenetics from main.cpp

target_link_libraries(integenetics # allows integenetics to know about the libraries 
    prog
    # utils
    model
)

