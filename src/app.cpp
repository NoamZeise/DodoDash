#include "app.h"

App::App()
{
	//set member variables
	mWindowWidth = 1600;
	mWindowHeight = 900;
	//init glfw window
	glfwSetErrorCallback(error_callback);
	if (!glfwInit())
			throw std::runtime_error("failed to initialise glfw!");
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); //using vulkan not openGL
	mWindow = glfwCreateWindow(mWindowWidth, mWindowHeight, "Dodo Dash", nullptr, nullptr);
	if(!mWindow)
	{
		glfwTerminate();
		throw std::runtime_error("failed to create glfw window!");
	}
	glfwSetWindowUserPointer(mWindow, this);
	glfwSetFramebufferSizeCallback(mWindow, framebuffer_size_callback);
	glfwSetCursorPosCallback(mWindow, mouse_callback);
	glfwSetScrollCallback(mWindow, scroll_callback);
	glfwSetKeyCallback(mWindow, key_callback);
	glfwSetMouseButtonCallback(mWindow, mouse_button_callback);
    glfwSetInputMode(mWindow, GLFW_RAW_MOUSE_MOTION, glfwRawMouseMotionSupported());
	glfwSetInputMode(mWindow, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

	int width = mWindowWidth;
	int height = mWindowHeight;
	if(settings::USE_TARGET_RESOLUTION)
	{
		width = settings::TARGET_WIDTH;
		height = settings::TARGET_HEIGHT;
	}
	if(settings::FIXED_RATIO)
		glfwSetWindowAspectRatio(mWindow, width, height);
	mRender = new Render(mWindow, glm::vec2(width, height));

	loadAssets();
	lifeTexTransform.resize(player.getHpMax());
	//audioManager.Play("audio/Dodo Hornpipe Demo.ogg", true, 0.5f);
	pauseMenu = PauseMenu(colourPixel, buttonTexture, font);
	finishedDrawSubmit = true;
}

App::~App()
{
	if(submitDraw.joinable())
		submitDraw.join();
	delete mRender;
	delete font;
	mRender = nullptr;
	glfwDestroyWindow(mWindow);
	glfwTerminate();
}

void App::loadAssets()
{
	font = mRender->LoadFont("textures/ui/menu/PermanentMarker-Regular.ttf");
	currentMapIndex = 1;
	maps.push_back(Map("maps/level1-jumptut.tmx", mRender, mapScale, font, 0.015f));
	maps.push_back(Map("maps/level2-poachertut.tmx", mRender, mapScale, font, 0.02f));
	currentMap = maps[currentMapIndex];
	player = Player(*mRender, 0.6f);
	poacher = Poacher(*mRender, mapScale, glm::vec4(0, 0, 0, 0));
	fruit = Fruit(*mRender);
	mapGoal = MapGoal(*mRender);
	lifeTex = mRender->LoadTexture("textures/ui/gameplay/feather.png");
	noLifeTex = mRender->LoadTexture("textures/ui/gameplay/darkfeather.png");
	buttonTexture = mRender->LoadTexture("textures/ui/menu/button.png");
	colourPixel = mRender->LoadTexture("textures/ui/pixel.png");
	loadMap();
	mRender->endResourceLoad();
}

void App::loadMap()
{
	currentMap.Reset();
	player.Reset(currentMap.getPlayerSpawn());
	bullets.clear();
	poachers.clear();
	fruits.clear();
	staticColliders.clear();
	staticColliders = currentMap.getColliders();
	cam.setCameraMapRect(currentMap.getMapRect());
	cam.SetCameraOffset(player.getMidPoint());
	mapGoal.setDrawRect(currentMap.getGoal(), mapScale);
	auto poacherRects = currentMap.getPoacherRects();
	for(const auto &rect : poacherRects)
	{
		poachers.push_back(poacher);
		poachers.back().setPoacher(rect);
	}
	auto fruitRects = currentMap.getFruitRect();
	for(const auto &rect : fruitRects)
	{
		fruits.push_back(fruit);
		fruits.back().setDrawRect(rect, mapScale);
	}
}

void App::nextMap()
{
	currentMapIndex++;
	if(currentMapIndex >= maps.size())
	{
		currentMapIndex = maps.size() - 1; 
		finishedAllMaps = true;
	}
	currentMap = maps[currentMapIndex];
	loadMap();
}

