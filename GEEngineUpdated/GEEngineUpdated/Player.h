#pragma once
#include "Maths.h"
#include "Window.h"
#include "Objects.h"
#include "AnimationManager.h"
#include "Textures.h"
#include "Collision.h"
#include "TRex.h"

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

    bool justFired = false; // Flag to signal a shot

    // PHYSICS VARIABLES
    float yVelocity = 0.0f;
    float gravity = -20.0f; // Downward acceleration
    float jumpForce = 8.0f;
    bool isGrounded = false; // Track if we are on the floor

    // Animation / Model Data
    animatedModel gunModel;
    AnimationInstance gunAnimInstance;
    AnimationManager<PlayerState> playerAnim;

    BoundingBox collider;

    MuzzleFlash flash;

    // Initialization
    void init(Core* core, PSOManager* psos, Shaders* shaders, TextureManager* textureManager) {
        position = Vec3(0.0f, 2.0f, -5.0f); // Start slightly back and up
        worldUp = Vec3(0.0f, 1.0f, 0.0f);
        yaw = 0.0f;
        pitch = 0.0f;
        speed = 10.0f;
        sensitivity = 0.002f;
        updateVectors();

        yVelocity = 0.0f;
        isGrounded = false;

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

    void setFlashMesh(Plane* p) {
        flash.init(p);
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

        static bool mouseReleased = true; // Prevents "machine gun" firing
        justFired = false; // Reset every frame

        if (input.fire) {
            if (mouseReleased) {
                justFired = true; // We shot this frame!
                // Play Gun Animation here if you want
                playerAnim.changeState(PlayerState::Fire);
                mouseReleased = false;
            }
        }
        else {
            mouseReleased = true;
        }

        // Camera Rotation
        handleMouse(win);
        handleMovement(dt);
    }

    // Main Update Function
    Matrix update(Window& win, float dt) {
        GameInput input;
        processInput(win, input, dt);

        Vec3 playerSize(0.5f, 1.0f, 0.5f);
        collider.set(position + Vec3(0, 1.0f, 0), playerSize);

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
        //flash.update(dt);

        // Return ViewProjection Matrix
        float aspect = (float)win.width / (float)win.height;
        Matrix projection, view;
        projection = projection.perspectiveProjection(aspect, 60.0f, 0.1f, 1000.0f);
        Vec3 target = position + forward;
        view = view.lookAtMatrix(position, target, up);
        return projection.multiply(view);
    }

    void handleShooting(TRex& target) {
        // 1. Check if we fired this frame
        if (!justFired) return;

        // Reset the flag so we don't fire again next frame
        justFired = false;

        // 2. Create Ray
        // Origin: Player Position + Eye Height (e.g., 2.0f units up)
        Vec3 eyePos = position + Vec3(0.0f, 2.0f, 0.0f);
        Ray shot(eyePos, forward);

        // 3. Variables to store hit data
        float hitDistance = 0.0f;
        float maxRange = 100.0f; // Gun range

        // 4. Check Collision against TRex
        // We access the TRex passed in by reference
        if (!target.isDead && Collision::CheckRay(shot, target.collider, hitDistance)) {
            // Validate Range
            if (hitDistance < maxRange && hitDistance > 0.0f) {
                // HIT CONFIRMED!
                // Apply damage directly to the target
                target.takeDamage(25.0f);

                // Optional: You could spawn a blood particle effect here later
                // core->spawnParticle(shot.at(hitDistance)); 
            }
        }

        Vec3 tip = position + (forward * 2.0f) + (right * -0.1f) + (up * -0.1f);

        flash.activate(tip);
    }

    // Add this to your update()
    void updateFlash(float dt) {
        flash.update(dt);
    }

    void draw(Core* core, PSOManager* psos, Shaders* shaders, Matrix& vp, TextureManager* textureManager) {
        // Calculate Model Matrix
        Matrix gunLogic = getGunModelMatrix();
        Matrix gunScale;
        gunScale.scaling(Vec3(0.1f, 0.1f, 0.1f));
        Matrix finalGunMatrix = gunLogic.multiply(gunScale);

        // Bind and Draw
        psos->bind(core, "animatedPSO");
        shaders->updateConstantVS("animated", "staticMeshBuffer", "W", &finalGunMatrix);
        shaders->updateConstantVS("animated", "staticMeshBuffer", "VP", &vp);
        shaders->updateConstantVS("animated", "staticMeshBuffer", "bones", gunAnimInstance.matrices);
        shaders->apply(core, "animated");
        gunModel.mesh.draw(core, shaders, textureManager);

    }

    void drawFlash(Core* core, PSOManager* psos, Shaders* shaders, Matrix& vp, TextureManager* tm) {
        // Pass 'position' so the billboard knows where the camera is
        flash.draw(core, psos, shaders, vp, tm, position);
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
        updateVectors();
        float velocity = speed * dt;

        Vec3 flatForward = Vec3(forward.x, 0.0f, forward.z).normalize();
        Vec3 flatRight = Vec3(right.x, 0.0f, right.z).normalize();

        if (GetAsyncKeyState('W') & 0x8000) position = position + (flatForward * velocity);
        if (GetAsyncKeyState('S') & 0x8000) position = position - (flatForward * velocity);
        if (GetAsyncKeyState('D') & 0x8000) position = position - (flatRight * velocity);
        if (GetAsyncKeyState('A') & 0x8000) position = position + (flatRight * velocity);

        if ((GetAsyncKeyState(VK_SPACE) & 0x8000) && isGrounded) {
            yVelocity = jumpForce;
            isGrounded = false;
        }

        yVelocity += gravity * dt;
        position.y += yVelocity * dt;

        // Ground Collision
        if (position.y < 3.0f) {
            position.y = 3.0f;      
            yVelocity = 0.0f;      
            isGrounded = true;      
        }
        else {
            if (position.y > 0.01f) isGrounded = false;
        }
        collider.set(position + Vec3(0, 1.0f, 0), Vec3(0.5f, 1.0f, 0.5f));
    }
};