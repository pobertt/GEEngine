#define STB_IMAGE_IMPLEMENTATION
#include "window.h"
#include "core.h"
#include "Player.h"
#include "TRex.h" 
#include "Objects.h" 
#include "Collision.h" 

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

    // --- 2. LOAD ASSETS ---
    textureManager.loadTexture(&core, "MuzzleFlashTex", "Resources/Models/Textures/muzzleflash.png");
    textureManager.loadTexture(&core, "SkyboxTex", "Resources/Models/Textures/sky_map.png");

    Plane floor; floor.init(&core, &psos, &shaders);
    Sphere sphere; sphere.init(&core, &psos, &shaders, 20, 20, 20);

    Cube skyBox; skyBox.init(&core, &psos, &shaders);

    InstancedTrees oakTrees;
    oakTrees.init(&core, &psos, &shaders, &textureManager, "Resources/Models/maple.gem", 20, 2.0f, -50.0f, 50.0f, -50.0f, 50.0f);

    Player player;
    player.init(&core, &psos, &shaders, &textureManager);
    player.setFlashMesh(&floor);

    TRex trex;
    trex.init(&core, &psos, &shaders, &textureManager);

    staticModel tree;
    tree.init(&core, &psos, &shaders, "Resources/Models/maple.gem", &textureManager);
    staticModel ammoBox;
    ammoBox.init(&core, &psos, &shaders, "Resources/Models/Ammo_Boxes_01a.gem", &textureManager);
    staticModel grass;
    grass.init(&core, &psos, &shaders, "Resources/Models/Grass_04g.gem", &textureManager);

    // Define tree transform
    Matrix treeMatrix;
    treeMatrix.scaling(Vec3(0.01f, 0.01f, 0.01f));
    treeMatrix.translation(Vec3(5, 0, 0));
    // Define ammo transform
    Matrix ammoMatrix;
    ammoMatrix.scaling(Vec3(25.0f, 25.0f, 25.0f));
    ammoMatrix.translation(Vec3(10, 0, 0));

    Matrix skyboxM;
    skyboxM.scaling(Vec3(125.0f, 125.0f, 125.0f));

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
        oakTrees.update(dt);
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

        
        oakTrees.draw(&core, &psos, &shaders, &textureManager, vp, 20);
        //should draw 5 of whatever model you put in 


        // If your cube needs a transform, set it here similarly to planeM
        skyBox.draw(&core, &psos, &shaders, &textureManager, vp, skyboxM);
        

        // Draw Solids
        Matrix planeM; planeM.translation(Vec3(0, 0, 0)); planeM.scaling(Vec3(50.0f, 50.0f, 50.0f));
        floor.draw(&core, &psos, &shaders, vp, planeM);
        //sphere.draw(&core, &psos, &shaders, &textureManager, vp, planeM);
        tree.draw(&core, &psos, &shaders, vp, treeMatrix, &textureManager);
        ammoBox.draw(&core, &psos, &shaders, vp, ammoMatrix, &textureManager);
        grass.draw(&core, &psos, &shaders, vp, ammoMatrix, &textureManager);
        trex.draw(&core, &psos, &shaders, vp, &textureManager);
        player.draw(&core, &psos, &shaders, vp, &textureManager);

        // Draw Transparents / Effects
        player.drawFlash(&core, &psos, &shaders, vp, &textureManager);

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