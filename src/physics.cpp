#include <imgui\imgui.h>
#include <imgui\imgui_impl_glfw_gl3.h>
#include <GL\glew.h>
#include <glm\gtc\type_ptr.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <cstdio>

bool show_test_window = false;

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

namespace Sphere {
	extern bool renderSphere = true;
	extern float spherePos[3] = { 0.f, 1.f, 0.f };
	extern float sphereRadius = 1.f;
	extern float sphereColor[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
	extern void setupSphere(glm::vec3 pos = glm::vec3(0.f, 1.f, 0.f));
	extern void cleanupSphere();
	extern void updateSphere(glm::vec3 pos);
	extern void drawSphere();
}

namespace Capsule {
	extern bool renderCapsule = true;
	extern float capsulePosA[3] = { -3.f, 2.f, -2.f }, capsulePosB[3] = { -4.f, 2.f, 2.f };
	extern float capsuleRadius = 1.f;
	extern float capsuleColor[4] = { 0.0f, 1.0f, 0.0f, 1.0f };
	extern void setupCapsule(glm::vec3 posA = glm::vec3(-3.f, 2.f, -2.f), glm::vec3 posB = glm::vec3(-4.f, 2.f, 2.f));
	extern void cleanupCapsule();
	extern void updateCapsule(glm::vec3 posA, glm::vec3 posB);
	extern void drawCapsule();
}

namespace LilSpheres {
	extern bool renderParticles = true;
	extern int maxParticles = 1000;
	extern float particlesRadius = .05f;
	extern float lifetime = 3.f;
	extern float bounceCoeficient = .0f, frictionCoeficient = .0f;
	extern int solver = 0; //0 Euler - 1 Verlet
	extern int form = 0; //0 Cascade - 1 Fountain
	extern float particlesFocus[3] = { .0f, .0f, .0f };
	extern float particlesColor[4] = { 0.0f, 0.0f, 1.0f, 1.0f };
	extern void setupParticles(int numTotalParticles);
	extern void cleanupParticles();
	extern void updateParticles(int startIdx, int count, float* array_data);
	extern void drawParticles(int startIdx, int count);
}

namespace ForceField {
	extern bool enableForce = false;
	extern float forceFocus[3] = { 10.0f, 10.0f, 10.0f };
	extern float force = 0.1f;
	extern int type = 0; //0 Repulsor - 1 Attractor - 2 PulseRepulse
}

void GUI() {
	{	//FrameRate
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

		ImGui::Separator();

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
			ImGui::SliderFloatPos("Focus Position", LilSpheres::particlesFocus, -10.0f, 10.0f);
			//Particles Lifetime
			ImGui::SliderFloat("Particles Lifetime", &LilSpheres::lifetime, 1.0f, 5.0f, "seconds = %.3f");
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
			ImGui::SliderFloatPos("Sphere Position", Sphere::spherePos, -10.0f, 10.0f);
			//Radius
			ImGui::SliderFloat("Sphere Radius", &Sphere::sphereRadius, .1f, 5.0f, "%.3f");
			//Color
			ImGui::ColorEdit4("Sphere Color", Sphere::sphereColor);
		}

		//Capsule
		if (ImGui::CollapsingHeader("Capsule")) {
			//Enable Capsule
			ImGui::Checkbox("Capsule Enabled", &Capsule::renderCapsule);
			//Position point 1
			ImGui::SliderFloatPos("Position A", Capsule::capsulePosA, -10.0f, 10.0f);
			//Position point 2
			ImGui::SliderFloatPos("Position B", Capsule::capsulePosB, -10.0f, 10.0f);
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
			ImGui::SliderFloatPos("Force Field Position", ForceField::forceFocus, -10.0f, 10.0f);
			//Force
			ImGui::SliderFloat("Force", &ForceField::force, .1f, 10.0f, "%.3f");
			//Interaction Type ( Repulsion, Atraction, Pulse)
			ImGui::RadioButton("Repulsion", &ForceField::type, 0); ImGui::SameLine();
			ImGui::RadioButton("Attraction", &ForceField::type, 1); ImGui::SameLine();
			ImGui::RadioButton("Pulse", &ForceField::type, 2);

		}

	}

	// ImGui test window. Most of the sample code is in ImGui::ShowTestWindow()
	if(show_test_window) {
		ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiSetCond_FirstUseEver);
		ImGui::ShowTestWindow(&show_test_window);
	}
}

void PhysicsInit() {
	//TODO
}
void PhysicsUpdate(float dt) {
	//TODO
}
void PhysicsCleanup() {
	//TODO
}