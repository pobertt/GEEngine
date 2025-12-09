#pragma once
#include "Maths.h"
#include "Window.h" // For mouse input
#include <cmath>

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

    // Initialization
    void init(Core* core, PSOManager* psos, Shaders* shaders) {
        position = Vec3(0.0f, 2.0f, -5.0f); // Start slightly back and up
        worldUp = Vec3(0.0f, 1.0f, 0.0f);
        yaw = 0.0f;
        pitch = 0.0f;
        speed = 10.0f;
        sensitivity = 0.002f;
        updateVectors();
    }

    // Main Update Function
    Matrix update(Window& win, float dt) {
        handleMouse(win);
        handleKeyboard(dt);

        float aspect = (float)win.width / (float)win.height;
        float fovDeg = 60.0f;

        Matrix projection;

        projection = projection.perspectiveProjection(aspect, fovDeg, 0.1f, 1000.0f);

        Vec3 target = position + forward;

        Matrix view;
        view = view.lookAtMatrix(position, target, up);

        Matrix vp = projection.multiply(view);
        
        return vp;
    }

    Matrix getGunModelMatrix() {
        float handRight = 0.0f;
        float handDown = -0.25f;
        float handFwd = -0.5f;

  
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

    void handleKeyboard(float dt) {
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