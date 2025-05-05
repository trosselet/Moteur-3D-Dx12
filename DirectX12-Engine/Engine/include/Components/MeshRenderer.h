#ifndef MESHRENDERER__H
#define MESHRENDERER__H


#include "Define.h"
#include "Vector.hpp"
#include "ComponentBase.h"

#include "PipelineStateObjectManager.h"

struct Geometry;
class Mesh;
class Material;

namespace Engine
{
	struct MeshRenderer : ComponentBase<Component::MeshRenderer>
	{
		uint8 renderLayer = 0;

		void SetRectangle();
		void SetRectangle(const char* texturePath);

		void SetCircle();
		void SetCircle(const char* texturePath);

		void SetSphere();
		void SetSphere(const char* texturePath);

		void SetCube();
		void SetCube(const char* texturePath);

		void SetMaterialProperties(Vector<float32, 4> lightColor, Vector<float32, 4> ambient, Vector<float32, 4> diffuse, Vector<float32, 4> specular, float32 shininess, bool ingoreLighting = false);

		~MeshRenderer() override;

	private:
		void Free();

		bool      m_primitive = false;
		Geometry* m_pGeometry = nullptr;
		Material* m_pMaterial = nullptr;
		Mesh* m_pMesh = nullptr;
		PipelineStateObjectManager::PipelineStateConfig* m_pShader = nullptr;

		friend class RenderSystem;
	};
}

#endif // !MESHRENDERER__H
