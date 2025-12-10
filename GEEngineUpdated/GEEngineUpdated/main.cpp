
#define STB_IMAGE_IMPLEMENTATION

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
#include "AnimationManager.h"

#include "Textures.h"

using namespace std;

enum class PlayerState {
	Idle,
	ADSIdle,
	Fire,
	ADSFire,
	ADSWalk,
	Run,
	Walk,
	Reload,
	EmptyReload,
	Inspect,
	MeleeAttack,
};

enum class TrexState {
	Idle,
	Walk,
	Run,
	Roar,
	Attack,
	Die
};

struct GameInput {
	bool reload = false;
	bool fire = false;
	bool walk = false;
	bool run = false;
	bool inspect = false;
	bool meleeAttack = false;
	bool ads = false;
	bool adsFire = false;
};

// For animations
void ReadInputs(Window& win, GameInput& input) {
	// Reset frame inputs
	input.reload = false;
	input.fire = false;
	input.walk = false;
	input.run = false;
	input.inspect = false;
	input.meleeAttack = false;
	input.ads = false;
	input.adsFire = false;

	// 1. CONTINUOUS INPUTS (Do NOT set to 0)
	// We want these to remain true as long as the key is physically held.
	if (win.keys['W'] || win.keys['A'] || win.keys['S'] || win.keys['D']) {
		input.walk = true;
	}

	// Check Shift only if we are already walking
	if (input.walk && win.keys[VK_SHIFT]) {
		input.walk = false; // Upgrade walk to run
		input.run = true;
	}

	// ADS (Continuous Hold)
	if (GetAsyncKeyState(VK_RBUTTON) & 0x8000) {
		input.ads = true;
	}
	// Fire (Mouse)
	if (GetAsyncKeyState(VK_LBUTTON) & 0x8000) {
		input.fire = true;
	}
	if (input.ads && input.fire == true) {
		input.adsFire = true;
	}
	// 2. TRIGGER INPUTS (Set to 0 to consume)
	if (win.keys['R'] == 1) {
		input.reload = true;
		win.keys['R'] = 0;
	}
	if (win.keys['F'] == 1) {
		input.inspect = true;
		win.keys['F'] = 0;
	}
	if (win.keys['V'] == 1) {
		input.meleeAttack = true;
		win.keys['V'] = 0;
	}
}

void RenderCharacter(Core* core, PSOManager* psos, Shaders* shaders,
	AnimationInstance* AnimInstance, animatedModel* AnimModel,
	Matrix& vp, Matrix& W, TextureManager* textureManager)
{
	// We assume AnimInstance->update() was already called by the Manager!

	// Just call the existing draw helper in Objects.h
	// This handles binding the PSO and updating all Constant Buffers (W, VP, Bones)
	AnimModel->draw(core, psos, shaders, AnimInstance, vp, W, textureManager); //
}


