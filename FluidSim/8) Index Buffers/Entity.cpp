#include "Entity.h"



Entity::Entity(Vector3 position, float mass, float radius, float scale, RenderObject * rendObj)
{
	this->rendObj = rendObj;
	setPosition(position);
	this->mass = mass;
	this->radius = radius;
	this->scale = scale;
	this->force = Vector3(0, 0, 0);
	this->velocity = Vector3(0, 0, 0);
}

Entity::Entity()
{
}


Entity::~Entity()
{
}

void Entity::setPosition(Vector3 position)
{
	this->position = position;
	this->rendObj->SetModelMatrix(Matrix4::Translation(Vector3(position.x, position.y, position.z)) * Matrix4::Scale(Vector3(scale, scale, scale)));
}
