#include "C_UI.h"
#include "GameObject.h"

C_UI::C_UI(UI_TYPE ui_t, C_TYPE t, GameObject* g, std::string n, Color c, int w, int h, int x, int y) : Component(t, n, g)
{
	UI_type = ui_t;
	state = UI_STATE::NORMAL;

	width = w;
	height = h;
	color = c;

	float3 position = gameObject->transform->position;

	float4x4 localTransform = gameObject->transform->GetLocalTransform();
	float3 localPos;
	Quat rot;
	float3 scale;
	gameObject->transform->localMatrix.Decompose(localPos, rot, scale);

	if (x != 0 || y != 0)
	{
		posX = x;
		posY = y;
	}
	else
	{
		posX = localPos.x;
		posY = localPos.y;
	}

	bounds = new UIBounds;

	bounds->index = new uint[6];
	bounds->index[0] = 0;
	bounds->index[1] = 1;
	bounds->index[2] = 2;
	bounds->index[3] = 2;
	bounds->index[4] = 1;
	bounds->index[5] = 3;

	bounds->vertex[0] = float3(position.x, position.y + height, localPos.z);
	bounds->vertex[1] = float3(position.x + width, position.y + height, localPos.z);
	bounds->vertex[3] = float3(position.x + width, position.y, localPos.z);
	bounds->vertex[2] = float3(position.x, position.y, localPos.z);

	//bounds->vertex[0] = float3(posX, posY + height, localPos.z);
	//bounds->vertex[1] = float3(posX + width, posY + height, localPos.z);//TODO: necesario?
	//bounds->vertex[3] = float3(posX + width, posY, localPos.z);
	//bounds->vertex[2] = float3(posX, posY, localPos.z);

	bounds->uvs[2] = float2(0, 1);
	bounds->uvs[3] = float2(1, 1);
	bounds->uvs[1] = float2(1, 0);
	bounds->uvs[0] = float2(0, 0);

	bounds->InitBuffers();

	//Mouse pick
	local_aabb.SetNegativeInfinity();
	local_aabb.Enclose((float3*)bounds->vertex, 4);
	obb = local_aabb;
	obb.Transform(gameObject->transform->GetGlobalTransform());
	global_aabb.SetNegativeInfinity();
	global_aabb.Enclose(obb);
}

C_UI::~C_UI()
{
	bounds->DeleteBuffers();
	//RELEASE(bounds->index);TODO: peta
	RELEASE(bounds);
}

void C_UI::Draw(bool game)
{
	if (game)
	{
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0.0, App->editor->GameViewSize.x, App->editor->GameViewSize.y, 0.0, 1.0, -1.0);//TODO: orginal con 0,0 bien en pantalla pero mueve al revés
		//glOrtho(App->editor->GameViewSize.x, 0.0, 0.0, App->editor->GameViewSize.y, 1.0, -1.0);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
	}

	else
	{
		glPushMatrix();
		glMultMatrixf(gameObject->transform->GetGLTransform());
	}

	glEnableClientState(GL_VERTEX_ARRAY);

	//normals
	glEnableClientState(GL_NORMAL_ARRAY);

	//texture
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	// Mesh buffers
	glBindBuffer(GL_ARRAY_BUFFER, bounds->VBO);
	glVertexPointer(3, GL_FLOAT, 0, NULL);
	glBindVertexArray(0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bounds->EBO);

	glColor4f(color.r, color.g, color.b, color.a);

	// Textures
	glBindBuffer(GL_ARRAY_BUFFER, bounds->id_tex_uvs);
	glTexCoordPointer(2, GL_FLOAT, 0, NULL);
	glActiveTexture(GL_TEXTURE0);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);

	// Clean textures
	glBindTexture(GL_TEXTURE_2D, 0); // Cleanning bind buffer;
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	if (!game)
	{
		glPopMatrix();
	}
}

