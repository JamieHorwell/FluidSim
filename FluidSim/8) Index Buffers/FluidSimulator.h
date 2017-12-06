#pragma once
#include "../../nclgl/window.h"
#include "Renderer.h"
#include <iostream>
#include "../../nclgl/Fluid.h"
#include "Entity.h"
#include "PhysicsLoop.h"
#pragma comment(lib, "nclgl.lib")

class FluidSimulator
{
public:
	FluidSimulator();
	~FluidSimulator();


	void GetInput();


private:
	Fluid* fluid;
	Entity* sphere;
	Renderer* renderer;
	Window w;
	PhysicsLoop pl;
	Shader* defaultShader;
};

