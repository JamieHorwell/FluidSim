#pragma once
#include "OpenCLprocessor.h"
class HeterogeneousProcessor : public OpenCLprocessor
{
public:
	HeterogeneousProcessor(bool interactive);
	~HeterogeneousProcessor();

	void initBuffers(float* heights, float* velocities, int width);

	void updateFluid(float* heights, float* velocities = nullptr);

private:
	cl::Kernel kernel1;
	cl::Kernel kernel2;
	cl::Buffer heightsbuf1;
	cl::Buffer heightsbuf2;
	cl::Buffer velocitiesBuf1;
	cl::Buffer velocitiesBuf2;
	cl::Buffer gridSizeBuf;
	cl::CommandQueue queueGPU;
	cl::CommandQueue queueCPU;
	bool interactive;
	int gridSize;
	int CPULoad;
	int GPULoad;

	float* CPUheights;
	float* GPUheights;
	float* CPUvelocities;
	float* GPUvelocities;
};