void C_UI::DebugDraw()
{
	glBegin(GL_LINE_LOOP);
	glColor4f(1.0f, 0.0f, 1.0f, 1.0f);

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

void C_UI::StateLogic()
{
	float2 mousePos = float2(App->editor->mouse.x, App->editor->mouse.y);

	switch (state)
	{
	case UI_STATE::DISABLED:
		break;
	case UI_STATE::NORMAL:
		OnNormal();
		if (MouseCheck(mousePos))
		{
			state = UI_STATE::FOCUSED;
		}
		break;
	case UI_STATE::FOCUSED:	// On hover
		OnFocused();
		if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_DOWN)
		{
			state = UI_STATE::PRESSED;
		}
		if (!MouseCheck(mousePos))
		{
			state = UI_STATE::NORMAL;
		}
		break;
	case UI_STATE::PRESSED: // On hold
		OnPressed();
		if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_UP && MouseCheck(mousePos))
		{
			state = UI_STATE::RELEASE;
		}
		else if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_UP && !MouseCheck(mousePos))
		{
			state = UI_STATE::NORMAL;
		}
		break;
	case UI_STATE::RELEASE:
		OnRelease();
		state = UI_STATE::SELECTED;
		break;
	case UI_STATE::SELECTED:
		OnSelected();
		if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_DOWN && MouseCheck(mousePos))
		{
			state = UI_STATE::PRESSED;
		}
		else if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_DOWN && !MouseCheck(mousePos))
		{
			state = UI_STATE::NORMAL;
		}
		break;
	case UI_STATE::NONE:
		break;
	default:
		break;
	}
}

bool C_UI::MouseCheck(float2 mouse)
{
	return (mouse.x >= posX / App->editor->GameViewSize.x && mouse.x <= (posX + width) / App->editor->GameViewSize.x 
		&& mouse.y >= (posY + 21.25) / App->editor->GameViewSize.y && mouse.y <= (posY + 21.25 + height) / App->editor->GameViewSize.y);
}

void C_UI::UpdateUITransform()
{
	float3 position = gameObject->transform->position;
	float4x4 localTransform = gameObject->transform->GetLocalTransform();
	float3 localPos;
	Quat rot;
	float3 scale;

	gameObject->transform->localMatrix.Decompose(localPos, rot, scale);
	width *= scale.x;//TODO: ta mal
	height *= scale.y;

	posX = position.x;//TODO: arreglar
	posY = position.y;

	bounds->vertex[0] = float3(position.x, position.y + height, localPos.z);
	bounds->vertex[1] = float3(position.x + width, position.y + height, localPos.z);
	bounds->vertex[3] = float3(position.x + width, position.y, localPos.z);
	bounds->vertex[2] = float3(position.x, position.y, localPos.z);

	//bounds->vertex[0] = float3(posX, posY + height, localPos.z);
	//bounds->vertex[1] = float3(posX + width, posY + height, localPos.z);//TODO: necesario?
	//bounds->vertex[3] = float3(posX + width, posY, localPos.z);
	//bounds->vertex[2] = float3(posX, posY, localPos.z);

	bounds->uvs[2] = float2(0, 1);
	bounds->uvs[3] = float2(1, 1);
	bounds->uvs[1] = float2(1, 0);
	bounds->uvs[0] = float2(0, 0);

	bounds->InitBuffers();
}

void C_UI::DrawABB()
{
	glBegin(GL_LINES);
	glLineWidth(3.0f);
	glColor4f(0.0f, 0.0f, 1.0f, 1.0f);

	for (uint i = 0; i < 12; i++)
	{
		glVertex3f(global_aabb.Edge(i).a.x, global_aabb.Edge(i).a.y, global_aabb.Edge(i).a.z);
		glVertex3f(global_aabb.Edge(i).b.x, global_aabb.Edge(i).b.y, global_aabb.Edge(i).b.z);
	}

	glEnd();
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
}

void C_UI::DrawOBB()
{
	glBegin(GL_LINES);
	glLineWidth(3.0f);
	glColor4f(0.0f, 1.0f, 0.0f, 1.0f);

	for (uint i = 0; i < 12; i++)
	{
		glVertex3f(obb.Edge(i).a.x, obb.Edge(i).a.y, obb.Edge(i).a.z);
		glVertex3f(obb.Edge(i).b.x, obb.Edge(i).b.y, obb.Edge(i).b.z);
	}

	glEnd();
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
}

void C_UI::UpdateBoundingBoxes()
{
	local_aabb.SetNegativeInfinity();
	local_aabb.Enclose((float3*)bounds->vertex, 4);
	obb = local_aabb;
	obb.Transform(gameObject->transform->GetGlobalTransform());
	global_aabb.SetNegativeInfinity();
	global_aabb.Enclose(obb);
}

bool UIBounds::InitBuffers()
{
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

	return true;
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


