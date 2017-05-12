#include <imgui\imgui.h>
#include <imgui\imgui_impl_glfw_gl3.h>
#include <GL\glew.h>
#include <glm\gtc\type_ptr.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <cstdio>
#include <iostream>
#include <time.h>

bool show_test_window = false;

bool start = false;
bool restart = true;

glm::vec3 gravity = glm::vec3(0, -9.8, 0);

float counter = 0.f;
float epsilon = 1.f;
float vert[24];



static void ShowHelpMarker(const char* desc)
{
	ImGui::TextDisabled("(?)");
	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(450.0f);
		ImGui::TextUnformatted(desc);
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
}

#if true
namespace Sphere {
	extern bool renderSphere = false;
	extern float spherePos[3] = { 0.f, 1.f, 0.f };
	extern float sphereRadius = 1.f;
	extern float sphereColor[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
	extern void setupSphere(float pos[3]);
	extern void cleanupSphere();
	extern void updateSphere(float pos[3]);
	extern void drawSphere();
}

namespace Capsule {
	extern bool renderCapsule = false;
	extern float capsulePosA[3] = { -3.f, 2.f, -2.f }, capsulePosB[3] = { -4.f, 2.f, 2.f };
	extern float capsuleRadius = 1.f;
	extern float capsuleColor[4] = { 0.0f, 1.0f, 0.0f, 1.0f };
	extern void setupCapsule(float posA[3], float posB[3]);
	extern void cleanupCapsule();
	extern void updateCapsule(float posA[3], float posB[3]);
	extern void drawCapsule();
}

namespace LilSpheres {
	extern bool renderParticles = false;
	extern int maxParticles = 1000;
	float *partPos = new float[LilSpheres::maxParticles * 3];
	float *partLastPos = new float[LilSpheres::maxParticles * 3];
	float *partVel = new float[LilSpheres::maxParticles * 3];
	float *partLife = new float[LilSpheres::maxParticles];
	int tempFirst = 0;
	int first = 0, last = 0;
	extern float gravity = -9.81f;
	extern float vel = 1.0f;
	extern float particlesRadius = .05f;
	extern float lifetime = 3.f;
	extern float bounceCoeficient = .0f, frictionCoeficient = .0f;
	extern int solver = 0; //0 Euler - 1 Verlet
	extern int form = 1; //0 Cascade - 1 Fountain
	extern float particlesFocus[3] = { 0.0f, 5.0f, 0.0f };
	extern float particlesColor[4] = { 0.0f, 0.0f, 1.0f, 1.0f };
	extern void eulerSolver(float dt);
	extern void verletSolver(float dt);
	extern void setupParticles(int numTotalParticles);
	extern void cleanupParticles();
	extern void updateParticles(int startIdx, int count, float* array_data);
	extern void drawParticles(int startIdx, int count);
}

namespace ForceField {
	extern bool enableForce = false;
	extern float forceFocus[3] = { 0.0f, 7.0f, 0.0f };
	extern float force = 0.1f;
	extern float forceRadius = 1.0f;

