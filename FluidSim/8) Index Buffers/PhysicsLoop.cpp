#include "PhysicsLoop.h"



PhysicsLoop::PhysicsLoop()
{
}


PhysicsLoop::~PhysicsLoop()
{
}

void PhysicsLoop::applyGravity(Entity * ent)
{
	ent->setForce(ent->getforce() + Vector3(0,gravity,0));
}

void PhysicsLoop::updateEntityTstep(Entity * ent, float tStep)
{
	SphereFluidCollision(ent, fluid);

	applyGravity(ent);

	Vector3 a = PhysicsCalculator::calculateAcceleration(ent->getforce(), ent->getMass());

	Vector3 newV = PhysicsCalculator::SEcalculateV(ent->getVelocity(), a, tStep) * DAMPING_FACTOR;

	if (newV.Length() < MIN_VELOCITY) {
		newV = Vector3(0, 0, 0);
	}

	ent->setVelocity(newV);
	
	ent->setPosition(PhysicsCalculator::SEcalculateS(ent->getPosition(), ent->getVelocity(), tStep));




}

void PhysicsLoop::updatePhysics(float tStep)
{
	//apply gravity to spheres
	if (sphere != nullptr && fluid != nullptr) {
		updateEntityTstep(sphere, tStep);

	}
	
		//collision detection (has sphere collided with fluid)?

		//collision resolution 
		//push fluid down, apply force from fluid back up against sphere
}


//to be called after displacement from sphere 
Vector3 PhysicsLoop::ForceFromFluidColumn(HeightMap * fluid, int index, float displacement)
{
	float force;
	force = -displacement * (fluid->getVertice(index).y*fluid->getVertice(index).y) * FLUID_DENSITY * gravity;
	return Vector3(0,force,0);
}

bool PhysicsLoop::SphereInColumn(Vector3 column, Vector3 sphereCentre, float radius)
{
	Vector3 distance = sphereCentre - column;
	float squaredDistance = Vector3::Dot(distance, distance);
	float difference = squaredDistance - radius*radius;
	if (squaredDistance > radius*radius) {
		return false;
	}
	else  true;
}

void PhysicsLoop::SphereFluidCollision(Entity * sphere, HeightMap* fluid)
{
	//check whether sphere is in column

	//loop through all potential columns there could be a collision
	float centreX = sphere->getPosition().x;
	float centreZ = sphere->getPosition().z;


	int xFrom = centreX - sphere->getRadius();
	int zFrom = centreZ - sphere->getRadius();


	//re work this to go from centre of circle to outside...
	//x = xfrom; x <= centrex; ++x
	//z = zfrom; z <= centrez: ++z



	for (int x = centreX; x >= xFrom; --x)
	{
		for (int z = centreZ; z >= zFrom; --z)
		{
			// we don't have to take the square root, it's slow
			if ((x - centreX)*(x - centreX) + (z - centreZ)*(z - centreZ) < sphere->getRadius()*sphere->getRadius())
			{

				int xSym = centreX - (x - centreX);
				int zSym = centreZ - (z - centreZ);

				
				// (x, z), (x, zSym), (xSym , z), (xSym, zSym) are in the bounding circle, check their collisions with fluid
				int index1 = x + z*fluid->getHeight();
				int index2 = x + zSym*fluid->getHeight();
				int index3 = xSym + z*fluid->getHeight();
				int index4 = xSym + zSym*fluid->getHeight();
				//apply collision resolution if these are true 
				if(SphereInColumn(fluid->getVertice(index1), sphere->getPosition(), sphere->getRadius())) SphereColumnResolution(sphere, fluid, index1);
				if(SphereInColumn(fluid->getVertice(index2), sphere->getPosition(), sphere->getRadius())) SphereColumnResolution(sphere, fluid, index2);
				if(SphereInColumn(fluid->getVertice(index3), sphere->getPosition(), sphere->getRadius())) SphereColumnResolution(sphere, fluid, index3);
				if(SphereInColumn(fluid->getVertice(index4), sphere->getPosition(), sphere->getRadius())) SphereColumnResolution(sphere, fluid, index4);

			}
		}
	}

}

