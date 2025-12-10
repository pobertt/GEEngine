#define STB_IMAGE_IMPLEMENTATION
#include "window.h"
#include "core.h"
#include "Player.h"
#include "TRex.h" 
#include "Objects.h" 

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nCmdShow) {
    Window win;
    win.initialize("Game Engine", 1024, 1024);
    Core core;
    core.init(win.hwnd, 1024, 1024);
    GamesEngineeringBase::Timer tim;

    // Managers
    Shaders shaders;
    PSOManager psos;
    TextureManager textureManager;

    // IMPORTANT: Load a default white texture for untextured objects (Plane/Sphere)
    //textureManager.loadTexture(&core, "White", "Resources/Models/Textures/White.png");

    // --- ENTITIES ---
    Player player;
    player.init(&core, &psos, &shaders, &textureManager);

    TRex trex;
    trex.init(&core, &psos, &shaders, &textureManager);

    // Static Objects
    Plane floor; floor.init(&core, &psos, &shaders);
    Sphere sphere; sphere.init(&core, &psos, &shaders, 20, 20, 20);

    staticModel tree; tree.init(&core, &psos, &shaders, "Resources/Models/acacia_003.gem");
    Matrix treeMatrix;
    treeMatrix.scaling(Vec3(0.01f, 0.01f, 0.01f));
    Matrix scale; scale.identity();
    treeMatrix.scaling(Vec3(0.01f, 0.01f, 0.01f));
    treeMatrix.translation(Vec3(5, 0, 0));
    Vec3 treePosition(5.0f, 0.0f, 0.0f);
    
    ShowCursor(FALSE);

    while (true) {
        core.beginFrame();
        win.processMessages();
        float dt = tim.dt();
        
        // Updates
        Matrix vp = player.update(win, dt);

        trex.update(dt, win);

        tree.update(&shaders, treeMatrix);

        // RENDER
        shaders.updateConstantVS("static", "staticMeshBuffer", "VP", &vp);
        core.beginRenderPass();

        // COLLISION
        BoundingBox treeBox = tree.mesh.meshes[0]->boundingBox; // Get raw box
        treeBox.min = treeBox.min * 0.01f; // Apply scale
        treeBox.max = treeBox.max * 0.01f; // Apply scale

        Vec3 resolution;

        // Player vs Tree
        if (!tree.mesh.meshes.empty()) {
            BoundingBox treeBox = tree.mesh.meshes[0]->boundingBox;

            treeBox.min = treeBox.min * 0.01f;
            treeBox.max = treeBox.max * 0.01f;

            treeBox.min = treeBox.min + treePosition;
            treeBox.max = treeBox.max + treePosition;

            Vec3 resolution;
            if (Collision::CheckBoundingBox(player.collider, treeBox, resolution)) {
                player.position = player.position + resolution;
            }
        }

        // Player vs TRex
        if (Collision::CheckBoundingBox(player.collider, trex.collider, resolution)) {
            // Push player out
            player.position = player.position + resolution;
        }

        // Floor collision
        if (player.position.y < 3.0f) {
            player.position.y = 3.0f;
        }

        // Static Draw
        Matrix planeM; planeM.translation(Vec3(0, 0, 0));
        floor.draw(&core, &psos, &shaders, vp, planeM);
        sphere.draw(&core, &psos, &shaders, vp);
        tree.draw(&core, &psos, &shaders, vp, treeMatrix);

        // Entity Draw
        trex.draw(&core, &psos, &shaders, vp, &textureManager);

        // Draw Player
        player.draw(&core, &psos, &shaders, vp, &textureManager);

        core.finishFrame();
    }
    core.flushGraphicsQueue();
}

// Things to do:
// Add grass
// Add a skylight
// Add trex AI
// Add trees into the level
// Add collision
// Bullets???
// Muzzle flash???
// Blood??? 
// could i do shadows?