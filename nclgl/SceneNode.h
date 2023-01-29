#pragma once

#include "Matrix4.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Mesh.h"
#include "BoundingSphere.h"
#include "Material.h"

#include <vector>

class HeightMap;

class SceneNode
{
public:
	SceneNode(int type, Mesh* m = NULL, Shader* s = NULL, int* matID = NULL, Vector4 colour = Vector4(1, 1, 1, 1));
	SceneNode(int type, HeightMap* m = NULL, Shader* s = NULL, Material* materials = NULL, int numMaterials = 0, int* matID = NULL, Vector4 colour = Vector4(1, 1, 1, 1));
	~SceneNode();

	void AddChild(SceneNode* s);
	void RemoveChild(SceneNode* s);

	inline void SetTransform(const Matrix4& matrix) { transform = matrix; }
	inline const Matrix4& GetTransform() const { return transform; }
	inline Matrix4 GetWorldTransform() const { return worldTransform; }

	inline Vector4 GetColour() const { return colour; }
	inline void SetColour(Vector4 c) { colour = c; }

	inline Vector3 GetModelScale() const { return modelScale; }
	inline void SetModelScale(Vector3 s) { modelScale = s; }

	inline Mesh* GetMesh() const { return mesh; }
	inline void SetMesh(Mesh* m) { mesh = m; }

	inline HeightMap* GetHeightMap() const { return hMap; }

	virtual void Update(float dt);
	virtual void Draw(const OGLRenderer& r);

	inline float GetCamreaDistance() const { return distanceFromCamera; }
	inline void SetCameraDistance(float f) { distanceFromCamera = f; }

	inline Material GetMaterial(int id) const { return materials[id]; }

	inline Shader* GetShader() const { return shader; }
	
	inline BoundingSphere* GetBoundingVolume() { return boundVol; }
	inline void SetBoundingRadius(float radius) {
		boundVol->SetBoundingRadius(radius);
	}

	inline std::vector<SceneNode*>::const_iterator GetChildIteratorStart() {
		return children.begin();
	}

	inline std::vector<SceneNode*>::const_iterator GetChildIteratorEnd() {
		return children.end();
	}

	inline static bool CompareByCameraDistance(SceneNode* a, SceneNode* b) {
		return (a->distanceFromCamera < b->distanceFromCamera) ? true : false;
	}

	inline int GetNodeType() { return nodeType; }

	inline int GetMatID(int index) { return matID[index]; }

protected:
	SceneNode* parent;
	Mesh* mesh;
	HeightMap* hMap;
	Shader* shader;
	BoundingSphere* boundVol;
	Matrix4 worldTransform;
	Matrix4 transform;
	Vector3 modelScale;
	Vector4 colour;
	std::vector<SceneNode*> children;

	float distanceFromCamera;
	Material* materials;
	int numMaterials;
	int* matID;
	int nodeType;
};

