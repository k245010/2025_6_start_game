#include "light.h"
#include "../ImGui/imgui.h"

Light::Light(SceneBase* _scene) :GameObject(_scene)
{

	SetLightEnable(FALSE);
	
	//SetLightDifColor(GetColorF(0.8f, 0.8f, 1.0f, 1.0f));
	hLight = CreateDirLightHandle(VGet(0.0f, -0.8f, 0.8f));
	SetLightDirectionHandle(hLight,VGet(0.0f, -1.0f, 0.8f));
	//SetLightDifColorHandle(hLight, GetColorF(1.0f, 0.9f, 0.9f, 1.0f));
	
	//SetLightAmbColor(GetColorF(1.0f, 0.8f, 0.8f, 1.0f));
	SetLightAmbColorHandle(hLight, GetColorF(1.0f, 0.8f, 0.8f, 1.0f));
	
}

Light::~Light()
{
	DeleteLightHandle(hLight);
}

void Light::Update()
{
	/*ImGui::Begin("LIGHT");
	ImGui::DragFloat3("Pos", &hLightPos.x, 0.0f, -1.0f, 1.0f);
	SetLightDirectionHandle(hLight, hLightPos);
	ImGui::End();*/
}

void Light::Draw()
{
}
