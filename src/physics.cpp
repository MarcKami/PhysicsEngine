#include <imgui\imgui.h>
#include <imgui\imgui_impl_glfw_gl3.h>
#include <GL\glew.h>
#include <glm\gtc\type_ptr.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <cstdio>
#include <cassert>
#include "GL_framework.h"
#include <iostream>
#include <time.h>
#include <algorithm>



namespace Box {
	extern void setupCube();
	extern void cleanupCube();
	extern void drawCube();
}

namespace ClothMesh {
	extern void updateClothMesh(float* array_data);
	extern const int numCols;
	extern const int numRows;
}

namespace Sphere {
	extern void updateSphere(glm::vec3 pos, float radius = 1.f);
}

bool show_test_window = false;
bool start = false;
bool restart = false;
bool stop = false;

//Global properties
glm::vec3 gravity = glm::vec3(0.f, -9.8f, 0.f);
float timer = 0.f;
float counter = 0.f;
glm::vec3 colPos;
glm::vec3 distance = glm::vec3(0.58598f, 0.f, 0.7669f); //To fit in the box simulation
glm::vec3* arrayPos = new glm::vec3[ClothMesh::numCols*ClothMesh::numRows];
glm::vec3* arrayPosOrg = new glm::vec3[ClothMesh::numCols*ClothMesh::numRows];

//Sphere
glm::vec3 spherePos;
glm::vec3 sphereVel = glm::vec3(0);
glm::vec3 sphereAccel = glm::vec3(0);
float sphereWeight = 10.f;

//Mesh (Wave Simulation)
float waveMulti1 = 1.f;
float waveMulti2 = 1.f;
float waveMulti3 = 1.f;;
glm::vec3 waveVecX = glm::vec3(0.6f, 0.f, 0.f);
glm::vec3 waveVecZ = glm::vec3(0.f, 0.f, 0.9f);
glm::vec3 waveVecCustom = glm::vec3(0.6f, 0.f, 0.6f); 
float waveAmplitude = 0.3f;
float waveFreqX = 2.f;
float waveFreqZ = 2.f;
float waveFreqCustom = 2.f;
float liquidDensity = 10.f;


void Play() {
	if (!start) {
		srand(time(NULL));

		spherePos = glm::vec3(0, 8.7, 0);
		spherePos.x = rand() % 9 + (-4);
		spherePos.y = rand() % 2 + 7;
		spherePos.z = rand() % 9 + (-4);

		Sphere::updateSphere(spherePos);

		start = true;
		for (int i = 0; i < ClothMesh::numCols; i++) {
			for (int j = 0; j < ClothMesh::numRows; j++) {
				arrayPosOrg[j * 14 + i] = (glm::vec3(j*distance.x - 5.f, 3.5, i*distance.z - 5.f));
				arrayPos[j * 14 + i] = (glm::vec3(j*distance.x - 5.f, 3.5, i*distance.z - 5.f));
			}
		}
	}

	if (restart) {
		sphereWeight = 10.f;
		waveMulti1 = 1.f;
		waveMulti2 = 1.f;
		waveMulti3 = 1.f;
		waveAmplitude = 0.3f;
		waveFreqX = 2.f; 
		waveFreqZ = 2.f; 
		waveFreqCustom = 2.f; 

		srand(time(NULL));

		sphereVel = glm::vec3(0);
		sphereAccel = glm::vec3(0);
		spherePos.x = rand() % 9 + (-4);
		spherePos.y = rand() % 2 + 7;
		spherePos.z = rand() % 9 + (-4);
		Sphere::updateSphere(spherePos);
		timer = 0.f;

		restart = false;
		for (int i = 0; i < ClothMesh::numCols; i++) {
			for (int j = 0; j < ClothMesh::numRows; j++) {
				arrayPos[j * 14 + i] = (glm::vec3(j*distance.x - 5.f, 3.5, i*distance.z - 5.f));
			}
		}
	}
}

