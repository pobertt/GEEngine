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

    textureManager.loadTexture(&core, "MuzzleFlashTex", "Resources/Models/Textures/muzzleflash.png");
    textureManager.loadTexture(&core, "SkyboxTex", "Resources/Models/Textures/sky_map.png");

    Plane floor; floor.init(&core, &psos, &shaders);
    Sphere sphere; sphere.init(&core, &psos, &shaders, 20, 20, 20);

    Skybox skyBox; skyBox.init(&core, &psos, &shaders);

    InstancedTrees oakTrees;
    oakTrees.init(&core, &psos, &shaders, &textureManager, "Resources/Models/maple.gem", 50, 0.0f, -50.0f, 50.0f, -50.0f, 50.0f, Vec3(0.01f, 0.01f, 0.01f));
    InstancedModels grassInstanced;
    grassInstanced.init(&core, &psos, &shaders, &textureManager, "Resources/Models/Grass_04g.gem", 50000, 0.0f, -50.0f, 50.0f, -50.0f, 50.0f, Vec3(2.0f, 2.0f, 2.0f));

    Player player;
    player.init(&core, &psos, &shaders, &textureManager);
    player.setFlashMesh(&floor);

    TRex trex;
    trex.init(&core, &psos, &shaders, &textureManager);

    staticModel ammoBox;
    ammoBox.init(&core, &psos, &shaders, "Resources/Models/Ammo_Boxes_01a.gem", &textureManager);

    Matrix treeMatrix;
    treeMatrix.scaling(Vec3(0.01f, 0.01f, 0.01f));
    treeMatrix.translation(Vec3(5, 0, 0));

    Matrix ammoMatrix;
    ammoMatrix.scaling(Vec3(25.0f, 25.0f, 25.0f));
    ammoMatrix.translation(Vec3(10, 0, 0));

    ShowCursor(FALSE);

    while (true) {
        core.beginFrame();
        win.processMessages();
        float dt = tim.dt();

        // Logic
        Matrix vp = player.update(win, dt);

        trex.update(dt, win, player.position);
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

        const std::vector<Vec3>& treePositions = oakTrees.mesh.getPositions();

        Vec3 treeSize(0.5f, 10.0f, 0.5f);

        // Player vs Instanced Trees
        for (const Vec3& treePos : treePositions) {

            // Create collider for a specific tree
            BoundingBox treeCollider;
            treeCollider.min = treePos - treeSize;
            treeCollider.max = treePos + treeSize;

            Vec3 resolution;
            if (Collision::CheckBoundingBox(player.collider, treeCollider, resolution)) {
                // Push player out of this specific tree
                player.position = player.position + resolution;
            }
        }
        
        oakTrees.draw(&core, &psos, &shaders, &textureManager, vp, 50);
        grassInstanced.draw(&core, &psos, &shaders, &textureManager, vp, 50000);

        skyBox.draw(&core, &psos, &shaders, &textureManager, vp, player.position);
        

        // Draw Solids
        Matrix planeM; planeM.translation(Vec3(0, 0, 0)); planeM.scaling(Vec3(50.0f, 50.0f, 50.0f));
        //floor.draw(&core, &psos, &shaders, vp, planeM);
        ammoBox.draw(&core, &psos, &shaders, vp, ammoMatrix, &textureManager);
        trex.draw(&core, &psos, &shaders, vp, &textureManager);
        player.draw(&core, &psos, &shaders, vp, &textureManager);

        // Draw Transparents / Effects
        player.drawFlash(&core, &psos, &shaders, vp, &textureManager);

        core.finishFrame();
    }
    core.flushGraphicsQueue();
}

// Things to do:
// Add grass
// Update trex collision box rotation
// Remove skybox line
// Add floor
