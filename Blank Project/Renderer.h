#pragma once
#include "../NCLGL/OGLRenderer.h"
#include <nclgl/Frustrum.h>

class SceneNode;
class Camera;
class Material;
class Layer;

class Renderer : public OGLRenderer	{
public:
	Renderer(Window &parent);
	 ~Renderer(void);

	 void RenderScene()				override;
	 void UpdateScene(float msec)	override;

	 Camera* getCamera() { return camera; }

	 void ToggleDayNight();

protected:
	void LoadMaterials();
	void LoadMeshes();
	void BuildSceneGraph();
	void GenerateLights();
	void InitialiseShader();

	void DrawMainScene();

	void DrawShadowScene();
	void DrawNode(SceneNode* node);
	void DrawSkybox();

	void BuildNodeLists(SceneNode* from);
	void SortNodeLists();
	void DrawNodes();
	void ClearNodeLists();

	Material* terrainMats;
	Material* meshMats;
	Shader* shader;
	SceneNode* root;
	Camera* camera;
	Light* lights;
	
	Frustrum frustrum;

	Mesh** meshes;
	GLuint cubemap, shadowTex;
	GLuint64 shadowHndl;

	GLuint materialBuffer[9], lightBuffer[6], shadowFBO;

	Vector3 worldCentre;

	std::vector<SceneNode*> transparentNodeList;
	std::vector<SceneNode*> nodeList;

	bool toggleDN;
	float sunSpeed = 50;
};