void GUI() {
	{	
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::DragFloat("Sphere Weight", &sphereWeight, 0.1f, 0.1f, 45.0f);
		ImGui::DragFloat("Wave X", &waveMulti1, 0.1f, 0.1, 5.0f);
		ImGui::DragFloat("Wave Y", &waveMulti2, 0.1f, 0.1, 5.0f);
		ImGui::DragFloat("Wave Custom", &waveMulti3, 0.1f, 0.1, 5.0f);
		ImGui::DragFloat("Frequency X", &waveFreqX, 0.1f, 1, 5.0f);
		ImGui::DragFloat("Frequency Y", &waveFreqZ, 0.1f, 1, 5.0f);
		ImGui::DragFloat("Frequency Custom", &waveFreqCustom, 0.1f, 1, 5.0f);
		ImGui::DragFloat("Amplitude", &waveAmplitude, 0.05f, 0.05f, 0.4f);
		ImGui::Checkbox("Stop", &stop);
		restart = ImGui::Button("Restart");
	}

	// ImGui test window. Most of the sample code is in ImGui::ShowTestWindow()
	if (show_test_window) {
		ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiSetCond_FirstUseEver);
		ImGui::ShowTestWindow(&show_test_window);
	}
}

void PhysicsInit() {
	//TODO
}

void UpdateSpherePos(float dt, glm::vec3 b, glm::vec3 drag) {
	//glm::vec3 aBuo = b / sphereMass;
	//glm::vec3 aDrag = drag / sphereMass;
	glm::vec3 fGrav = gravity * sphereWeight;
	glm::vec3 sumF = fGrav + b + drag;


	sphereAccel = sumF / sphereWeight;
	sphereVel += sphereAccel*dt;
	spherePos += sphereVel*dt;
}

void bajarArray(float* distancias, int* indices, int a) {
	for (int i = 3; i > a; i--) {
		distancias[i] = distancias[i - 1];
		indices[i] = indices[i - 1];
	}

}

