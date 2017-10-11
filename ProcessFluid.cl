#define C 25.0f
#define H 1.0f
#define T 0.0033f
#define DAMPING 0.9995f;



__kernel void ProcessFluidInteractive(__global float* prvHeights, __global float* nxtHeights, __global float* prvVelocities, __global float* nxtVelocities,  unsigned int gridWidth)
{
	int workid = get_global_id(0);
	int gridSize = gridWidth*gridWidth;
	float f;
	int workidUp = workid - 1;
	if(workidUp % gridWidth == (gridWidth - 1) || workidUp < 0) {
		workidUp = workid;
	}
	
	int workidDown = workid + 1;
	if(workidDown % gridWidth == 0) {
		workidDown = workid;
	}
	
	int workidLeft = workid - gridWidth;
	if(workidLeft < 0) {
		workidLeft = workid;
	}
	
	
	int workidRight = workid + gridWidth;
	if(workidRight >= (gridSize)) {
		workidRight = workid;
	}
	
	
	f = (C*C)*(prvHeights[workidUp] + prvHeights[workidDown] + prvHeights[workidLeft] + prvHeights[workidRight] - 4*(prvHeights[workid]))/(H*H);
	
	nxtVelocities[workid] = (f*T + prvVelocities[workid]) * DAMPING;
	nxtHeights[workid] = prvHeights[workid] + (nxtVelocities[workid] * T);

	
}





__kernel void ProcessFluid(__global float* velocities, __global float* prvHeights, __global float* nxtHeights, unsigned int gridWidth)
{
	int workid = get_global_id(0);
	int gridSize = gridWidth*gridWidth;
	float f;
	int workidUp = workid - 1;
	if(workidUp % gridWidth == (gridWidth - 1) || workidUp < 0) {
		workidUp = workid;
	}
	
	int workidDown = workid + 1;
	if(workidDown % gridWidth == 0) {
		workidDown = workid;
	}
	
	int workidLeft = workid - gridWidth;
	if(workidLeft < 0) {
		workidLeft = workid;
	}
	
	
	int workidRight = workid + gridWidth;
	if(workidRight >= (gridSize)) {
		workidRight = workid;
	}
	
	
	f = (C*C)*(prvHeights[workidUp] + prvHeights[workidDown] + prvHeights[workidLeft] + prvHeights[workidRight] - 4*(prvHeights[workid]))/(H*H);
	
	velocities[workid] += f*T;
	velocities[workid] *= DAMPING;
	nxtHeights[workid] = prvHeights[workid] + (velocities[workid] * T);
}
