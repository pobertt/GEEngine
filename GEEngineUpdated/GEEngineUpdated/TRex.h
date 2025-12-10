#pragma once

#pragma once
#include "Objects.h"
#include "AnimationManager.h"
#include "window.h"

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
    Matrix transform; // To store the calculated world matrix

    void init(Core* core, PSOManager* psos, Shaders* shaders, TextureManager* texMan) {
        position = Vec3(0, 0, 0);
        scale = 0.01f;

        // Load Assets
        model.init(core, psos, shaders, "Resources/Models/TRex.gem", texMan);
        animInstance.init(&model.mesh.animation, 0);
        animManager.init(&animInstance, &model, TrexState::Idle);

        // Setup States (Ensure these strings match exactly what is in your output window)
        animManager.addState(TrexState::Idle, "idle", true);
        animManager.addState(TrexState::Walk, "walk", true);
        animManager.addState(TrexState::Run, "run", true);
        animManager.addState(TrexState::Roar, "roar", false);
        animManager.addState(TrexState::Attack, "attack", false);
        animManager.addState(TrexState::Die, "death", false);
    }

    void update(float dt, Window& win) {
        // --- AI / Logic ---
        // For now, we still check keys, but this is inside the Entity class now.
        if (win.keys['T']) {
            animManager.changeState(TrexState::Roar);
        }
        else if (win.keys['Y']) {
            animManager.changeState(TrexState::Walk);
        }
        else {
            TrexState current = animManager.getState();
            // If not playing a "One-Shot" animation (Roar/Attack/Die), go back to idle
            bool isOneShot = (current == TrexState::Roar || current == TrexState::Attack || current == TrexState::Die);
            if (!isOneShot) {
                animManager.changeState(TrexState::Idle);
            }
        }

        animManager.update(dt);

        // --- Update World Matrix ---
        Matrix S, T;
        S.scaling(Vec3(scale, scale, scale));
        T.translation(position);
        transform = T.multiply(S); // Translate * Scale
    }

    void draw(Core* core, PSOManager* psos, Shaders* shaders, Matrix& vp, TextureManager* texMan) {
        psos->bind(core, "animatedPSO");

        // Pass the internal 'transform' matrix we calculated in update()
        shaders->updateConstantVS("animated", "staticMeshBuffer", "W", &transform);
        shaders->updateConstantVS("animated", "staticMeshBuffer", "VP", &vp);
        shaders->updateConstantVS("animated", "staticMeshBuffer", "bones", animInstance.matrices);

        shaders->apply(core, "animated");
        model.mesh.draw(core, shaders, texMan);
    }
};