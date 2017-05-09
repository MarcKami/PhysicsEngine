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

float epsilon;
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

namespace Box {
	extern void setupCube();
	extern void cleanupCube();
	extern void drawCube();
}

class Boxes {
public:

	class Plane {
	public:
		float d;
		glm::vec3 n;
		Plane() {
			d = n.x = n.y = n.z = 0;
		}
		Plane(float nx, float ny, float nz, float d) {
			this->n.x = nx;
			this->n.y = ny;
			this->n.z = nz;
			this->d = d;
		}
		void SetPlaneStats(float nx, float ny, float nz, float d) {
			n.x = nx;
			n.y = ny;
			n.z = nz;
			this->d = d;
		}
	};
	Plane planos[6];


	//Constantes
	glm::mat3 iBody;
	float mass;
	//Variables
	glm::vec3 orgPos;
	glm::mat4 prevMod;
	glm::vec3 pos;
	glm::quat quaternion;
	glm::vec3 linealMom;
	glm::vec3 angMom;
	glm::vec3 torque;
	glm::mat3 rotation;
	int escalado;
	glm::mat3 inverseInertia;
	glm::vec3 verticesAnteriores[8];
	glm::vec3 verticesActuales[8];
	//REINICIO
	void Restart(float dt) {

		linealMom = angMom = glm::vec3(0);

		pos = orgPos;

		srand(time(NULL));
		glm::vec3 tempForce;
		tempForce.x = (float)rand() / RAND_MAX * 1000 - 500;
		tempForce.y = (float)rand() / RAND_MAX * 2000;
		tempForce.z = (float)rand() / RAND_MAX * 1000 - 500;
		linealMom = linealMom + ((gravity*mass) + tempForce)*dt;

		glm::vec3 forcePos;

		//(float)rand() / RAND_MAX;
		forcePos.x = (float)rand() / RAND_MAX * 2 - 1;
		forcePos.y = (float)rand() / RAND_MAX * 2 - 1;
		forcePos.z = (float)rand() / RAND_MAX * 2 - 1;

		//std::cout << tempForce.x << " " << tempForce.y << " " << tempForce.z << "\n";

		torque = glm::cross(forcePos - pos, tempForce);
		angMom = angMom + torque*dt;

		torque = glm::vec3(0);
	}

	//CONSTRUCTOR
	Boxes(glm::vec3 orgPos, int s) {
		this->orgPos = orgPos;
		pos = orgPos;
		escalado = s;
		mass = escalado;

		//iBody es la misma en las tres partes de la matriz porque es un cubo y los lados son iguales
		iBody[0][0] = iBody[1][1] = iBody[2][2] = (1.0f / 12.0f)*mass*(escalado*escalado + escalado*escalado);
		planos[0].SetPlaneStats(0.0f, 1.0f, 0.0f, 0.0f); // Parte abajo del cubo
		planos[1].SetPlaneStats(0.0f, 0.0f, -1.0f, -5.0f); //Parte del fondo
		planos[2].SetPlaneStats(-1.0f, 0.0f, 0.0f, 5.0f); //Parte izquierda
		planos[3].SetPlaneStats(0.0f, 0.0f, 1.0f, -5.0f); //Parte de delante
		planos[4].SetPlaneStats(1.0f, 0.0f, 0.0f, 5.0f); //Parte derechas
		planos[5].SetPlaneStats(0.0f, -1.0f, 0.0f, 10.0f); //Parte de arriba
	}

	//ACTUALIZACIÓN DEL PINTADO
	void UpdateDraw() {


		Cube::modelMat = glm::mat4(1.f);

		Cube::modelMat = glm::translate(Cube::modelMat, pos);

		prevMod = Cube::modelMat;


		glm::mat4 tempRot;

		tempRot = rotation;

		Cube::modelMat *= tempRot;

		Cube::modelMat = glm::scale(Cube::modelMat, glm::vec3(escalado, escalado, escalado));

	}

