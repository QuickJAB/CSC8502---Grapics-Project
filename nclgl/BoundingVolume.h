#pragma once
class BoundingVolume
{
public:
	BoundingVolume() : width(1.0f), height(1.0f) {};

	float GetBoundingWidth() { return height; }
	float GetBoundingHeight() { return width; }
	void SetBoundingDimensions(float w, float h) {
		width = w;
		height = h;
	}

protected:
	float width, height;
};

