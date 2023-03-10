#include "CubeRobot.h"

CubeRobot::CubeRobot(Mesh* cube, Shader** shaders, GLuint* textures) {
	SceneNode* body = new SceneNode(cube, shaders[0], Vector4(1, 0, 0, 1));
	body->SetModelScale(Vector3(10, 15, 5));
	body->SetTransform(Matrix4::Translation(Vector3(0, 35, 0)));
	body->SetBoundingRadius(15.0f);
	body->SetTexture(textures[0]);
	AddChild(body);

	head = new SceneNode(cube, shaders[1], Vector4(0, 1, 0, 1));
	head->SetModelScale(Vector3(5, 5, 5));
	head->SetTransform(Matrix4::Translation(Vector3(0, 30, 0)));
	head->SetBoundingRadius(5.0f);
	head->SetTexture(textures[1]);
	body->AddChild(head);

	leftArm = new SceneNode(cube, shaders[2], Vector4(0, 0, 1, 1));
	leftArm->SetModelScale(Vector3(3, -18, 3));
	leftArm->SetTransform(Matrix4::Translation(Vector3(-12, 30, -1)));
	leftArm->SetBoundingRadius(18.0f);
	leftArm->SetTexture(textures[2]);
	body->AddChild(leftArm);

	rightArm = new SceneNode(cube, shaders[2], Vector4(0, 0, 1, 1));
	rightArm->SetModelScale(Vector3(3, -18, 3));
	rightArm->SetTransform(Matrix4::Translation(Vector3(12, 30, -1)));
	rightArm->SetBoundingRadius(18.0f);
	rightArm->SetTexture(textures[2]);
	body->AddChild(rightArm);

	hips = new SceneNode(NULL, shaders[0], Vector4(1, 0, 0, 1));
	body->AddChild(hips);

	SceneNode* leftLeg = new SceneNode(cube, shaders[3], Vector4(0, 0, 1, 1));
	leftLeg->SetModelScale(Vector3(3, -17.5, 3));
	leftLeg->SetTransform(Matrix4::Translation(Vector3(-8, 0, 0)));
	leftLeg->SetBoundingRadius(18.0f);
	leftLeg->SetTexture(textures[3]);
	hips->AddChild(leftLeg);

	SceneNode* rightLeg = new SceneNode(cube, shaders[3], Vector4(0, 0, 1, 1));
	rightLeg->SetModelScale(Vector3(3, -17.5, 3));
	rightLeg->SetTransform(Matrix4::Translation(Vector3(8, 0, 0)));
	rightLeg->SetBoundingRadius(18.0f);
	rightLeg->SetTexture(textures[3]);
	hips->AddChild(rightLeg);
}

void CubeRobot::Update(float dt) {
	transform = transform * Matrix4::Rotation(30.0f * dt, Vector3(0, 1, 0));

	head->SetTransform(head->GetTransform() * Matrix4::Rotation(-30.0f * dt, Vector3(0, 1, 0)));
	leftArm->SetTransform(leftArm->GetTransform() * Matrix4::Rotation(-30.0f * dt, Vector3(1, 0, 0)));
	rightArm->SetTransform(rightArm->GetTransform() * Matrix4::Rotation(30.0f * dt, Vector3(1, 0, 0)));

	SceneNode::Update(dt);
}