	void Bounce(Plane plano, float dt, glm::vec3 verticeEnContacto) {
		float velocidadRelativa;
		glm::vec3 derivadaPA;

		derivadaPA = linealMom / mass + glm::cross((inverseInertia * glm::vec3(angMom.x, angMom.y, angMom.z)), glm::vec3(Cube::modelMat*glm::vec4(verticeEnContacto, 1)) - glm::vec3((Cube::modelMat*glm::vec4(0, 0, 0, 1))));

		velocidadRelativa = glm::dot(plano.n, derivadaPA);

		glm::vec3 posicionDeVertice = glm::vec3(Cube::modelMat*glm::vec4(verticeEnContacto.x, verticeEnContacto.y, verticeEnContacto.z, 1.0f));

		glm::vec3 parentesis1 = glm::cross(posicionDeVertice, plano.n);
		glm::vec3 parentesis2 = inverseInertia*parentesis1;
		glm::vec3 parentesis3 = glm::cross(parentesis2, posicionDeVertice);

		float laJota = (-(1 + epsilon)*velocidadRelativa) / (1 / mass + glm::dot(plano.n, parentesis3));

		glm::vec3 vecJota = laJota*plano.n;

		glm::vec3 torqueNuevo = glm::cross(posicionDeVertice, vecJota);

		linealMom += vecJota;

		angMom += torqueNuevo;

	}

	void CheckCol(float dt, glm::vec3 prevVert[], glm::vec3 curVert[]) {
		for (int j = 0; j < 8; j++) {
			for (int i = 0; i < 6; i++) {

				if ((glm::dot(planos[i].n, prevVert[j]) + planos[i].d)*(glm::dot(planos[i].n, curVert[j]) + planos[i].d) <= 0) {

					//glm::mat4 temp;

					simulateEulerStep(dt, i, j);

					Bounce(planos[i], dt, curVert[j]);

					//Cube::modelMat = temp;


				}
			}
		}
	}

	//ACTUALIZADO
	void Update(float dt) {
		
		UpdatePosAndRot(dt);
		int contador = 0;

		for (int i = 0; i < 8; i++) {
			for (int j = 0; j < 3; j++) {
				vert[contador] = Cube::cubeVerts[i * 6 + j];
				contador++;
			}
		}


		int contador2 = 0;
		for (int i = 0; i < 24; i += 3) {

			//			for(int k = 0;k<3;k++){
			//				for (int j = 0; j < 3; j++){
			//				std::cout << prevMod[k][j] << " ";
			//}
			//				std::cout<<"\n";
			//			}
			verticesAnteriores[contador2] = prevMod * (glm::vec4(vert[i], vert[i + 1], vert[i + 2], 1));
			contador2++;
		}

		contador2 = 0;
		for (int i = 0; i < 24; i += 3) {

			//std::cout << verticesActuales[i].x << " " << verticesActuales[i].y << " " << verticesActuales[i].z << std::endl;

			verticesActuales[contador2] = Cube::modelMat * (glm::vec4(vert[i], vert[i + 1], vert[i + 2], 1));
			contador2++;
		}

		//for (int i = 0; i < 24; i++) {
		//	//std::cout << verticesActuales[i].x << " " << verticesActuales[i].y << " " << verticesActuales[i].z << std::endl;

		//}

		for (int i = 0; i < 8; i++) {
			if (verticesActuales[i].x > 5) {
				float offsetX = 5 - verticesActuales[i].x;
			}
			else if (verticesActuales[i].x < -5) {
				float offsetX = -5 + verticesActuales[i].x;
			}

			if (verticesActuales[i].y > 5) {
				float offsetY = 5 - verticesActuales[i].y;

			}
			else if (verticesActuales[i].y < -5) {
				float offsetY = -5 - verticesActuales[i].y;
			}
			if (verticesActuales[i].z > 5) {
				float offsetZ = 5 - verticesActuales[i].z;

			}
			else if (verticesActuales[i].z < -5) {
				float offsetZ = -5 - verticesActuales[i].z;

			}
		}


		CheckCol(dt, verticesAnteriores, verticesActuales);



		UpdateDraw();
	}



	void UpdatePosAndRot(float dt) {
		linealMom = linealMom + dt*(gravity*mass);
		angMom = angMom + dt*torque;
		glm::vec3 velocity = linealMom / mass;
		pos = pos + velocity*dt;
		inverseInertia = rotation * glm::inverse(iBody)*glm::transpose(rotation);
		glm::vec3 angularVelocity = inverseInertia * glm::vec3(angMom.x, angMom.y, angMom.z);
		//AL DECLARAR MATRICES EN GLM CON EL CONSTRUCTOR, RECIBE LA TRASPOSADA, ES DECIR SI QUIERES PONER UNA MATRIZ DIRECTAMENTE, LA DECLARAS TRASPOSADA
		glm::mat3 angularMatrix{ 0, angularVelocity.z, -angularVelocity.y,
			-angularVelocity.z, 0,angularVelocity.x,
			angularVelocity.y,-angularVelocity.x,0 };

		glm::quat angularSpeed4(0, angularVelocity.x, angularVelocity.y, angularVelocity.z);
		quaternion = quaternion + (angularSpeed4 * quaternion*0.5f)*dt;
		quaternion = glm::normalize(quaternion);
		rotation = glm::mat4_cast(quaternion);
	}

