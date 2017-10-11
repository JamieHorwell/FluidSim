#pragma once
#include "OpenCLSetup.h"
#include <CL\cl.hpp>
class OpenCLprocessor
{
public:
	OpenCLprocessor(bool interactive);
	~OpenCLprocessor();


	virtual void initBuffers(float* heights, float* velocities, int width) = 0;

	virtual void updateFluid(float* heights, float* velocities = nullptr) = 0;



};