void PhysicsLoop::SphereColumnResolution(Entity* sphere, HeightMap* fluid, int index)
{
	
	//need to push fluid below circumference of sphere

	//hyp is radius
	float hyp = sphere->getRadius();
	//op is x,z difference between point and sphere centre
	
	Vector2 sphereTofluid = Vector2(sphere->getPosition().x, sphere->getPosition().z) - Vector2(fluid->getVertice(index).x, fluid->getVertice(index).z);
	float op = sphereTofluid.Length();

	//now use trig to work out d
	float angle = asin(op/hyp);
	float adj = hyp*cos(angle);


	//fluid displacement = fluid height - adj
	
	float fluidD = fluid->getVertice(index).y - adj;
	if (fluidD > 0) {
		fluid->updateVelocity(index, -fluidD);
		sphereTofluid.Normalise();



		Vector3 pointA;
		Vector3 pointB;


		//determine 2 coords to draw line between to evenly disperse fluid
		if (sphereTofluid.x < 0) {
			int step = 1;
			while (SphereInColumn(fluid->getVertice(index + step), sphere->getPosition(), sphere->getRadius())) {
				step++;
			}

			//now we have one point to interpolate from
			pointA = fluid->getVertice(index + step);


			//fluid->updateVelocity(index + step, fluidD/4);
			//fluid->setVertice(index + step, fluid->getVertice(index + step).y + fluidD / 2);
		}
		else if (sphereTofluid.x > 0) {
			int step = 1;
			while (SphereInColumn(fluid->getVertice(index - step), sphere->getPosition(), sphere->getRadius())) {
				step++;
			}

			//now we have one point to interpolate from
			pointA = fluid->getVertice(index + step);


			//fluid->updateVelocity(index - step, fluidD / 4);
			//fluid->setVertice(index - step, fluid->getVertice(index - step).y + fluidD / 2);

		}
		else {
			pointA = fluid->getVertice(index);
		}

		if (sphereTofluid.y < 0) {
			int step = 1;
			while (SphereInColumn(fluid->getVertice(index + (fluid->getHeight()* step)), sphere->getPosition(), sphere->getRadius())) {
				step++;
			}


			//now we have another point to interpolate from
			pointB = fluid->getVertice(index + (fluid->getHeight()* step));


			//fluid->updateVelocity(index + (fluid->getHeight()* step), fluidD / 4);
			//fluid->setVertice(index + (fluid->getHeight()* step), fluid->getVertice(index + (fluid->getHeight()* step)).y + fluidD / 2);
		}
		else if (sphereTofluid.y > 0) {
			int step = 1;
			while (SphereInColumn(fluid->getVertice(index - (fluid->getHeight()* step)), sphere->getPosition(), sphere->getRadius())) {
				step++;
			}

			//now we have another point to interpolate from
			pointB = fluid->getVertice(index - (fluid->getHeight()* step));


			//fluid->updateVelocity(index - (fluid->getHeight()* step), fluidD / 4);
			//fluid->setVertice(index - (fluid->getHeight()* step), fluid->getVertice(index - (fluid->getHeight()* step)).y + fluidD / 2);
		}
		else {
			pointB = fluid->getVertice(index);
		}

		//use bresenhams line algorithm to distribute fluid across line evenly
		distributefluid(fluid, pointA, pointB, fluidD);










		//now apply force back against sphere!!!
		//try apply in direction of sphere centre? 
		Vector3 sphereToFluid = sphere->getPosition() - fluid->getVertice(index);
		sphereToFluid.Normalise();


		sphere->setForce(sphere->getforce() + (ForceFromFluidColumn(fluid, index, fluidD)*sphereToFluid));
	}
	




	//push fluid away from sphere centre?
	//get direction vector away from centre
	

}

void PhysicsLoop::distributefluid(HeightMap * fluid, Vector3 pointA, Vector3 pointB, float fluidD)
{
	Vector3 direction = pointB - pointA;

	int xDir = (direction.x < 0.0f) ? -1 : 1;
	int zDir = (direction.z < 0.0f) ? -1 : 1;

	int x = (int)pointA.x;
	int z = (int)pointA.z;

	int* target = NULL; //increment on error overfliow, x or z?
	int* scan = NULL; //increment every iteration, x or z?
	int scanVal = 0;
	int targetVal = 0;

	float slope = 0.0f;
	int range = 0;

	if (abs(direction.z) > abs(direction.x)) {
		slope = (direction.x / direction.z);
		range = (int)abs(direction.z);

		target = &x;
		scan = &z;
		scanVal = zDir;
		targetVal = xDir;
	}
	else {
		slope = (direction.z / direction.x);
		range = (int)abs(direction.x);

		target = &z;
		scan = &x;
		scanVal = xDir;
		targetVal = zDir;
	}

	float absSlope = abs(slope);
	float error = 0.0f;


	float fluiadded = 0;
	
	for (int i = 0; i < range; ++i) {
		//distribute fluid
		int index = x + (z*fluid->getHeight());

		fluid->updateVelocity(index , ((fluidD / range)));
		error += absSlope;
		fluiadded += (fluidD / range);
		if (error > 0.5f) {
			error -= 1.0f;
			(*target) += targetVal;
		}
		(*scan) += scanVal;

	}

}