void PhysicsUpdate(float dt) {

	if (!stop) {
		//void updateClothMesh(float* array_data);

		waveVecX = glm::vec3(0.6f, 0, 0) * waveMulti1;
		waveVecZ = glm::vec3(0, 0, 0.9) * waveMulti2;
		waveVecCustom = glm::vec3(0.6, 0, 0.6) * waveMulti3;



		timer += dt;

		Play();

		//movimiento de las olas
		for (int i = 0; i < ClothMesh::numCols*ClothMesh::numRows; i++) {

			glm::vec2 orgX = glm::vec2(arrayPosOrg[i].x, arrayPosOrg[i].z);
			glm::vec2 currentX;
			currentX = orgX - (glm::normalize(glm::vec2(waveVecX.x, waveVecX.z)) * waveAmplitude * glm::sin(glm::dot(glm::vec2(waveVecX.x, waveVecX.z), orgX) - waveFreqX * timer)
				+ glm::normalize(glm::vec2(waveVecZ.x, waveVecZ.z)) * waveAmplitude * glm::sin(glm::dot(glm::vec2(waveVecZ.x, waveVecZ.z), orgX) - waveFreqZ * timer)
				+ glm::normalize(glm::vec2(waveVecCustom.x, waveVecCustom.z)) * waveAmplitude * glm::sin(glm::dot(glm::vec2(waveVecCustom.x, waveVecCustom.z), orgX) - waveFreqCustom * timer)
				);

			float currentY = 5 + (waveAmplitude * glm::cos(glm::dot(glm::vec2(waveVecX.x, waveVecX.z), orgX) - waveFreqX * timer) +
				waveAmplitude * glm::cos(glm::dot(glm::vec2(waveVecZ.x, waveVecZ.z), orgX) - waveFreqZ * timer)/* + otras olas*/ +
				waveAmplitude * glm::cos(glm::dot(glm::vec2(waveVecCustom.x, waveVecCustom.z), orgX) - waveFreqCustom * timer)
				);

			arrayPos[i] = glm::vec3(currentX.x, currentY, currentX.y);

		}

		//Inicializamos los indices a -1
		int closestVertexIndex[4] = { -1, -1, -1, -1 };
		float closestDistances[4] = { 20,20,20,20 };

		for (int i = 0; i < ClothMesh::numCols*ClothMesh::numRows; i++) {

			float distanciaActual = glm::distance(arrayPos[i], spherePos) - 1;

			if (distanciaActual < (closestDistances[0])) {

				bajarArray(closestDistances, closestVertexIndex, 0);
				closestDistances[0] = distanciaActual;
				closestVertexIndex[0] = i;

			}
			else if ((distanciaActual < (closestDistances[1]))) {

				bajarArray(closestDistances, closestVertexIndex, 1);
				closestDistances[1] = distanciaActual;
				closestVertexIndex[1] = i;
			}

			else if (distanciaActual < (closestDistances[2])) {

				bajarArray(closestDistances, closestVertexIndex, 2);
				closestDistances[2] = distanciaActual;
				closestVertexIndex[2] = i;
			}

			else if (distanciaActual < (closestDistances[2])) {
				closestDistances[3] = distanciaActual;
				closestVertexIndex[3] = i;
			}
		}



		glm::vec3 buoyancy = glm::vec3(0, 0, 0);
		glm::vec3 fDrag = glm::vec3(0, 0, 0);

		float alturaSumergida = 0;


		//AQUI YACE EL INTENTO FALLIDO DE INTERPOLACION

		//std::sort(closestVertexIndex,closestVertexIndex+sizeof(closestVertexIndex)/sizeof(int));

		//float distanceToSphere = spherePos.x-arrayPos[closestVertexIndex[0]].x;
		//float distanceBetweenP = arrayPos[closestVertexIndex[0]].x - arrayPos[closestVertexIndex[1]].x;
		//float alpha = glm::abs(distanceToSphere / distanceBetweenP);


		//distanceToSphere = spherePos.x - arrayPos[closestVertexIndex[2]].x;
		//distanceBetweenP = arrayPos[closestVertexIndex[2]].x - arrayPos[closestVertexIndex[3]].x;
		//float beta = glm::abs(distanceToSphere / distanceBetweenP);

		//float puntoVirtual1 = glm::mix(arrayPos[closestVertexIndex[0]].y,arrayPos[closestVertexIndex[1]].y,alpha);
		//float puntoVirtual2 = glm::mix(arrayPos[closestVertexIndex[2]].y, arrayPos[closestVertexIndex[3]].y, beta);
		//
		//distanceToSphere = spherePos.z - arrayPos[closestVertexIndex[1]].z;
		//distanceBetweenP = arrayPos[closestVertexIndex[1]].z - arrayPos[closestVertexIndex[3]].z;
		//
		//alpha = glm::abs(distanceToSphere / distanceBetweenP);

		//float puntoVirtualDef = glm::mix(puntoVirtual2,puntoVirtual1, alpha);

		float alturaApprox = (arrayPos[closestVertexIndex[0]].y + arrayPos[closestVertexIndex[1]].y + arrayPos[closestVertexIndex[2]].y + arrayPos[closestVertexIndex[3]].y) / 4;

		if (spherePos.y - 1 < alturaApprox) {
			alturaSumergida = glm::clamp(glm::abs(spherePos.y - 1 - alturaApprox), 0.f, 2.f);
			float volumenSumergido = ((alturaSumergida * alturaSumergida *3.1416f) / 3) * (3 - alturaSumergida);
			buoyancy = liquidDensity * glm::length(gravity) * glm::abs(volumenSumergido) * glm::vec3(0, 1, 0);

			//fuerza de drag
			float coefDrag = 1.f;
			float crossSectionalArea;
			alturaSumergida = glm::clamp(alturaSumergida, 0.0f, 1.0f);

			crossSectionalArea = 3.1416 * glm::pow(((alturaSumergida)), 2);

			fDrag = (liquidDensity * coefDrag * crossSectionalArea * glm::length(sphereVel) * sphereVel);
			fDrag = fDrag * (-1.f / 2.f);
		}




		UpdateSpherePos(dt, buoyancy, fDrag);

		ClothMesh::updateClothMesh((float*)&arrayPos[0].x);
		Sphere::updateSphere(spherePos);

		//TODO

	}
}
void PhysicsCleanup() {
	//TODO
}