#include "window.h"
#include "core.h"
#include "maths.h"
#include "PipeLineState.h"
#include "mesh.h"
#include "Prim_Vertex.h"
#include "ScreenSpaceTriangle.h"
#include "ConstantBufferClass.h"
#include "Shader.h"
#include "Plane.h"
#include "Cube.h"
#include "Sphere.h"
#include "Tree.h"

using namespace std;

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nCmdShow)   // similar to main which was done previously
{
	// creates an instance object of win
	Window win;    

	// create an instance object of core
	Core core;    

	// deltaTime from Game Engineering Base
	GamesEngineeringBase::Timer tim;

	// Create instance of plane
	Plane planeDraw;

	//Create instance of a cube
	Cube cube;
	Cube cube2;

	//Create instance of a sphere
	Sphere SkyBox;

	//Create instance of static mesh
	Tree tree;


	// Creates a window
	win.initialize("My Window", 1024, 1024); 

	// Initializes the Direct3D12 core engine with that window
	core.init(win.hwnd, 1024, 1024);           

	// Initialises the plane/cube/sphere
	//planeDraw.init(&core);
	//cube.init(&core);
	//cube2.init(&core);
	SkyBox.init(&core, 32, 32, 100);

	tree.init(&core);
	
	// World matrix
	Matrix matrixWorld;
	Matrix matrixWorld2;
	Matrix SkyBoxMatrix;

	SkyBoxMatrix.translation(Vec3(0, 2, 0));

	matrixWorld2.translation(Vec3(5, 0, 0));
	matrixWorld2.scaling(Vec3(0.5, 0.5, 0.5));

	float t = 0;

	while (true)
	{
		core.beginFrame();              // begins a new rendering frame

		win.processMessages();           // processes pending windows message

		if (win.keys[VK_ESCAPE] == 1)   // escape key closes the window
		{
			break;
		}

		// Cumulative time in seconds since last frame
		t += tim.dt();

		// needed for perspectiveProjection matrix
		float aspect = (float)win.width / (float)win.height;
		float fieldOfView = 60.0f;
		float _near = 0.01f;
		float _far = 10000.0f;

		// Perspective Projection - aspect, fov, near, far
		Matrix p = p.perspectiveProjection(aspect, fieldOfView, _near, _far);

		// Camera orbit
		Vec3 from = Vec3(11 * cos(t), 5, 11 * sinf(t));

		// View Matrix - eye, target, up
		Matrix v = v.lookAtMatrix(from, Vec3(0, 0, 0), Vec3(0, 1, 0));

		// Combined view perspective
		Matrix vp = p.multiply(v);

		// Updates the GPU constant buffer and issues the draw call.
		// The CPU - side buffer is copied to the GPU constant buffer
		//planeDraw.draw(&core, matrixWorld, vp);
		SkyBox.draw(&core, SkyBoxMatrix, vp);
		//cube.draw(&core, matrixWorld, vp);
		//cube2.draw(&core, matrixWorld2, vp);
		matrixWorld.scaling(Vec3(0.01f, 0.01f, 0.01f));
		tree.draw(&core, matrixWorld, vp);
		

		// finished rendering
		core.finishFrame();             

	}
	// ensures GPU commands finish
	core.flushGraphicsQueue();         
	
	return 0;
}

// Screen shake - changing FOV basically changing something to 5 3 4 1 2 (shaking the screen basically)