	extern int type = 0; //0 Repulsor - 1 Attractor - 2 PulseRepulse
}

namespace ClothMesh {
	extern bool renderCloth = true;
	extern const int numCols;
	extern const int numRows;
	extern const int numVerts;
	glm::vec3 *meshVertPos = new glm::vec3[numVerts];
	glm::vec3 *meshVertPosLast = new glm::vec3[numVerts];
	extern void updateClothMesh(glm::vec3* array_data);
	void setVertexClothMesh(float x, float y, float z);
	void verletSolver(float dt);
}

using namespace ClothMesh;

void LilSpheres::eulerSolver(float dt) {

	LilSpheres::last += 3;
	if (LilSpheres::last >= LilSpheres::maxParticles) LilSpheres::last -= LilSpheres::maxParticles; //reset the pointer last to 0
	
	LilSpheres::tempFirst = LilSpheres::first; //asign temp first value

	//When last is over First
	if (LilSpheres::last >= LilSpheres::first) { 
		for (int i = LilSpheres::first; i < LilSpheres::last; ++i) { //pointer last over first
			//Calc Velocities
			LilSpheres::partVel[i * 3 + 0] += dt * 0;
			LilSpheres::partVel[i * 3 + 1] += dt * gravity;
			LilSpheres::partVel[i * 3 + 2] += dt * 0;
			//Calc Positions
			LilSpheres::partPos[i * 3 + 0] += dt * LilSpheres::partVel[i * 3 + 0];
			LilSpheres::partPos[i * 3 + 1] += dt * LilSpheres::partVel[i * 3 + 1];
			LilSpheres::partPos[i * 3 + 2] += dt * LilSpheres::partVel[i * 3 + 2];
			//LifeTime calc
			LilSpheres::partLife[i] += dt;
			if (LilSpheres::partLife[i] >= LilSpheres::lifetime) { //reset the particle to initial pos
				LilSpheres::tempFirst++;
				//init Positions
				LilSpheres::partPos[i * 3 + 0] = LilSpheres::particlesFocus[0];
				LilSpheres::partPos[i * 3 + 1] = LilSpheres::particlesFocus[1];
				LilSpheres::partPos[i * 3 + 2] = LilSpheres::particlesFocus[2];
				//Init Velocities
				if (LilSpheres::form == 0) { //Cascade
					LilSpheres::partVel[i * 3 + 0] = (((float)rand() / RAND_MAX) * 6.0f) - 2.0f * LilSpheres::vel;
					LilSpheres::partVel[i * 3 + 1] = -5.0f * LilSpheres::vel;
					LilSpheres::partVel[i * 3 + 2] = (((float)rand() / RAND_MAX) * 6.0f) - 2.0f * LilSpheres::vel;
				}
				else if (LilSpheres::form == 1) { //Font
					LilSpheres::partVel[i * 3 + 0] = (((float)rand() / RAND_MAX) * 6.0f) - 2.0f * LilSpheres::vel;
					LilSpheres::partVel[i * 3 + 1] = 7.0f * LilSpheres::vel;
					LilSpheres::partVel[i * 3 + 2] = (((float)rand() / RAND_MAX) * 6.0f) - 2.0f * LilSpheres::vel;
				}
				LilSpheres::partLife[i] = 0;
			}
		}
	} 
	//When last is under First
	else if (LilSpheres::last < LilSpheres::first) { //pointer last is under first
		for (int i = LilSpheres::first; i < LilSpheres::maxParticles; ++i) {
			//Calc Velocities
			LilSpheres::partVel[i * 3 + 0] += dt * 0;
			LilSpheres::partVel[i * 3 + 1] += dt * gravity;
			LilSpheres::partVel[i * 3 + 2] += dt * 0;
			//Calc Positions
			LilSpheres::partPos[i * 3 + 0] += dt * LilSpheres::partVel[i * 3 + 0];
			LilSpheres::partPos[i * 3 + 1] += dt * LilSpheres::partVel[i * 3 + 1];
			LilSpheres::partPos[i * 3 + 2] += dt * LilSpheres::partVel[i * 3 + 2];
			//LifeTime calc
			LilSpheres::partLife[i] += dt;
			if (LilSpheres::partLife[i] >= LilSpheres::lifetime) { //reset the particle to initial pos
				LilSpheres::tempFirst++;
				//init Positions
				LilSpheres::partPos[i * 3 + 0] = LilSpheres::particlesFocus[0];
				LilSpheres::partPos[i * 3 + 1] = LilSpheres::particlesFocus[1];
				LilSpheres::partPos[i * 3 + 2] = LilSpheres::particlesFocus[2];
				//Init Velocities
				if (LilSpheres::form == 0) { //Cascade
					LilSpheres::partVel[i * 3 + 0] = (((float)rand() / RAND_MAX) * 6.0f) - 2.0f * LilSpheres::vel;
					LilSpheres::partVel[i * 3 + 1] = -5.0f * LilSpheres::vel;
					LilSpheres::partVel[i * 3 + 2] = (((float)rand() / RAND_MAX) * 6.0f) - 2.0f * LilSpheres::vel;
				}
				else if (LilSpheres::form == 1) { //Font
					LilSpheres::partVel[i * 3 + 0] = (((float)rand() / RAND_MAX) * 6.0f) - 2.0f * LilSpheres::vel;
					LilSpheres::partVel[i * 3 + 1] = 7.0f * LilSpheres::vel;
					LilSpheres::partVel[i * 3 + 2] = (((float)rand() / RAND_MAX) * 6.0f) - 2.0f * LilSpheres::vel;
				}
				LilSpheres::partLife[i] = 0;
			}
		}
		for (int i = 0; i < LilSpheres::last; ++i) {
			//Calc Velocities
			LilSpheres::partVel[i * 3 + 0] += dt * 0;
			LilSpheres::partVel[i * 3 + 1] += dt * gravity;
			LilSpheres::partVel[i * 3 + 2] += dt * 0;
			//Calc Positions
			LilSpheres::partPos[i * 3 + 0] += dt * LilSpheres::partVel[i * 3 + 0];
			LilSpheres::partPos[i * 3 + 1] += dt * LilSpheres::partVel[i * 3 + 1];
			LilSpheres::partPos[i * 3 + 2] += dt * LilSpheres::partVel[i * 3 + 2];
			//LifeTime calc
			LilSpheres::partLife[i] += dt; 
			if (LilSpheres::partLife[i] >= LilSpheres::lifetime) { //reset the particle to initial pos
				LilSpheres::tempFirst++;
				//init Positions
				LilSpheres::partPos[i * 3 + 0] = LilSpheres::particlesFocus[0];
				LilSpheres::partPos[i * 3 + 1] = LilSpheres::particlesFocus[1];
				LilSpheres::partPos[i * 3 + 2] = LilSpheres::particlesFocus[2];
				//Init Velocities
				if (LilSpheres::form == 0) { //Cascade
					LilSpheres::partVel[i * 3 + 0] = (((float)rand() / RAND_MAX) * 6.0f) - 2.0f * LilSpheres::vel;
					LilSpheres::partVel[i * 3 + 1] = -5.0f * LilSpheres::vel;
					LilSpheres::partVel[i * 3 + 2] = (((float)rand() / RAND_MAX) * 6.0f) - 2.0f * LilSpheres::vel;
				}
				else if (LilSpheres::form == 1) { //Font
					LilSpheres::partVel[i * 3 + 0] = (((float)rand() / RAND_MAX) * 6.0f) - 2.0f * LilSpheres::vel;
					LilSpheres::partVel[i * 3 + 1] = 7.0f * LilSpheres::vel;
					LilSpheres::partVel[i * 3 + 2] = (((float)rand() / RAND_MAX) * 6.0f) - 2.0f * LilSpheres::vel;
				}
				LilSpheres::partLife[i] = 0;
			}
		}
	}

	if (LilSpheres::tempFirst >= LilSpheres::maxParticles) LilSpheres::tempFirst -= LilSpheres::maxParticles; //reset the pointer tempFirst to 0
	LilSpheres::first = LilSpheres::tempFirst; //asign first value

}

void LilSpheres::verletSolver(float dt) { // TO REPAIR
	float *tempPos = LilSpheres::partPos;

	LilSpheres::last += 3;
	if (LilSpheres::last >= LilSpheres::maxParticles) LilSpheres::last -= LilSpheres::maxParticles; //reset the pointer last to 0

	if (LilSpheres::last >= LilSpheres::first) {
		for (int i = LilSpheres::first; i < LilSpheres::last; ++i) { //pointer last over first
			//Calc Velocities
			LilSpheres::partVel[i * 3 + 0] += dt * 0;
			LilSpheres::partVel[i * 3 + 1] += dt * gravity;
			LilSpheres::partVel[i * 3 + 2] += dt * 0;
			//Calc Positions
			LilSpheres::partPos[i * 3 + 0] += (LilSpheres::partPos[i * 3 + 0] - LilSpheres::partLastPos[i * 3 + 0]) + 0 * (dt*dt);
			LilSpheres::partPos[i * 3 + 1] += (LilSpheres::partPos[i * 3 + 0] - LilSpheres::partLastPos[i * 3 + 0]) + gravity * (dt*dt);
			LilSpheres::partPos[i * 3 + 2] += (LilSpheres::partPos[i * 3 + 0] - LilSpheres::partLastPos[i * 3 + 0]) + 0 * (dt*dt);
			//LifeTime calc
			LilSpheres::partLife[i] += dt;
			if (LilSpheres::partLife[i] >= LilSpheres::lifetime) { //reset the particle to initial pos
				LilSpheres::tempFirst++;
				//init Positions
				LilSpheres::partPos[i * 3 + 0] = LilSpheres::particlesFocus[0];
				LilSpheres::partPos[i * 3 + 1] = LilSpheres::particlesFocus[1];
				LilSpheres::partPos[i * 3 + 2] = LilSpheres::particlesFocus[2];
				//Init Velocities
				if (LilSpheres::form == 0) { //Cascade
					LilSpheres::partVel[i * 3 + 0] = (((float)rand() / RAND_MAX) * 6.0f) - 2.0f * LilSpheres::vel;
					LilSpheres::partVel[i * 3 + 1] = -5.0f * LilSpheres::vel;
					LilSpheres::partVel[i * 3 + 2] = (((float)rand() / RAND_MAX) * 6.0f) - 2.0f * LilSpheres::vel;
				}
				else if (LilSpheres::form == 1) { //Font
					LilSpheres::partVel[i * 3 + 0] = (((float)rand() / RAND_MAX) * 6.0f) - 2.0f * LilSpheres::vel;
					LilSpheres::partVel[i * 3 + 1] = 7.0f * LilSpheres::vel;
					LilSpheres::partVel[i * 3 + 2] = (((float)rand() / RAND_MAX) * 6.0f) - 2.0f * LilSpheres::vel;
				}
				LilSpheres::partLife[i] = 0;
			}
		}
	}
	else if (LilSpheres::last < LilSpheres::first){ //pointer last is under first
		for (int i = LilSpheres::first; i < LilSpheres::maxParticles; ++i) {
			//Calc Velocities
			LilSpheres::partVel[i * 3 + 0] += dt * 0;
			LilSpheres::partVel[i * 3 + 1] += dt * gravity;
			LilSpheres::partVel[i * 3 + 2] += dt * 0;
			//Calc Positions
			LilSpheres::partPos[i * 3 + 0] += (LilSpheres::partPos[i * 3 + 0] - LilSpheres::partLastPos[i * 3 + 0]) + 0 * (dt*dt);
			LilSpheres::partPos[i * 3 + 1] += (LilSpheres::partPos[i * 3 + 0] - LilSpheres::partLastPos[i * 3 + 0]) + gravity * (dt*dt);
			LilSpheres::partPos[i * 3 + 2] += (LilSpheres::partPos[i * 3 + 0] - LilSpheres::partLastPos[i * 3 + 0]) + 0 * (dt*dt);
			LilSpheres::partLife[i] += dt;
			if (LilSpheres::partLife[i] >= LilSpheres::lifetime) { //reset the particle to initial pos
				LilSpheres::tempFirst++;
				//init Positions
				LilSpheres::partPos[i * 3 + 0] = LilSpheres::particlesFocus[0];
				LilSpheres::partPos[i * 3 + 1] = LilSpheres::particlesFocus[1];
				LilSpheres::partPos[i * 3 + 2] = LilSpheres::particlesFocus[2];
				//Init Velocities
				if (LilSpheres::form == 0) { //Cascade
					LilSpheres::partVel[i * 3 + 0] = (((float)rand() / RAND_MAX) * 6.0f) - 2.0f * LilSpheres::vel;
					LilSpheres::partVel[i * 3 + 1] = -5.0f * LilSpheres::vel;
					LilSpheres::partVel[i * 3 + 2] = (((float)rand() / RAND_MAX) * 6.0f) - 2.0f * LilSpheres::vel;
				}
				else if (LilSpheres::form == 1) { //Font
					LilSpheres::partVel[i * 3 + 0] = (((float)rand() / RAND_MAX) * 6.0f) - 2.0f * LilSpheres::vel;
					LilSpheres::partVel[i * 3 + 1] = 7.0f * LilSpheres::vel;
					LilSpheres::partVel[i * 3 + 2] = (((float)rand() / RAND_MAX) * 6.0f) - 2.0f * LilSpheres::vel;
				}
				LilSpheres::partLife[i] = 0;
			}
		}
		for (int i = 0; i < LilSpheres::last; ++i) {
			//Calc Velocities
			LilSpheres::partVel[i * 3 + 0] += dt * 0;
			LilSpheres::partVel[i * 3 + 1] += dt * gravity;
			LilSpheres::partVel[i * 3 + 2] += dt * 0;
			//Calc Positions
			LilSpheres::partPos[i * 3 + 0] += (LilSpheres::partPos[i * 3 + 0] - LilSpheres::partLastPos[i * 3 + 0]) + 0 * (dt*dt);
			LilSpheres::partPos[i * 3 + 1] += (LilSpheres::partPos[i * 3 + 0] - LilSpheres::partLastPos[i * 3 + 0]) + gravity * (dt*dt);
			LilSpheres::partPos[i * 3 + 2] += (LilSpheres::partPos[i * 3 + 0] - LilSpheres::partLastPos[i * 3 + 0]) + 0 * (dt*dt);
			LilSpheres::partLife[i] += dt;
			if (LilSpheres::partLife[i] >= LilSpheres::lifetime) { //reset the particle to initial pos
				LilSpheres::tempFirst++;
				//init Positions
				LilSpheres::partPos[i * 3 + 0] = LilSpheres::particlesFocus[0];
				LilSpheres::partPos[i * 3 + 1] = LilSpheres::particlesFocus[1];
				LilSpheres::partPos[i * 3 + 2] = LilSpheres::particlesFocus[2];
				//Init Velocities
				if (LilSpheres::form == 0) { //Cascade
					LilSpheres::partVel[i * 3 + 0] = (((float)rand() / RAND_MAX) * 6.0f) - 2.0f * LilSpheres::vel;
					LilSpheres::partVel[i * 3 + 1] = -5.0f * LilSpheres::vel;
					LilSpheres::partVel[i * 3 + 2] = (((float)rand() / RAND_MAX) * 6.0f) - 2.0f * LilSpheres::vel;
				}
				else if (LilSpheres::form == 1) { //Font
					LilSpheres::partVel[i * 3 + 0] = (((float)rand() / RAND_MAX) * 6.0f) - 2.0f * LilSpheres::vel;
					LilSpheres::partVel[i * 3 + 1] = 7.0f * LilSpheres::vel;
					LilSpheres::partVel[i * 3 + 2] = (((float)rand() / RAND_MAX) * 6.0f) - 2.0f * LilSpheres::vel;
				}
				LilSpheres::partLife[i] = 0;
			}
		}
	}

	if (LilSpheres::tempFirst >= LilSpheres::maxParticles) LilSpheres::tempFirst -= LilSpheres::maxParticles; //reset the pointer tempFirst to 0
	LilSpheres::first = LilSpheres::tempFirst; //asign first value

	LilSpheres::partLastPos = tempPos;
}

void ClothMesh::setVertexClothMesh(float x, float y, float z) {
	float tempX = x, tempY = y, tempZ = z;

	meshVertPos[0].x = tempX; //X
	meshVertPos[0].y = tempY; //Y
	meshVertPos[0].z = tempZ; //Z

	for (int i = 1; i < numVerts; i++) {
		tempX += 0.5f;
		if (i % 14 == 0) {
			tempX = x;
			tempZ += 0.5f;
		}
		meshVertPos[i].x = tempX; //X
		meshVertPos[i].y = tempY; //Y
		meshVertPos[i].z = tempZ; //Z
		
	}
}

void ClothMesh::verletSolver(float dt) {
	glm::vec3 *meshVertNext = new glm::vec3[numVerts];
	/*
	for (int i = 0; i < numVerts; i++) {
<<<<<<< Updated upstream
		meshVertNext = meshVertPos + (meshVertPos - meshVertPosLast) + f * (dt*dt);
	}*/
		//meshVertNext = meshVertPos + (meshVertPos - meshVertPosLast) + f * (dt*dt);
	

}
#endif // false


namespace Cube {
	extern void  setupCube();
	extern void  drawCube();
	extern void cleanupCube();
	extern glm::mat4 modelMat;
	extern float cubeVerts[];
}

class Boxes {
public:

