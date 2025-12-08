#include "core.h"
#include "maths.h"
#include "Shader.h"
#include "Mesh.h"

class animatedModel {
public:
	AnimatedMesh mesh;
	std::string shaderName;

	void init(Core* core, PSOManager* psos, Shaders* shaders, std::string filename) {
		mesh.init(core, filename);
		shaders->load(core, "animatedModel", "VSAnim.hlsl", "PSUntextured.hlsl");
		psos->createPSO(core, "animatedModelPSO", shaders->find("animatedModel")->vs, shaders->find("animatedModel")->ps, VertexLayoutCache::getAnimatedLayout());
	}

	void update(Shaders* shaders, Matrix& w) {
		shaders->updateConstantVS("animatedModel", "staticMeshBuffer", "W", &w);
	}

	void draw(Core* core, PSOManager* psos, Shaders* shaders, AnimationInstance* instance, Matrix& vp, Matrix& w) {
		psos->bind(core, "animatedModelPSO");
		shaders->updateConstantVS("animatedModel", "staticMeshBuffer", "W", &w);
		shaders->updateConstantVS("animatedModel", "staticMeshBuffer", "VP", &vp);
		shaders->updateConstantVS("animatedModel", "staticMeshBuffer", "bones", instance->matrices);
		shaders->apply(core, "animatedModel");
		mesh.draw(core);
	}
};