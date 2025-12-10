#pragma once
#include "Maths.h"
#include "Window.h"
#include "Objects.h" // For animatedModel, AnimationInstance
#include "AnimationManager.h"
#include "Textures.h"

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

class Player {
public:
    Vec3 position;
    Vec3 forward;
    Vec3 right;
    Vec3 up;
    Vec3 worldUp;

    float yaw;   // Left/Right
    float pitch; // Up/Down

    float speed;
    float sensitivity;

    // Animation / Model Data
    animatedModel gunModel;
    AnimationInstance gunAnimInstance;
    AnimationManager<PlayerState> playerAnim;

    // Initialization
    void init(Core* core, PSOManager* psos, Shaders* shaders, TextureManager* textureManager) {
        position = Vec3(0.0f, 2.0f, -5.0f); // Start slightly back and up
        worldUp = Vec3(0.0f, 1.0f, 0.0f);
        yaw = 0.0f;
        pitch = 0.0f;
        speed = 10.0f;
        sensitivity = 0.002f;
        updateVectors();

        gunModel.init(core, psos, shaders, "Resources/Models/AutomaticCarbine.gem", textureManager);
        gunAnimInstance.init(&gunModel.mesh.animation, 0);
        playerAnim.init(&gunAnimInstance, &gunModel, PlayerState::Idle);

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
    }

    void processInput(Window& win, GameInput& input, float dt) {
        // Reset triggers
        input.reload = false; input.inspect = false; input.meleeAttack = false;

        // Continuous
        input.walk = (win.keys['W'] || win.keys['A'] || win.keys['S'] || win.keys['D']);
        input.run = (input.walk && win.keys[VK_SHIFT]);
        input.ads = (GetAsyncKeyState(VK_RBUTTON) & 0x8000);
        input.fire = (GetAsyncKeyState(VK_LBUTTON) & 0x8000);
        input.adsFire = (input.ads && input.fire);

        // Triggers
        if (win.keys['R'] == 1) { input.reload = true; win.keys['R'] = 0; }
        if (win.keys['F'] == 1) { input.inspect = true; win.keys['F'] = 0; }
        if (win.keys['V'] == 1) { input.meleeAttack = true; win.keys['V'] = 0; }

        // Camera Rotation
        handleMouse(win);
        handleMovement(dt);
    }

    // Main Update Function
    Matrix update(Window& win, float dt) {
        GameInput input;
        processInput(win, input, dt);

        // --- State Machine Logic ---
        PlayerState currentState = playerAnim.getState();
        bool isBusy = (currentState == PlayerState::Reload || currentState == PlayerState::MeleeAttack || currentState == PlayerState::EmptyReload);

        if (!isBusy) {
            if (input.reload) playerAnim.changeState(PlayerState::Reload);
            else if (input.meleeAttack) playerAnim.changeState(PlayerState::MeleeAttack);
            else if (input.adsFire) playerAnim.changeState(PlayerState::ADSFire);
            else if (input.ads) playerAnim.changeState(PlayerState::ADSIdle);
            else if (input.fire) playerAnim.changeState(PlayerState::Fire);
            else if (input.run) playerAnim.changeState(PlayerState::Run);
            else if (input.walk) playerAnim.changeState(PlayerState::Walk);
            else if (input.inspect) playerAnim.changeState(PlayerState::Inspect);
            else if (currentState != PlayerState::Inspect) playerAnim.changeState(PlayerState::Idle);
        }

        gunModel.mesh.animation.debugListAnimations();

        playerAnim.update(dt);

        // Return ViewProjection Matrix
        float aspect = (float)win.width / (float)win.height;
        Matrix projection, view;
        projection = projection.perspectiveProjection(aspect, 60.0f, 0.1f, 1000.0f);
        Vec3 target = position + forward;
        view = view.lookAtMatrix(position, target, up);
        return projection.multiply(view);
    }

    void draw(Core* core, PSOManager* psos, Shaders* shaders, Matrix& vp, TextureManager* textureManager) {
        // Calculate Model Matrix
        Matrix gunLogic = getGunModelMatrix();
        Matrix gunScale;
        gunScale.scaling(Vec3(0.1f, 0.1f, 0.1f));
        Matrix finalGunMatrix = gunLogic.multiply(gunScale); // R * T * S order fix from previous step might apply here

        // Bind and Draw
        psos->bind(core, "animatedPSO");
        shaders->updateConstantVS("animated", "staticMeshBuffer", "W", &finalGunMatrix);
        shaders->updateConstantVS("animated", "staticMeshBuffer", "VP", &vp);
        shaders->updateConstantVS("animated", "staticMeshBuffer", "bones", gunAnimInstance.matrices);
        shaders->apply(core, "animated");
        gunModel.mesh.draw(core, shaders, textureManager);
    }
    

    Matrix getGunModelMatrix() {
        float handRight = -0.25f;
        float handDown = -0.25f;
        float handFwd = 0.0f;

  
        Vec3 gunPos = position
            + (right * handRight)
            + (up * handDown)
            + (forward * handFwd);

   
        Matrix translation;
        translation.translation(gunPos);

        Matrix rx;
        rx.rotAroundX(pitch);

        Matrix ry;
        // PI flips it round, it was facing backwards
        ry.rotAroundY(yaw + 3.14159f);

        Matrix rotation = ry.multiply(rx);

        return translation.multiply(rotation);
    }

private:
    void handleMouse(Window& win) {
        POINT p;
        if (GetCursorPos(&p)) {
            ScreenToClient(win.hwnd, &p);

            // Assuming 1024x1024 window. 
            // Better to use win.getWidth() if available, otherwise hardcode center
            int centerX = 1024 / 2;
            int centerY = 1024 / 2;

            float deltaX = (float)(p.x - centerX);
            float deltaY = (float)(p.y - centerY);

            // Apply rotation
            yaw += deltaX * sensitivity;
            pitch -= deltaY * sensitivity;

            // Clamp Pitch to prevent flipping
            if (pitch > 1.5f) pitch = 1.5f;
            if (pitch < -1.5f) pitch = -1.5f;

            // Lock Cursor to Center
            POINT center = { centerX, centerY };
            ClientToScreen(win.hwnd, &center);
            SetCursorPos(center.x, center.y);
        }
    }

    void updateVectors() {
        // Calculate Forward based on Yaw/Pitch
        Vec3 newForward;
        newForward.x = sinf(yaw) * cosf(pitch);
        newForward.y = sinf(pitch);
        newForward.z = cosf(yaw) * cosf(pitch);

        forward = newForward.normalize();

        right = forward.Cross(worldUp).normalize();

        up = right.Cross(forward).normalize();
    }

    void handleMovement(float dt) {
        // Recalculate vectors so we move in the direction we look
        updateVectors();

        float velocity = speed * dt;

        // Move Forward/Back
        if (GetAsyncKeyState('W') & 0x8000) {
            position = position + (forward * velocity);
        }
        if (GetAsyncKeyState('S') & 0x8000) {
            position = position - (forward * velocity);
        }

        // Strafe Left/Right
        if (GetAsyncKeyState('D') & 0x8000) {
            position = position - (right * velocity);
        }
        if (GetAsyncKeyState('A') & 0x8000) {
            position = position + (right * velocity);
        }
    }
};