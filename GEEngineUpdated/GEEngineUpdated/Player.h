#pragma once
#include "Objects.h"
#include "maths.h"
#include "window.h"
#include "core.h"

class Player
{
public:
    Cube cube;
    Matrix world;         // player's world transform
    Vec3 position;        // x left/right, y up, z forward
    Vec3 scale;           // size of the cube
    float lateralSpeed;   // units/sec for A/D movement
    float forwardSpeed;   // units/sec forward

    void init(Core* core, PSOManager* psos, Shaders* shaders)
    {
        cube.init(core, psos, shaders);
        position = Vec3(0.0f, 0.5f, 0.0f); // start slightly above the plane
        scale = Vec3(0.5f, 0.5f, 0.5f);
        lateralSpeed = 5.0f;
        forwardSpeed = 6.0f;
        world.identity();
        world.scaling(scale);
        world.translation(position);
    }

    void update(float dt, const bool* keys /* from Window */)
    {
        // Only left/right input
        if (keys['A'] == 1) position.x += lateralSpeed * dt;
        if (keys['D'] == 1) position.x -= lateralSpeed * dt;

        // Always move forward (+z)
            position.z -= forwardSpeed * dt;

        // Clamp lanes if desired (optional):
        // position.x = std::max(-4.0f, std::min(4.0f, position.x));

        // Rebuild world transform (scale -> translation)
        world.identity();
        world.scaling(scale);
        world.translation(position);
    }

    Matrix OrbitCamera(Window& win, float dt) {
        float aspect = (float)win.width / (float)win.height;
        float fovDeg = 60.0f; // if your Matrix::perspectiveProjection expects degrees
        Matrix p; p = p.perspectiveProjection(aspect, fovDeg, 0.01f, 1000.0f);

        Vec3 from = Vec3(11.0f * cosf(dt), 5.0f, 11.0f * sinf(dt));
        Matrix v; v = v.lookAtMatrix(from, Vec3(0, 0, 0), Vec3(0, 1, 0));

        Matrix vp = p.multiply(v);

        return vp;
    }

    Matrix NewCamera(Window& win, const Vec3& playerPos, float dt) {
        float aspect = (float)win.width / (float)win.height;

        // 1. Setup Projection (Degrees)
        Matrix p; p = p.perspectiveProjection(aspect, 60.0f, 0.01f, 1000.0f);

        // 2. Define Ideal Offset
        const float camHeight = 5.0f;
        const float camDist = 11.0f;

        // The "Goal" position for this frame
        Vec3 targetPos = Vec3(playerPos.x, playerPos.y + camHeight, playerPos.z + camDist);

        // 3. Smoothly Interpolate (Lerp)
        // We use a static variable to store where the camera WAS last frame.
        // Initialize it to targetPos so it doesn't fly in from (0,0,0) at the start.
        static Vec3 currentCamPos = targetPos;

        // Calculate smoothness (Higher 6.0f = faster/stiffer, Lower = slower/floatier)
        float alpha = 6.0f * dt;
        if (alpha > 1.0f) alpha = 1.0f; // Clamp to prevent overshooting

        // Linear Interpolation: Current = Current + (Target - Current) * alpha
        currentCamPos.x += (targetPos.x - currentCamPos.x) * alpha;
        currentCamPos.y += (targetPos.y - currentCamPos.y) * alpha;
        currentCamPos.z += (targetPos.z - currentCamPos.z) * alpha;

        // 4. Create View Matrix
        // We look at the player's current position, but from our smoothed camera position
        Matrix v; v = v.lookAtMatrix(currentCamPos, playerPos, Vec3(0, 1, 0));

        return p.multiply(v);
    }

    void draw(Core* core, PSOManager* psos, Shaders* shaders, Matrix& vp)
    {
        cube.draw(core, psos, shaders, vp);
    }
};