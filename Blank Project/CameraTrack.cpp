#include "CameraTrack.h"
#include <nclgl/Vector2.h>

void CameraTrack::Update(float dt, double time) {
	if (time - startTime < 15) {
		if (curScene != 1) StartScene(1);
		camera->Move(Vector3(0, 0, 1), Vector2(), 300 * dt, 0);
	}
	else if (time - startTime < 40) {
		if (curScene != 2) StartScene(2);
		camera->Move(Vector3(0, 1, 0), Vector2(0, 1), 100 * dt, 10 * dt);
	}
	else if (time - startTime < 55) {
		if (curScene != 3) StartScene(3);
		camera->Move(Vector3(-1, 0, 0), Vector2(0, -1), 100 * dt, 1 * dt);
	}
	else if (time - startTime < 65) {
		if (curScene != 4) StartScene(4);
		camera->Move(Vector3(0, 0, 0), Vector2(-1, 0), 0, 0.5 * dt);
	}
	else {
		startTime = time;
	}
}

void CameraTrack::StartScene(int scene) {
	curScene = scene;
	switch (scene) {
	case 1:
		camera->SetPosition(Vector3(2621, 4804, 31717));
		camera->SetYaw(315);
		camera->SetPitch(-11);
		break;
	case 2:
		camera->SetPosition(Vector3(15628, 1502, 17200));
		camera->SetYaw(269);
		camera->SetPitch(2);
		break;
	case 3:
		camera->SetPosition(Vector3(18173, 1260, 14103));
		camera->SetYaw(319);
		camera->SetPitch(-4);
		break;
	case 4:
		camera->SetPosition(Vector3(16523, 2229, 16089));
		camera->SetYaw(290);
		camera->SetPitch(-11);
		break;
	}
}

void CameraTrack::goToStart() {
	StartScene(1);
}