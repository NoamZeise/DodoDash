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
	mWindow = glfwCreateWindow(mWindowWidth, mWindowHeight, "The Last Dodo", nullptr, nullptr);
	if(!mWindow)
	{
		glfwTerminate();
		throw std::runtime_error("failed to create glfw window!");
	}
	GLFWimage winIcon[1];
	winIcon[0].pixels = stbi_load("textures/icon.png", &winIcon[0].width, &winIcon[0].height, 0, 4); //rgba channels
	glfwSetWindowIcon(mWindow, 1, winIcon);
	glfwSetWindowUserPointer(mWindow, this);
	glfwSetFramebufferSizeCallback(mWindow, framebuffer_size_callback);
	glfwSetWindowPosCallback(mWindow, window_size_callback);
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
	currentCutsceneMusic = "audio/cutsceneVO/Intro.ogg";
	audioManager.Play("audio/cutsceneVO/Intro.ogg", false, 1.0f);
	activeCutsene = openingCutscene;
	inCutscene = true;
	lifeTexTransform.resize(player.getHpMax());
	pauseMenu = PauseMenu(colourPixel, buttonTexture, font);
	finishedDrawSubmit = true;
}

App::~App()
{
	StopAudio();
	if(submitDraw.joinable())
		submitDraw.join();
	delete mRender;
	delete font;
	delete openingFont;
	mRender = nullptr;
	glfwDestroyWindow(mWindow);
	glfwTerminate();
}

void App::loadAssets()
{
	font = mRender->LoadFont("textures/ui/menu/PermanentMarker-Regular.ttf");
	openingFont = mRender->LoadFont("textures/ui/Charm-Bold.ttf");

	openingCutscene = Opening(*mRender, openingFont);
	victoryCutscene = Victory(*mRender, font);
	extinctCutscene = Extinct(*mRender, font);

	currentMapIndex = 0;

	std::ifstream savefile("save.data");
	if (savefile.is_open())
	{
		int line = 0;
		std::string currentLine;
		while(std::getline(savefile, currentLine))
		{
			if(line == 0)
				currentMapIndex = std::stoi(currentLine);
			line++;
		}
		savefile.close();
	}

	maps.push_back(Map("maps/level1.tmx", mRender, mapScale, font, 0.03f));
	maps.push_back(Map("maps/level2.tmx", mRender, mapScale, font, 0.04f));
	maps.push_back(Map("maps/level3.tmx", mRender, mapScale, font, 0.07f));
	currentMap = maps[currentMapIndex];
	particleManager = ParticleManager(*mRender);
	player = Player(*mRender, 0.6f, &particleManager, &audioManager);
	poacher = Poacher(*mRender, mapScale, glm::vec4(0, 0, 0, 0));
	fruit = Fruit(*mRender);
	crab = Crab(*mRender, mapScale, glm::vec4(0.0f));
	mapGoal = MapGoal(*mRender);
	lifeTex = mRender->LoadTexture("textures/ui/gameplay/live.png");
	noLifeTex = mRender->LoadTexture("textures/ui/gameplay/livelost.png");
	buttonTexture = mRender->LoadTexture("textures/ui/menu/button.png");
	colourPixel = mRender->LoadTexture("textures/ui/pixel.png");
	bgBeachback = mRender->LoadTexture("textures/bg/beach/BG.png");
	bgBeach = mRender->LoadTexture("textures/bg/beach/Beach.png");
	bgBeachRock = mRender->LoadTexture("textures/bg/beach/Rock.png");
	bgBeachFog = mRender->LoadTexture("textures/bg/beach/Fog.png");
	bgForest = mRender->LoadTexture("textures/bg/jungle/Jungle.png");
	bgForestFog = mRender->LoadTexture("textures/bg/jungle/Fog.png");
	bgForestLeaves = mRender->LoadTexture("textures/bg/jungle/Leaves.png");
	bgMountainback = mRender->LoadTexture("textures/bg/mountain/BG.png");
	bgMountain = mRender->LoadTexture("textures/bg/mountain/Mountain.png");
	bgMountainRock = mRender->LoadTexture("textures/bg/mountain/Rock.png");
	bgMountainFog = mRender->LoadTexture("textures/bg/mountain/Fog.png");

	for( int i = 1; i < 4; i++)
		audioManager.LoadAudioFile("audio/music/lvl" + std::to_string(i) + ".ogg");
	audioManager.LoadAudioFile("audio/sfx/WaveLoop.ogg");
	audioManager.LoadAudioFile("audio/cutsceneVO/Ending.ogg");
	audioManager.LoadAudioFile("audio/music/death.ogg");
	audioManager.LoadAudioFile("audio/music/complete.ogg");
	for( int i = 1; i < 6; i++)
		audioManager.LoadAudioFile("audio/sfx/MonkeyDeath0" + std::to_string(i) + ".wav");
	audioManager.LoadAudioFile("audio/sfx/PoacherShoots2.wav");
	audioManager.LoadAudioFile("audio/sfx/Playerlands1.wav");
	audioManager.LoadAudioFile("audio/sfx/Playerlands2.wav");

	mRender->endResourceLoad();
}