	class Plane {
	public:
		float d;
		glm::vec3 m;

		//Default Constructor
		Plane() {
			d = m.x = m.y = m.z = 0;
		}

		//Param Constructor
		Plane(float mx, float my, float mz, float d) {
			this->m.x = mx;
			this->m.y = my;
			this->m.z = mz;
			this->d = d;
		}

		//Set Plane Function
		void SetPlane(float mx, float my, float mz, float d) {
			m.x = mx;
			m.y = my;
			m.z = mz;
			this->d = d;
		}
	};

	Plane myPlanes[6];


	glm::mat3 iBody;
	float mass;

	int scale;
	glm::vec3 orgPos;
	glm::mat4 prevMod;
	glm::vec3 beforeVerts[8];
	glm::vec3 actualVerts[8];
	glm::vec3 pos;
	glm::vec3 linealMomentum;
	glm::vec3 angularMomentum;
	glm::mat3 rotation;
	glm::quat quaternion;
	glm::vec3 torque;
	glm::mat3 inverse;

	Boxes(glm::vec3 orgPos, int s) {
		this->orgPos = orgPos;
		pos = orgPos;
		scale = s;
		mass = scale;

		iBody[0][0] = iBody[1][1] = iBody[2][2] = (1.0f / 12.0f) * mass * (scale * scale + scale * scale);
		myPlanes[0].SetPlane(0.0f, 1.0f, 0.0f, 0.0f); 
		myPlanes[1].SetPlane(0.0f, 0.0f, -1.0f, -5.0f); 
		myPlanes[2].SetPlane(-1.0f, 0.0f, 0.0f, 5.0f); 
		myPlanes[3].SetPlane(0.0f, 0.0f, 1.0f, -5.0f); 
		myPlanes[4].SetPlane(1.0f, 0.0f, 0.0f, 5.0f);
		myPlanes[5].SetPlane(0.0f, -1.0f, 0.0f, 10.0f); 
	}

