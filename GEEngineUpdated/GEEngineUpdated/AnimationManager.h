#pragma once
#include <string>
#include <map>
#include "Objects.h"


struct AnimInfo {
	std::string name;
	bool loops;
	float speed;
};

template <typename StateEnum>
class AnimationManager {
public:
	AnimationInstance* animInstance;
	animatedModel* animModel;

	StateEnum currentState;
	StateEnum defaultState;
	std::map<StateEnum, AnimInfo> config;

	// Initialise with pointers and the "idle" state as a start state (usually)
	void init(AnimationInstance* inst, animatedModel* model, StateEnum startState) {
		animInstance = inst;
		animModel = model;
		currentState = startState;
		defaultState = startState;
	}

	// Register an animation state
	void addState(StateEnum state, std::string gemName, bool loops, float speed = 1.0f) {
		config[state] = { gemName, loops, speed };
	}

	// Switch states
	void changeState(StateEnum newState) {
		if (currentState == newState) {
			return;
		}

		currentState = newState;
		animInstance->resetAnimationTime();
	}

	void update(float dt) {
		AnimInfo& currentAnim = config[currentState];

		// Update the instance
		animInstance->update(currentAnim.name, dt * currentAnim.speed);

		// Check if the animation has finished
		if (animInstance->animationFinished()) {

			// CASE A: It is a One-Shot animation (like Reload)
			if (!currentAnim.loops) {
				changeState(defaultState); // Go back to Idle
			}
			// CASE B: It is a Looping animation (like Fire or Run)
			else {
				animInstance->resetAnimationTime();
			}
		}
	}

	StateEnum getState() {
		return currentState;
	}

	/*
	void PlayAnimations(Core* core, PSOManager* psos, Shaders* shaders,
                    AnimationInstance* AnimInstance, animatedModel* AnimModel,
                    float dt, Matrix& vp, Matrix& W, const std::string& anim) {
		AnimInstance->update(anim, dt);
		shaders->updateConstantVS("animated", "staticMeshBuffer", "VP", &vp);
		AnimModel->draw(core, psos, shaders, AnimInstance, vp, W);
	}
	*/
};