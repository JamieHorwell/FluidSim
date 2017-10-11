#pragma once
#include "OpenCLSetup.h"
#include <CL\cl.hpp>

class GPUfluidProcessor
{
public:
	GPUfluidProcessor();
	GPUfluidProcessor(float * heights, float * velocities, int height, bool interactive);
	GPUfluidProcessor(float * heights, float * velocities, int height, bool allDevices, bool interactive);
	~GPUfluidProcessor();

	void updateHeights(float* heights);

	void updateHeightsInteractive(float* heights, float * velocities);

	void updateHeightsAllDevices(float * heights, float * velocities);

private:
	cl::Kernel kernel1;
	cl::Kernel kernel2;
	cl::Buffer heightsbuf1;
	cl::Buffer heightsbuf2;
	cl::Buffer velocitiesBuf1;
	cl::Buffer velocitiesBuf2;
	cl::Buffer gridSizeBuf;
	cl::CommandQueue queue;
	cl::CommandQueue queue2;
	int pingpong;
	int gridSize;
	

	float* heights;
	float* velocities;
	float* heightsf;
	float* velocitiesf;
};

