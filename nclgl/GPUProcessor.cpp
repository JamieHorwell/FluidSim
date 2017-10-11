#include "GPUProcessor.h"
#include <iostream>
#include <CL\cl.hpp>
using namespace std;


GPUProcessor::GPUProcessor(bool interactive)
	:OpenCLprocessor(interactive)
{

	this->interactive = interactive;

}
	

	GPUProcessor::~GPUProcessor()
{
}

	void GPUProcessor::initBuffers(float * heights, float * velocities, int width)
	{
		cl::Program program = CreateProgramGPU("../../ProcessFluid.cl");

		cl::Context & context = program.getInfo<CL_PROGRAM_CONTEXT>();
		auto devices = context.getInfo<CL_CONTEXT_DEVICES>();
		auto& device = devices.front();
		cout << devices.front().getInfo<CL_DEVICE_NAME>() << "\n";
		cl_int err = 0;
		this->gridSize = width*width;
		this->heights = new float[gridSize];
		this->velocities = new float[gridSize];

		if (interactive) {
			//host will write in this buffer each iteration, kernel will use this to update velocities
			//clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, sizeof(float) * DATA_SIZE, inputdata, NULL);
			heightsbuf1 = cl::Buffer(context, CL_MEM_READ_WRITE, sizeof(float) * gridSize, NULL, &err);
			//host will read from this buffer each iteration, kernel will use this to fill heights for next iteration
			heightsbuf2 = cl::Buffer(context, CL_MEM_READ_WRITE | CL_MEM_HOST_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(float) * gridSize, heights, &err);

			//host will write in this buffer each iteration, kernel will use this to update velocities
			velocitiesBuf1 = cl::Buffer(context, CL_MEM_READ_WRITE, sizeof(float) * gridSize, NULL, &err);
			//host will read from this buffer each iteration, kernel will use this to update velocities
			velocitiesBuf2 = cl::Buffer(context, CL_MEM_READ_WRITE | CL_MEM_HOST_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(float) * gridSize, velocities, &err);


			//setup kernel
			kernel1 = cl::Kernel(program, "ProcessFluidInteractive");

			err = kernel1.setArg(0, heightsbuf1);
			err = kernel1.setArg(1, heightsbuf2);
			err = kernel1.setArg(2, velocitiesBuf1);
			err = kernel1.setArg(3, velocitiesBuf2);
			err = kernel1.setArg(4, sizeof(int), &width);

			queue = cl::CommandQueue(context, device);
		}
		else {
			//intialise buffers
			heightsbuf1 = cl::Buffer(context, CL_MEM_READ_WRITE | CL_MEM_HOST_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(float) * gridSize, heights, &err);
			heightsbuf2 = cl::Buffer(context, CL_MEM_READ_WRITE | CL_MEM_HOST_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(float) * gridSize, heights, &err);
			velocitiesBuf1 = cl::Buffer(context, CL_MEM_READ_WRITE | CL_MEM_HOST_NO_ACCESS | CL_MEM_COPY_HOST_PTR, sizeof(float) * gridSize, velocities, &err);

			kernel1 = cl::Kernel(program, "ProcessFluid");
			kernel2 = cl::Kernel(program, "ProcessFluid");

			err = kernel1.setArg(0, velocitiesBuf1);
			err = kernel1.setArg(1, heightsbuf1);
			err = kernel1.setArg(2, heightsbuf2);
			err = kernel1.setArg(3, sizeof(int), &width);

			err = kernel2.setArg(0, velocitiesBuf1);
			err = kernel2.setArg(1, heightsbuf2);
			err = kernel2.setArg(2, heightsbuf1);
			err = kernel2.setArg(3, sizeof(int), &width);



			queue = cl::CommandQueue(context, device);
			pingpong = 0;
		}
	}



	void GPUProcessor::updateFluid(float * heights, float * velocities)
	{
		if (interactive) {
			//first step is to write heights and velocities to buffers
			queue.enqueueWriteBuffer(heightsbuf1, CL_TRUE, 0, sizeof(float) * gridSize, heights, NULL, NULL);
			queue.enqueueWriteBuffer(velocitiesBuf1, CL_TRUE, 0, sizeof(float) * gridSize, velocities, NULL, NULL);
			queue.finish();

			//next run the kernel and read
			queue.enqueueNDRangeKernel(kernel1, cl::NullRange, cl::NDRange(gridSize));
			queue.enqueueReadBuffer(heightsbuf2, CL_TRUE, 0, sizeof(float)* gridSize, heights);
			queue.enqueueReadBuffer(velocitiesBuf2, CL_TRUE, 0, sizeof(float)* gridSize, velocities);
			queue.finish();

			
		}
		else {
			queue.enqueueNDRangeKernel((pingpong % 2 ? kernel1 : kernel2), cl::NullRange, cl::NDRange(gridSize));
			queue.enqueueReadBuffer((pingpong % 2 ? heightsbuf2 : heightsbuf1), CL_TRUE, 0, sizeof(float)* gridSize, heights);
			pingpong = 1 - pingpong;
		}
	}

	
