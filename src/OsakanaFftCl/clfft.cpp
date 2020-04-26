#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <iostream>
#include <vector>

#define __CL_ENABLE_EXCEPTIONS
#if defined(__APPLE__)
// place cl.hpp to local directory for Mac OS
#include "cl.hpp"
#else
#include <CL/cl.hpp>
#endif

#include "util.hpp"
#include "err_code.h"
#include "device_picker.hpp"

//------------------------------------------------------------------------------
//  Constants
//------------------------------------------------------------------------------
#if defined(__APPLE__)
// my personal debug setting. need to change later
#define CL_PATH         "/Users/nobu/Documents/OpenClFftDebug/OpenClFftDebug/clfft.cl"
#else
#define CL_PATH         "clfft.cl"
#endif
#define SAMPLE_SIZE_N   8       // Sample Size
#define LOG2N           3       // log2(SAMPLE_SIZE_N)
#define WORK_GROUP_SIZE 4       // Workgroup size
#define TOL             (0.001) // Tolerance used in floating point comparisons

//------------------------------------------------------------------------------
//  functions
//------------------------------------------------------------------------------
extern double wtime();   // Returns time since some fixed past point (wtime.c)

using namespace std;

int main(int argc, char *argv[])
{
    int         N          = SAMPLE_SIZE_N;  // Real data sampling size
    int         log2N      = LOG2N;          // log2(N)
    double      start_time = 0.0;            // Starting time
    double      run_time   = 0.0;            // Timing data
    util::Timer timer;                       // timing

    std::vector<float> h_sample( N , 0);   // N complex samples as an input

    for ( int i = 0; i < h_sample.size(); i++) {
        h_sample[i]   = (float)sin(3.5 * i * 2.0 * M_PI / N);
        cout << "orig[" << i << "].re = " << h_sample[i] << endl;
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

        // Create the compute program from the source buffer
        cl::Program program = cl::Program(context, util::loadProgram(CL_PATH), true);
        // Create the compute kernel from the program for bit-reverse ordering
        cl::NDRange global(N);
        // order bit reversealy
        std::vector<float> h_x( N * 2, 0);   // N complex samples as an output
        cl::Buffer         d_x = cl::Buffer(context,
                                            CL_MEM_READ_WRITE,
                                            sizeof(h_x[0]) * h_x.size()
                                            );
        cl::make_kernel<int, cl::Buffer, cl::Buffer> bitrevese( program, "bitrevese");
        // compute bit-reverse ordering
        bitrevese(cl::EnqueueArgs( queue, global),
                  log2N,
                  d_sample,
                  d_x
                  );

        // debug
        cl::copy(queue, d_x, h_x.begin(), h_x.end());
        cout << "bit reversed --" << endl;
        for( int i = 0; i < h_x.size(); i+=2) {
            cout << "[" << i/2 << "].re = " << h_x[i] << ", im = " << h_x[i+1] << endl;
        }

        // Create the compute kernel from the program for fft
        cl::make_kernel<int, int, int, int, cl::Buffer, cl::LocalSpaceArg> clfft_multi( program, "clfft_multi_stages");
        cl::make_kernel<int, int, int, cl::Buffer> clfft_single( program, "clfft_single_stage");

        cl::NDRange local(WORK_GROUP_SIZE);
        start_time = static_cast<double>(timer.getTimeMilliseconds()) / 1000.0;
        {
            cout << "stage 0,1" << endl;
            cl::LocalSpaceArg localmem = cl::Local(sizeof(float) * 2 * WORK_GROUP_SIZE);
            clfft_multi(cl::EnqueueArgs( queue, global, local),
                        N,
                        log2N,
                        0,
                        2,
                        d_x,
                        localmem
                        );
            queue.finish();

            cout << "stage 2" << endl;
            clfft_single(cl::EnqueueArgs( queue, global),
                         N,
                         log2N,
                         2,
                         d_x
                         );
            queue.finish();
            cout << "stage 3" << endl;
            clfft_single(cl::EnqueueArgs( queue, global),
                         N,
                         log2N,
                         3,
                         d_x
                         );
            queue.finish();
        }
        run_time   = static_cast<double>( timer.getTimeMilliseconds()) / 1000.0 - start_time;

        cl::copy(queue, d_x, h_x.begin(), h_x.end());
        cout << "output --" << endl;
        for( int i = 0; i < h_x.size(); i+=2) {
            cout << "[" << i/2 << "].re = " << h_x[i] << ", im = " << h_x[i+1] << endl;
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
