#include "GPUfluidProcessor.h"
#include <iostream>
using namespace std;


GPUfluidProcessor::GPUfluidProcessor()
{
}

GPUfluidProcessor::GPUfluidProcessor(float* heights, float* velocities, int height, bool interactive)
{
	cl::Program program = CreateProgramAll("../../ProcessFluid.cl");
	auto context = program.getInfo<CL_PROGRAM_CONTEXT>();
	auto devices = context.getInfo<CL_CONTEXT_DEVICES>();
	auto& device = devices.front();
	cout << devices.front().getInfo<CL_DEVICE_NAME>() << "\n";
	cout << devices.at(1).getInfo<CL_DEVICE_NAME>() << "\n";
	cl_int err = 0;
	this->gridSize = height*height;
	this->heights = new float[gridSize];
	this->velocities = new float[gridSize];
	if (interactive) {
		//host will write in this buffer each iteration, kernel will use this to update velocities
		//clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, sizeof(float) * DATA_SIZE, inputdata, NULL);
		heightsbuf1 = cl::Buffer(context, CL_MEM_READ_WRITE, sizeof(double) * gridSize, NULL, &err);
		//host will read from this buffer each iteration, kernel will use this to fill heights for next iteration
		heightsbuf2 = cl::Buffer(context, CL_MEM_READ_WRITE | CL_MEM_HOST_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(double) * gridSize, heights, &err);
		
		//host will write in this buffer each iteration, kernel will use this to update velocities
		velocitiesBuf1 = cl::Buffer(context, CL_MEM_READ_WRITE, sizeof(double) * gridSize,NULL, &err);
		//host will read from this buffer each iteration, kernel will use this to update velocities
		velocitiesBuf2 = cl::Buffer(context, CL_MEM_READ_WRITE | CL_MEM_HOST_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(double) * gridSize, velocities, &err);

		//setup kernel
		kernel1 = cl::Kernel(program, "ProcessFluidInteractive");

		err = kernel1.setArg(0, heightsbuf1);
		err = kernel1.setArg(1, heightsbuf2);
		err = kernel1.setArg(2, velocitiesBuf1);
		err = kernel1.setArg(3, velocitiesBuf2);
		err = kernel1.setArg(4, sizeof(int), &height);

		queue = cl::CommandQueue(context, device);
	}
	else {
		//intialise buffers
		heightsbuf1 = cl::Buffer(context, CL_MEM_READ_WRITE | CL_MEM_HOST_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(double) * gridSize, heights, &err);
		heightsbuf2 = cl::Buffer(context, CL_MEM_READ_WRITE | CL_MEM_HOST_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(double) * gridSize, heights, &err);
		velocitiesBuf1 = cl::Buffer(context, CL_MEM_READ_WRITE | CL_MEM_HOST_NO_ACCESS | CL_MEM_COPY_HOST_PTR, sizeof(double) * gridSize, velocities, &err);

		kernel1 = cl::Kernel(program, "ProcessFluid");
		kernel2 = cl::Kernel(program, "ProcessFluid");

		err = kernel1.setArg(0, velocitiesBuf1);
		err = kernel1.setArg(1, heightsbuf1);
		err = kernel1.setArg(2, heightsbuf2);
		err = kernel1.setArg(3, sizeof(int), &height);

		err = kernel2.setArg(0, velocitiesBuf1);
		err = kernel2.setArg(1, heightsbuf2);
		err = kernel2.setArg(2, heightsbuf1);
		err = kernel2.setArg(3, sizeof(int), &height);



		queue = cl::CommandQueue(context, device);
		pingpong = 0;


	}

}

GPUfluidProcessor::GPUfluidProcessor(float * heights, float * velocities, int height, bool allDevices, bool interactive)
{
	cl::Program program = CreateProgramAll("../../ProcessFluid.cl");
	auto context = program.getInfo<CL_PROGRAM_CONTEXT>();
	auto devices = context.getInfo<CL_CONTEXT_DEVICES>();
	auto& device1 = devices.front();
	auto& device2 = devices.back();
	std::cout << device1.getInfo<CL_DEVICE_VENDOR>() << "\n";
	std::cout << device2.getInfo<CL_DEVICE_VENDOR>() << "\n";
	cl_int err = 0;
	this->gridSize = height*height;
	this->heightsf = new float[gridSize];
	this->velocitiesf = new float[gridSize];

	//host will write in this buffer each iteration, kernel will use this to update velocities
	//clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, sizeof(float) * DATA_SIZE, inputdata, NULL);
	heightsbuf1 = cl::Buffer(context, CL_MEM_READ_WRITE, sizeof(float) * gridSize, NULL, &err);
	//host will read from this buffer each iteration, kernel will use this to fill heights for next iteration
	heightsbuf2 = cl::Buffer(context, CL_MEM_READ_WRITE | CL_MEM_HOST_READ_ONLY | CL_MEM_USE_HOST_PTR, sizeof(float) * gridSize, heightsf, &err);

	//host will write in this buffer each iteration, kernel will use this to update velocities
	velocitiesBuf1 = cl::Buffer(context, CL_MEM_READ_WRITE, sizeof(float) * gridSize, NULL, &err);
	//host will read from this buffer each iteration, kernel will use this to update velocities
	velocitiesBuf2 = cl::Buffer(context, CL_MEM_READ_WRITE | CL_MEM_HOST_READ_ONLY | CL_MEM_USE_HOST_PTR, sizeof(float) * gridSize, velocitiesf, &err);

	//setup kernel
	kernel1 = cl::Kernel(program, "ProcessFluidInteractive");
	kernel2 = cl::Kernel(program, "ProcessFluidInteractive");

	err = kernel1.setArg(0, heightsbuf1);
	err = kernel1.setArg(1, heightsbuf2);
	err = kernel1.setArg(2, velocitiesBuf1);
	err = kernel1.setArg(3, velocitiesBuf2);
	err = kernel1.setArg(4, sizeof(int), &height);

	err = kernel2.setArg(0, heightsbuf1);
	err = kernel2.setArg(1, heightsbuf2);
	err = kernel2.setArg(2, velocitiesBuf1);
	err = kernel2.setArg(3, velocitiesBuf2);
	err = kernel2.setArg(4, sizeof(int), &height);

	queue = cl::CommandQueue(context, device1, CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE);
	queue2 = cl::CommandQueue(context, device2, CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE);


	


}


