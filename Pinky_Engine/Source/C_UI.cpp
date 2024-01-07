#include "C_UI.h"
#include "GameObject.h"

C_UI::C_UI(C_TYPE t, GameObject* g, std::string n, Color c, int w, int h, int x, int y) : Component(t, n, g)
{
	width = w;
	height = h;
	posX = x;
	posY = y;
	color = c;

	float3 position = gameObject->transform->position;

	float4x4 localTransform = gameObject->transform->GetLocalTransform();
	float3 localPos;
	Quat rot;
	float3 scale;

	gameObject->transform->localMatrix.Decompose(localPos, rot, scale);

	/*bounds = new UIBounds;

	bounds->vertex[0] = float3(position.x, position.y + height, localPos.z);
	bounds->vertex[1] = float3(position.x + width, position.y + height, localPos.z);
	bounds->vertex[3] = float3(position.x + width, position.y, localPos.z);
	bounds->vertex[2] = float3(position.x, position.y, localPos.z);

	bounds->vertex[0] = float3(posX, posY + height, localPos.z);
	bounds->vertex[1] = float3(posX + width, posY + height, localPos.z);
	bounds->vertex[3] = float3(posX + width, posY, localPos.z);
	bounds->vertex[2] = float3(posX, posY, localPos.z);

	bounds->uvs[2] = float2(0, 1);
	bounds->uvs[3] = float2(1, 1);
	bounds->uvs[1] = float2(1, 0);
	bounds->uvs[0] = float2(0, 0);

	bounds->uvs[2] = float2(0, 1);
	bounds->uvs[3] = float2(1, 1);
	bounds->uvs[1] = float2(1, 0);
	bounds->uvs[0] = float2(0, 0);

	bounds->InitBuffers();*/
}

C_UI::~C_UI()
{
	RELEASE(bounds);
}

void C_UI::Draw()
{

	//glMatrixMode(GL_PROJECTION);
	//glLoadIdentity();
	//glOrtho(0.0, App->editor->panel_play->window_size.x, App->editor->panel_play->window_size.y, 0.0, 1.0, -1.0);

	//Initialize Modelview Matrix
	//glMatrixMode(GL_MODELVIEW);


	glPushMatrix();
	glMultMatrixf(gameObject->transform->GetGLTransform());

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0.0f);

	glColor4f(color.r, color.g, color.b, color.a);

	glEnable(GL_TEXTURE_2D);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glEnableClientState(GL_VERTEX_ARRAY);

	glBindTexture(GL_TEXTURE_2D, bounds->textureID);

	glBindBuffer(GL_ARRAY_BUFFER, bounds->VBO);
	glVertexPointer(3, GL_FLOAT, 0, NULL);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, bounds->id_tex_uvs);
	glTexCoordPointer(2, GL_FLOAT, 0, NULL);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bounds->EBO);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);

	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_ALPHA_TEST);

	glPopMatrix();

}

void C_UI::DebugDraw()
{
	glBegin(GL_LINE_LOOP);
	glColor4f(color.r, color.g, color.b, color.a);

	float3 position = gameObject->transform->position;

	float3 v1 = float3(position.x, position.y, position.z);
	float3 v2 = float3(position.x + width, position.y, position.z);
	float3 v3 = float3(position.x, position.y + height, position.z);
	float3 v4 = float3(position.x + width, position.y + height, position.z);

	glVertex3f(v1.x, v1.y, v1.z);
	glVertex3f(v2.x, v2.y, v2.z);
	glVertex3f(v4.x, v4.y, v4.z);
	glVertex3f(v3.x, v3.y, v3.z);

	glEnd();
}

bool UIBounds::InitBuffers()
{
	uint* index = new uint[6];
	index[0] = 0;
	index[1] = 1;
	index[2] = 2;
	index[3] = 2;
	index[4] = 1;
	index[5] = 3;

	VBO = 0;
	EBO = 0;
	id_tex_uvs = 0;

	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	glGenBuffers(1, &id_tex_uvs);

	if (VBO == 0 || EBO == 0 || id_tex_uvs == 0)
	{
		LOG("[ERROR] buffer not created");
		return false;
	}

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float3) * 4, vertex, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * 6, index, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, id_tex_uvs);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float2) * 4, uvs, GL_STATIC_DRAW);

	////---Local AABB---
	//local_aabb.SetNegativeInfinity();
	//local_aabb.Enclose((float3*)vertex, 4);

	return true;
}

void UIBounds::RgenerateVAO()
{
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float3) * 4, vertex, GL_STATIC_DRAW);
}

void UIBounds::DeleteBuffers()
{
	glDeleteBuffers(1, &VBO);
	VBO = 0;
	glDeleteBuffers(1, &EBO);
	EBO = 0;
	glDeleteBuffers(1, &id_tex_uvs);
	id_tex_uvs = 0;
}



