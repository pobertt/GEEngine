#include "core.h"
#include "maths.h"
#include "Shader.h"
#include "Mesh.h"
#include "Static_Vertex.h"

class animatedModel {
public:
	AnimatedMesh mesh;
	std::string shaderName;
	Shader shader;
	PSOManager psos;

	void init(Core* core, std::string filename) {
		mesh.init(core, filename);
		shader.LoadShaders("VSAnim.hlsl", "PSUntextured.hlsl");
		shader.ReflectShaders(core, shader.vertexShader, true);
		shader.ReflectShaders(core, shader.pixelShader, false);

		psos.createPSO(
			core,
			"AnimatedModelPSO",
			shader.vertexShader,
			shader.pixelShader,
			mesh.meshes[0]->inputLayoutDesc
		);
	}

	void update(Matrix& w) {
		shader.vsConstantBuffers[0]->update("W", &w);
	}

	void draw(Core* core, AnimationInstance* instance, Matrix& vp, Matrix& w) {
		psos.bind(core, "AnimatedModelPSO");
		shader.vsConstantBuffers[0]->update("W", &w);
		shader.vsConstantBuffers[0]->update("VP", &vp);
		shader.vsConstantBuffers[0]->update("bones", instance->matrices);
		shader.apply(core);
		mesh.draw(core);
	}
};