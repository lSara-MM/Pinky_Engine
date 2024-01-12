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
	//RELEASE(bounds);
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


void C_UI::MousePick(LineSegment ray)
{
	intersects.clear();

	rayCam = ray;
	entryDist = 0.0f;
	exitDist = rayCam.Length();

	CheckGameObjectsIntersection(App->scene->rootNode);

	if (intersects.size() > 0)
	{
		CheckTriangleIntersection();
	}
}

void C_UI::CheckGameObjectsIntersection(GameObject* go)
{
	if (go->GetComponentByType(C_TYPE::UI) != nullptr)
	{
		CheckIntersection(go);
	}

	if (!go->vChildren.empty())
	{
		for (auto i = 0; i < go->vChildren.size(); i++)
		{
			CheckGameObjectsIntersection(go->vChildren[i]);
		}
	}
}

void C_UI::CheckIntersection(GameObject* go)
{
	C_UI* ui = static_cast<C_UI*>(go->GetComponentByType(C_TYPE::UI));

	if (ui->isActive)
	{
		if (rayCam.Intersects(ui->global_aabb, entryDist, exitDist))
		{
			intersects[entryDist] = ui;
		}
	}
}

bool C_UI::CheckTriangleIntersection()
{
	bool ret = false;
	float3 hit_point = float3::zero;
	localrayCam = rayCam;
	C_UI* closest = nullptr;
	std::map<float, C_UI*>::iterator iterator;

	for (iterator = intersects.begin(); iterator != intersects.end(); ++iterator)
	{
		if (iterator->second->gameObject->transform != nullptr)
		{
			float4x4 object_transform = iterator->second->gameObject->transform->GetGlobalTransform();
			localrayCam.Transform(object_transform.Inverted());

			for (uint i = 0; i < 6; i += 3)
			{
				uint index1 = bounds->index[i] * 3;
				uint index2 = bounds->index[i + 1] * 3;
				uint index3 = bounds->index[i + 2] * 3;
				float3 vec1 = bounds->vertex[index1];
				float3 vec2 = bounds->vertex[index1];
				float3 vec3 = bounds->vertex[index1];

				Triangle tri(vec1, vec2, vec3);

				if (localrayCam.Intersects(tri, nullptr, nullptr))
				{
					closest = iterator->second;
					App->scene->hierarchy->SetSelected(closest->gameObject);
					App->camera->objectReference = closest->global_aabb.CenterPoint();
					ret = true;
					return ret;
				}
			}
		}
	}

	return ret;
}
