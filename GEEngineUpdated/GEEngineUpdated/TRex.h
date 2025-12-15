#pragma once

#include <windows.h> // add at top so OutputDebugStringA is available
#include "Objects.h"
#include "AnimationManager.h"
#include "window.h"
#include "Collision.h"

enum class TrexState {
    Idle, Walk, Run, Roar, Attack, Die
};

// Helper: convert enum to string
inline const char* toString(TrexState s) {
    switch (s) {
    case TrexState::Idle:   return "Idle";
    case TrexState::Walk:   return "Walk";
    case TrexState::Run:    return "Run";
    case TrexState::Roar:   return "Roar";
    case TrexState::Attack: return "Attack";
    case TrexState::Die:    return "Die";
    default:                return "Unknown";
    }
}

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
    float max_health = 1000.0f;
    float health = max_health;
    bool attacking = false;

    TrexState currentState;

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

        if (isDead && currentState == TrexState::Die) return;

        Vec3 direction = playerPos - position;
        direction.y = 0; // Ignore height (don't fly towards player)

        float dist = direction.length(direction);

        currentState = animManager.getState();

        if (attacking) {
            // Keep current rotation (optional: face player)
            // Block movement while attacking
            if (animInstance.animationFinished()) {
                animInstance.resetAnimationTime();
                attacking = false;
                // Transition after attack based on distance and health
                if (dist < 50.0f) {
                    animManager.changeState(health <= max_health / 2 ? TrexState::Walk : TrexState::Run);
                }
                else {
                    animManager.changeState(TrexState::Idle);
                }
            }
        }
        else {
            // Not attacking: decide behavior by distance
            if (dist < 10.0f) {
                // Enter attack and stop moving
                animManager.changeState(TrexState::Attack);
                attacking = true;
                // Face player when starting attack
                Vec3 dirNorm = direction.normalize();
                rotationY = atan2(dirNorm.x, dirNorm.z);
            }
            else if (dist < 50.0f) {
                // Move toward player
                Vec3 dirNorm = direction.normalize();
                position = position + (dirNorm * speed * dt);
                rotationY = atan2(dirNorm.x, dirNorm.z);
                animManager.changeState(health <= max_health / 2 ? TrexState::Walk : TrexState::Run);
            }
            else {
                animManager.changeState(TrexState::Idle);
            }
        }

        std::string dist_msg = std::string("Trex state: ") + toString(currentState) + "\n";
        OutputDebugStringA(dist_msg.c_str());

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
        if (isDead && currentState == TrexState::Die) return;
        /*std::string dist_msg = "Trex heakth : " + std::to_string(health);
        dist_msg += "\n";
        OutputDebugStringA(dist_msg.c_str());*/
        health -= amount;
        if (health <= 0) {
            health = 0;
            
            animManager.changeState(TrexState::Die);
            if (animInstance.animationFinished()) {
                isDead = true;
            }
        }
        else {
            animManager.changeState(TrexState::Roar); // React to hit
        }
    }
};