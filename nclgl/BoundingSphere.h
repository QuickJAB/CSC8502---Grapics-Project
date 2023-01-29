#pragma once

#include "BoundingVolume.h"

class BoundingSphere : public BoundingVolume
{
public:
	BoundingSphere() { boundingRadius = 1.0f; }

	float GetBoundingRadius() const { return boundingRadius; }
	void SetBoundingRadius(float f) { boundingRadius = f; }

protected:
	float boundingRadius;
};

