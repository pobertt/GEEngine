#include "window.h"
#include "core.h"
#include "maths.h"
#include "PipeLineState.h"
#include "mesh.h"
#include "Prim_Vertex.h"
#include "ScreenSpaceTriangle.h"
#include "Shader.h"
#include "Objects.h"
#include "Player.h"

using namespace std;

void drawTrees(Core core, Matrix& vp) {
	//Create instance of static mesh
	
}

void PlayAnimations(Core* core, PSOManager* psos, Shaders* shaders, AnimationInstance* trexAnimation, animatedModel* trex, float dt, Matrix& vp, Matrix& W) {
	//t-rex
	trexAnimation->update("roar", dt);
	if (trexAnimation->animationFinished() == true) {
		trexAnimation->resetAnimationTime();
	}
	shaders->updateConstantVS("animated", "staticMeshBuffer", "VP", &vp);
	W.scaling(Vec3(0.01f, 0.01f, 0.01f));
	
	trex->draw(core, psos, shaders, trexAnimation, vp, W);
}

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nCmdShow) {
	Window win;
	win.initialize("My Window", 1024, 1024);
	Core core;
	core.init(win.hwnd, 1024, 1024);

	GamesEngineeringBase::Timer tim;

	Shaders shaders;
	PSOManager psos;

	//plane
	Plane floor;
	floor.init(&core, &psos, &shaders);

	Cube cube;
	cube.init(&core, &psos, &shaders);

	Player player;
	player.init(&core, &psos, &shaders);

	Sphere sphere;
	sphere.init(&core, &psos, &shaders, 20, 20, 20);

	//tree
	staticModel tree;
	tree.init(&core, &psos, &shaders, "Resources/Models/acacia_003.gem");

	//t-rex
	animatedModel trex;
	trex.init(&core, &psos, &shaders, "Resources/Models/TRex.gem");
	AnimationInstance trexAnimation;
	trexAnimation.init(&trex.mesh.animation, 0);

	float t = 0;
	float trexX = 0.0f;

	while (true) {
		//core.resetCommandList();
		core.beginFrame();

		win.processMessages();

		//camera things here
		float dt = tim.dt();
		t += dt;

		//player.update(t, win.keys);
		if (win.keys['A'] == 1) { trexX += 5.0 * dt; }
		if (win.keys['D'] == 1) { trexX -= 5.0 * dt; }

		Matrix vp = player.OrbitCamera(win, t);


		//update shaders 
		shaders.updateConstantVS("static", "staticMeshBuffer", "VP", &vp);
		core.beginRenderPass();

		//draw static objects
		//plane 
		Matrix planeM;
		planeM.translation(Vec3(0.0f, 0.0f, 0.0f));
		floor.draw(&core, &psos, &shaders, vp, planeM);

		//cube.draw(&core, &psos, &shaders, vp);
		sphere.draw(&core, &psos, &shaders, vp);
		//player.draw(&core, &psos, &shaders, vp, planeM);
		

		//tree
		Matrix T;
		T.scaling(Vec3(0.01f, 0.01f, 0.01f));
		tree.update(&shaders, T);
		tree.draw(&core, &psos, &shaders, vp);

		Matrix W;
		W.translation(Vec3(trexX, 0.0f, 0.0f));
		PlayAnimations(&core, &psos, &shaders, &trexAnimation, &trex, dt, vp, W);

		core.finishFrame();
	}
	core.flushGraphicsQueue();
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