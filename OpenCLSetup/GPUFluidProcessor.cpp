#include "GPUFluidProcessor.h"



GPUFluidProcessor::GPUFluidProcessor(float* heights, float* velocities, int gridSize)
{
	cl::Program program = CreateProgramGPU("ProcessArray.cl");
	auto context = program.getInfo<CL_PROGRAM_CONTEXT>();
	auto devices = context.getInfo<CL_CONTEXT_DEVICES>();
	auto& device = devices.front();

	cl_int err = 0;
	//ping pong buffers 
	//cl::Buffer heightsBuf1(context, CL_MEM_READ_WRITE | CL_MEM_HOST_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(float) * gridSize, heights, &err);
	
	

	//intialise buffers
	heightsbuf1 = cl::Buffer(context, CL_MEM_READ_WRITE | CL_MEM_HOST_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(float) * gridSize, heights, &err);
	heightsbuf2 = cl::Buffer(context, CL_MEM_READ_WRITE | CL_MEM_HOST_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(float) * gridSize, heights, &err);
	velocitiesBuf = cl::Buffer(context, CL_MEM_READ_WRITE | CL_MEM_HOST_NO_ACCESS | CL_MEM_COPY_HOST_PTR, sizeof(float) * gridSize, velocities, &err);


	kernel1.setArg(0, velocitiesBuf);
	kernel1.setArg(1, heightsbuf1);
	kernel1.setArg(2, heightsbuf2);
	kernel1.setArg(3, sizeof(int), &gridSize);

	err = kernel2.setArg(0, velocitiesBuf);
	err = kernel2.setArg(1, heightsbuf2);
	err = kernel2.setArg(2, heightsbuf1);
	err = kernel2.setArg(3, sizeof(int), &gridSize);

	
	
	queue = cl::CommandQueue(context, device);
	pingpong = 0;
	this->gridSize = gridSize;
	heights = new float[gridSize];
}


GPUFluidProcessor::~GPUFluidProcessor()
{
}

int * GPUFluidProcessor::updateHeights(int * heights)
{
	pingpong = 1 - pingpong;
	queue.enqueueNDRangeKernel((pingpong % 2 ? kernel1 : kernel2), cl::NullRange, cl::NDRange(gridSize));
	queue.enqueueReadBuffer((pingpong % 2 ? heightsbuf2 : heightsbuf1), CL_TRUE, 0, sizeof(float)* gridSize, heights);
	return heights;
}
