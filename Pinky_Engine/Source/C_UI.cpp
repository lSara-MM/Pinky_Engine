#include "C_UI.h"
#include "GameObject.h"
#include "G_UI.h"

#include "UI_Canvas.h"

#include "Application.h"
#include "ModuleEditor.h"

C_UI::C_UI(UI_TYPE ui_t, C_TYPE t, GameObject* g, std::string n, int x, int y, int w, int h, Color c) : Component(t, n, g)
{
	UI_type = ui_t;
	state = UI_STATE::NORMAL;

	width = w;
	height = h;
	color = c;

	isDragging = false;
	isDraggable = false;
	fade = false;

	gameObject->transform->SetPosition(float3(x, y, 0));
	gameObject->transform->UpdateGlobalMatrix();

	float3 position = gameObject->transform->position;

	float3 globalPos;
	Quat rot;
	float3 scale;
	gameObject->transform->globalMatrix.Decompose(globalPos, rot, scale);

	if (x != 0 || y != 0)
	{
		posX = x;
		posY = y;
	}
	else
	{
		posX = globalPos.x;
		posY = globalPos.y;
	}

	scaleBounds = scale;

	boundsEditor = new UIBounds;
	boundsGame = new UIBounds;

	boundsEditor->index = new uint[6];
	boundsEditor->index[0] = 0;
	boundsEditor->index[1] = 1;
	boundsEditor->index[2] = 2;
	boundsEditor->index[3] = 2;
	boundsEditor->index[4] = 1;
	boundsEditor->index[5] = 3;

	boundsGame->index = new uint[6];
	boundsGame->index[0] = 0;
	boundsGame->index[1] = 1;
	boundsGame->index[2] = 2;
	boundsGame->index[3] = 2;
	boundsGame->index[4] = 1;
	boundsGame->index[5] = 3;

	boundsEditor->vertex[0] = float3(position.x, position.y + (height * scaleBounds.y), 0);
	boundsEditor->vertex[1] = float3(position.x + (width * scaleBounds.x), position.y + ((height * scaleBounds.y) * scaleBounds.y), 0);
	boundsEditor->vertex[2] = float3(position.x, position.y, 0);
	boundsEditor->vertex[3] = float3(position.x + (width * scaleBounds.x), position.y, 0);

	boundsGame->vertex[0] = float3(posX, posY + (height * scaleBounds.y), 0);
	boundsGame->vertex[1] = float3(posX + (width * scaleBounds.x), posY + (height * scaleBounds.y), 0);
	boundsGame->vertex[2] = float3(posX, posY, 0);
	boundsGame->vertex[3] = float3(posX + (width * scaleBounds.x), posY, 0);

	boundsGame->uvs[0] = float2(0, 1);
	boundsGame->uvs[1] = float2(1, 1);
	boundsGame->uvs[2] = float2(0, 0);
	boundsGame->uvs[3] = float2(1, 0);

	boundsEditor->uvs[0] = float2(0, 1);
	boundsEditor->uvs[1] = float2(1, 1);
	boundsEditor->uvs[2] = float2(0, 0);
	boundsEditor->uvs[3] = float2(1, 0);

	boundsEditor->InitBuffers();
	boundsGame->InitBuffers();

	//Mouse pick
	local_aabb.SetNegativeInfinity();
	local_aabb.Enclose((float3*)boundsEditor->vertex, 4);
	obb = local_aabb;
	obb.Transform(gameObject->transform->GetGlobalTransform());
	global_aabb.SetNegativeInfinity();
	global_aabb.Enclose(obb);
}

C_UI::~C_UI()
{
	boundsEditor->DeleteBuffers();
	boundsGame->DeleteBuffers();
	RELEASE_ARRAY(boundsEditor->index);
	RELEASE_ARRAY(boundsGame->index);
	RELEASE(boundsEditor);
	RELEASE(boundsGame);
}