	void simulateEulerStep(float dt, int planoId, int vertId) {
		float newDT = dt / 2;
		float trackDT = dt / 2;
		glm::vec3 linealMomL;
		glm::vec3 angMomL;
		glm::mat3 inverseInertiaL = inverseInertia;
		glm::quat quaternionL = quaternion;
		glm::mat4 rotationL = rotation;
		glm::vec3 posL;
		for (int i = 0; i < 5; i++) {
			trackDT = trackDT / 2;
			linealMomL = linealMom;
			linealMomL = linealMomL + newDT*(gravity*mass);
			angMomL = angMom + newDT*torque;
			glm::vec3 velocity = linealMomL / mass;
			posL = pos + velocity*newDT;
			inverseInertiaL = rotation * glm::inverse(iBody)*glm::transpose(rotation);
			glm::vec3 angularVelocity = inverseInertia * glm::vec3(angMom.x, angMom.y, angMom.z);
			//AL DECLARAR MATRICES EN GLM CON EL CONSTRUCTOR, RECIBE LA TRASPOSADA, ES DECIR SI QUIERES PONER UNA MATRIZ DIRECTAMENTE, LA DECLARAS TRASPOSADA
			glm::mat3 angularMatrix{ 0, angularVelocity.z, -angularVelocity.y,
				-angularVelocity.z, 0,angularVelocity.x,
				angularVelocity.y,-angularVelocity.x,0 };

			glm::quat angularSpeed4(0, angularVelocity.x, angularVelocity.y, angularVelocity.z);
			quaternionL = quaternion + (angularSpeed4 * quaternion*0.5f)*newDT;
			quaternionL = glm::normalize(quaternionL);
			rotationL = glm::mat4_cast(quaternionL);

			glm::vec3 verticeLocal = glm::vec3(vert[vertId * 3], vert[vertId * 3 + 1], vert[vertId * 3 + 2]);

			glm::mat4 modelMatL;

			modelMatL = glm::translate(modelMatL, posL);

			glm::mat4 tempRot;

			tempRot = rotationL;

			modelMatL *= tempRot;

			modelMatL = glm::scale(modelMatL, glm::vec3(escalado, escalado, escalado));

			verticeLocal = modelMatL * glm::vec4(verticeLocal, 1);

			if ((glm::dot(planos[planoId].n, verticesAnteriores[vertId]) + planos[planoId].d)*(glm::dot(planos[planoId].n, verticeLocal) + planos[planoId].d) <= 0) {
				newDT -= trackDT;
				//std::cout << "resta\n";
			}
			else {
				newDT += trackDT;
				//std::cout << "suma\n";
			}
		}

		pos = posL;
		angMom = angMomL;
		linealMom = linealMomL;
		quaternion = quaternionL;
		rotation = rotationL;
		inverseInertia = inverseInertiaL;

		Cube::modelMat = glm::translate(Cube::modelMat, posL);

		glm::mat4 tempRot;

		tempRot = rotationL;

		Cube::modelMat *= tempRot;

		Cube::modelMat = glm::scale(Cube::modelMat, glm::vec3(escalado, escalado, escalado));





		UpdateDraw();
		//return modelMat;
	}

};

Boxes 	myBox = Boxes(glm::vec3(0, 5, 0), 2);

void GUI() {
	{	//FrameRate
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::SliderFloat("Elasticity", &epsilon, 0, 1);
		restart = ImGui::Button("Restart");
		ImGui::Separator();
		
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
	epsilon = 1.f;

}

void PhysicsUpdate(float dt) {
	if (!start) {
		myBox.Restart(dt);
		start = true;
	}
	if (restart) {
		restart = false;
		myBox.Restart(dt);
	}

	myBox.Update(dt);
	Cube::drawCube();

}

void PhysicsCleanup() {
	Cube::cleanupCube();
	Box::cleanupCube();
}