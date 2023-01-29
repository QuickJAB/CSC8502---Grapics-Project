#pragma once

#include "../nclgl/Camera.h"

class CameraTrack
{
public:
	CameraTrack(Camera* camera) : camera(camera) {}
	~CameraTrack() {}

	void Update(float dt, double time);

	void goToStart();

	void setStartTime(double time) { startTime = time; }

protected:
	Camera* camera;
	double startTime;
	int curScene = 0;

	void StartScene(int scene);
};

