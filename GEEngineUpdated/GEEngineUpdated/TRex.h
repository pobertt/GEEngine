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

    float rotationY = 0.0f;
    float speed = 2.5f;

    bool isDead = false;
    float health = 1000.0f;

    BoundingBox collider;

    void init(Core* core, PSOManager* psos, Shaders* shaders, TextureManager* texMan) {
        position = Vec3(25.0f, 0.0f, 5.0f);
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

    void update(float dt, Window& win, Vec3 playerPos) {
        // AI/logic later

        Vec3 dinoSize(2.0f, 4.0f, 6.0f);
        Vec3 centerOffset(0.0f, 2.0f, 0.0f);
        collider.set(position + centerOffset, dinoSize);

        if (isDead) return;

        Vec3 direction = playerPos - position;
        direction.y = 0; // Ignore height (don't fly towards player)

        float dist = direction.length(direction);

        if (dist < 20.0f) {
            Vec3 dirNorm = direction.normalize();

            // Move Position
            position = position + (dirNorm * speed * dt);

            // Calculate Rotation (Face the player)
            // atan2(x, z) gives the angle in radians
            rotationY = atan2(dirNorm.x, dirNorm.z);

            // Ensure Walk animation is playing
            // (If you have an 'Idle', you could switch to it in the 'else' block)
            if (health <= health / 2) {
                animManager.changeState(TrexState::Walk);
            }
            else {
                animManager.changeState(TrexState::Run);
            }
        }
        else if (dist < 5.0f) {
            animManager.changeState(TrexState::Roar);
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

        
    }

    void draw(Core* core, PSOManager* psos, Shaders* shaders, Matrix& vp, TextureManager* texMan) {
        // Update World Matrix 
        Matrix S, T, R;
        S.scaling(Vec3(scale, scale, scale));
        R.rotAroundY(rotationY);
        T.translation(position);

        transform = T.multiply(R).multiply(S);

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