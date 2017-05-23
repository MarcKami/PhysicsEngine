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

void GUI() {
	{	
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::SliderFloat("Sphere Weight", &sphereWeight, 0.1f, 45.0f);
		ImGui::SliderFloat("Liquid Density", &liquidDensity, .0f, 20.0f);
		ImGui::SliderFloat("Wave X", &waveMulti1, 0.1f, 5.0f);
		ImGui::SliderFloat("Wave Y", &waveMulti2, 0.1f, 5.0f);
		ImGui::SliderFloat("Wave Custom", &waveMulti3, 0.1f, 5.0f);
		ImGui::SliderFloat("Frequency X", &waveFreqX, 0.1f, 5.0f);
		ImGui::SliderFloat("Frequency Y", &waveFreqZ, 0.1f, 5.0f);
		ImGui::SliderFloat("Frequency Custom", &waveFreqCustom, 0.1f, 5.0f);
		ImGui::SliderFloat("Amplitude", &waveAmplitude, 0.05f, 0.4f);
		restart = ImGui::Button("Restart"); ImGui::SameLine();
		ImGui::Checkbox("Stop", &stop);
	}

	// ImGui test window. Most of the sample code is in ImGui::ShowTestWindow()
	if (show_test_window) {
		ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiSetCond_FirstUseEver);
		ImGui::ShowTestWindow(&show_test_window);
	}
}

void PhysicsInit() {
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

void Restart() {
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

	for (int i = 0; i < ClothMesh::numCols; i++) {
		for (int j = 0; j < ClothMesh::numRows; j++) {
			arrayPos[j * 14 + i] = (glm::vec3(j*distance.x - 5.f, 3.5, i*distance.z - 5.f));
		}
	}
	restart = false;
}

void MyUpdateSphere(float dt, glm::vec3 b, glm::vec3 drag) {
	glm::vec3 gravityForces = gravity * sphereWeight;
	glm::vec3 sumForces = gravityForces + b + drag;

	sphereAccel = sumForces / sphereWeight;
	sphereVel += sphereAccel * dt;
	spherePos += sphereVel * dt;
}

void arrayMod(float* distance, int* index, int a) {
	for (int i = 3; i > a; i--) {
		distance[i] = distance[i - 1];
		index[i] = index[i - 1];
	}

}

void PhysicsUpdate(float dt) {
	if (!stop) {
		//Waves
		waveVecX = glm::vec3(0.6f, 0, 0) * waveMulti1;
		waveVecZ = glm::vec3(0, 0, 0.9) * waveMulti2;
		waveVecCustom = glm::vec3(0.6, 0, 0.6) * waveMulti3;

		timer += dt;

		if (restart) Restart();

		//Waves Movements
		for (int i = 0; i < ClothMesh::numCols * ClothMesh::numRows; i++) {
			glm::vec2 orgX = glm::vec2(arrayPosOrg[i].x, arrayPosOrg[i].z);
			glm::vec2 currentX;
			currentX = orgX - (glm::normalize(glm::vec2(waveVecX.x, waveVecX.z)) * waveAmplitude * glm::sin(glm::dot(glm::vec2(waveVecX.x, waveVecX.z), orgX) - waveFreqX * timer)
				+ glm::normalize(glm::vec2(waveVecZ.x, waveVecZ.z)) * waveAmplitude * glm::sin(glm::dot(glm::vec2(waveVecZ.x, waveVecZ.z), orgX) - waveFreqZ * timer)
				+ glm::normalize(glm::vec2(waveVecCustom.x, waveVecCustom.z)) * waveAmplitude * glm::sin(glm::dot(glm::vec2(waveVecCustom.x, waveVecCustom.z), orgX) - waveFreqCustom * timer)
				);

			float currentY = 5 + (waveAmplitude * glm::cos(glm::dot(glm::vec2(waveVecX.x, waveVecX.z), orgX) - waveFreqX * timer) +
				waveAmplitude * glm::cos(glm::dot(glm::vec2(waveVecZ.x, waveVecZ.z), orgX) - waveFreqZ * timer) +
				waveAmplitude * glm::cos(glm::dot(glm::vec2(waveVecCustom.x, waveVecCustom.z), orgX) - waveFreqCustom * timer)
				);

			arrayPos[i] = glm::vec3(currentX.x, currentY, currentX.y);
		}

		int nearVertexIndex[4] = { -1, -1, -1, -1 };
		float nearDistance[4] = { 20, 20, 20, 20 };

		for (int i = 0; i < ClothMesh::numCols*ClothMesh::numRows; i++) {
			float actualDist = glm::distance(arrayPos[i], spherePos) - 1;

			if (actualDist < (nearDistance[0])) {
				arrayMod(nearDistance, nearVertexIndex, 0);
				nearDistance[0] = actualDist;
				nearVertexIndex[0] = i;
			}
			else if ((actualDist < (nearDistance[1]))) {
				arrayMod(nearDistance, nearVertexIndex, 1);
				nearDistance[1] = actualDist;
				nearVertexIndex[1] = i;
			}

			else if (actualDist < (nearDistance[2])) {
				arrayMod(nearDistance, nearVertexIndex, 2);
				nearDistance[2] = actualDist;
				nearVertexIndex[2] = i;
			}

			else if (actualDist < (nearDistance[2])) {
				nearDistance[3] = actualDist;
				nearVertexIndex[3] = i;
			}
		}

		//Sphere Immersion Calcs
		glm::vec3 flotage = glm::vec3(0, 0, 0);
		glm::vec3 fDrag = glm::vec3(0, 0, 0);
		float immersion = 0;
		float immersionAproximation = (arrayPos[nearVertexIndex[0]].y + arrayPos[nearVertexIndex[1]].y + arrayPos[nearVertexIndex[2]].y + arrayPos[nearVertexIndex[3]].y) / 4;

		if (spherePos.y - 1 < immersionAproximation) {
			immersion = glm::clamp(glm::abs(spherePos.y - 1 - immersionAproximation), 0.f, 2.f);
			float immersionVolume = ((immersion * immersion *3.1416f) / 3) * (3 - immersion);
			flotage = liquidDensity * glm::length(gravity) * glm::abs(immersionVolume) * glm::vec3(0, 1, 0);

			float dragCoefficient = 1.f;
			float crossSectionArea;

			immersion = glm::clamp(immersion, 0.0f, 1.0f);
			crossSectionArea = 3.1416f * glm::pow(((immersion)), 2);
			fDrag = (liquidDensity * dragCoefficient * crossSectionArea * glm::length(sphereVel) * sphereVel);
			fDrag *= -0.5f;
		}


		MyUpdateSphere(dt, flotage, fDrag);
		ClothMesh::updateClothMesh((float*)&arrayPos[0].x);
		Sphere::updateSphere(spherePos);
	}
}

void PhysicsCleanup() {
	//TODO
}