void App::StopAudio()
{
	if(toggleMusicThread.joinable())
		toggleMusicThread.join();
	audioManager.StopAll();
}

void App::loadMap()
{
	backgrounds.clear();
	switch(currentMapIndex)
	{
		case 0:
			particleManager.SetRainParams(30.0f, 8);
			backgrounds.push_back(Background(bgBeachback, 0, 0.7, 0.0f,-9.0f, currentMap.getMapRect()));
			backgrounds.push_back(Background(bgBeach, 0, 1.0, 0.0f, -8.9f, currentMap.getMapRect()));
			backgrounds.push_back(Background(bgBeachFog, 0, 0.8, -0.05f, -8.8f, currentMap.getMapRect()));
			backgrounds.push_back(Background(bgBeachRock, 0, 0.5, 0.0f,-8.7f, currentMap.getMapRect()));
			break;
		case 1:
			particleManager.SetRainParams(20.0f, 15);
			backgrounds.push_back(Background(bgForest, 0, 0.9, 0.0f,-9.0f, currentMap.getMapRect()));
			backgrounds.push_back(Background(bgForestLeaves, 0, 0.7, 0.0f,-8.7f, currentMap.getMapRect()));
			backgrounds.push_back(Background(bgForestFog, 0, 0.75, -0.05f, -8.6f, currentMap.getMapRect()));
			break;
		case 2:
			particleManager.SetRainParams(15.0f, 16);
			backgrounds.push_back(Background(bgMountainback, 0, 0.9, 0.0f,-9.0f, currentMap.getMapRect()));
			backgrounds.push_back(Background(bgMountain, 0, 0.8, 0.0f, -8.9f, currentMap.getMapRect()));
			backgrounds.push_back(Background(bgMountainFog, 0, 0.7, -0.05f, -8.8f, currentMap.getMapRect()));
			backgrounds.push_back(Background(bgMountainRock, 100, 0.5, 0.0f,-8.7f, currentMap.getMapRect()));
			break;

	}
	audioManager.RemovePlayed();
	audioManager.SetVolume(currentBg, mainGameMusicVolume);
	audioManager.Play("audio/sfx/WaveLoop.ogg", true, 0.0f);
	currentMap.Reset();
	player.Reset(currentMap.getPlayerSpawn());
	bullets.clear();
	poachers.clear();
	fruits.clear();
	crabs.clear();
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
	auto crabRects = currentMap.getCrabSpawns();
	for(const auto &rect : crabRects)
	{
		crabs.push_back(crab);
		crabs.back().setCrab(rect);
	}
}

