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
	void SetAspectRatio(int width, int height);//resize window
	void UpdateCameraFrustum();
	void FrustumCulling();
	void GetObjectsToCull(GameObject* go, std::vector<GameObject*>& list_go);

	//user settings
	void SetFOV(float horizontalFOV);


	//TODO: VIEWPORT
	void draw();
	void endDraw();

	void createFrameBuffer();
	void bindFrameBuffer();
	void unbindFrameBuffer();
	void rescaleFrameBuffer(float width, float height);



public:
	ai::texture* tex;
	bool checkered;
	Frustum frustum;

	Color color{};




	//TODO: igual poner en render3d
	GLuint FBO; // frame buffer object
	GLuint RBO; // rendering buffer object
	GLuint texture_colour_buffer; // the texture id we'll need later to create a texture 
	const GLuint WIDTH = 800;
	const GLint HEIGHT = 600;
private:

	//Editor settings
	bool isCullEnabled = false;
	//bool is_main = false;
	float width = 0.0f;
	float height = 0.0f;
	float aspect_ratio = 0.0f;
	float fov = 0.0f;
};