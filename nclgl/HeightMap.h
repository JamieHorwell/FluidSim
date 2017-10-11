#pragma once
#pragma comment(lib, "opencl.lib")
#include <string>
#include <iostream>
#include <fstream>
#include "Mesh.h"
#include "GPUfluidProcessor.h"
#include "OpenCLprocessor.h"

# define RAW_WIDTH 300
# define RAW_HEIGHT 300

#define HEIGHTMAP_X 1.0f
#define HEIGHTMAP_Z 1.0f
#define HEIGHTMAP_Y 0.1f
#define HEIGHTMAP_TEX_X 1.0f / 1.0f
#define HEIGHTMAP_TEX_Z 1.0f / 1.0f
enum class Processor {
	CPU,
	GPU,
	HeteroGenous
};


class HeightMap :
	public Mesh
{
public:
	HeightMap(std::string name, Processor processor, bool complex, bool interactive = false, int width = 200);

	~HeightMap(void);
	void increaseVertice(int idx, float height);	virtual void updateFluid();	void updateSimpleCPU();
	void updateComplexCPU();
	void updateOpenCL();
	void updateHeteroGenous();
	void updateInteractiveGPU();



	Vector3 getVertice(int index) { return vertices[index]; };
	void setVertice(int index, float h) { vertices[index].y = h; };
	int getHeight() { return height; };
	float getDensity() { return density; };

	void updateVelocity(int index, float velocity) { velocities[index] += velocity; };
	double totalFluid();

	void updateHeight(int index, float height) { heights[index] += height; };

	bool getGPU() { return GPU; };
	bool getComplex() { return complex; };

	

	void setTitle();
	string getTitle() { return title; };

	HeightMap& operator=(HeightMap rhs) {	};

private:
	float* heights = NULL;
	float* velocities = NULL;
	float density;
	int height;
	int width;
	GPUfluidProcessor gpufluidprocessor;
	bool GPU;
	bool complex;
	bool interactive;
	string title;
	Processor processorToUse;
	OpenCLprocessor* openCLProcessor;


};

