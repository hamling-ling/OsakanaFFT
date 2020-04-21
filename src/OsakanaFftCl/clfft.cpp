#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <iostream>
#include <vector>

#define __CL_ENABLE_EXCEPTIONS
#include <CL/cl.hpp>

#include "util.hpp"
#include "err_code.h"
#include "device_picker.hpp"

//------------------------------------------------------------------------------
//  Constants
//------------------------------------------------------------------------------
#define SAMPLE_SIZE_N   16      // Sample Size
#define LOG2N           4       // log2(SAMPLE_SIZE_N)
#define TOL             (0.001) // Tolerance used in floating point comparisons

//------------------------------------------------------------------------------
//  functions
//------------------------------------------------------------------------------
extern double wtime();   // Returns time since some fixed past point (wtime.c)

using namespace std;

void reverse(unsigned int x, unsigned int width)
{
    cout << x;
    x = (x & 0x55555555) <<  1 | (x >>  1) & 0x55555555; 
    x = (x & 0x33333333) <<  2 | (x >>  2) & 0x33333333; 
    x = (x & 0x0F0F0F0F) <<  4 | (x >>  4) & 0x0F0F0F0F; 
    x = (x << 24) | ((x & 0xFF00) << 8) | ((x >> 8) & 0xFF00) | (x >> 24); 
    x = x >> (32-width);
    cout <<  " -> " << x << endl;
}

int main(int argc, char *argv[])
{
    int         N          = SAMPLE_SIZE_N;  // Real data sampling size
    int         log2N      = LOG2N;          // log2(N)
    double      start_time = 0.0;            // Starting time
    double      run_time   = 0.0;            // Timing data
    util::Timer timer;                       // timing

    std::vector<float> h_sample( N * 2, 0);   // N complex samples as an input
    std::vector<float> h_output( N * 2, 0);   // N complex samples as an output

	for ( int i = 0; i < h_sample.size(); i+=2) {
		//h_sample[i]   = (float)sin(3.5 * i * 2.0 * M_PI / N);
        //h_sample[i+1] = 0.0f;
        h_sample[i]   = (float)i/2;
        h_sample[i+1] = (float)(0);
	}

    for(int i = 0; i < N; i++) {
        reverse(i, log2N);
    }
    try
    {
        cl_uint deviceIndex = 0;

        // Get list of devices
        std::vector<cl::Device> devices;
        unsigned numDevices = getDeviceList( devices);

        // Check device index in range
        if ( deviceIndex >= numDevices)
        {
          std::cout << "Invalid device index (try '--list')\n";
          return EXIT_FAILURE;
        }

        cl::Device device = devices[deviceIndex];

        std::string name;
        getDeviceName( device, name);
        std::cout << "\nUsing OpenCL device: " << name << "\n";

        std::vector<cl::Device> chosen_device;
        chosen_device.push_back( device);

        cl::Context      context( chosen_device);
        cl::CommandQueue queue( context, device);

        // setup device global memory and write into global memory
        cl::Buffer d_sample = cl::Buffer( context, h_sample.begin(), h_sample.end(), true);
        cl::Buffer d_output = cl::Buffer( context, CL_MEM_WRITE_ONLY, sizeof(h_output[0]) * h_output.size());

        // Create the compute program from the source buffer
        cl::Program program = cl::Program(context, util::loadProgram( "clfft.cl"), true);
        // Create the compute kernel from the program
        cl::make_kernel<int, int, cl::Buffer, cl::Buffer, cl::LocalSpaceArg> clfft( program, "clfft");

        start_time = static_cast<double>(timer.getTimeMilliseconds()) / 1000.0;

        cl::NDRange global(N/2);
        cl::LocalSpaceArg localmem = cl::Local(sizeof(float) * N);
        clfft( cl::EnqueueArgs( queue, global),
               N/2, log2N, d_sample, d_output, localmem);

        queue.finish();

        run_time  = static_cast<double>( timer.getTimeMilliseconds()) / 1000.0 - start_time;

        cl::copy(queue, d_output, h_output.begin(), h_output.end());
        for( int i = 0; i < h_output.size(); i+=2) {
            cout << "[" << i/2 << "].re = " << h_output[i] << ", im = " << h_output[i+1] << endl;
        }

    } catch (cl::Error err) {
        std::cout << "Exception\n";
        std::cerr << "ERROR: "
                  << err.what()
                  << "("
                  << err_code(err.err())
                  << ")"
                  << std::endl;
    }

    return EXIT_SUCCESS;
}
