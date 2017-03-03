#include <imgui\imgui.h>
#include <imgui\imgui_impl_glfw_gl3.h>

bool show_test_window = true;

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

void GUI() {
	{	//FrameRate
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

		ImGui::Separator();

		//Particles
		if (ImGui::CollapsingHeader("Particles")) {
			//Enable Particles
			static bool particlesEnabled = true;
			ImGui::Checkbox("Particles Enabled", &particlesEnabled);
			//Max Particles
			static int maxParticles = 1000;
			ImGui::SliderInt("Max Particles", &maxParticles, 1000, 10000);
			ImGui::SameLine(); ShowHelpMarker("CTRL+click to input value.");
			//Particles Lifetime
			static float lifetime = 3.f;
			ImGui::SliderFloat("Particles Lifetime", &lifetime, 1.0f, 5.0f, "seconds = %.3f");
			//Bounce Coefficient
			static float bounceCoeficient = 0.f;
			ImGui::SliderFloat("Bounce Coefficient", &bounceCoeficient, 0.0f, 1.0f, "%.3f");
			//Friction Coefficient
			static float frictionCoeficient = 0.f;
			ImGui::SliderFloat("Friction Coefficient", &frictionCoeficient, 0.0f, 1.0f, "%.3f");
			//Solver Method
			static int solver = 0;
			ImGui::RadioButton("Euler Method", &solver, 0); ImGui::SameLine();
			ImGui::RadioButton("Verlet Method", &solver, 1);
			//Pull Method
			static int form = 0;
			ImGui::RadioButton("Cascade", &form, 0); ImGui::SameLine();
			ImGui::RadioButton("Fountain", &form, 1);
		}

		//Sphere
		if (ImGui::CollapsingHeader("Sphere")) {
			//Enable Sphere
			static bool sphereEnabled = false;
			ImGui::Checkbox("Sphere Enabled", &sphereEnabled);
			//Position
			static float spherePos[2] = { 10, 10 };
			ImGui::SliderFloat2("Position", spherePos, 0.0f, 500.0f, "X = %.3f");
			//Radius

			//Color

		}

		//Capsule
		if (ImGui::CollapsingHeader("Capsule")) {
			//Enable Capsule
			static bool capsuleEnabled = false;
			ImGui::Checkbox("Capsule Enabled", &capsuleEnabled);
			//Position point 1

			//Position point 2

			//Radius

			//Color

		}

		//Force Field
		//Sphere
		if (ImGui::CollapsingHeader("Force Field")) {
			//Enable Force Field
			static bool forceEnabled = false;
			ImGui::Checkbox("Force Field Enabled", &forceEnabled);
			//Position
			
			//Force

			//Interaction (Atraction, Repulsion, Pulse)

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