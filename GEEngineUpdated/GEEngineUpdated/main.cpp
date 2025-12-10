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

    ShowCursor(FALSE);

    while (true) {
        core.beginFrame();
        win.processMessages();
        float dt = tim.dt();

        // Player updates inputs, state, camera, and gun matrix internally
        Matrix vp = player.update(win, dt);

        // Trex updates AI, state, and world matrix internally
        trex.update(dt, win);

        // Tree update (Scale)
        Matrix treeMatrix;
        treeMatrix.scaling(Vec3(0.01f, 0.01f, 0.01f));
        tree.update(&shaders, treeMatrix);

        // RENDER
        shaders.updateConstantVS("static", "staticMeshBuffer", "VP", &vp);
        core.beginRenderPass();

        // Static Draw
        Matrix planeM; planeM.translation(Vec3(0, 0, 0));
        floor.draw(&core, &psos, &shaders, vp, planeM);
        sphere.draw(&core, &psos, &shaders, vp);
        tree.draw(&core, &psos, &shaders, vp);

        // Entity Draw
        trex.draw(&core, &psos, &shaders, vp, &textureManager);

        // Draw Player Gun
        player.draw(&core, &psos, &shaders, vp, &textureManager);

        core.finishFrame();
    }
    core.flushGraphicsQueue();
}