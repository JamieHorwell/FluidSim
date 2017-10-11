#include "HeterogeneousProcessor.h"




HeterogeneousProcessor::HeterogeneousProcessor(bool interactive) : OpenCLprocessor(interactive)
{




}

HeterogeneousProcessor::~HeterogeneousProcessor()
{
}

void HeterogeneousProcessor::initBuffers(float * heights, float * velocities, int width)
{
	cl::Program program = CreateProgramAll("../../ProcessFluid.cl");
	auto context = program.getInfo<CL_PROGRAM_CONTEXT>();
	auto devices = context.getInfo<CL_CONTEXT_DEVICES>();
	auto& device2 = devices.back();
	auto& device1 = devices.front();
	cl_int err = 0;

	this->gridSize = width*width;
	 CPULoad = gridSize / 2;
	 GPULoad = gridSize - CPULoad;

	


	//host will write in this buffer each iteration, kernel will use this to update velocities
	//clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, sizeof(float) * DATA_SIZE, inputdata, NULL);
	heightsbuf1 = cl::Buffer(context, CL_MEM_READ_WRITE, sizeof(float) * gridSize, NULL, &err);
	//host will read from this buffer each iteration, kernel will use this to fill heights for next iteration
	heightsbuf2 = cl::Buffer(context, CL_MEM_READ_WRITE | CL_MEM_HOST_READ_ONLY | CL_MEM_USE_HOST_PTR, sizeof(float) * gridSize, heights, &err);

	//host will write in this buffer each iteration, kernel will use this to update velocities
	velocitiesBuf1 = cl::Buffer(context, CL_MEM_READ_WRITE, sizeof(float) * gridSize, NULL, &err);
	//host will read from this buffer each iteration, kernel will use this to update velocities
	velocitiesBuf2 = cl::Buffer(context, CL_MEM_READ_WRITE | CL_MEM_HOST_READ_ONLY | CL_MEM_USE_HOST_PTR,  sizeof(float) * gridSize, velocities, &err);

	//setup kernel
	kernel1 = cl::Kernel(program, "ProcessFluidInteractive");
	kernel2 = cl::Kernel(program, "ProcessFluidInteractive");

	err = kernel1.setArg(0, heightsbuf1);
	err = kernel1.setArg(1, heightsbuf2);
	err = kernel1.setArg(2, velocitiesBuf1);
	err = kernel1.setArg(3, velocitiesBuf2);
	err = kernel1.setArg(4, sizeof(int), &width);

	err = kernel2.setArg(0, heightsbuf1);
	err = kernel2.setArg(1, heightsbuf2);
	err = kernel2.setArg(2, velocitiesBuf1);
	err = kernel2.setArg(3, velocitiesBuf2);
	err = kernel2.setArg(4, sizeof(int), &width);

	queueGPU = cl::CommandQueue(context, device1);
	queueCPU = cl::CommandQueue(context, device2);


}

void HeterogeneousProcessor::updateFluid(float * heights, float * velocities)
{

	std::vector<cl::Event> eventss;
	eventss.push_back(cl::Event());
	eventss.push_back(cl::Event());
	eventss.push_back(cl::Event());
	eventss.push_back(cl::Event());


	//have both devices write...
	queueCPU.enqueueWriteBuffer(heightsbuf1, CL_FALSE, 0, sizeof(float) * CPULoad, heights, NULL, &eventss.at(0));
	queueGPU.enqueueWriteBuffer(heightsbuf1, CL_FALSE, sizeof(float)*CPULoad, sizeof(float) * GPULoad, heights + CPULoad, NULL, &eventss.at(1));

	queueCPU.enqueueWriteBuffer(velocitiesBuf1, CL_FALSE, 0, sizeof(float) * CPULoad, velocities, NULL, &eventss.at(2));
	queueGPU.enqueueWriteBuffer(velocitiesBuf1, CL_FALSE, sizeof(float)*CPULoad, sizeof(float) * GPULoad, velocities+CPULoad, NULL, &eventss.at(3));


	std::vector<cl::Event> eventsss;
	eventsss.push_back(cl::Event());
	eventsss.push_back(cl::Event());

	//both devices execute
	queueCPU.enqueueNDRangeKernel(kernel1, 0, cl::NDRange(CPULoad), cl::NullRange, &eventss);
	queueGPU.enqueueNDRangeKernel(kernel2, cl::NDRange(CPULoad), cl::NDRange(GPULoad), cl::NullRange, &eventss);

	

	



	//both devices read

	queueCPU.enqueueReadBuffer(velocitiesBuf2, CL_FALSE, 0, sizeof(float) * CPULoad, velocities, &eventsss);
	queueGPU.enqueueReadBuffer(velocitiesBuf2, CL_FALSE, sizeof(float)*CPULoad, sizeof(float) * GPULoad, velocities + CPULoad, &eventsss);
	queueCPU.enqueueReadBuffer(heightsbuf2, CL_FALSE, 0, sizeof(float) * CPULoad, heights, &eventsss);
	queueGPU.enqueueReadBuffer(heightsbuf2, CL_FALSE, sizeof(float)*CPULoad, sizeof(float) * GPULoad, heights + CPULoad, &eventsss);

	queueCPU.finish();
	queueGPU.finish();





}
