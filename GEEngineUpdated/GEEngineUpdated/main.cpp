#define STB_IMAGE_IMPLEMENTATION
#include "window.h"
#include "core.h"
#include "Player.h"
#include "TRex.h" 
#include "Objects.h" 
#include "Collision.h" 

// [REMOVED DrawSolidBox Function]

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nCmdShow) {
    Window win;
    win.initialize("Game Engine", 1024, 1024);
    Core core;
    core.init(win.hwnd, 1024, 1024);
    GamesEngineeringBase::Timer tim;

    Shaders shaders;
    PSOManager psos;
    TextureManager textureManager;

    // --- 1. INIT SHADERS & PSOs ---

    // Static Shader
    shaders.load(&core, "static", "Resources/Shaders/VS.hlsl", "Resources/Shaders/PS.hlsl");
    psos.createPSO(&core, "staticPSO", shaders.find("static")->vs, shaders.find("static")->ps, VertexLayoutCache::getStaticLayout());

    // Transparent PSO (For Muzzle Flash)
    psos.createTransparentPSO(
        &core,
        "transparent",
        shaders.find("static")->vs,
        shaders.find("static")->ps,
        VertexLayoutCache::getStaticLayout(),
        true
    );

    // [REMOVED SolidShader Loading] (It was only for debug boxes)


    // --- 2. LOAD ASSETS ---
    textureManager.loadTexture(&core, "MuzzleFlashTex", "Resources/Models/Textures/muzzleflash.png");
    textureManager.loadTexture(&core, "GrassTexture", "Resources/Models/Textures/TX_Grass_Sets_01a_ALB.png");

    Grass grassField;
    grassField.init(&core, &psos, &shaders, "Resources/Models/Grass_Sets_01a.gem", 10000);

    Plane floor; floor.init(&core, &psos, &shaders);
    Sphere sphere; sphere.init(&core, &psos, &shaders, 20, 20, 20);

    Player player;
    player.init(&core, &psos, &shaders, &textureManager);
    player.setFlashMesh(&floor);

    TRex trex;
    trex.init(&core, &psos, &shaders, &textureManager);

    staticModel tree;
    tree.init(&core, &psos, &shaders, "Resources/Models/Ash_Tree_Full_01j.gem", &textureManager);
    staticModel ammoBox;
    ammoBox.init(&core, &psos, &shaders, "Resources/Models/Ammo_Boxes_01a.gem", &textureManager);

    // Define tree transform
    Matrix treeMatrix;
    treeMatrix.scaling(Vec3(1.0f, 1.0f, 1.0f));
    treeMatrix.translation(Vec3(5, 0, 0));
    // Define ammo transform
    Matrix ammoMatrix;
    ammoMatrix.scaling(Vec3(5.0f, 5.0f, 5.0f));
    ammoMatrix.translation(Vec3(10, 0, 0));

    ShowCursor(FALSE);

    // --- 3. GAME LOOP ---
    while (true) {
        core.beginFrame();
        win.processMessages();
        float dt = tim.dt();

        // Logic
        Matrix vp = player.update(win, dt);
        trex.update(dt, win, player.position);
        tree.update(&shaders, treeMatrix);
        ammoBox.update(&shaders, ammoMatrix);
        player.handleShooting(trex);

        // Render Setup
        shaders.updateConstantVS("static", "staticMeshBuffer", "VP", &vp);
        core.beginRenderPass();

        Vec3 resolution;

        // Player vs TRex
        if (Collision::CheckBoundingBox(player.collider, trex.collider, resolution)) {
            // Push player out
            player.position = player.position + resolution;
        }

        // Draw Solids
        Matrix planeM; planeM.translation(Vec3(0, 0, 0));
        floor.draw(&core, &psos, &shaders, vp, planeM);
        sphere.draw(&core, &psos, &shaders, vp);
        tree.draw(&core, &psos, &shaders, vp, treeMatrix, &textureManager);
        ammoBox.draw(&core, &psos, &shaders, vp, ammoMatrix, &textureManager);
        trex.draw(&core, &psos, &shaders, vp, &textureManager);
        player.draw(&core, &psos, &shaders, vp, &textureManager);

        // Draw Transparents / Effects
        player.drawFlash(&core, &psos, &shaders, vp, &textureManager);
        grassField.draw(&core, &psos, &shaders, vp, dt, &textureManager);

        // [REMOVED Debug Drawing Section]

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

/*

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

    shaders.load(&core, "static", "Resources/Shaders/VS.hlsl", "Resources/Shaders/PS.hlsl");

    psos.createTransparentPSO(
        &core,
        "transparent",                  // Name must match MuzzleFlash::draw
        shaders.find("static")->vs,     // Use standard Vertex Shader
        shaders.find("static")->ps,     // Use standard Pixel Shader
        VertexLayoutCache::getStaticLayout(),
        true
    );

    textureManager.loadTexture(&core, "MuzzleFlashTex", "Resources/Models/Textures/muzzleflash.png");
    player.setFlashMesh(&floor);

    staticModel tree; tree.init(&core, &psos, &shaders, "Resources/Models/acacia_003.gem");
    Matrix treeMatrix;
    treeMatrix.scaling(Vec3(0.01f, 0.01f, 0.01f));
    Matrix scale; scale.identity();
    treeMatrix.scaling(Vec3(0.01f, 0.01f, 0.01f));
    treeMatrix.translation(Vec3(5, 0, 0));
    Vec3 treePosition(5.0f, 0.0f, 0.0f);

    // 1. Load Texture
    textureManager.loadTexture(&core, "MuzzleFlashTex", "Resources/Textures/MuzzleFlash.png");

    // 2. Init Flash


    ShowCursor(FALSE);



    while (true) {
        core.beginFrame();
        win.processMessages();
        float dt = tim.dt();

        // Updates
        Matrix vp = player.update(win, dt);

        trex.update(dt, win);

        tree.update(&shaders, treeMatrix);

        player.handleShooting(trex);

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


        // Static Draw
        Matrix planeM; planeM.translation(Vec3(0, 0, 0));
        floor.draw(&core, &psos, &shaders, vp, planeM);
        sphere.draw(&core, &psos, &shaders, vp);
        tree.draw(&core, &psos, &shaders, vp, treeMatrix);

        // Entity Draw
        trex.draw(&core, &psos, &shaders, vp, &textureManager);

        // Draw Player
        player.draw(&core, &psos, &shaders, vp, &textureManager);

        player.drawFlash(&core, &psos, &shaders, vp, &textureManager);

        core.finishFrame();
    }
    core.flushGraphicsQueue();
}
*/