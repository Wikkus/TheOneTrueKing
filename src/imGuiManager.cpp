#include "imGuiManager.h"
#include "gameEngine.h"

void ImGuiHandler::Init() {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
	ImGuiSDL::Initialize(renderer, windowWidth, windowHeight);
}

void ImGuiHandler::ShowFloatValue(const char* name, const char* label, float a) {
	ImGui::Begin(name);
	ImGui::Text(label); 
	ImGui::SameLine(); 
	ImGui::Text(": %f", a);
	ImGui::End();
}

void ImGuiHandler::ShowFloat2Value(const char* name, const char* label, float a, float b) {
	ImGui::Begin(name);
	ImGui::Text(label);
	ImGui::SameLine();
	ImGui::Text(": %f, %f", a, b);
	ImGui::End();
}

void ImGuiHandler::ShowVector2Value(const char* name, const char* label, Vector2<float> value) {
	ImGui::Begin(name);
	ImGui::Text(label);
	ImGui::SameLine();
	ImGui::Text(": %f, %f", value.x, value.y);
	ImGui::End();
}

void ImGuiHandler::InputFloat(const char* name, const char* label, float& a) {
	ImGui::Begin(name);
	ImGui::InputFloat(label, &a);
	ImGui::End();
}

void ImGuiHandler::InputFloat2(const char* name, const char* label, float& a, float& b) {
	ImGui::Begin(name);
	ImGui::InputFloat2(label, (&b, &a));
	ImGui::End();
}

void ImGuiHandler::SliderFloat2(const char* name, const char* label, float& a, float& b, float min, float max) {
	ImGui::Begin(name);
	ImGui::SliderFloat2(label, (&b, &a), min, max);
	ImGui::End();
}

void ImGuiHandler::SliderFloat(const char* name, const char* label, float& a,float min, float max) {
	ImGui::Begin(name);
	ImGui::SliderFloat(label, &a, min, max);
	ImGui::End();
}

void ImGuiHandler::Render() {
	ImGui::Render();
	ImGuiSDL::Render(ImGui::GetDrawData());
}

void ImGuiHandler::ShutDown() {
	ImGui_ImplSDL2_Shutdown();
	ImGuiSDL::Deinitialize();
	ImGui::DestroyContext();
}