	void Restart(float dt) {
		srand(time(NULL));

		pos = orgPos;

		linealMomentum = angularMomentum = glm::vec3(0);

		glm::vec3 force;
		force.x = (float)rand() / RAND_MAX * 750 - 250;
		force.y = (float)rand() / RAND_MAX * 1500;
		force.z = (float)rand() / RAND_MAX * 750 - 250;
		linealMomentum = linealMomentum + (force + (mass * gravity)) * dt;

		glm::vec3 forcePos;

		forcePos.x = (float)rand() / RAND_MAX * 2 - 1;
		forcePos.y = (float)rand() / RAND_MAX * 2 - 1;
		forcePos.z = (float)rand() / RAND_MAX * 2 - 1;
		
		torque = glm::cross(forcePos - pos, force);
		angularMomentum = angularMomentum + torque * dt;
		torque = glm::vec3(0);
	}
	
	void Bounce(Plane plane, float dt, glm::vec3 collision) {
		float relVel;
		glm::vec3 derPA;

		derPA = linealMomentum / mass + glm::cross((inverse * glm::vec3(angularMomentum.x, angularMomentum.y, angularMomentum.z)), glm::vec3(Cube::modelMat*glm::vec4(collision, 1)) - glm::vec3((Cube::modelMat*glm::vec4(0, 0, 0, 1))));

		relVel = glm::dot(plane.m, derPA);

		glm::vec3 position = glm::vec3(Cube::modelMat*glm::vec4(collision.x, collision.y, collision.z, 1.0f));

		glm::vec3 var1 = glm::cross(position, plane.m);
		glm::vec3 var2 = inverse*var1;
		glm::vec3 var3 = glm::cross(var2, position);

		float j = (-(1 + epsilon) * relVel) / (1 / mass + glm::dot(plane.m, var3));
		glm::vec3 vec = j * plane.m;
		glm::vec3 torque = glm::cross(position, vec);

		linealMomentum += vec;
		angularMomentum += torque;

	}

