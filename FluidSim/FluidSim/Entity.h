#include "../../nclgl/Vector3.h"
#include "../../nclgl/RenderObject.h"
#include "../../nclgl/Matrix4.h"
#pragma once
class Entity1
{
public:
	Entity1(Vector3 position, float mass, float radius, float scale, RenderObject* rendObj);
	Entity1();
	~Entity1();


	inline Vector3 getPosition() { return position; };
	inline Vector3 getforce() { return force; };
	inline Vector3 getVelocity() { return velocity; };
	inline float getMass() { return mass; };
	inline float getRadius() { return radius; };
	inline RenderObject* getRendObj() { return rendObj; };

	inline void setRendObj(RenderObject* renObj) { this->rendObj = renObj; };

	void setPosition(Vector3 position);
	inline void setForce(Vector3 force) { this->force = force; };
	void setVelocity(Vector3 velocity) { this->velocity = velocity; };




private:
	Vector3 position;
	Vector3 force;
	Vector3 velocity;
	float scale;
	float mass;
	float radius;

	RenderObject* rendObj;



};