void App::run()
{
	while (!glfwWindowShouldClose(mWindow))
	{
#ifdef TIME_GAMELOOP
		auto start = std::chrono::high_resolution_clock::now();
#endif
		update();
		if(mWindowWidth != 0 && mWindowHeight != 0)
			draw();

#ifdef TIME_GAMELOOP
		auto stop = std::chrono::high_resolution_clock::now();
		std::cout
		<< "gameloop: "
		<< std::chrono::duration_cast<std::chrono::microseconds>(stop - start).count()
		<< " microseconds" << std::endl;
#endif
	}
}

void App::resize(int windowWidth, int windowHeight)
{
	if(submitDraw.joinable())
		submitDraw.join();
	mWindowWidth = windowWidth;
	mWindowHeight = windowHeight;
	if(mRender != nullptr && mWindowWidth != 0 && mWindowHeight != 0)
		mRender->framebufferResize();
}

void App::update()
{
#ifdef TIME_APP_DRAW_UPDATE
	auto start = std::chrono::high_resolution_clock::now();
#endif
	glfwPollEvents();

	if (input.Keys[GLFW_KEY_F] && !previousInput.Keys[GLFW_KEY_F])
	{
		if (glfwGetWindowMonitor(mWindow) == nullptr)
		{
			const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
			glfwSetWindowMonitor(mWindow, glfwGetPrimaryMonitor(), 0, 0, mode->width, mode->height, mode->refreshRate);
		}
		else
		{
			const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
			glfwSetWindowMonitor(mWindow, NULL, 0, 0, mWindowWidth, mWindowHeight, mode->refreshRate);
		}
	}
	if(input.Keys[GLFW_KEY_ESCAPE] && !previousInput.Keys[GLFW_KEY_ESCAPE])
	{
		isPaused = !isPaused;
		pauseToggled();
	}

	if(!isPaused)
		gameUpdate();

	postUpdate();
#ifdef TIME_APP_DRAW_UPDATE
	auto stop = std::chrono::high_resolution_clock::now();
	std::cout
		 << "update: "
         << std::chrono::duration_cast<std::chrono::microseconds>(stop - start).count()
		 << " microseconds" << std::endl;
#endif
}

void App::gameUpdate()
{
	if(!player.isAlive())
	{
		loadMap();
	}

	player.Update(timer, input, staticColliders);
	auto playerMid = player.getMidPoint();
	auto playerHitBox = player.getHitBox();
	mapGoal.Update(timer, cam.getCameraArea());
	if(gh::colliding(mapGoal.getHitBox(), playerHitBox))
	{
		nextMap();
	}

	for(int i = 0; i < fruits.size(); i++)
	{
		fruits[i].Update(timer, cam.getCameraArea());
		if(gh::colliding(fruits[i].getHitBox(), playerHitBox))
		{
			player.addHP(1);
			fruits.erase(fruits.begin() + i--);
		}
	}
	if(playerHitBox.y > currentMap.getMapRect().w || playerMid.y > currentMap.getWaterLevel())
	{
		loadMap();
	}
	for (int i = 0; i < poachers.size(); i++)
	{
		poachers[i].Update(timer, cam.getCameraArea(), playerMid, &bullets);
		if(poachers[i].isAlive())
			if(gh::colliding(playerHitBox, poachers[i].getHitBox()))
			{
				if(playerHitBox.y < poachers[i].getHitBox().y)
				{
					player.bounce();
					poachers[i].kill();
				}
				else
				{
					player.damage();
				}
			}
	}

	for(unsigned int i = 0; i < bullets.size(); i++)
	{
		bullets[i].Update(timer, cam.getCameraArea());
		if(gh::colliding(playerHitBox, bullets[i].getHitBox()))
		{
			player.damage();
			bullets.erase(bullets.begin() + i--);
			continue;
		}
		for(const auto& rect: staticColliders)
			if(gh::colliding(rect, bullets[i].getHitBox()))
			{
				bullets.erase(bullets.begin() + i--);
				break;
			}
	}

	cam.Target(player.getMidPoint(), timer);
	currentMap.Update(cam.getCameraArea(), timer);
}

