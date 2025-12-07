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
#include "Player.h"

using namespace std;

void drawTrees(Core core, Matrix& vp) {
	//Create instance of static mesh
	
}

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

	//Create instance of a sphere
	Sphere SkyBox;
	Tree tree;
	
	Player player;

	

	AnimatedModel animatedModel;
	AnimationInstance animatedInstance;

	// Creates a window
	win.initialize("My Window", 1024, 1024); 

	// Initializes the Direct3D12 core engine with that window
	core.init(win.hwnd, 1024, 1024);   

	// Then initialize GPU-dependent assets
	animatedModel.init(&core, "Resources/Models/TRex.gem");
	animatedInstance.init(&animatedModel.animation, 0);
	for (int i = 0; i < 256; ++i) animatedInstance.matrices[i].identity();

	// Initialises the plane/cube/sphere
	//planeDraw.init(&core);
	cube.init(&core);
	SkyBox.init(&core, 32, 32, 100);

	tree.init(&core);
	player.init(&core);

	// World matrix
	Matrix matrixWorld;
	Matrix SkyBoxMatrix;
	Matrix TRex;

	SkyBoxMatrix.translation(Vec3(0, 2, 0));
	//SkyBoxMatrix.scaling(Vec3(1000, 1000, 1000));

	TRex.scaling(Vec3(0.01f, 0.01f, 0.01f));

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
		float dt = tim.dt();
		t += dt;

		// Update cube world transform at origin with current X offset
		matrixWorld.identity();
		matrixWorld.translation(Vec3(0.0f, 0.0f, 0.0f));

		player.update(dt, win.keys);

		Matrix vp = player.camera(win, player.position, dt);

		core.beginRenderPass();

		// Updates the GPU constant buffer and issues the draw call.
		// The CPU - side buffer is copied to the GPU constant buffer
		//planeDraw.draw(&core, matrixWorld, vp);
		
		//cube.draw(&core, matrixWorld, vp);
		
		// Draw a row of 10 trees
		
		for (int i = 0; i < 10; i++)
		{
			Matrix currentTreeWorld; // Creates a new Identity matrix

			currentTreeWorld.scaling(Vec3(0.01f, 0.01f, 0.01f));


			currentTreeWorld.translation(Vec3(i * 2.0f, 0, 0));
			tree.draw(&core, currentTreeWorld, vp);
		}
		
		
		animatedInstance.update("run", dt);
		if (animatedInstance.animationFinished() == true)
		{
			animatedInstance.resetAnimationTime();
		}
		//animatedModel.draw(&core, &animatedInstance, vp, TRex);
		
		

		// Draw with vp
		// debugFloor.draw(&core, floorWorld, vp);
		player.draw(&core, vp);
		// SkyBox.draw(&core, SkyBoxMatrix, vp);
		// finished rendering
		core.finishFrame();             

	}
	// ensures GPU commands finish
	core.flushGraphicsQueue();         
	
	return 0;
}



// Screen shake - changing FOV basically changing something to 5 3 4 1 2 (shaking the screen basically)

// Animation file- whole header file, an example animated model class (both on moodle)
/* We have to do slide 45 to 47
* How it works/Implementation:
* Define structures that hold bones and skeleton
* Create animation sequence class
* - Stores animation data for one animation
* Create animation class
* Stores skeleton and animations
* Animation instance
* */
/* Animation controller is transitioning animations between each other (states) e.g. running -> dead
*/