#pragma once
#include "OpenCLprocessor.h"
class GPUProcessor : public OpenCLprocessor
{
public:
	GPUProcessor(bool interactive);
	~GPUProcessor();


	void updateFluid(float* heights, float* velocities = nullptr);

	void initBuffers(float* heights, float* velocities, int width);


private:
	cl::Kernel kernel1;
	cl::Kernel kernel2;
	cl::Buffer heightsbuf1;
	cl::Buffer heightsbuf2;
	cl::Buffer velocitiesBuf1;
	cl::Buffer velocitiesBuf2;
	cl::Buffer gridSizeBuf;
	cl::CommandQueue queue;
	int pingpong;
	bool interactive;
	int gridSize;
	float* heights;
	float* velocities;
};

