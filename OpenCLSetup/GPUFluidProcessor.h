#pragma once
#include "OpenCLSetup.h"
#include <CL/cl.hpp>

class GPUFluidProcessor
{
public:
	GPUFluidProcessor();
	GPUFluidProcessor(float * heights, float * velocities, int gridSize);
	~GPUFluidProcessor();

	int* updateHeights(int* heights);



private:
	cl::Kernel kernel1;
	cl::Kernel kernel2;

	cl::Buffer heightsbuf1;
	cl::Buffer heightsbuf2;
	cl::Buffer velocitiesBuf;
	cl::Buffer gridSizeBuf;
	cl::CommandQueue queue;
	int pingpong;
	int gridSize;
	int* heights;
};