void App::nextMap()
{
	currentMapIndex++;
	if(currentMapIndex >= maps.size())
	{
		currentMapIndex = 0;
		finishedAllMaps = true;
		//didTransition = true;
		//transitionTimer = transitionDelay + 1;
		return;
	}
	currentMap = maps[currentMapIndex];
	StopAudio();
	currentBg = "audio/music/lvl" + std::to_string(currentMapIndex + 1) + ".ogg";
	audioManager.Play(currentBg, true, 0.7f);
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

void App::RenderingStopped()
{
	if(!isPaused && !windowPause)
	{
		windowPause = true;
		pauseToggled();
	}
}

void App::resize(int windowWidth, int windowHeight)
{
	RenderingStopped();
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
	if(!glfwGetWindowAttrib(mWindow, GLFW_FOCUSED))
	{
		RenderingStopped();
	}
	else if(timer.FrameElapsed() < 100)
	{

	if(!isPaused)
	{

		transitionTimer += timer.FrameElapsed();
	}
	if(windowPause)
	{
		windowPause = false;
		pauseToggled();
	}

	if(finishedAllMaps && !inCutscene && !playedVictory)
	{
		playedVictory = true;
		inCutscene = true;
		StopAudio();
		transitionTimer = transitionDelay;
		currentCutsceneMusic = "audio/cutsceneVO/Ending.ogg";
		audioManager.Play(currentCutsceneMusic, false, 1.0f);
		activeCutsene = victoryCutscene;
		activeCutsene.Reset();
	}
	else if(finishedAllMaps && !inCutscene)
	{
		Close();
	}

	if(transitionTimer >= transitionDelay / 2)
	{

	if(inCutscene)
	{
		audioManager.SetVolume(currentCutsceneMusic, activeCutsene.ZPressRatio() * activeCutsene.ZPressRatio());
		if(activeCutsene.isOver())
		{
			if(!didTransition)
			{
				didTransition = true;
				transitionTimer = 0.0f;
			}
			else
			{
			didTransition = false;
			StopAudio();
			inCutscene = false;
			activeCutsene = Cutscene();
			currentMapIndex--;
			nextMap();
			}
		}
	}

	if (input.Keys[GLFW_KEY_F] && !previousInput.Keys[GLFW_KEY_F])
	{
		if (glfwGetWindowMonitor(mWindow) == nullptr)
		{
			const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
			if((float)mode->width / (float)mode->height == (float)settings::TARGET_WIDTH / (float)settings::TARGET_HEIGHT)
				glfwSetWindowMonitor(mWindow, glfwGetPrimaryMonitor(), 0, 0, mode->width, mode->height, mode->refreshRate);
		}
		else
		{
			const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
			glfwSetWindowMonitor(mWindow, NULL, 0, 50, mWindowWidth, mWindowHeight, mode->refreshRate);
		}
	}
	if(input.Keys[GLFW_KEY_ESCAPE] && !previousInput.Keys[GLFW_KEY_ESCAPE])
	{
		isPaused = !isPaused;
		pauseToggled();
	}

	if(!inCutscene)
	{

		if(!isPaused)
			gameUpdate();
	}
	else
	{
		if(!isPaused)
		{
			activeCutsene.Update(timer, input);
			cam.SetCameraOffset(glm::vec2(settings::TARGET_WIDTH/2, settings::TARGET_HEIGHT/2));
		}
	}

	} //skip on long loop

	}//transition

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
	staticColliders = currentMap.getColliders();
	if(!player.isAlive())
	{
		if(!didTransition)
		{

			didTransition = true;
			transitionTimer = 0.0f;
			audioManager.SetVolume(currentBg, 0.3f);
			audioManager.Play("audio/music/death.ogg", false, 0.7f);
		}
		else
		{
			didTransition = false;
			loadMap();
		}
	}

	player.Update(timer, input, staticColliders);

	if(!playerLastJump && player.isJumping())
	{
		currentMap.BlockSwitch();
	}
	playerLastJump = player.isJumping();

	auto playerMid = player.getMidPoint();
	auto playerHitBox = player.getHitBox();
	auto camArea = cam.getCameraArea();

	mapGoal.Update(timer, camArea);
	if(gh::colliding(mapGoal.getHitBox(), playerHitBox))
	{
		timeSinceEgg += timer.FrameElapsed();
		auto musicFade = (3200.0f - timeSinceEgg) / 3200.0f;
		musicFade *= musicFade;
		if(musicFade > mainGameMusicVolume)
			musicFade = mainGameMusicVolume;
		audioManager.SetVolume(currentBg, (3200.0f - timeSinceEgg) / 3200.0f);
		player.EndLevel();
		if(player.EggFinished())
		{
			if(!didTransition)
			{
				didTransition = true;
				transitionTimer = 0.0f;
				StopAudio();
				audioManager.Play("audio/music/complete.ogg", false, 1.0f);
			}
			else
			{
				didTransition = false;
				nextMap();
			}
		}
	}
	else
	{
		if(playerHitBox.y > currentMap.getMapRect().w || playerMid.y > currentMap.getWaterLevel())
		{
			player.kill();
		}
		timeSinceEgg = 0.0f;
	}
	auto distToPlayer = (playerMid.y / currentMap.getWaterLevel());
	distToPlayer = distToPlayer*distToPlayer;
	distToPlayer -= 0.35f;
	distToPlayer = distToPlayer < 0.0f ? 0.0f: distToPlayer;
	audioManager.SetVolume("audio/sfx/WaveLoop.ogg", distToPlayer);

	for(int i = 0; i < fruits.size(); i++)
	{
		fruits[i].Update(timer, camArea);
		if(gh::colliding(fruits[i].getHitBox(), playerHitBox))
		{
			player.addHP(1);
			fruits.erase(fruits.begin() + i--);
		}
	}
	for(int i = 0; i < crabs.size(); i++)
	{
		crabs[i].Update(timer, camArea, playerMid);
		if(gh::colliding(crabs[i].getHitBox(), playerHitBox))
		{
			if(playerHitBox.y + playerHitBox.w/2 < crabs[i].getHitBox().y)
			{
				audioManager.Play("audio/sfx/Playerlands" + std::to_string((int)(rand.PositiveReal()*2) + 1) + ".wav", false, 0.5f);
				audioManager.Play("audio/sfx/MonkeyDeath0" + std::to_string((int)(rand.PositiveReal()*5) + 1) + ".wav", false, 0.45f);
				player.bounce();
				crabs.erase(crabs.begin() + i--);
			}
			else
			{
				player.damage();
			}
		}
	}
	for (int i = 0; i < poachers.size(); i++)
	{
		poachers[i].Update(timer, camArea, playerMid, &bullets);
		if(poachers[i].isAlive())
			if(gh::colliding(playerHitBox, poachers[i].getHitBox()))
			{
				if(playerHitBox.y + playerHitBox.w/2 < poachers[i].getHitBox().y)
				{
					audioManager.Play("audio/sfx/Playerlands" + std::to_string((int)(rand.PositiveReal()*2) + 1) + ".wav", false, 0.5f);
					audioManager.Play("audio/sfx/PoacherDeath.wav", false, 0.35f);
					player.bounce();
					poachers[i].kill();
				}
				else
				{
					player.damage();
				}
			}
	}

	if(prevBullets < bullets.size())
		audioManager.Play("audio/sfx/PoacherShoots2.wav", false, 0.9f);

	for(unsigned int i = 0; i < bullets.size(); i++)
	{
		bullets[i].Update(timer, camArea);
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

	prevBullets = bullets.size();

	cam.Target(player.getMidPoint(), timer);
	particleManager.Update(timer, cam.getCameraArea());
	for(auto&& bg: backgrounds)
		bg.Update(timer, cam.getCameraArea());
	currentMap.Update(cam.getCameraArea(), timer);
}

void App::postUpdate()
{

	if(!inCutscene)
	{

	for(int i = 0; i < lifeTexTransform.size(); i++)
	{
		lifeTexTransform[i] = glmhelper::calcMatFromRect(
			glm::vec4(
				(40 + i * (lifeTex.dim.x/3 + 20)) + (int)cam.getCameraOffset().x,
				(20) + (int)cam.getCameraOffset().y,
				lifeTex.dim.x / 3,
				 lifeTex.dim.y / 3), 0.0f, 2.0f);
	}
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
			Close();
	}

	if(transitionTimer < transitionDelay)
		fadeMat = glmhelper::calcMatFromRect(glm::vec4((int)cam.getCameraArea().x, (int)cam.getCameraArea().y,
				settings::TARGET_WIDTH, settings::TARGET_HEIGHT), 0.0f, 6.0f);

	mRender->set2DViewMatrix(cam.getViewMat());
	time += timer.FrameElapsed();
	timer.Update();
	previousInput = input;
	input.offset = 0;
}

void App::Close()
{
	std::ofstream savefile("save.data");
	if (savefile.is_open())
	{
		savefile.seekp(0);
		savefile << currentMapIndex;
		savefile.close();
	}

	glfwSetWindowShouldClose(mWindow, GLFW_TRUE);
}

void App::pauseToggled()
{
	if(toggleMusicThread.joinable())
		toggleMusicThread.join();
	pauseMenu.Reset();
	if(isPaused || windowPause)
	{
		toggleMusicThread = std::thread(&Audio::Manager::PauseAll, &audioManager);
		glfwSetInputMode(mWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}
	else
	{
		audioManager.ResumeAll();
		glfwSetInputMode(mWindow, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
	}
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

	if(inCutscene)
	{
		activeCutsene.Draw(*mRender);
	}
	else
	{
		gameDraw();
	}

	if(!isPaused)
	if(transitionTimer < transitionDelay)
	{
		if(transitionTimer < transitionDelay/2)
			mRender->DrawQuad(colourPixel, fadeMat, glm::vec4(0.0f, 0.0f, 0.0f, (transitionTimer / transitionDelay) *2));
		else
			mRender->DrawQuad(colourPixel, fadeMat, glm::vec4(0.0f, 0.0f, 0.0f, 2.0f - (transitionTimer / transitionDelay)*2 ));
	}

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

	for(auto&& bg: backgrounds)
		bg.Draw(*mRender);

	currentMap.Draw(*mRender);

	for (auto& poacher: poachers)
		poacher.Draw(*mRender);

	for(auto& crab: crabs)
		crab.Draw(*mRender);
		
	player.Draw(*mRender);

	particleManager.Draw(*mRender);

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

void App::window_size_callback(GLFWwindow* window, int xpos, int ypos)
{
	auto app = reinterpret_cast<App*>(glfwGetWindowUserPointer(window));
	app->RenderingStopped();
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
