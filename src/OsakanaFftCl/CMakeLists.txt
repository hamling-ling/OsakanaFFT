 cmake_minimum_required(VERSION 3.7)
project(osakana_cl_fft CXX)

find_package(OpenCL REQUIRED)

add_executable(clfft clfft.cpp)
target_link_libraries(clfft OpenCL::OpenCL)

configure_file(clfft.cl clfft.cl COPYONLY)
