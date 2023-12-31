#include <iostream>
#include <vector>
#include <fstream>

#define CL_HPP_CL_1_2_DEFAULT_BUILD
#define CL_HPP_TARGET_OPENCL_VERSION 120
#define CL_HPP_MINIMUM_OPENCL_VERSION 120
#define CL_HPP_ENABLE_PROGRAM_CONSTRUCTION_FROM_ARRAY_COMPATIBILITY 1
#define CL_USE_DEPRECATED_OPENCL_1_2_APIS

#include <CL/cl2.hpp>
#include <CL/cl_ext_xilinx.h>

#define OCL_CHECK(error,call)                                       \
    call;                                                           \
    if (error != CL_SUCCESS) {                                      \
      printf("%s:%d Error calling " #call ", error code is: %d\n",  \
              __FILE__,__LINE__, error);                            \
      exit(EXIT_FAILURE);                                           \
    }

std::string xclbin_file_name;

template <typename T>
struct aligned_allocator
{
  using value_type = T;
  T* allocate(std::size_t num)
  {
    void* ptr = nullptr;
    if (posix_memalign(&ptr,4096,num*sizeof(T)))
      throw std::bad_alloc();
    return reinterpret_cast<T*>(ptr);
  }
  void deallocate(T* p, std::size_t num)
  {
    free(p);
  }
};

cl::Program::Binaries import_binary_file()
{
    std::cout << "\n Loading: "<< xclbin_file_name.c_str() << "\n";
    std::ifstream bin_file(xclbin_file_name.c_str(), std::ifstream::binary);
    bin_file.seekg (0, bin_file.end);
    unsigned nb = bin_file.tellg();
    bin_file.seekg (0, bin_file.beg);
    char *buf = new char [nb];
    bin_file.read(buf, nb);

    cl::Program::Binaries bins;
    bins.push_back({buf,nb});
    return bins;
}

std::vector<cl::Device> get_devices() {
    size_t i;
    cl_int err;
    std::vector<cl::Platform> platforms;
    OCL_CHECK(err, err = cl::Platform::get(&platforms));
    cl::Platform platform;
    for (i  = 0 ; i < platforms.size(); i++){
        platform = platforms[i];
        OCL_CHECK(err, std::string platformName = platform.getInfo<CL_PLATFORM_NAME>(&err));
        if (platformName == "Xilinx"){
            std::cout << "\nFound Platform" << std::endl;
            std::cout << "\nPlatform Name: " << platformName.c_str() << std::endl;
            break;
        }
    }
    if (i == platforms.size()) {
        std::cout << "Error: Failed to find Xilinx platform" << std::endl;
        exit(EXIT_FAILURE);
    }
    //Getting ACCELERATOR Devices and selecting 1st such device
    std::vector<cl::Device> devices;
    OCL_CHECK(err, err = platform.getDevices(CL_DEVICE_TYPE_ACCELERATOR, &devices));
    return devices;
}

/* Helper Function */
void host_serialize_cin(std::vector<float, aligned_allocator<float>> &cin_to, std::vector<float, aligned_allocator<float>> &cin_from){
  /* Variable Declaration */
  unsigned int cnt = 0;
  /* Variable Declaration */

  for (int c1 = 0; c1 <= 15; c1 += 1)
    for (int c2 = 0; c2 <= 63; c2 += 1)
      for (int c3 = 0; c3 <= 1; c3 += 1) {
        // array
        // io_L3
        for (int c4 = 0; c4 <= 15; c4 += 1) {
          // io_L2
          for (int c5 = 0; c5 <= 2; c5 += 1)
            for (int c6 = 0; c6 <= 5; c6 += 1)
              for (int c7 = 0; c7 <= 7; c7 += 1)
                cin_to[cnt++] = cin_from[((16 * c1 + c4 + c5) * 258 + (4 * c2 + c6)) * 16 + (8 * c3 + c7)];
        }
      }
}
/* Helper Function */

/* Helper Function */
void host_serialize_w(std::vector<float, aligned_allocator<float>> &w_to, std::vector<float, aligned_allocator<float>> &w_from){
  /* Variable Declaration */
  unsigned int cnt = 0;
  /* Variable Declaration */

  for (int c1 = 0; c1 <= 15; c1 += 1)
    for (int c2 = 0; c2 <= 63; c2 += 1)
      for (int c3 = 0; c3 <= 1; c3 += 1) {
        // array
        // io_L3
        for (int c4 = 0; c4 <= 3; c4 += 1) {
          // io_L2
          for (int c5 = 0; c5 <= 3; c5 += 1)
            for (int c6 = 0; c6 <= 2; c6 += 1)
              for (int c7 = 0; c7 <= 2; c7 += 1)
                for (int c8 = 0; c8 <= 7; c8 += 1)
                  w_to[cnt++] = w_from[(((4 * c4 + c5) * 3 + c6) * 3 + c7) * 16 + (8 * c3 + c8)];
        }
      }
}
/* Helper Function */

/* Helper Function */
void host_deserialize_cout(std::vector<float, aligned_allocator<float>> &cout_to, std::vector<float, aligned_allocator<float>> &cout_from){
  /* Variable Declaration */
  unsigned int cnt = 0;
  /* Variable Declaration */

  for (int c1 = 0; c1 <= 15; c1 += 1)
    for (int c2 = 0; c2 <= 63; c2 += 1) {
      // array
      // io_L3
      for (int c4 = 0; c4 <= 15; c4 += 1) {
        // io_L2
        for (int c5 = 0; c5 <= 3; c5 += 1) {
          // io_L1
          // pe
          for (int c7 = 0; c7 <= 3; c7 += 1)
            for (int c8 = 0; c8 <= 3; c8 += 1)
              cout_to[((16 * c1 + c4) * 256 + (4 * c2 + c7)) * 16 + (4 * c5 + c8)] = cout_from[cnt++];
        }
      }
    }
}
/* Helper Function */