void App::postUpdate()
{
	time += timer.FrameElapsed();
	timer.Update();

	for(int i = 0; i < lifeTexTransform.size(); i++)
	{
		lifeTexTransform[i] = glmhelper::calcMatFromRect(
			glm::vec4(
				(10 + i * lifeTex.dim.x) + (int)cam.getCameraOffset().x,
				(10) + (int)cam.getCameraOffset().y,
				lifeTex.dim.x,
				 lifeTex.dim.y), 0.0f, 2.0f);
	}
	if(isPaused)
	{
		pauseMenu.Update(cam.getCameraArea(), correctedMouse(), input, previousInput, timer);
		if(pauseMenu.isResumed())
		{
			player.setJumpPressed();
			isPaused = false;
			pauseToggled();
		}
		if(pauseMenu.isExit())
			glfwSetWindowShouldClose(mWindow, GLFW_TRUE);
	}
	mRender->set2DViewMatrix(cam.getViewMat());
	previousInput = input;
	input.offset = 0;
}

void App::pauseToggled()
{
	pauseMenu.Reset();
	if(isPaused)
		glfwSetInputMode(mWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	else
		glfwSetInputMode(mWindow, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
}


void App::draw()
{
#ifdef TIME_APP_DRAW_UPDATE
	auto start = std::chrono::high_resolution_clock::now();
#endif

#ifdef MULTI_UPDATE_ON_SLOW_DRAW
	if(!finishedDrawSubmit)
		return;
	finishedDrawSubmit = false;
#endif
	if(submitDraw.joinable())
		submitDraw.join();

	mRender->begin2DDraw();

	gameDraw();

	if(isPaused)
		pauseMenu.Draw(*mRender);

	submitDraw = std::thread(&Render::endDraw, mRender, std::ref(finishedDrawSubmit));

#ifdef TIME_APP_DRAW_UPDATE
	auto stop = std::chrono::high_resolution_clock::now();
	std::cout
	<< "draw: "
	<< std::chrono::duration_cast<std::chrono::microseconds>(stop - start).count()
	<< " microseconds" << std::endl;
#endif
}

void App::gameDraw()
{
	currentMap.Draw(*mRender);

	for (auto& poacher: poachers)
		poacher.Draw(*mRender);

	player.Draw(*mRender);

	for(unsigned int i = 0; i < bullets.size(); i++)
	{
		bullets[i].Draw(*mRender);
	}

	for(int i = 0; i < fruits.size(); i++)
	{
		fruits[i].Draw(*mRender);
	}

	mapGoal.Draw(*mRender);

	for(int i = 0; i < lifeTexTransform.size(); i++)
	{
		if(i < player.getHp())
			mRender->DrawQuad(lifeTex, lifeTexTransform[i]);
		else
			mRender->DrawQuad(noLifeTex, lifeTexTransform[i]);
	}
}

glm::vec2 App::correctedPos(glm::vec2 pos)
{
	if(settings::USE_TARGET_RESOLUTION)
		return glm::vec2(pos.x * ((float)settings::TARGET_WIDTH / (float)mWindowWidth), pos.y * ((float)settings::TARGET_HEIGHT / (float)mWindowHeight));
	return glm::vec2(pos.x, pos.y);
}

glm::vec2 App::correctedMouse()
{
	return correctedPos(glm::vec2(input.X, input.Y));
}

#pragma region GLFW_CALLBACKS


void App::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	auto app = reinterpret_cast<App*>(glfwGetWindowUserPointer(window));
	app->resize(width, height);
}

void App::mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	App* app = reinterpret_cast<App*>(glfwGetWindowUserPointer(window));
	app->input.X = xpos;
	app->input.Y = ypos;
}
void App::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	App* app = reinterpret_cast<App*>(glfwGetWindowUserPointer(window));
	app->input.offset = yoffset;
}

void App::key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	App* app = reinterpret_cast<App*>(glfwGetWindowUserPointer(window));
	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
		{
			app->input.Keys[key] = true;
		}
		else if (action == GLFW_RELEASE)
		{
			app->input.Keys[key] = false;
		}
	}
}

void App::mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	App* app = reinterpret_cast<App*>(glfwGetWindowUserPointer(window));

	if (button >= 0 && button < 8)
	{
		if (action == GLFW_PRESS)
		{
			app->input.Buttons[button] = true;
		}
		else if (action == GLFW_RELEASE)
		{
			app->input.Buttons[button] = false;
		}
	}
}

void App::error_callback(int error, const char* description)
{
    throw std::runtime_error(description);
}

#pragma endregion