update_status C_UI::Update(float dt)
{
	update_status ret = UPDATE_CONTINUE;

	return ret;
}

void C_UI::Draw(bool game)
{
	UIBounds* boundsDrawn = nullptr;

	if (game)
	{
		boundsDrawn = boundsGame;

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0.0, App->editor->gameViewSize.x, 0.0, App->editor->gameViewSize.y, 1.0, -1.0);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
	}

	else
	{
		boundsDrawn = boundsEditor;

		glPushMatrix();
		glMultMatrixf(gameObject->transform->GetGLTransform());
	}

	glEnableClientState(GL_VERTEX_ARRAY);

	//normals
	glEnableClientState(GL_NORMAL_ARRAY);

	//texture
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	// Mesh buffers
	glBindBuffer(GL_ARRAY_BUFFER, boundsDrawn->VBO);
	glVertexPointer(3, GL_FLOAT, 0, NULL);
	glBindVertexArray(0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, boundsDrawn->EBO);

	//alpha material
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0.0f);

	glColor4f(color.r, color.g, color.b, color.a);

	// Textures
	glBindBuffer(GL_ARRAY_BUFFER, boundsDrawn->id_tex_uvs);
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

	boundsDrawn = nullptr;
}

void C_UI::DebugDraw()
{
	glBegin(GL_LINE_LOOP);
	glColor4f(1.0f, 0.0f, 1.0f, 1.0f);

	float3 position = gameObject->transform->position;

	float3 v1 = float3(position.x, position.y, position.z);
	float3 v2 = float3(position.x + (width * scaleBounds.x), position.y, position.z);
	float3 v3 = float3(position.x, position.y + (height * scaleBounds.y), position.z);
	float3 v4 = float3(position.x + (width * scaleBounds.x), position.y + (height * scaleBounds.y), position.z);

	glVertex3f(v1.x, v1.y, v1.z);
	glVertex3f(v2.x, v2.y, v2.z);
	glVertex3f(v4.x, v4.y, v4.z);
	glVertex3f(v3.x, v3.y, v3.z);

	glEnd();
}

void C_UI::StateLogic()
{
	if (App->editor->g->HoveredWindow != nullptr)
	{
		std::string name = App->editor->g->HoveredWindow->Name;
		if (name == "Game" && !static_cast<G_UI*>(gameObject)->canvas->fade)
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
	}
}

void C_UI::OnNormal()
{
}

void C_UI::OnFocused()
{
}

void C_UI::OnPressed()
{
	if (isDraggable)
	{
		isDragging = true;
	}
}

void C_UI::OnRelease()
{
	isDragging = false;
}

void C_UI::OnSelected()
{
}

bool C_UI::MouseCheck(float2 mouse)
{
	return (mouse.x >= posX / App->editor->gameViewSize.x && mouse.x <= (posX + (width * scaleBounds.x)) / App->editor->gameViewSize.x
		&& mouse.y >= posY / App->editor->gameViewSize.y && mouse.y <= (posY  + (height * scaleBounds.y)) / App->editor->gameViewSize.y);
}

