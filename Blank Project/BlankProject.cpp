#include "../NCLGL/window.h"
#include "Renderer.h"
#include <Blank Project/CameraTrack.h>

int main()	{
	Window w("Make your own project!", 1280, 720, false);

	if(!w.HasInitialised()) {
		return -1;
	}
	
	Renderer renderer(w);
	if(!renderer.HasInitialised()) {
		return -1;
	}

	w.LockMouseToWindow(true);
	w.ShowOSPointer(false);

	CameraTrack* track = new CameraTrack(renderer.getCamera());
	track->setStartTime(w.GetTimer()->GetTotalTimeSeconds());
	track->goToStart();
	bool onTrack = true;

	while(w.UpdateWindow()  && !Window::GetKeyboard()->KeyDown(KEYBOARD_ESCAPE)){
		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_1)) {
			onTrack = !onTrack;
			track->setStartTime(w.GetTimer()->GetTotalTimeSeconds());
		}
		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_2)) {
			renderer.ToggleDayNight();
		}
		if (onTrack) {
			track->Update(w.GetTimer()->GetTimeDeltaSeconds(), w.GetTimer()->GetTotalTimeSeconds());
		}
		else {
			renderer.getCamera()->UpdateCamera(w.GetTimer()->GetTimeDeltaSeconds());
		}
		renderer.UpdateScene(w.GetTimer()->GetTimeDeltaSeconds());
		renderer.RenderScene();
		renderer.SwapBuffers();
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_F5)) {
			Shader::ReloadAllShaders();
		}
	}
	return 0;
}