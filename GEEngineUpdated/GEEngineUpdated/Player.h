#pragma once
#include "Cube.h"
#include "maths.h"
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

    void init(Core* core)
    {
        cube.init(core);
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
        if (keys['A'] == 1) position.x -= lateralSpeed * dt;
        if (keys['D'] == 1) position.x += lateralSpeed * dt;

        // Always move forward (+z)
            position.z += forwardSpeed * dt;

        // Clamp lanes if desired (optional):
        // position.x = std::max(-4.0f, std::min(4.0f, position.x));

        // Rebuild world transform (scale -> translation)
        world.identity();
        world.scaling(scale);
        world.translation(position);
    }

    Matrix camera(Window& win, const Vec3& playerPos, float dt) {
        /*
        // needed for perspectiveProjection matrix
		float aspect = (float)win.width / (float)win.height;
		float fieldOfView = 60.0f;
		float _near = 0.01f;
		float _far = 10000.0f;

		// Perspective Projection - aspect, fov, near, far
		Matrix p = p.perspectiveProjection(aspect, fieldOfView, _near, _far);

		// Camera orbit
		Vec3 from = Vec3(0.0f, 5.0f, 15.0f);//Vec3(11 * cos(t), 5, 11 * sinf(t));
        */


        float aspect = (float)win.width / (float)win.height;
        float fovRad = 70.0f * (3.14159265358979323846f / 180.0f);
        Matrix p; p = p.perspectiveProjection(aspect, fovRad, 0.01f, 2000.0f);

        const float camHeight = 800.0f;
        const float camBack   = 1000.0f;

        // Lock X for the camera
        static float camXLocked = 0.0f; // or set this once to playerPos.x
        Vec3 desiredCamPos = Vec3(camXLocked, playerPos.y + camHeight, playerPos.z - camBack);
        Vec3 camTarget     = Vec3(camXLocked, playerPos.y + 1.0f,      playerPos.z + 18.0f);

        static Vec3 camPos = desiredCamPos;
        float alpha = min(6.0f * dt, 1.0f);
        camPos.y += (desiredCamPos.y - camPos.y) * alpha;
        camPos.z += (desiredCamPos.z - camPos.z) * alpha;
        camPos.x  = camXLocked;

        Matrix v; v = v.lookAtMatrix(camPos, camTarget, Vec3(0, 1, 0));
        return p.multiply(v);
    }

    void draw(Core* core, Matrix& vp)
    {
        cube.draw(core, world, vp);
    }
};