GPUfluidProcessor::~GPUfluidProcessor()
{
	
	queue.flush();
	queue.finish();
}

void GPUfluidProcessor::updateHeights(float * heights)
{



	queue.enqueueNDRangeKernel((pingpong % 2 ? kernel1 : kernel2), cl::NullRange, cl::NDRange(gridSize));
	queue.enqueueReadBuffer((pingpong % 2 ? heightsbuf2 : heightsbuf1), CL_TRUE, 0, sizeof(double)* gridSize, heights);
	pingpong = 1 - pingpong;
	
}

void GPUfluidProcessor::updateHeightsInteractive(float * heights, float * velocities)
{
	//first step is to write heights and velocities to buffers
	queue.enqueueWriteBuffer(heightsbuf1, CL_TRUE, 0, sizeof(double) * gridSize, heights, NULL, NULL);
	queue.enqueueWriteBuffer(velocitiesBuf1, CL_TRUE, 0, sizeof(double) * gridSize, velocities, NULL, NULL);
	queue.finish();

	//next run the kernel and read
	queue.enqueueNDRangeKernel(kernel1, cl::NullRange, cl::NDRange(gridSize));
	queue.enqueueReadBuffer(heightsbuf2, CL_TRUE, 0, sizeof(double)* gridSize, this->heights);
	queue.enqueueReadBuffer(velocitiesBuf2, CL_TRUE, 0, sizeof(double)* gridSize, this->velocities);
	queue.finish();
	
	for (int i = 0; i < gridSize; ++i) {
		heights[i] = this->heights[i];
	}
	
	for (int i = 0; i < gridSize; ++i) {
		velocities[i] = this->velocities[i];
	}

}


void GPUfluidProcessor::updateHeightsAllDevices(float * heights, float * velocities)
{
	//split work up 9 / 1 on GPU/CPU
	int CPUload = gridSize / 10;
	int GPUload = gridSize - CPUload;


	


	std::vector<cl::Event> eventss;
	eventss.push_back(cl::Event());
	eventss.push_back(cl::Event());
	eventss.push_back(cl::Event());
	eventss.push_back(cl::Event());


	//have both devices write...
	queue2.enqueueWriteBuffer(heightsbuf1, CL_FALSE, 0, sizeof(float) * CPUload, heights, NULL, &eventss.at(0));
	queue.enqueueWriteBuffer(heightsbuf1, CL_FALSE, sizeof(float)*CPUload, sizeof(float) * GPUload, heights + CPUload, NULL, &eventss.at(1));

	queue2.enqueueWriteBuffer(velocitiesBuf1, CL_FALSE, 0, sizeof(float) * CPUload, velocities, NULL, &eventss.at(2));
	queue.enqueueWriteBuffer(velocitiesBuf1, CL_FALSE, sizeof(float)*CPUload, sizeof(float) * GPUload, velocities + CPUload, NULL, &eventss.at(3));

	
	std::vector<cl::Event> eventsss;
	eventsss.push_back(cl::Event());
	eventsss.push_back(cl::Event());
	
	//both devices execute
	queue2.enqueueNDRangeKernel(kernel1, 0, cl::NDRange(CPUload), cl::NullRange, &eventss, &eventsss.at(0));
	queue.enqueueNDRangeKernel(kernel2, cl::NDRange(CPUload), cl::NDRange(GPUload), cl::NullRange, &eventss, &eventsss.at(1));
	
	queue.finish();
	
	cl_int err;


	
	//both devices read
	queue2.enqueueReadBuffer(velocitiesBuf2, CL_TRUE, 0, sizeof(float) * CPUload, velocities, &eventsss);
	queue.enqueueReadBuffer(velocitiesBuf2, CL_TRUE, sizeof(float)*CPUload, sizeof(float) * GPUload, velocities + CPUload, &eventsss);
	queue2.enqueueReadBuffer(heightsbuf2, CL_TRUE, 0, sizeof(float) * CPUload, heights, &eventsss);
	queue.enqueueReadBuffer(heightsbuf2, CL_TRUE, sizeof(float)*CPUload, sizeof(float) * GPUload, heights + CPUload, &eventsss);
	
	
	


	queue.finish();
	queue2.finish();



	

}