	void Checker(float dt, glm::vec3 prevVert[], glm::vec3 currentVert[]) {
		for (int j = 0; j < 8; j++) { 
			for (int i = 0; i < 6; i++) { 
				if ((glm::dot(myPlanes[i].m, prevVert[j]) + myPlanes[i].d)*(glm::dot(myPlanes[i].m, currentVert[j]) + myPlanes[i].d) <= 0) {
					EulerSolver(dt, i, j);
					Bounce(myPlanes[i], dt, currentVert[j]);
				}
			}
		}
	}

	void PositionRotation(float dt) {
		linealMomentum = linealMomentum + dt * (mass * gravity);
		angularMomentum = angularMomentum + dt * torque;
		glm::vec3 vel = linealMomentum / mass;
		pos = pos + vel * dt;
		inverse = rotation * glm::inverse(iBody) * glm::transpose(rotation);
		glm::vec3 angularVel = inverse * glm::vec3(angularMomentum.x, angularMomentum.y, angularMomentum.z);
		glm::mat3 angularMatrix{ 
			0, angularVel.z, -angularVel.y,
			-angularVel.z, 0,angularVel.x,
			angularVel.y,-angularVel.x,0 };
		glm::quat angularVelQuat(0, angularVel.x, angularVel.y, angularVel.z);
		quaternion = quaternion + (angularVelQuat * quaternion*0.5f)*dt;
		quaternion = glm::normalize(quaternion);
		rotation = glm::mat4_cast(quaternion);
	}

