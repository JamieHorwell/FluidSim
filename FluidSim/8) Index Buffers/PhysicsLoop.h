#pragma once
#include "Entity.h"
#include "PhysicsCalculator.h"
#include "../../nclgl/HeightMap.h"
#include "../../nclgl/Vector2.h"

#define gravity -40.0f
#define DAMPING_FACTOR 0.75
#define FLUID_DENSITY 0.00053
//96
#define MIN_VELOCITY 0.1


class PhysicsLoop
{
public:
	PhysicsLoop();
	~PhysicsLoop();


	void applyGravity(Entity* ent);

	void updateEntityTstep(Entity* ent, float tStep);

	void updatePhysics(float tStep);




	Vector3 ForceFromFluidColumn(HeightMap*  fluid, int index, float displacement);


	//to check each individual column within spheres bounds
	bool SphereInColumn(Vector3 column, Vector3 sphereCentre, float radius);

	void SphereFluidCollision(Entity* sphere,HeightMap* fluid);

	void SphereColumnResolution(Entity* sphere, HeightMap* fluid, int index);

	void distributefluid(HeightMap* fluid, Vector3 pointA, Vector3 pointB, float fluidD);


	Entity* sphere;
	HeightMap* fluid;

};

