#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <dxgidebug.h>
#include <d3dcompiler.h>

#include <DirectXMath.h>

//PreCompiled lib files for d12:
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "D3DCompiler.lib")

#include <vector>

#include "DXGIManager.h"
#include "D3D12Manager.h"
#include "AssetManager.h"

#include "ECSManager.h"

class Camera;

class Component;
class InputControlerComponent;
class TransformComponent;
class CameraComponent;

class GraphicEngine
{
private:

	struct Vertex
	{
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT2 uv;
		DirectX::XMFLOAT3 normal;
	};

	struct Frustum
	{
		DirectX::XMFLOAT4 planes[6];
	};

	struct BoundingBox
	{
		DirectX::XMFLOAT3 center;
		DirectX::XMFLOAT3 scaling;


		bool Contains(const BoundingBox& box);
		bool Intersects(const BoundingBox& box);
		bool IsInFrustum(const Frustum& frustum);

	};

	struct Constant
	{
		DirectX::XMMATRIX m_world;
		DirectX::XMMATRIX m_view;
		DirectX::XMMATRIX m_proj;
		DirectX::XMFLOAT4 mLightDirection;
		DirectX::XMFLOAT4 mCameraPosition;
	};

	struct PostProcessParams
	{
		float brightness = 0.0f;
		float contrast = 1.0f;
		float saturation = 1.0f;
		float damageIntensity = 0.0f;
		float pad[12];
	};

public:
	struct Mesh
	{
		float width, height, depth = 0.0f;
		float x, y, z = 0.0f;
		DirectX::XMMATRIX localMatrix = {};
		DirectX::XMMATRIX worldMatrix = {};

		ID3D12Resource* constantBuffer = nullptr;
		ID3D12Resource* indexBuffer = nullptr;
		ID3D12Resource* vertexBuffer = nullptr;

		ID3D12DescriptorHeap* srvHeap = nullptr;

		D3D12_VERTEX_BUFFER_VIEW vertexBufferView = {};
		D3D12_INDEX_BUFFER_VIEW indexBufferView = {};
		Mesh* parent = nullptr;

		BoundingBox boundingBox = {};
	};

	struct UIElement
	{
		unsigned int id;

		DirectX::XMFLOAT2 position = { 0, 0 };
		DirectX::XMFLOAT2 size = { 0, 0 };
		ID3D12DescriptorHeap* srvHeap = nullptr;

		ID3D12Resource* constantBuffer = nullptr;
		ID3D12Resource* vertexBuffer = nullptr;
		ID3D12Resource* indexBuffer = nullptr;

		D3D12_VERTEX_BUFFER_VIEW vertexBufferView = {};
		D3D12_INDEX_BUFFER_VIEW indexBufferView = {};

		DirectX::XMMATRIX transform;

		UIElement(DirectX::XMFLOAT2 pos, DirectX::XMFLOAT2 sz, ID3D12DescriptorHeap* tex);

	};

	struct PostProcessQuad
	{
		unsigned int id;

		ID3D12DescriptorHeap* srvHeap = nullptr;

		ID3D12Resource* constantBuffer = nullptr;
		ID3D12Resource* vertexBuffer = nullptr;
		ID3D12Resource* indexBuffer = nullptr;

		D3D12_VERTEX_BUFFER_VIEW vertexBufferView = {};
		D3D12_INDEX_BUFFER_VIEW indexBufferView = {};

		PostProcessParams ppp = { 0.0f, 1.0f, 1.0f, 0.0f };
	};

public:
	GraphicEngine() = default;
	~GraphicEngine();

	void Initialize(HWND hwnd, int width, int height);

	void UpdateDraw();

	static GraphicEngine* Get();

private:
	DXGIManager* mpDxgiManager = nullptr;
	D3D12Manager* mpD3D12Manager = nullptr;
	AssetManager* mpAssetManager = nullptr;

private:
	Frustum GetFrustrum(const DirectX::XMMATRIX& viewProj);
	

	void UpdateWorldMatrix();

	DirectX::XMFLOAT3 ComputeNormal(DirectX::XMFLOAT3 p1, DirectX::XMFLOAT3 p2, DirectX::XMFLOAT3 p3);

	void DrawScene();
	void RenderUIElements();
	void ApplyPostProcessing();
	void UpdatePostProcessSRV(PostProcessQuad* ppq);

	void UpdateMeshToDelete();


public:
	void CreateCube(float width, float height, float depth, float x, float y, float z, LPCWSTR texturePath, Mesh* parent);
	Mesh* CreateSphere(float radius, int sliceCount, int stackCount, float x, float y, float z, LPCWSTR texturePath, bool isInversed, bool isMissile, Mesh* parent = nullptr);
	void CreateMeshFromFile(LPCWSTR filename, float width, float height, float depth, float x, float y, float z, LPCWSTR texturePath, Mesh* parent);

	void Delete(Mesh* mesh);

	void CreateUI(DirectX::XMFLOAT2 pos, DirectX::XMFLOAT2 sz, LPCWSTR tex, unsigned int id);
	void CreatePostProcess(float brightness, float contrast, float saturation, float damageIntensity, unsigned int id);

	void SetActiveUI(unsigned int id);
	void SetActivePostProcessing(unsigned int id);
	void RemoveActiveUI(unsigned int id);
	void RemoveActivePostProcessing(unsigned int id);
	void ModifyPostProcess(unsigned int id, float brightness, float contrast, float saturation, float damageIntensity);

	inline CameraComponent* GetCamera() { return mECSManager->GetComponent<CameraComponent>(mECSManager->enumCameraComponent, mCamera); }
	inline InputControlerComponent* GetInput() { return mECSManager->GetComponent<InputControlerComponent>(mECSManager->enumInputControlerComponent, mCamera); }
	inline TransformComponent* GetCameraTransform() { return mECSManager->GetComponent<TransformComponent>(mECSManager->enumTransformComponent, mCamera); }
	inline int GetWidth() { return mWidth; };
	inline int GetHeight() { return mHeight; };

private:


	D3D12_VIEWPORT mViewport;
	D3D12_RECT mRect;

	int mCamera;
	/*CameraComponent* mCameraComponent;
	TransformComponent* mTransformComponent;*/
	

private:
	int mWidth, mHeight;

	float mXPosition = 0.0f;
	float mYPosition = 0.0f;
	float mZPosition = 0.0f;
	float mXRotationAngle = 0.0f;
	float mYRotationAngle = 0.0f;
	float mZRotationAngle = 0.0f;


	DirectX::XMFLOAT4 mLightPos;

	std::vector<Mesh*> mMeshToDelete;
	std::vector<UIElement*> mUIElements;
	std::vector<PostProcessQuad*> mPPQuad;

	std::vector<UIElement*> mActiveUIElements;
	std::vector<PostProcessQuad*> mActivePostProcesses;

	std::vector<unsigned int> cubeIndices;
	ID3D12Resource* cubeIndexBuffer;
	D3D12_INDEX_BUFFER_VIEW cubeIndexBufferView;

	ID3D12Resource* sphereIndexBuffer;
	D3D12_INDEX_BUFFER_VIEW sphereIndexBufferView;

	bool isCubeInit = false;
	bool isSphereInit = false;

public:
	std::vector<Mesh*> mObjects;
	ECSManager* mECSManager;


};