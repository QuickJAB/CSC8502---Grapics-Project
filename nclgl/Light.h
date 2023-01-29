#pragma once

#include "Vector3.h"
#include "Vector4.h"

class Light
{
public:
	Light() {}
	Light(const Vector3& position, const Vector4& colour, float radius) {
		this->pos_dir = position;
		this->colour = colour;

		this->radius = radius;

		this->facingDirection = Vector3();
		this->rotAround = Vector3();
		this->rotSpeed = NULL;
		this->rotValue = NULL;
	}
	Light(const Vector3& direction, const Vector4& colour, const Vector3& axes, const float& speed) {
		this->pos_dir = direction;
		this->colour = colour;

		this->radius = NULL;

		this->facingDirection = direction;
		this->rotAround = axes;
		this->rotSpeed = speed;
		this->rotValue = 0.5f;
	}

	~Light() {}

	void UpdateLight(float dt);

	Vector3 GetDirection() { return pos_dir; }

protected:
	Vector4 colour;
	Vector3 pos_dir;
	float radius;

	Vector3 facingDirection;
	float rotSpeed;
	Vector3 rotAround;
	float rotValue;
};

