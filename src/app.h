#ifndef APP_H
#define APP_H

#include <input.h>
#include <audio.h>
#include <timer.h>

#ifndef GLFW_INCLUDE_VULKAN
#define GLFW_INCLUDE_VULKAN
#endif
#include <GLFW/glfw3.h>
#ifndef GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#endif
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <thread>
#include <atomic>
#include <fstream>
#include <string>

#include <glmhelper.h>

#include "vulkan-render/render.h"
#include "camera.h"

#include "map.h"
#include "player.h"
#include "poacher.h"
#include "bullet.h"
#include "crab.h"
#include "mapobj.h"
#include "menu.h"
#include "background.h"

#include "cutscene.h"

//#define TIME_APP_DRAW_UPDATE
//#define TIME_GAMELOOP
//#define MULTI_UPDATE_ON_SLOW_DRAW

class App
{
public:
	App();
	~App();
	void run();
	void resize(int windowWidth, int windowHeight);

#pragma region GLFW_CALLBACKS
	static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
	static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
	static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
	static void error_callback(int error, const char* description);
#pragma endregion
	Input input;
private:
	void loadAssets();
	void update();
	void postUpdate();
	void draw();
	void gameUpdate();
	void gameDraw();
	void nextMap();
	void loadMap();
	void pauseToggled();
	void Close();

	glm::vec2 correctedPos(glm::vec2 pos);
	glm::vec2 correctedMouse();

	GLFWwindow* mWindow;
	Render* mRender;
	int mWindowWidth, mWindowHeight;
	Input previousInput;
	Timer timer;
	Audio::Manager audioManager;

	std::thread submitDraw;
	std::atomic<bool> finishedDrawSubmit;

	float time = 0.0f;

	camera::camera2D cam;

	Resource::Texture bgBeach;
	Resource::Texture bgBeachFar;
	Resource::Texture bgCloud;
	std::vector<Background> backgrounds;
	std::vector<glm::vec4> staticColliders;
	std::vector<Poacher> poachers;
	std::vector<Bullet> bullets;
	std::vector<Fruit> fruits;
	std::vector<Crab> crabs;
	Poacher poacher;
	Fruit fruit;
	Crab crab;
	MapGoal mapGoal;

	Resource::Texture lifeTex;
	Resource::Texture noLifeTex;
	std::vector<glm::mat4> lifeTexTransform;
	
	std::vector<Map> maps;
	int currentMapIndex = 0;
	bool finishedAllMaps = false;
	Player player;
	bool playerLastJump = false;
	Map currentMap;
	float zoom = 1.0f;	
	float mapScale = 1.0f;

	Resource::Font* font;
	Resource::Texture buttonTexture;
	Resource::Texture colourPixel;

	bool isPaused = false;
	PauseMenu pauseMenu;

	Opening openingCutscene;
	Extinct extinctCutscene;
	Victory victoryCutscene;
	Cutscene activeCutsene;
	bool inCutscene = false;
	bool playedVictory = false;

	bool didTransition = false;
	float transitionDelay = 3000.0f;
	float transitionTimer = 3001.0f;
	glm::mat4 fadeMat;
};

#endif