void C_UI::UpdateUITransform()
{
	float3 position = gameObject->transform->position;
	float3 globalPos;
	Quat rot;
	float3 scale;

	gameObject->transform->globalMatrix.Decompose(globalPos, rot, scale);

	posX = globalPos.x;//TODO: arreglar
	posY = globalPos.y;
	scaleBounds = scale;

	boundsEditor->vertex[0] = float3(position.x, position.y + ((height * scaleBounds.y) * scale.y), 0);
	boundsEditor->vertex[1] = float3(position.x + (width * scaleBounds.x), position.y + ((height * scaleBounds.y) * scale.y), 0);
	boundsEditor->vertex[2] = float3(position.x, position.y, 0);
	boundsEditor->vertex[3] = float3(position.x + (width * scaleBounds.x), position.y, 0);

	boundsGame->vertex[0] = float3(posX, posY + (height * scaleBounds.y), 0);
	boundsGame->vertex[1] = float3(posX + (width * scaleBounds.x), posY + (height * scaleBounds.y), 0);
	boundsGame->vertex[2] = float3(posX, posY, 0);
	boundsGame->vertex[3] = float3(posX + (width * scaleBounds.x), posY, 0);

	boundsEditor->RegenerateVBO();
	boundsGame->RegenerateVBO();

	/*if (!gameObject->vChildren.empty())
	{
		for (auto i = 0; i < gameObject->vChildren.size(); i++)
		{
			if (gameObject->vChildren[i]->GetComponentByType(C_TYPE::UI) != nullptr)
			{
				static_cast<C_UI*>(gameObject->vChildren[i]->GetComponentByType(C_TYPE::UI))->UpdateUITransform();
			}
		}
	}*/
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
	local_aabb.Enclose((float3*)boundsEditor->vertex, 4);
	obb = local_aabb;
	obb.Transform(gameObject->transform->GetGlobalTransform());
	global_aabb.SetNegativeInfinity();
	global_aabb.Enclose(obb);
}

void C_UI::Drag(float dt)
{
	int movementX = App->input->GetMouseXMotion() * dt * 30;
	int movementY = App->input->GetMouseYMotion() * dt * 30;
	posX += movementX;
	posY += movementY;


	float3 globalPos;
	Quat rot;
	float3 scale;
	//TODO:MILAGRO
	gameObject->transform->SetPosition(float3(gameObject->transform->position.x + movementX, gameObject->transform->position.y + movementY, 0));
	gameObject->transform->globalMatrix.Decompose(globalPos, rot, scale);

	scaleBounds = scale;

	float3 position = gameObject->transform->position;

	boundsEditor->vertex[0] = float3(position.x + movementX, position.y + (height * scaleBounds.y) + movementY, 0);
	boundsEditor->vertex[1] = float3(position.x + (width * scaleBounds.x) + movementX, position.y + (height * scaleBounds.y) + movementY, 0);
	boundsEditor->vertex[2] = float3(position.x + movementX, position.y + movementY, 0);
	boundsEditor->vertex[3] = float3(position.x + (width * scaleBounds.x) + movementX, position.y + movementY, 0);

	boundsGame->vertex[0] = float3(posX, posY + (height * scaleBounds.y), 0);
	boundsGame->vertex[1] = float3(posX + (width * scaleBounds.x), posY + (height * scaleBounds.y), 0);
	boundsGame->vertex[2] = float3(posX, posY, 0);
	boundsGame->vertex[3] = float3(posX + (width * scaleBounds.x), posY, 0);

	boundsEditor->RegenerateVBO();
	boundsGame->RegenerateVBO();

	//if (!gameObject->vChildren.empty())
	//{
	//	for (auto i = 0; i < gameObject->vChildren.size(); i++)
	//	{
	//		if (gameObject->vChildren[i]->GetComponentByType(C_TYPE::UI) != nullptr)
	//		{
	//			static_cast<C_UI*>(gameObject->vChildren[i]->GetComponentByType(C_TYPE::UI))->Drag(dt);
	//		}
	//	}
	//}
}

void C_UI::FadeUI(float dt)
{
	if (color.a <= 0)
	{
		gameObject->isActive = false;
		App->scene->crossHair = true;
	}
	else
	{
		//color.a -= 0.1 * dt;
		color.a -= 1 * dt;
	}

	if (!gameObject->vChildren.empty())
	{
		for (auto i = 0; i < gameObject->vChildren.size(); i++)
		{
			if (gameObject->vChildren[i]->GetComponentByType(C_TYPE::UI) != nullptr)
			{
				static_cast<C_UI*>(gameObject->vChildren[i]->GetComponentByType(C_TYPE::UI))->FadeUI(dt);
			}
		}
	}
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

void UIBounds::RegenerateVBO()
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


