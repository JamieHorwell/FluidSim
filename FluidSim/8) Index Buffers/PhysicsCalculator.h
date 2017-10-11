#pragma once
#include "../nclgl/Vector3.h"
class PhysicsCalculator
{
public:
	PhysicsCalculator();
	~PhysicsCalculator();

	static Vector3 calculateAcceleration(Vector3 f, float m);


	//calculate velocity
	static Vector3 CalculateV(Vector3 ux, Vector3 ax, float t);
	static Vector3 CalculateS(Vector3 ux, Vector3 ax, float t);

	//Explicit Euler
	static Vector3 EEcalculateV(Vector3 vx, Vector3 ax, float tStep);
	static Vector3 EEcalculateS(Vector3 sx, Vector3 vx, float tStep);


	//Implicit Euler
	static Vector3 IEcalculateV(Vector3 vx, Vector3 ax1, float tStep);
	static Vector3 IEcalculateS(Vector3 sx, Vector3 vx1, float tStep);


	//Semi-Implicit Euler
	static Vector3 SEcalculateV(Vector3 vx, Vector3 ax, float tStep);
	static Vector3 SEcalculateS(Vector3 sx, Vector3 vx1, float tStep);

	static Vector3 VIcalculateS(Vector3 sx, Vector3 sxPrev, Vector3 ax, Vector3 tStep);


	//damping factor
	static Vector3 applyDamping(Vector3 vx, float df);


};