	void EulerSolver(float dt, int myPlane, int myVert) {
		float dtTracker = dt / 2;
		float myDt = dt / 2;

		glm::vec3 posNew;
		glm::vec3 linealMomentumNew;
		glm::vec3 angularMomentumNew;
		glm::mat4 rotationNew = rotation;
		glm::quat quaternionNew = quaternion;
		glm::mat3 inverseNew = inverse;
		

		for (int i = 0; i < 5; i++) {
			dtTracker = dtTracker / 2;
			linealMomentumNew = linealMomentum;
			linealMomentumNew = linealMomentumNew + myDt * (mass * gravity);
			angularMomentumNew = angularMomentum + myDt * torque;

			glm::vec3 vel = linealMomentumNew / mass;
			posNew = pos + vel * myDt;
			inverseNew = rotation * glm::inverse(iBody) * glm::transpose(rotation);

			glm::vec3 angularVel = inverse * glm::vec3(angularMomentum.x, angularMomentum.y, angularMomentum.z);

			glm::mat3 angularMatrix{ 
				0, angularVel.z, -angularVel.y,
				-angularVel.z, 0, angularVel.x,
				angularVel.y, -angularVel.x, 0 };

			glm::quat angularVelQuat(0, angularVel.x, angularVel.y, angularVel.z);
			quaternionNew = quaternion + (angularVelQuat * quaternion*0.5f)*myDt;
			quaternionNew = glm::normalize(quaternionNew);
			rotationNew = glm::mat4_cast(quaternionNew);

			glm::vec3 localVert = glm::vec3(vert[myVert * 3], vert[myVert * 3 + 1], vert[myVert * 3 + 2]);

			glm::mat4 modelMatL;
			modelMatL = glm::translate(modelMatL, posNew);

			glm::mat4 tempRot;
			tempRot = rotationNew;
			modelMatL *= tempRot;
			modelMatL = glm::scale(modelMatL, glm::vec3(scale, scale, scale));
			localVert = modelMatL * glm::vec4(localVert, 1);

			if ((glm::dot(myPlanes[myPlane].m, beforeVerts[myVert]) + myPlanes[myPlane].d)*(glm::dot(myPlanes[myPlane].m, localVert) + myPlanes[myPlane].d) <= 0) myDt -= dtTracker;
			else myDt += dtTracker;
		}

		pos = posNew;
		angularMomentum = angularMomentumNew;
		linealMomentum = linealMomentumNew;
		quaternion = quaternionNew;
		rotation = rotationNew;
		inverse = inverseNew;

		Cube::modelMat = glm::translate(Cube::modelMat, posNew);

		glm::mat4 tempRot;

		tempRot = rotationNew;

		Cube::modelMat *= tempRot;

		Cube::modelMat = glm::scale(Cube::modelMat, glm::vec3(scale, scale, scale));

		UpdateDraw();
	}

