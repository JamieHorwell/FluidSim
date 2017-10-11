#include "HeightMap.h"
#include <math.h>
#include "HeterogeneousProcessor.h"
#include "GPUProcessor.h"

HeightMap::HeightMap(std::string name, Processor processor, bool complex, bool interactive, int width) {
	//default dimensions of 257,257
	this->height = width;
	this->width = width;
	this->processorToUse = processor;
	this->interactive = interactive;
	setTitle();
	std::ifstream file(name.c_str(), ios::binary);
	if (!file) {
		return;

	}

	int max = 257 * 257;

	numVertices = width * width;
	numIndices = (width - 1)*(width - 1) * 6;
	vertices = new Vector3[numVertices];
	textureCoords = new Vector2[numVertices];
	indices = new GLuint[numIndices];
	file.read((char *)data, max * sizeof(unsigned char));
	file.close();

	//initialise arrays
	heights = new float[numVertices];
	velocities = new float[numVertices];

	



	for (int x = 0; x < width; ++x) {
		for (int z = 0; z < width; ++z) {
			int offset = (x * width) + z;

			vertices[offset] = Vector3(
				x * HEIGHTMAP_X, data[offset % max] * HEIGHTMAP_Y , z * HEIGHTMAP_Z);

			textureCoords[offset] = Vector2(
				x * HEIGHTMAP_TEX_X, z * HEIGHTMAP_TEX_Z);

			heights[offset] = (data[offset] * HEIGHTMAP_Y);
			velocities[offset] = 0;

		}

	}





	delete data;

	for (int x = 0; x < width - 1; ++x) {
		for (int z = 0; z < width - 1; ++z) {
			int a = (x * (width)) + z;
			int b = ((x + 1) * (width)) + z;
			int c = ((x + 1) * (width)) + (z + 1);
			int d = (x * (width)) + (z + 1);

			indices[numIndices++] = c;
			indices[numIndices++] = b;
			indices[numIndices++] = a;

			indices[numIndices++] = a;
			indices[numIndices++] = d;
			indices[numIndices++] = c;

		}

	}
	

	//initialise the OpenCL implementation
	if (processorToUse == Processor::GPU) {
		openCLProcessor = new GPUProcessor(interactive);
		openCLProcessor->initBuffers(heights, velocities, this->width);
	}
	else if (processorToUse == Processor::HeteroGenous) {
		openCLProcessor = new HeterogeneousProcessor(interactive);
		openCLProcessor->initBuffers(heights, velocities, this->width);
	}
	GenerateNormals();
	GenerateTangents();
	BufferData();




	delete heights;
	delete velocities;
}



void HeightMap::increaseVertice(int idx, float height)
{
	vertices[idx].y += height;
}

void HeightMap::updateFluid()
{
	if(processorToUse == Processor::CPU) {
		if (complex == true) updateComplexCPU();
		else updateSimpleCPU();
	}
	else if (processorToUse == Processor::GPU || processorToUse == Processor::HeteroGenous) {
		updateOpenCL();
	}
	
}

void HeightMap::updateSimpleCPU()
{

	for (int i = 0; i < this->height; ++i) {


	GenerateNormals();
	GenerateTangents();
	RebufferData();
}


	for (int i = 0; i < height*height; ++i) {
			
			vertices[i].y =  heights[i];
			
		
	}
	
	
	GenerateNormals();
	RebufferData();
void HeightMap::updateOpenCL()
{
	openCLProcessor->updateFluid(heights, velocities);
	for (int i = 0; i < width*height; ++i) {
		vertices[i].y = heights[i];
	}
	GenerateNormals();
	GenerateTangents();
	RebufferData();
}

void HeightMap::updateHeteroGenous()
{
	gpufluidprocessor.updateHeightsAllDevices(heights,velocities);
	for (int i = 0; i < width*height; ++i) {
		vertices[i].y = heights[i];
	
	}
	GenerateNormals();
	GenerateTangents();
	RebufferData();
}

void HeightMap::updateInteractiveGPU()
{
	gpufluidprocessor.updateHeightsInteractive(heights, velocities);
	for (int i = 0; i < width*height; ++i) {
		vertices[i].y = heights[i];
	}
	GenerateNormals();
	GenerateTangents();
	RebufferData();
}





double HeightMap::totalFluid()
{
	double totalFluid = 0;
	for (int i = 0; i < numVertices; ++i) {
		totalFluid += vertices[i].y;

	}
	return totalFluid;
}

void HeightMap::setTitle()
{
	if (processorToUse == Processor::CPU && !complex) {
		title = "CPU Simple Fluid Representation";
	}
	else if (!GPU && complex) {
		title = "CPU Complex Fluid Representation";
	}
	else if (GPU && complex) {
		title = "GPU Complex Fluid Representation using OpenCL";
	}


}


