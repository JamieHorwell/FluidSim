#include <string>
#include <CL\cl.hpp>
#include <fstream>

cl::Program CreateProgramGPU(const std::string& file) {

	std::vector<cl::Platform> platforms;
	cl::Platform::get(&platforms);

	auto platform = platforms.front();
	std::vector<cl::Device> devices;
	platform.getDevices(CL_DEVICE_TYPE_GPU, &devices);

	auto device = devices.front();

	std::ifstream KernerlFile(file);
	std::string src(std::istreambuf_iterator<char>(KernerlFile), (std::istreambuf_iterator<char>()));

	cl::Program::Sources sources(1, std::make_pair(src.c_str(), src.length() + 1));

	cl::Context context(device);
	cl::Program program(context, sources);

	program.build("-cl-std=CL1.2");
	return program;
}

cl::Program CreateProgramAll(const std::string& file) {

	std::vector<cl::Platform> platforms;
	cl::Platform::get(&platforms);

	auto platform = platforms.front();
	std::vector<cl::Device> devices;
	platform.getDevices(CL_DEVICE_TYPE_ALL, &devices);


	std::ifstream KernerlFile(file);
	std::string src(std::istreambuf_iterator<char>(KernerlFile), (std::istreambuf_iterator<char>()));

	cl::Program::Sources sources(1, std::make_pair(src.c_str(), src.length() + 1));

	cl::Context context(CL_DEVICE_TYPE_ALL);
	cl::Program program(context, sources);

	program.build("-cl-std=CL1.2");
	return program;
}






