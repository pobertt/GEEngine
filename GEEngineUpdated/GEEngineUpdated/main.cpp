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

void PlayAnimations(Core* core, PSOManager* psos, Shaders* shaders,
                    AnimationInstance* AnimInstance, animatedModel* AnimModel,
                    float dt, Matrix& vp, Matrix& W, const std::string& anim) {
    AnimInstance->update(anim, dt);
    shaders->updateConstantVS("animated", "staticMeshBuffer", "VP", &vp);
    AnimModel->draw(core, psos, shaders, AnimInstance, vp, W);
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

	animatedModel FPSModel;
	FPSModel.init(&core, &psos, &shaders, "Resources/Models/AutomaticCarbine.gem");
	AnimationInstance FPSAnim;
	FPSAnim.init(&FPSModel.mesh.animation, 0);

	float t = 0;
	float trexX = 0.0f;
	bool reloading = false;

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

		//Matrix vp = player.NewCamera(win, player.position, dt);
		Matrix vp = player.OrbitCamera(win, t);


		//update shaders 
		shaders.updateConstantVS("static", "staticMeshBuffer", "VP", &vp);
		core.beginRenderPass();

		//draw static objects
		//plane 
		Matrix planeM;
		planeM.translation(Vec3(0.0f, 0.0f, 0.0f));
		//floor.draw(&core, &psos, &shaders, vp, planeM);

		//cube.draw(&core, &psos, &shaders, vp);
		sphere.draw(&core, &psos, &shaders, vp);
		//player.draw(&core, &psos, &shaders, vp, planeM);
		

		//tree
		Matrix T;
		T.scaling(Vec3(0.01f, 0.01f, 0.01f));
		tree.update(&shaders, T);
		tree.draw(&core, &psos, &shaders, vp);

		//TRex Animation
		Matrix TrexM;
		TrexM.scaling(Vec3(0.01f, 0.01f, 0.01f));
		TrexM.translation(Vec3(trexX, 0.0f, 0.0f));

		Matrix CarbineM;
		CarbineM.translation(Vec3(0.0f, 0.0f, 10.0f));
		CarbineM.scaling(Vec3(0.25f, 0.25f, 0.25f));

		// Debug: list animations and verify names (Output window)
		trex.mesh.animation.debugListAnimations();
		OutputDebugStringA(trex.mesh.animation.hasAnimation("roar") ? "Has \"roar\"? yes\n" : "Has \"roar\"? no\n");

		FPSModel.mesh.animation.debugListAnimations();
		OutputDebugStringA(FPSModel.mesh.animation.hasAnimation("empty reload") ? "Has \"empty reload\"? yes\n" : "Has \"empty reload\"? no\n");

		// Guard: avoid calling missing animation
		if (trex.mesh.animation.hasAnimation("roar")) {
		    PlayAnimations(&core, &psos, &shaders, &trexAnimation, &trex, dt, vp, TrexM, "roar");
			if (trexAnimation.animationFinished()) {
				trexAnimation.resetAnimationTime();
			}
			
		}
		
		// Start reload on key press
		if (win.keys['R'] == 1) {
			reloading = true;
			FPSAnim.resetAnimationTime(); // start from 0
			win.keys['R'] = 0;
		}

		// Reload state
		if (reloading) {
			const char* reloadName = "17 reload"; // or the exact name from debugListAnimations()
			if (FPSModel.mesh.animation.hasAnimation(reloadName)) {
				PlayAnimations(&core, &psos, &shaders, &FPSAnim, &FPSModel, dt, vp, CarbineM, reloadName);
				if (FPSAnim.animationFinished()) {
					reloading = false;               // exit reload
					FPSAnim.resetAnimationTime();    // prepare next fire loop
				}
			} else {
				OutputDebugStringA("Missing reload animation!\n");
				reloading = false; // fail-safe to avoid being stuck
			}
		} else {
			const char* fireName = "08 fire"; // verify exact name
			if (FPSModel.mesh.animation.hasAnimation(fireName)) {
				PlayAnimations(&core, &psos, &shaders, &FPSAnim, &FPSModel, dt, vp, CarbineM, fireName);
				if (FPSAnim.animationFinished()) {
					FPSAnim.resetAnimationTime(); // loop fire
				}
			} else {
				OutputDebugStringA("Missing fire animation!\n");
			}
		}
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