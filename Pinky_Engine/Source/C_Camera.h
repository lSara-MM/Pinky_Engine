#pragma once
#include "Globals.h"
#include "Light.h"


#include "Component.h"
#include "Color.h"

enum class FrustumCulling
{
	CULLING_IN,
	CULLING_OUT,
	CULLING_INTERSECT
};

class C_Camera : public Component
{
public:
	C_Camera(GameObject* g = nullptr, uint i = 0, bool start_enabled = true);
	~C_Camera();

	void ShowInInspector();
	void LookAt(const float3& position);
	float* GetViewMatrix() const;//recalculate view matrix every frame 
	float* GetProjectionMatrix() const;//recalculate projection matrix only when frustum is changed
	void DrawDebug();
	FrustumCulling ContainsAABox(const AABB& refBox) const;
	//resize window
	void SetAspectRatio(int width, int height);
	void SetAsMain(bool mainCam);
	void OnResize(int width, int height);
	void UpdateCameraFrustum();
	void FrustumCulling();
	void GetObjectsToCull(GameObject* go, std::vector<GameObject*>& list_go);

	//user settings
	void SetFOV(float horizontalFOV);

	//restart values culling
	void RestartCulling();


	//Buffer functions
	void Draw();
	void createCamBuffers(int width, int height);
	void BindFrameBuffer();
	void UnbindFrameBuffer();
	void DeleteBuffers();

public:
	ai::texture* tex;
	bool checkered;
	Frustum frustum;

	Color color{};

	//Camera buffers
	GLuint FBO; //frame buffer object
	GLuint RBO; //rendering buffer object
	GLuint textureColourBuffer; //texture id 

	bool isMainCam = false;
	//Editor settings
	bool isCullEnabled = false;

private:

	//bool is_main = false;
	float width = 0.0f;
	float height = 0.0f;
	float aspect_ratio = 0.0f;
	float fov = 0.0f;
};