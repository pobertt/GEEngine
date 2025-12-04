#pragma once

#include "Core.h"
#include "GEMLoader.h"
#include "Animation.h"
#include "Mesh.h"
#include "PipeLineState.h"
#include "Shader.h"


class AnimatedModel
{
public:
	std::vector<Mesh*> meshes;
	Animation animation;
	std::vector<std::string> textureFilenames;
	void load(Core* core, std::string filename, PSOManager* psos, Shaders* shaders)
	{
		GEMLoader::GEMModelLoader loader;
		std::vector<GEMLoader::GEMMesh> gemmeshes;
		GEMLoader::GEMAnimation gemanimation;
		loader.load(filename, gemmeshes, gemanimation);
		for (int i = 0; i < gemmeshes.size(); i++)
		{
			Mesh* mesh = new Mesh();
			std::vector<ANIMATED_VERTEX> vertices;
			for (int j = 0; j < gemmeshes[i].verticesAnimated.size(); j++)
			{
				ANIMATED_VERTEX v;
				memcpy(&v, &gemmeshes[i].verticesAnimated[j], sizeof(ANIMATED_VERTEX));
				vertices.push_back(v);
			}
			mesh->init(core, vertices, gemmeshes[i].indices);
			meshes.push_back(mesh);
		}
		shaders->load(core, "AnimatedUntextured", "VSAnim.txt", "PSUntextured.txt");
		psos->createPSO(core, "AnimatedModelPSO", shaders->find("AnimatedUntextured")->vs, shaders->find("AnimatedUntextured")->ps, VertexLayoutCache::getAnimatedLayout());
		memcpy(&animation.skeleton.globalInverse, &gemanimation.globalInverse, 16 * sizeof(float));
		for (int i = 0; i < gemanimation.bones.size(); i++)
		{
			Bone bone;
			bone.name = gemanimation.bones[i].name;
			memcpy(&bone.offset, &gemanimation.bones[i].offset, 16 * sizeof(float));
			bone.parentIndex = gemanimation.bones[i].parentIndex;
			animation.skeleton.bones.push_back(bone);
		}
		for (int i = 0; i < gemanimation.animations.size(); i++)
		{
			std::string name = gemanimation.animations[i].name;
			AnimationSequence aseq;
			aseq.ticksPerSecond = gemanimation.animations[i].ticksPerSecond;
			for (int j = 0; j < gemanimation.animations[i].frames.size(); j++)
			{
				AnimationFrame frame;
				for (int index = 0; index < gemanimation.animations[i].frames[j].positions.size(); index++)
				{
					Vec3 p;
					Quaternion q;
					Vec3 s;
					memcpy(&p, &gemanimation.animations[i].frames[j].positions[index], sizeof(Vec3));
					frame.positions.push_back(p);
					memcpy(&q, &gemanimation.animations[i].frames[j].rotations[index], sizeof(Quaternion));
					frame.rotations.push_back(q);
					memcpy(&s, &gemanimation.animations[i].frames[j].scales[index], sizeof(Vec3));
					frame.scales.push_back(s);
				}
				aseq.frames.push_back(frame);
			}
			animation.animations.insert({ name, aseq });
		}
	}
	void updateWorld(Shaders* shaders, Matrix& w)
	{
		shaders->updateConstantVS("AnimatedUntextured", "staticMeshBuffer", "W", &w);
	}
	void draw(Core* core, PSOManager* psos, Shaders* shaders, AnimationInstance* instance, Matrix& vp, Matrix& w)
	{
		psos->bind(core, "AnimatedModelPSO");
		shaders->updateConstantVS("AnimatedUntextured", "staticMeshBuffer", "W", &w);
		shaders->updateConstantVS("AnimatedUntextured", "staticMeshBuffer", "VP", &vp);
		shaders->updateConstantVS("AnimatedUntextured", "staticMeshBuffer", "bones", instance->matrices);
		shaders->apply(core, "AnimatedUntextured");
		for (int i = 0; i < meshes.size(); i++)
		{
			meshes[i]->draw(core);
		}
	}
};