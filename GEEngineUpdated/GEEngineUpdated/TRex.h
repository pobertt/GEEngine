#pragma once

#pragma once
#include "Objects.h"
#include "AnimationManager.h"
#include "window.h"
#include "Collision.h"

enum class TrexState {
    Idle, Walk, Run, Roar, Attack, Die
};

class TRex {
public:
    animatedModel model;
    AnimationInstance animInstance;
    AnimationManager<TrexState> animManager;

    Vec3 position;
    float scale;
    Matrix transform;

    bool isDead = false;
    float health = 1000.0f;

    BoundingBox collider;

    void init(Core* core, PSOManager* psos, Shaders* shaders, TextureManager* texMan) {
        position = Vec3(5.0f, 0.0f, 5.0f);
        scale = 0.01f;

        // Load Assets
        model.init(core, psos, shaders, "Resources/Models/TRex.gem", texMan);
        animInstance.init(&model.mesh.animation, 0);
        animManager.init(&animInstance, &model, TrexState::Idle);

        // Setup States 
        animManager.addState(TrexState::Idle, "idle", true);
        animManager.addState(TrexState::Walk, "walk", true);
        animManager.addState(TrexState::Run, "run", true);
        animManager.addState(TrexState::Roar, "roar", false);
        animManager.addState(TrexState::Attack, "attack", false);
        animManager.addState(TrexState::Die, "death", false);
    }

    void update(float dt, Window& win) {
        // AI/logic later

        Vec3 dinoSize(1.5f, 1.5f, 1.5f);
        collider.set(position + Vec3(0, 1.5f, 0), dinoSize);

        if (win.keys['T']) {
            animManager.changeState(TrexState::Roar);
        }
        else if (win.keys['Y']) {
            animManager.changeState(TrexState::Walk);
        }
        else {
            TrexState current = animManager.getState();
            // If not playing a one-shot animation go back to idle
            bool isOneShot = (current == TrexState::Roar || current == TrexState::Attack || current == TrexState::Die);
            if (!isOneShot) {
                animManager.changeState(TrexState::Idle);
            }
        }

        animManager.update(dt);

        // Update World Matrix 
        Matrix S, T;
        S.scaling(Vec3(scale, scale, scale));
        T.translation(position);

        transform = T.multiply(S);

        Vec3 halfSize(2.0f, 4.0f, 2.0f);

        // 2. Center it correctly
        // If the height is 8.0, the center should be at Y=4.0 so the box sits on the floor
        Vec3 centerOffset(0.0f, 3.0f, 2.0f);

        collider.set(position + centerOffset, halfSize);
    }

    void draw(Core* core, PSOManager* psos, Shaders* shaders, Matrix& vp, TextureManager* texMan) {
        psos->bind(core, "animatedPSO");

        shaders->updateConstantVS("animated", "staticMeshBuffer", "W", &transform);
        shaders->updateConstantVS("animated", "staticMeshBuffer", "VP", &vp);
        shaders->updateConstantVS("animated", "staticMeshBuffer", "bones", animInstance.matrices);

        shaders->apply(core, "animated");
        model.mesh.draw(core, shaders, texMan);
    }

    void takeDamage(float amount) {
        if (isDead) return;
        health -= amount;
        if (health <= 0) {
            health = 0;
            isDead = true;
            animManager.changeState(TrexState::Die);
        }
        else {
            animManager.changeState(TrexState::Roar); // React to hit
        }
    }
};