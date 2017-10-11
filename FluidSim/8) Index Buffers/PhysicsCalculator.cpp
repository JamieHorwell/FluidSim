#include "PhysicsCalculator.h"



PhysicsCalculator::PhysicsCalculator()
{
}


PhysicsCalculator::~PhysicsCalculator()
{
}

Vector3 PhysicsCalculator::calculateAcceleration(Vector3 f, float m)
{
	return f / m;
}

Vector3 PhysicsCalculator::CalculateV(Vector3 u, Vector3 a, float t)
{
	return u + (a*t);
}

Vector3 PhysicsCalculator::CalculateS(Vector3 u, Vector3 a, float t)
{
	return (u*t) + (a*(t*t))*0.5;

}

Vector3 PhysicsCalculator::EEcalculateV(Vector3 vx, Vector3 ax, float tStep)
{
	return vx + (ax*tStep);
}

Vector3 PhysicsCalculator::EEcalculateS(Vector3 sx, Vector3 vx, float tStep)
{
	return sx + (vx*tStep);
}

Vector3 PhysicsCalculator::IEcalculateV(Vector3 vx, Vector3 ax1, float tStep)
{
	return vx + ax1*tStep;
}

Vector3 PhysicsCalculator::IEcalculateS(Vector3 sx, Vector3 vx1, float tStep)
{
	return sx + vx1*tStep;
}

Vector3 PhysicsCalculator::SEcalculateV(Vector3 vx, Vector3 ax, float tStep)
{
	return vx + ax*tStep;
}

Vector3 PhysicsCalculator::SEcalculateS(Vector3 s, Vector3 vx1, float tStep)
{
	return s + vx1*tStep;
}

Vector3 PhysicsCalculator::VIcalculateS(Vector3 sx, Vector3 sxPrev, Vector3 ax, Vector3 tStep)
{
	return sx + (sx - sxPrev) + ax*(tStep*tStep);
}

Vector3 PhysicsCalculator::applyDamping(Vector3 vx, float df)
{
	return vx * df;
}