	void UpdateDraw() {
		Cube::modelMat = glm::mat4(1.f);
		Cube::modelMat = glm::translate(Cube::modelMat, pos);
		prevMod = Cube::modelMat;

		glm::mat4 tempRot;
		tempRot = rotation;

		Cube::modelMat *= tempRot;
		Cube::modelMat = glm::scale(Cube::modelMat, glm::vec3(scale, scale, scale));
	}

	void Update(float dt) {
		PositionRotation(dt);
		int count1 = 0;
		for (int i = 0; i < 8; i++) {
			for (int j = 0; j < 3; j++) {
				vert[count1] = Cube::cubeVerts[i * 6 + j];
				count1++;
			}
		}

		int count2 = 0;
		for (int i = 0; i < 24; i += 3) {
			beforeVerts[count2] = prevMod * (glm::vec4(vert[i], vert[i + 1], vert[i + 2], 1));
			count2++;
		}
		count2 = 0;

		for (int i = 0; i < 24; i += 3) {
			actualVerts[count2] = Cube::modelMat * (glm::vec4(vert[i], vert[i + 1], vert[i + 2], 1));
			count2++;
		}

		for (int i = 0; i < 8; i++) {
			if (actualVerts[i].x > 5) float offsetX = 5 - actualVerts[i].x;
			else if (actualVerts[i].x < -5) float offsetX = -5 + actualVerts[i].x;

			if (actualVerts[i].y > 5) float offsetY = 5 - actualVerts[i].y;
			else if (actualVerts[i].y < -5) float offsetY = -5 - actualVerts[i].y;

			if (actualVerts[i].z > 5) float offsetZ = 5 - actualVerts[i].z;
			else if (actualVerts[i].z < -5) float offsetZ = -5 - actualVerts[i].z;
		}

		Checker(dt, beforeVerts, actualVerts);
		UpdateDraw();

	}
};

Boxes 	myBox = Boxes(glm::vec3(0, 5, 0), 2);

void GUI() {
	{	//FrameRate
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::SliderFloat("Elasticity", &epsilon, 0, 1);
		ImGui::Text("The simulation restarts every 2.5 sec, push the button to force restart.");
		restart = ImGui::Button("Restart");
		
#if false


		//Particles
		if (ImGui::CollapsingHeader("Particles")) {
			//Enable Particles
			ImGui::Checkbox("Particles Enabled", &LilSpheres::renderParticles);
			//Max Particles
			ImGui::SliderInt("Max Particles", &LilSpheres::maxParticles, 1000, 10000);
			ImGui::SameLine(); ShowHelpMarker("CTRL+click to input value.");
			//Radius
			ImGui::SliderFloat("Particles Radius", &LilSpheres::particlesRadius, .01f, .25f, "%.3f");
			//Particles Focus Position
			ImGui::SliderFloat("Focus Position X", &LilSpheres::particlesFocus[0], (-5.0f + LilSpheres::particlesRadius), (5.0f - LilSpheres::particlesRadius), "X = %.3f");
			ImGui::SliderFloat("Focus Position Y", &LilSpheres::particlesFocus[1], (0.0f + LilSpheres::particlesRadius), (10.0f - LilSpheres::particlesRadius), "Y = %.3f");
			ImGui::SliderFloat("Focus Position Z", &LilSpheres::particlesFocus[2], (-5.0f + LilSpheres::particlesRadius), (5.0f - LilSpheres::particlesRadius), "Z = %.3f");
			//Particles Speed
			ImGui::SliderFloat("Initial Speed", &LilSpheres::vel, 0.1f, 2.0f, "%.3f");
			//Particles Lifetime
			ImGui::SliderFloat("Particles Lifetime", &LilSpheres::lifetime, 0.5f, 4.0f, "seconds = %.3f");
			//Bounce Coefficient
			ImGui::SliderFloat("Bounce Coefficient", &LilSpheres::bounceCoeficient, 0.0f, 1.0f, "%.3f");
			//Friction Coefficient
			ImGui::SliderFloat("Friction Coefficient", &LilSpheres::frictionCoeficient, 0.0f, 1.0f, "%.3f");
			//Color
			ImGui::ColorEdit4("Particles Color", LilSpheres::particlesColor);
			//Solver Method
			ImGui::RadioButton("Euler Method", &LilSpheres::solver, 0); ImGui::SameLine();
			ImGui::RadioButton("Verlet Method", &LilSpheres::solver, 1);
			//Pull Method
			ImGui::RadioButton("Cascade", &LilSpheres::form, 0); ImGui::SameLine();
			ImGui::RadioButton("Fountain", &LilSpheres::form, 1);
		}

		//Sphere
		if (ImGui::CollapsingHeader("Sphere")) {
			//Enable Sphere
			ImGui::Checkbox("Sphere Enabled", &Sphere::renderSphere);
			//Position
			//ImGui::SliderFloatPos("Sphere Position", Sphere::spherePos, -10.0f, 10.0f);
			ImGui::SliderFloat("Sphere Position X", &Sphere::spherePos[0], (-5.0f + Sphere::sphereRadius), (5.0f - Sphere::sphereRadius), "X = %.3f");
			ImGui::SliderFloat("Sphere Position Y", &Sphere::spherePos[1], (0.0f + Sphere::sphereRadius), (10.0f - Sphere::sphereRadius), "Y = %.3f");
			ImGui::SliderFloat("Sphere Position Z", &Sphere::spherePos[2], (-5.0f + Sphere::sphereRadius), (5.0f - Sphere::sphereRadius), "Z = %.3f");
			//Radius
			ImGui::SliderFloat("Sphere Radius", &Sphere::sphereRadius, .1f, 5.0f, "%.3f");
			//Color
			ImGui::ColorEdit4("Sphere Color", Sphere::sphereColor);
		}

		//Capsule
		if (ImGui::CollapsingHeader("Capsule")) {
			//Enable Capsule
			ImGui::Checkbox("Capsule Enabled", &Capsule::renderCapsule);
			//Position point A
			if (ImGui::CollapsingHeader("Position A")) {
				//ImGui::SliderFloatPos("Position A", Capsule::capsulePosA, -10.0f, 10.0f);
				ImGui::SliderFloat("A Capsule Position X", &Capsule::capsulePosA[0], (-5.0f + Capsule::capsuleRadius), (5.0f - Capsule::capsuleRadius), "X = %.3f");
				ImGui::SliderFloat("A Capsule Position Y", &Capsule::capsulePosA[1], (0.0f + Capsule::capsuleRadius), (10.0f - Capsule::capsuleRadius), "Y = %.3f");
				ImGui::SliderFloat("A Capsule Position Z", &Capsule::capsulePosA[2], (-5.0f + Capsule::capsuleRadius), (5.0f - Capsule::capsuleRadius), "Z = %.3f");
			}
			//Position point B
			if (ImGui::CollapsingHeader("Position B")) {
				//ImGui::SliderFloatPos("Position B", Capsule::capsulePosB, -10.0f, 10.0f);
				ImGui::SliderFloat("B Capsule Position X", &Capsule::capsulePosB[0], (-5.0f + Capsule::capsuleRadius), (5.0f - Capsule::capsuleRadius), "X = %.3f");
				ImGui::SliderFloat("B Capsule Position Y", &Capsule::capsulePosB[1], (0.0f + Capsule::capsuleRadius), (10.0f - Capsule::capsuleRadius), "Y = %.3f");
				ImGui::SliderFloat("B Capsule Position Z", &Capsule::capsulePosB[2], (-5.0f + Capsule::capsuleRadius), (5.0f - Capsule::capsuleRadius), "Z = %.3f");
			}
			//Radius
			ImGui::SliderFloat("Capsule Radius", &Capsule::capsuleRadius, .1f, 2.5f, "%.3f");
			//Color
			ImGui::ColorEdit4("Capsule Color", Capsule::capsuleColor);
		}

		//Force Field
		if (ImGui::CollapsingHeader("Force Field")) {
			//Enable Force Field
			ImGui::Checkbox("Force Field Enabled", &ForceField::enableForce);
			//Position
			//ImGui::SliderFloatPos("Force Field Position", ForceField::forceFocus, -10.0f, 10.0f);
			ImGui::SliderFloat("Force Field Position X", &ForceField::forceFocus[0], -5.0f, 5.0f, "X = %.3f");
			ImGui::SliderFloat("Force Field Position Y", &ForceField::forceFocus[1], 0.0f, 10.0f, "Y = %.3f");
			ImGui::SliderFloat("Force Field Position Z", &ForceField::forceFocus[2], -5.0f, 5.0f, "Z = %.3f");
			//Force
			ImGui::SliderFloat("Force", &ForceField::force, .1f, 10.0f, "%.3f");
			//Interaction Type ( Repulsion, Atraction, Pulse)
			ImGui::RadioButton("Repulsion", &ForceField::type, 0); ImGui::SameLine();
			ImGui::RadioButton("Attraction", &ForceField::type, 1); ImGui::SameLine();
			ImGui::RadioButton("Pulse", &ForceField::type, 2); 
		}

#endif // false

	}

	// ImGui test window. Most of the sample code is in ImGui::ShowTestWindow()
	if(show_test_window) {
		ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiSetCond_FirstUseEver);
		ImGui::ShowTestWindow(&show_test_window);
	}
}

void PhysicsInit() {
}

void PhysicsUpdate(float dt) {
	counter += dt;
	if (!start) {
		myBox.Restart(dt);
		start = true;
	}
	if (restart || counter >= 2.5f) {
		restart = false;
		myBox.Restart(dt);
		counter = 0.f;
	}

	myBox.Update(dt);
	Cube::drawCube();

}

void PhysicsCleanup() {
	Cube::cleanupCube();
}