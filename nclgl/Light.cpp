#include "Light.h"

#include "Matrix4.h"

void Light::UpdateLight(float dt) {
	rotValue += rotSpeed * dt;
	if (rotValue < 0) {
		rotValue += 360.0f;
	}
	if (rotValue > 360.0f) {
		rotValue -= 360.0f;
	}
	Vector3 dir = Matrix4::Rotation(rotValue, rotAround) * facingDirection;
	pos_dir = dir;
}