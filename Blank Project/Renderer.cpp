#include "Renderer.h"

#include "../nclgl/Camera.h"
#include "../nclgl/SceneNode.h"
#include "../nclgl/HeightMap.h"
#include "../nclgl/Light.h"
#include "../nclgl/Material.h"

#include <algorithm>

#define SHADOWSIZE 2048

Renderer::Renderer(Window &parent) : OGLRenderer(parent)	{
	shader = new Shader("UberVertexShader.glsl", "UberFragmentShader.glsl");
	LoadMaterials();
	LoadMeshes();
	BuildSceneGraph();
	GenerateLights();

	if(!shader->LoadSuccess()) {
		return;
	}

	camera = new Camera(-20, -45, 0, Vector3(2621, 4804, 31717));
	projMatrix = Matrix4::Perspective(1.0f, 1000000.0f, (float)width / (float)height, 45.0f);

	glGenBuffers(6, lightBuffer);
	glGenBuffers(9, materialBuffer);

	glGenFramebuffers(1, &shadowFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowTex, 0);
	glDrawBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	InitialiseShader();

	init = true;
}

void Renderer::LoadMaterials() {
	cubemap = SOIL_load_OGL_cubemap(TEXTUREDIR"Daylight Box_Right.bmp",
		TEXTUREDIR"Daylight Box_Left.bmp",
		TEXTUREDIR"Daylight Box_Top.bmp",
		TEXTUREDIR"Daylight Box_Bottom.bmp",
		TEXTUREDIR"Daylight Box_Front.bmp",
		TEXTUREDIR"Daylight Box_Back.bmp",
		SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0);

	terrainMats = new Material[4];
	GLuint texture = SOIL_load_OGL_texture(TEXTUREDIR"FD_gravel_2.bmp", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	GLuint bumpmap = SOIL_load_OGL_texture(TEXTUREDIR"FD_gravel_NM_2.bmp", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	terrainMats[0] = Material(texture, bumpmap, 0.33f, 0.0f, 0.1f, 0.0f);

	texture = SOIL_load_OGL_texture(TEXTUREDIR"FD_grass_1.bmp", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	bumpmap = SOIL_load_OGL_texture(TEXTUREDIR"FD_grass_NM_1.bmp", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	terrainMats[1] = Material(texture, bumpmap, 0.33f, 0.0f, 0.3f, 0.0f);

	texture = SOIL_load_OGL_texture(TEXTUREDIR"FD_cliff_1.bmp", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	bumpmap = SOIL_load_OGL_texture(TEXTUREDIR"FD_cliff_NM_1.bmp", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	terrainMats[2] = Material(texture, bumpmap, 0.33f, 0.0f, 0.6f, 0.0f);

	texture = SOIL_load_OGL_texture(TEXTUREDIR"FD_cliff_2.bmp", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	bumpmap = SOIL_load_OGL_texture(TEXTUREDIR"FD_cliff_NM_2.bmp", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	terrainMats[3] = Material(texture, bumpmap, 0.33f, 0.0f, 0.7f, 0.0f);

	meshMats = new Material[5];
	texture = SOIL_load_OGL_texture(TEXTUREDIR"tower/Brick.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	bumpmap = SOIL_load_OGL_texture(TEXTUREDIR"tower/Brick_bump.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	meshMats[0] = Material(texture, bumpmap, 0.33f, 0.0f, 0.4f, 0.0f);

	texture = SOIL_load_OGL_texture(TEXTUREDIR"tower/Fine Wood.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	bumpmap = SOIL_load_OGL_texture(TEXTUREDIR"stubdot3.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	meshMats[1] = Material(texture, bumpmap, 0.33f, 0.0f, 0.2f, 0.0f);

	texture = SOIL_load_OGL_texture(TEXTUREDIR"tower/Wood Planks.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	bumpmap = SOIL_load_OGL_texture(TEXTUREDIR"stubdot3.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	meshMats[2] = Material(texture, bumpmap, 0.33f, 0.0f, 0.1f, 0.0f);

	texture = SOIL_load_OGL_texture(TEXTUREDIR"water.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	bumpmap = SOIL_load_OGL_texture(TEXTUREDIR"waterbump.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	meshMats[3] = Material(texture, bumpmap, 0.33f, 0.0f, 0.05f, 0.02f);

	texture = SOIL_load_OGL_texture(TEXTUREDIR"stainedglass.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	bumpmap = SOIL_load_OGL_texture(TEXTUREDIR"stubdot3.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	meshMats[4] = Material(texture, bumpmap, 0.33f, 0.5f, 0.0f, 0.0f);

	glGenTextures(1, &shadowTex);
	glBindTexture(GL_TEXTURE_2D, shadowTex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOWSIZE, SHADOWSIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);
	shadowHndl = glGetTextureHandleARB(shadowTex);
	glMakeTextureHandleResidentARB(shadowHndl);
}

void Renderer::LoadMeshes() {
	meshes = new Mesh*[4];
	
	meshes[0] = Mesh::GenerateQuad();
	meshes[1] = Mesh::LoadFromMeshFile("tower.msh");
	meshes[2] = new HeightMap(TEXTUREDIR"blank.png");
	meshes[3] = Mesh::LoadFromMeshFile("offsetcubey.msh");
}

void Renderer::BuildSceneGraph() {
	root = new SceneNode(0, NULL, NULL, NULL, Vector4());
	HeightMap* hMap = new HeightMap(TEXTUREDIR"terrainNoise3.png");
	worldCentre = (hMap->GetHeightmapSize() * Vector3(4, 32, 4) * Vector3(0.5, 1, 0.5));
	SceneNode* node = new SceneNode(1, hMap, shader, terrainMats, 3);
	node->SetTransform(Matrix4::Translation(Vector3(0, 0, 0)) * Matrix4::Scale(Vector3(4, 32, 4)));
	node->SetBoundingRadius(1000000.0f);
	root->AddChild(node);

	int* matID = new int[3];
	matID[0] = 0;
	matID[1] = 1;
	matID[2] = 2;
	node = new SceneNode(2, meshes[1], NULL, matID);
	node->SetTransform(Matrix4::Translation(Vector3(16500, 1550, 16000)) * Matrix4::Scale(Vector3(100, 100, 100)));
	node->SetBoundingRadius(10000.0f);
	root->AddChild(node);

	matID = new int[1];
	matID[0] = 3;
	node = new SceneNode(4, meshes[2], NULL, matID);
	node->SetTransform(Matrix4::Translation(Vector3(7000, 500, 9000)) * Matrix4::Scale(Vector3(5, 5, 5)));
	node->SetBoundingRadius(1000000.0f);
	node->SetColour(Vector4(1, 1, 1, 0.3));
	root->AddChild(node);

	matID = new int[1];
	matID[0] = 4;
	node = new SceneNode(2, meshes[3], NULL, matID);
	node->SetTransform(Matrix4::Translation(worldCentre + Vector3(2000, -7200, 0)) * Matrix4::Rotation(37, Vector3(0, 1, 0)) * Matrix4::Scale(Vector3(400, 900, 20)));
	node->SetBoundingRadius(10000.0f);
	node->SetColour(Vector4(1, 1, 1, 0.1));
	root->AddChild(node);
}

void Renderer::GenerateLights() {
	lights = new Light[6];
	lights[0] = Light(Vector3(0, 1, 0), Vector4(1, 1, 1, 4), Vector3(0, 0, 1), sunSpeed);
	toggleDN = true;
	lights[1] = Light((worldCentre + Vector3(-640, -5025, 900)), Vector4(0.55, 0.2, 0.82, 10), 550);
	lights[2] = Light((worldCentre + Vector3(-640, -5425, 900)), Vector4(0.82, 0.55, 0.2, 10), 550);
	lights[3] = Light((worldCentre + Vector3(-640, -5825, 900)), Vector4(0.2, 0.55, 0.82, 10), 550);
	lights[4] = Light((worldCentre + Vector3(-640, -6225, 900)), Vector4(0.2, 0.82, 0.55, 10), 550);
	lights[5] = Light((worldCentre + Vector3(-640, -6625, 900)), Vector4(0.7, 0.70, 0.70, 10), 550);
}

void Renderer::InitialiseShader() {
	BindShader(shader);

	float layerStart[4] = { 0.0, 0.1, 0.2, 0.9 };
	glUniform1fv(glGetUniformLocation(shader->GetProgram(), "layerStart"), 4, layerStart);
	glUniform1f(glGetUniformLocation(shader->GetProgram(), "maxHeight"), (255 * 32));

	Material** allMats = new Material*[2];
	allMats[0] = terrainMats;
	allMats[1] = meshMats;

	int* numMats = new int[2];
	numMats[0] = 4;
	numMats[1] = 5;

	SetShaderMaterial(materialBuffer, 2, 9, numMats, allMats);
}

void Renderer::UpdateScene(float dt) {
	viewMatrix = camera->BuildViewMatrix();
	frustrum.FromMatrix(projMatrix * viewMatrix);
	root->Update(dt);
	if (toggleDN) lights[0].UpdateLight(dt);
}

void Renderer::RenderScene()	{
	BuildNodeLists(root);
	SortNodeLists();
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	DrawSkybox();
	DrawShadowScene();
	DrawMainScene();
	ClearNodeLists();
}

void Renderer::BuildNodeLists(SceneNode* from) {
	if (frustrum.InsideFrustrum(*from)) {
		Vector3 dir = from->GetWorldTransform().GetPositionVector() - camera->GetPosition();
		from->SetCameraDistance(Vector3::Dot(dir, dir));

		if (from->GetColour().w < 1.0f) {
			transparentNodeList.push_back(from);
		}
		else {
			nodeList.push_back(from);
		}
	}

	for (vector<SceneNode*>::const_iterator i = from->GetChildIteratorStart(); i != from->GetChildIteratorEnd(); i++) {
		BuildNodeLists((*i));
	}
}

void Renderer::SortNodeLists() {
	if (transparentNodeList.size() != 0) {
		std::sort(transparentNodeList.rbegin(), transparentNodeList.rend(), SceneNode::CompareByCameraDistance);
	}
	if (nodeList.size() != 0) {
		std::sort(nodeList.begin(), nodeList.end(), SceneNode::CompareByCameraDistance);
	}
}

void Renderer::DrawSkybox() {
	glDepthMask(GL_FALSE);
	UpdateShaderMatrices();
	glUniform1f(glGetUniformLocation(shader->GetProgram(), "fragType"), 0);
	meshes[0]->Draw();
	glDepthMask(GL_TRUE);
}

void Renderer::DrawShadowScene() {
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);

	glClear(GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, SHADOWSIZE, SHADOWSIZE);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

	glCullFace(GL_FRONT);

	glUniform1f(glGetUniformLocation(shader->GetProgram(), "fragType"), 3);

	float lightDistance = 20000;
	Vector3 sunPos = worldCentre + (lights[0].GetDirection() * lightDistance);

	glUniform3fv(glGetUniformLocation(shader->GetProgram(), "sunPosition"), 1, (float*)&sunPos);

	viewMatrix = Matrix4::BuildViewMatrix(sunPos, worldCentre);
	projMatrix = Matrix4::Perspective(3600, 38000, 1, 120);
	shadowMatrix = projMatrix * viewMatrix;

	for (const auto& i : nodeList) {
		UpdateShaderMatrices();
		Matrix4 model = i->GetWorldTransform() * Matrix4::Scale(i->GetModelScale());
		glUniformMatrix4fv(glGetUniformLocation(shader->GetProgram(), "modelMatrix"), 1, false, model.values);
		i->Draw(*this);
	}
	for (const auto& i : transparentNodeList) {
		UpdateShaderMatrices();
		Matrix4 model = i->GetWorldTransform() * Matrix4::Scale(i->GetModelScale());
		glUniformMatrix4fv(glGetUniformLocation(shader->GetProgram(), "modelMatrix"), 1, false, model.values);
		i->Draw(*this);
	}

	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glViewport(0, 0, width, height);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glCullFace(GL_BACK);
}

void Renderer::DrawMainScene() {
	SetShaderLight(lightBuffer, 6, lights);
	
	viewMatrix = camera->BuildViewMatrix();
	projMatrix = Matrix4::Perspective(1.0f, 1000000.0f, (float)width / (float)height, 60.0f);

	DrawNodes();
}

void Renderer::DrawNodes() {
	for (const auto& i : nodeList) {
		DrawNode(i);
	}
	for (const auto& i : transparentNodeList) {
		DrawNode(i);
	}
}

void Renderer::DrawNode(SceneNode* node) {
	if (node->GetMesh() || node->GetHeightMap()) {
		
		UpdateShaderMatrices();
		
		glUniform1f(glGetUniformLocation(shader->GetProgram(), "fragType"), node->GetNodeType());

		Matrix4 model = node->GetWorldTransform() * Matrix4::Scale(node->GetModelScale());
		glUniformMatrix4fv(glGetUniformLocation(shader->GetProgram(), "modelMatrix"), 1, false, model.values);

		glUniform3fv(glGetUniformLocation(shader->GetProgram(), "cameraPos"), 1, (float*)&camera->GetPosition());
		glUniform1ui64ARB(glGetUniformLocation(shader->GetProgram(), "shadowTex"), shadowHndl);

		if (node->GetNodeType() == 1) {
			node->Draw(*this);
		}
		else {
			if (node->GetNodeType() == 4) {
				glUniform1f(glGetUniformLocation(shader->GetProgram(), "time"), w->GetTimer()->GetTotalTimeSeconds());
			}

			if (node->GetMesh()->GetSubMeshCount() > 0) {
				for (int i = 0; i < node->GetMesh()->GetSubMeshCount(); i++) {
					glUniform1f(glGetUniformLocation(shader->GetProgram(), "textureID"), node->GetMatID(i));
					node->GetMesh()->DrawSubMesh(i);
				}
			}
			else {
				glUniform1f(glGetUniformLocation(shader->GetProgram(), "textureID"), node->GetMatID(0));
				node->Draw(*this);
			}
		}
	}

	for (vector<SceneNode*>::const_iterator i = node->GetChildIteratorStart(); i != node->GetChildIteratorEnd(); i++) {
		DrawNode(*i);
	}
}

void Renderer::ClearNodeLists() {
	if (transparentNodeList.size() != 0) {
		transparentNodeList.clear();
	}
	if (nodeList.size() != 0) {
		nodeList.clear();
	}
}

void Renderer::ToggleDayNight() {
	toggleDN = !toggleDN;
}

Renderer::~Renderer(void) {
	ClearNodeLists();

	glDeleteBuffers(6, lightBuffer);
	glDeleteBuffers(9, materialBuffer);

	delete[] terrainMats;
	delete[] meshMats;
	delete shader;
	delete root;
	delete[] meshes;
	delete camera;
	delete[] lights;
}