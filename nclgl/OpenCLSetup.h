#include <CL\cl.hpp>
#define CL_USE_DEPRECATED_OPENCL_2_0_APIS
cl::Program CreateProgramGPU(const std::string& file);
cl::Program CreateProgramAll(const std::string& file);
