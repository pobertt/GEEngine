#pragma once
#include <string>
#include <map>
#include <windows.h>
#include "Objects.h" // Includes your Animation.h

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
    std::map<StateEnum, AnimInfo> config;

    void init(AnimationInstance* inst, animatedModel* model, StateEnum startState) {
        animInstance = inst;
        animModel = model;
        currentState = startState;
    }

    void addState(StateEnum state, std::string gemName, bool loops, float speed = 1.0f) {
        config[state] = { gemName, loops, speed };
    }

    void changeState(StateEnum newState) {
        if (currentState == newState) return;

        currentState = newState;
        // Reset time in your instance to 0
        animInstance->resetAnimationTime();
    }

    void update(float dt) {
        AnimInfo& currentAnim = config[currentState];

        // Update your instance
        animInstance->update(currentAnim.name, dt * currentAnim.speed);

        // Handle Looping
        if (animInstance->animationFinished()) {
            if (currentAnim.loops) {
                animInstance->resetAnimationTime();
            }
            // If NOT looping, we do nothing. The TRex class handles it.
        }
    }

    StateEnum getState() {
        return currentState;
    }

    bool AnimationFinished() {
        AnimInfo& currentAnim = config[currentState];
        if (currentAnim.loops) return false;

        // Use Animation.h method
        return animInstance->animationFinished();
    }
};