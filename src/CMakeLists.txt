add_subdirectory(utils)

# will make a foobar.o file and pack it into a library so that
# when hello.h is included the foobar.o library is available.
add_library(foobar hello.cpp blah.cpp) 

add_executable(bin_main main.cpp) # builds an executable called ./bin_main from main.cpp

target_link_libraries(bin_main # allows bin_main to know about the libraries 
    utils   # utils
    foobar) # foobar