int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nCmdShow) {
	Window win;
	win.initialize("My Window", 1024, 1024);
	Core core;
	core.init(win.hwnd, 1024, 1024);

	GamesEngineeringBase::Timer tim;

	Shaders shaders;
	PSOManager psos;
	TextureManager textureManager;

	Player player;
	player.init(&core, &psos, &shaders);
	ShowCursor(FALSE);

	//Objects
	Plane floor;
	floor.init(&core, &psos, &shaders);
	Cube cube;
	cube.init(&core, &psos, &shaders);
	Sphere sphere;
	sphere.init(&core, &psos, &shaders, 20, 20, 20);
	//tree
	staticModel tree;
	tree.init(&core, &psos, &shaders, "Resources/Models/acacia_003.gem");

	//t-rex
	animatedModel trex;
	trex.init(&core, &psos, &shaders, "Resources/Models/TRex.gem", &textureManager);
	AnimationInstance trexAnimation;
	trexAnimation.init(&trex.mesh.animation, 0);

	// FPSModel
	animatedModel FPSModel;
	FPSModel.init(&core, &psos, &shaders, "Resources/Models/AutomaticCarbine.gem", &textureManager);
	AnimationInstance FPSAnim;
	FPSAnim.init(&FPSModel.mesh.animation, 0);

	// Player Animations
	AnimationManager<PlayerState> playerAnim;
	playerAnim.init(&FPSAnim, &FPSModel, PlayerState::Idle);

	AnimationManager<TrexState> trexAnimManager;
	trexAnimManager.init(&trexAnimation, &trex, TrexState::Idle);

	// Map the Enum to the string names in your loaded model
	playerAnim.addState(PlayerState::Idle, "04 idle", true);
	playerAnim.addState(PlayerState::ADSIdle, "11 zoom idle", true);
	playerAnim.addState(PlayerState::Fire, "08 fire", true);
	playerAnim.addState(PlayerState::ADSFire, "13 zoom fire", true);
	playerAnim.addState(PlayerState::ADSWalk, "12 zoom walk", true);
	playerAnim.addState(PlayerState::Run, "07 run", true);
	playerAnim.addState(PlayerState::Walk, "06 walk", true);
	playerAnim.addState(PlayerState::Reload, "17 reload", false);
	playerAnim.addState(PlayerState::EmptyReload, "18 empty reload", false);
	playerAnim.addState(PlayerState::Inspect, "05 inspect", false);
	playerAnim.addState(PlayerState::MeleeAttack, "10 melee attack", false);

	trexAnimManager.addState(TrexState::Idle, "idle", true);  // Looping
	trexAnimManager.addState(TrexState::Walk, "walk", true);  // Looping
	trexAnimManager.addState(TrexState::Run, "run", true);  // Looping
	trexAnimManager.addState(TrexState::Roar, "roar", false); // Play once
	trexAnimManager.addState(TrexState::Attack, "attack", false); // Play once
	trexAnimManager.addState(TrexState::Die, "death", false); // Play once

	GameInput input;

	float t = 0;
	float trexX = 0.0f;
	bool reloading = false;

	//TRex Matrix
	Matrix TrexM;
	TrexM.scaling(Vec3(0.01f, 0.01f, 0.01f));
	TrexM.translation(Vec3(trexX, 0.0f, 0.0f));

	Matrix CarbineM;
	CarbineM.translation(Vec3(0.0f, 0.0f, 10.0f));
	CarbineM.scaling(Vec3(0.25f, 0.25f, 0.25f));

	while (true) {
		//core.resetCommandList();
		core.beginFrame();

		win.processMessages();

		float dt = tim.dt();
		t += dt;

		// Read Raw Keys
		ReadInputs(win, input);

		// Update Camera
		Matrix vp = player.update(win, dt);

		Matrix gunLogic = player.getGunModelMatrix(); // (Translation * Rotation)

		// 2. Define the Scale (Keep your 0.25f scale)
		Matrix gunScale;
		gunScale.scaling(Vec3(0.25f, 0.25f, 0.25f));

		// 3. Combine them: (Translation * Rotation) * Scale
		// This moves/rotates the gun, then applies the size.
		Matrix finalGunMatrix = gunLogic.multiply(gunScale);

		Matrix trexM;
		trexM.scaling(Vec3(0.01f,0.01f,0.01f));

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
		
		RenderCharacter(&core, &psos, &shaders, &FPSAnim, &FPSModel, vp, finalGunMatrix, &textureManager);
		RenderCharacter(&core, &psos, &shaders, &trexAnimation, &trex, vp, trexM, &textureManager);

		//tree
		Matrix T;
		T.scaling(Vec3(0.01f, 0.01f, 0.01f));
		tree.update(&shaders, T);
		tree.draw(&core, &psos, &shaders, vp);

		// Debug: list animations and verify names (Output window)
		trex.mesh.animation.debugListAnimations();

		PlayerState currentState = playerAnim.getState();
		bool isBusy = (currentState == PlayerState::Reload ||
			currentState == PlayerState::MeleeAttack ||
			currentState == PlayerState::EmptyReload);

		if (isBusy) {
			// Wait for these to finish naturally
		}
		else if (input.reload) {
			playerAnim.changeState(PlayerState::Reload);
		}
		else if (input.meleeAttack) {
			playerAnim.changeState(PlayerState::MeleeAttack);
		}
		else if (input.adsFire) {
			playerAnim.changeState(PlayerState::ADSFire);
		}
		else if (input.ads) {
			playerAnim.changeState(PlayerState::ADSIdle);
		}
		else if (input.fire) {
			playerAnim.changeState(PlayerState::Fire);
		}
		else if (input.run) {
			playerAnim.changeState(PlayerState::Run);
		}
		else if (input.walk) {
			playerAnim.changeState(PlayerState::Walk);
		}
		else if (input.inspect) {
			playerAnim.changeState(PlayerState::Inspect);
		}
		else {
			if (currentState == PlayerState::Inspect) {
				// Do nothing. Stay in Inspect state until:
				// A. You press a different button (caught by 'else if' above)
				// B. The animation finishes (caught by AnimationManager update)
			}
			else {
				// Otherwise, nothing is happening, so stand still.
				playerAnim.changeState(PlayerState::Idle);
			}
		}

		playerAnim.update(dt);

		if (win.keys['T']) {
            trexAnimManager.changeState(TrexState::Roar);
        }
        // Make him walk if we press 'Y'
        else if (win.keys['Y']) {
            trexAnimManager.changeState(TrexState::Walk);
        }
        // Return to Idle if nothing else is happening
        else {
            TrexState currentTrexState = trexAnimManager.getState();
            bool isAction = (currentTrexState == TrexState::Roar || currentTrexState == TrexState::Attack);
            
            if (!isAction) {
                trexAnimManager.changeState(TrexState::Idle);
            }
        }

		// UPDATE THE T-REX ANIMATION
		trexAnimManager.update(dt);

		RenderCharacter(&core, &psos, &shaders, &trexAnimation, &trex, vp, trexM, &textureManager);

		

		core.finishFrame();
	}
	core.flushGraphicsQueue();
}