#include "../Assimp/include/cimport.h"
#include "../Assimp/include/scene.h"
#include "../Assimp/include/postprocess.h"
#pragma comment (lib, "Assimp/libx86/assimp.lib")

#include "../Source/Globals.h"

namespace ai
{
	struct mesh
	{
		uint id_index = 0; // index in VRAM
		uint num_index = 0;
		uint* index = nullptr;

		uint id_vertex = 0; // unique vertex in VRAM
		uint num_vertex = 0;
		float* vertex = nullptr;
	};

	void EnableDebug();
	void DisableDebug();

	bool LoadObject(const char* fileDir);
}