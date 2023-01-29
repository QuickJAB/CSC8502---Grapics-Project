#include "SceneNode.h"
#include "HeightMap.h"
#include "Material.h"

SceneNode::SceneNode(int type, Mesh* mesh, Shader* shader, int* matID, Vector4 colour) {
	nodeType = type;
	this->mesh = mesh;
	this->hMap = NULL;
	this->shader = shader;
	this->colour = colour;
	this->matID = matID;
	boundVol = new BoundingSphere();
	distanceFromCamera = 0.0f;
	materials = NULL;
	parent = NULL;
	modelScale = Vector3(1, 1, 1);
}

SceneNode::SceneNode(int type, HeightMap* hMap, Shader* shader, Material* materials, int numMaterials, int* matID, Vector4 colour) {
	nodeType = type;
	
	this->hMap = hMap;
	this->mesh = NULL;
	this->shader = shader;
	this->colour = colour;
	boundVol = new BoundingSphere();
	distanceFromCamera = 0.0f;
	this->materials = materials;
	this->numMaterials = numMaterials;
	this->matID = matID;
	parent = NULL;
	modelScale = Vector3(1, 1, 1);
}

SceneNode::~SceneNode(void) {
	for (unsigned int i = 0; i < children.size(); i++) {
		delete children[i];
	}
}

void SceneNode::AddChild(SceneNode* s) {
	children.push_back(s);
	s->parent = this;
}

void SceneNode::RemoveChild(SceneNode* s) {
	auto location = std::find(children.begin(), children.end(), s);
	if (location != children.end()) {
		int index = location - children.begin();
		children.erase(std::next(children.begin(), index));
		delete s;
	}
}

void SceneNode::Draw(const OGLRenderer& r) {
	if (mesh) {
		mesh->Draw();
	}
	if (hMap) {
		hMap->Draw();
	}
}

void SceneNode::Update(float dt) {
	if (parent) {
		worldTransform = parent->worldTransform * transform;
	}
	else {
		worldTransform = transform;
	}
	for (vector<SceneNode*>::iterator i = children.begin(); i != children.end(); i++) {
		(*i)->Update(dt);
	}
}