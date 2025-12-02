#include "Mesh.h"
#include "core.h"
#include "maths.h"
#include "Prim_Vertex.h"


class ScreenSpaceTriangle     // only comtains one triangle with three vertices
{
public:
	PRIM_VERTEX vertices[3];
	Mesh mesh; // stores vertex buffer

	ScreenSpaceTriangle() {}

	void init(Core* core)
	{
		vertices[0].position = Vec3(0, 1.0f, 0);
		vertices[0].colour = Colour(0, 1.0f, 0);
		vertices[1].position = Vec3(-1.0f, -1.0f, 0);
		vertices[1].colour = Colour(1.0f, 0, 0);
		vertices[2].position = Vec3(1.0f, -1.0f, 0);
		vertices[2].colour = Colour(0, 0, 1.0f);

		// Indices order as we go round the triangle
		unsigned int indices[3] = { 0, 1, 2 };

		mesh.init(core, &vertices[0], sizeof(PRIM_VERTEX), 3, indices, 3);
	}

	void draw(Core* core)
	{
		mesh.draw(core);
	}
};