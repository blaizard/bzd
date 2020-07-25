To generate the sdk image from esp-idf:

1. Copy the sdkconfig file into the example project: examples/get-started/hello_world
2. Replace the source file of the project with generate/main.cpp (and adjust the Makefile)
3. Run the make command `make VERBOSE=1`
4. Copy one compilation and the last linker command to the generate.py script
5. Run it
