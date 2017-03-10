#include <GL\glew.h>
#include <glm\gtc\type_ptr.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <cstdio>

//Boolean variables allow to show/hide the primitives

namespace Sphere {
extern bool renderSphere;
extern float spherePos[3];
extern float sphereRadius;
extern void setupSphere(float pos[3]);
extern void cleanupSphere();
extern void updateSphere(float pos[3]);
extern void drawSphere();
}

namespace Capsule {
extern bool renderCapsule;
extern float capsulePosA[3], capsulePosB[3];
extern float capsuleRadius;
extern void setupCapsule(float posA[3], float posB[3]);
extern void cleanupCapsule();
extern void updateCapsule(float posA[3], float posB[3]);
extern void drawCapsule();
}

namespace LilSpheres {
extern float particlesFocus[3];
extern bool renderParticles;
extern int maxParticles;
extern float particlesRadius;
extern void setupParticles(int numTotalParticles);
extern void cleanupParticles();
extern void updateParticles(int startIdx, int count, float* array_data);
extern void drawParticles(int startIdx, int count);
}

void setupPrims() {
	Sphere::setupSphere(Sphere::spherePos);
	Capsule::setupCapsule(Capsule::capsulePosA, Capsule::capsulePosB);

	//TODO
	//You define how many particles will be in the simulation (maxParticles number in render.cpp is defined to SHRT_MAX, 
	//	you can change it if you want, but be aware of troubled outcomes, 
	//	like having to create multiple buffers because of interger overflow...)
	//Link the parameter of setupParticles to the max number of particles in the physics simulation you want to have
	LilSpheres::setupParticles(LilSpheres::maxParticles);
	//
	//
}
void cleanupPrims() {
	Sphere::cleanupSphere();
	Capsule::cleanupCapsule();
	LilSpheres::cleanupParticles();
}

void renderPrims() {
	if(Sphere::renderSphere)
		Sphere::drawSphere();
	if(Capsule::renderCapsule)
		Capsule::drawCapsule();

	//TODO drawParticles can only draw a contiguous amount of particles in its array from start idx to idx+count
	//Depending the alive particles that have to be rendered, you may need to do multiple calls for this function
	if(LilSpheres::renderParticles)
		LilSpheres::drawParticles(0, LilSpheres::maxParticles);
	//
}
