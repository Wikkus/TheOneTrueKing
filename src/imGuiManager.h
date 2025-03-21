#pragma once
#include "ImGui/imgui.h"
#include "ImGui/imgui_sdl.h"
#include "ImGui/imgui_impl_sdl.h"

#include "vector2.h"

class ImGuiHandler {
public:
	ImGuiHandler() {}
	~ImGuiHandler() {}

	void Init();
	void ShowFloatValue(const char* name, const char* label, const float& a);
	void ShowFloat2Value(const char* name, const char* label, const float& a, const float& b);
	void ShowVector2Value(const char* name, const char* label, const Vector2<float>& value);
	
	void InputFloat(const char* name, const char* label, float& a);
	void InputFloat2(const char* name, const char* label, float& a, float& b);
	
	void SliderFloat(const char* name, const char* label, float& a, const float& min, const float& max);
	void SliderFloat2(const char* name, const char* label, float& a, float& b, const float& min, const float& max);

	void Render();
	void ShutDown();

private:
	const char* _formatV = ": %f, %f";
	const char* _formatF = ": %f";

};

