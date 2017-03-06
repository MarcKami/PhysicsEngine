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

	//TODO
	//updateParticles is the function you can use to update the position of the particles (directly from the physics code)
	//The access is contiguous from an start idx to idx+count particles. You may need to do multiple calls.
	//Called here as an example to initialize to random values all particles inside the box. This code can be removed.
	float *partVerts = new float[LilSpheres::maxParticles * 3];
	for(int i = 0; i < LilSpheres::maxParticles; ++i) {
		partVerts[i * 3 + 0] = ((float)rand() / RAND_MAX) * 10.f - 5.f;
		partVerts[i * 3 + 1] = ((float)rand() / RAND_MAX) * 10.f;
		partVerts[i * 3 + 2] = ((float)rand() / RAND_MAX) * 10.f - 5.f;
	}
	LilSpheres::updateParticles(0, LilSpheres::maxParticles, partVerts);
	delete[] partVerts;
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
