#include "window.h"
#include "core.h"
#include "maths.h"
#include "PipeLineState.h"
#include "mesh.h"
#include "Prim_Vertex.h"
#include "ScreenSpaceTriangle.h"
#include "Shader.h"
#include "Plane.h"
#include "Cube.h"
#include "Sphere.h"
#include "Tree.h"
#include "Player.h"
#include "Skybox.h"
#include "Objects.h"

using namespace std;

void drawTrees(Core core, Matrix& vp) {
	//Create instance of static mesh
	
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
	plane floor;
	floor.init(&core, &psos, &shaders);

	//tree
	staticModel tree;
	tree.init(&core, &psos, &shaders, "Resources/Models/acacia_003.gem");

	//t-rex
	animatedModel trex;
	trex.init(&core, &psos, &shaders, "Resources/Models/TRex.gem");
	AnimationInstance trexAnimation;
	trexAnimation.init(&trex.mesh.animation, 0);

	float t = 0;

	while (true) {
		
		


		core.resetCommandList();
		core.beginFrame();

		win.processMessages();

		//camera things here
		float dt = tim.dt();
		t += dt;

		float aspect = (float)win.width / (float)win.height;
		float fieldOfView = 60.0f;
		float _near = 0.01f;
		float _far = 10000.0f;

	
		Matrix p = p.perspectiveProjection(aspect, fieldOfView, _near, _far);

		// Camera orbit
		Vec3 from = Vec3(11 * cos(t), 5, 11 * sinf(t));

		// View Matrix - eye, target, up
		Matrix v = v.lookAtMatrix(from, Vec3(0, 0, 0), Vec3(0, 1, 0));

		// Combined view perspective
		Matrix vp = p.multiply(v);

		//update shaders 
		shaders.updateConstantVS("staticModel", "staticMeshBuffer", "VP", &vp);
		core.beginRenderPass();

		

		//plane 
		floor.draw(&core, &psos, &shaders, vp);

		//tree
		Matrix W;
		W.scaling(Vec3(0.01f, 0.01f, 0.01f));
		tree.update(&shaders, W);
		tree.draw(&core, &psos, &shaders, vp);

		//t-rex
		trexAnimation.update("run", dt);
		if (trexAnimation.animationFinished() == true) {
			trexAnimation.resetAnimationTime();
		}
		shaders.updateConstantVS("animatedModel", "staticMeshBuffer", "VP", &vp);
		W.scaling(Vec3(0.01f, 0.01f, 0.01f));
		trex.draw(&core, &psos, &shaders, &trexAnimation, vp, W);

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