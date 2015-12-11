#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN

#define GLX_GLXEXT_LEGACY //Must be declared so that our local glxext.h is picked up, rather than the system one


#include <windows.h>
#include "windowOGL.h"
#include "GameConstants.h"
#include "cWNDManager.h"
#include "cInputMgr.h"
#include "cSoundMgr.h"
#include "cSprite.h"
#include "cBkGround.h"
#include "cFontMgr.h"
#include "cButton.h"
#include "asteroidsGame.h"


int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR cmdLine,
                   int cmdShow)
{

    //Set the window settings
    const int windowWidth = 1024;
    const int windowHeight = 768;
    const int windowBPP = 16;

	//Amount of removed enemies
	int destroyedEnemies = 0;

    //This is the window
	static cWNDManager* pgmWNDMgr = cWNDManager::getInstance();

	//This is the input manager
	static cInputMgr* theInputMgr = cInputMgr::getInstance();
	
	//This is the Font manager
	static cFontMgr* theFontMgr = cFontMgr::getInstance();

    //The example OpenGL code
    windowOGL theOGLWnd;
	
    //Attach our the OpenGL window
	pgmWNDMgr->attachOGLWnd(&theOGLWnd);

	//Attach the keyboard manager
	pgmWNDMgr->attachInputMgr(theInputMgr);

	//This is the sound manager
	static cSoundMgr* theSoundMgr = cSoundMgr::getInstance();

    //Attempt to create the window
	if (!pgmWNDMgr->createWND(windowWidth, windowHeight, windowBPP))
    {
        //If it fails
        MessageBox(NULL, "Unable to create the OpenGL Window", "An error occurred", MB_ICONERROR | MB_OK);
		pgmWNDMgr->destroyWND(); //Reset the display and exit
        return 1;
    }

	if (!theOGLWnd.initOGL(windowWidth, windowHeight)) //Initialize our example
    {
        MessageBox(NULL, "Could not initialize the application", "An error occurred", MB_ICONERROR | MB_OK);
		pgmWNDMgr->destroyWND(); //Reset the display and exit
        return 1;
    }

	//Clear key buffers
	theInputMgr->clearBuffers(theInputMgr->KEYS_DOWN_BUFFER | theInputMgr->KEYS_PRESSED_BUFFER | theInputMgr->MOUSE_BUFFER);

	//Load Fonts
	LPCSTR gameFonts[2] = { "Fonts/digital-7.ttf", "Fonts/space age.ttf" };

	theFontMgr->addFont("SevenSeg", gameFonts[0], 24);
	theFontMgr->addFont("Space", gameFonts[1], 24);

	//Create vector array of scene textures
	vector<cTexture*> textureBkgList;
	LPCSTR bkgTexturesToUse[] = { "Images/GameSceneBackground.png", "Images/MenuSceneBackground.png", "Images/EndSceneBackground.png" };
	for (int tCount = 0; tCount < 3; tCount++)
	{
		textureBkgList.push_back(new cTexture());
		textureBkgList[tCount]->createTexture(bkgTexturesToUse[tCount]);
	}

	//Transparent sprite
	cTexture transSprite;
	transSprite.createTexture("Images/blank.png");

	//Displays the game scene background image
	cBkGround spriteBkgd;
	spriteBkgd.setSpritePos(glm::vec2(0.0f, 0.0f));
	spriteBkgd.setTexture(textureBkgList[0]->getTexture());
	spriteBkgd.setTextureDimensions(textureBkgList[0]->getTWidth(), textureBkgList[0]->getTHeight());

	//Displays the menu scene background image
	cBkGround spriteStartBkgd;
	spriteStartBkgd.setSpritePos(glm::vec2(0.0f, 0.0f));
	spriteStartBkgd.setTexture(textureBkgList[1]->getTexture());
	spriteStartBkgd.setTextureDimensions(textureBkgList[1]->getTWidth(), textureBkgList[1]->getTHeight());

	//Displays the end scene background image
	cBkGround spriteEndBkgd;
	spriteEndBkgd.setSpritePos(glm::vec2(0.0f, 0.0f));
	spriteEndBkgd.setTexture(textureBkgList[2]->getTexture());
	spriteEndBkgd.setTextureDimensions(textureBkgList[2]->getTWidth(), textureBkgList[2]->getTHeight());

	//Create vector array of button textures
	vector<cTexture*> btnTextureList;
	LPCSTR btnTexturesToUse[] = { "Images/Buttons/StartButton.png", "Images/Buttons/MenuButton.png", "Images/Buttons/ExitButton.png", "Images/Buttons/ReplayButton.png" };
	for (int tCount = 0; tCount < 4; tCount++)
	{
		btnTextureList.push_back(new cTexture());
		btnTextureList[tCount]->createTexture(btnTexturesToUse[tCount]);
	}

	//Displays start button 
	cButton startButton;
	startButton.attachInputMgr(theInputMgr);
	startButton.setTexture(btnTextureList[0]->getTexture());
	startButton.setTextureDimensions(btnTextureList[0]->getTWidth(), btnTextureList[0]->getTHeight());

	//Displays menu button
	cButton menuButton;
	menuButton.attachInputMgr(theInputMgr);
	menuButton.setTexture(btnTextureList[1]->getTexture());
	menuButton.setTextureDimensions(btnTextureList[1]->getTWidth(), btnTextureList[1]->getTHeight());

	//Displays exit button
	cButton exitButton;
	exitButton.attachInputMgr(theInputMgr);
	exitButton.setTexture(btnTextureList[2]->getTexture());
	exitButton.setTextureDimensions(btnTextureList[2]->getTWidth(), btnTextureList[2]->getTHeight());

	//Displays replay button
	cButton replayButton;
	replayButton.attachInputMgr(theInputMgr);
	replayButton.setTexture(btnTextureList[3]->getTexture());
	replayButton.setTextureDimensions(btnTextureList[3]->getTWidth(), btnTextureList[3]->getTHeight());

	string outputMsg;

	//Create vector array of in game textures
	LPCSTR gameTexturesToUse[] = { "Images\\Enemy.png", "Images\\Enemy2.png", "Images\\Enemy.png", "Images\\Enemy2.png", "Images\\bullet.png" };
	for (int tCount = 0; tCount < 5; tCount++)
	{
		theGameTextures.push_back(new cTexture());
		theGameTextures[tCount]->createTexture(gameTexturesToUse[tCount]);
	}

	//Sets enemy amount, position and movement
	for (int astro = 0; astro < 5; astro++)
	{
		theAsteroids.push_back(new cAsteroid);
		theAsteroids[astro]->setSpritePos(glm::vec2(1000.0f, 0 + (rand() % 750)));
		theAsteroids[astro]->setSpriteTranslation(glm::vec2(-100.0f, 0.0f));
		int randAsteroid = rand() % 4;
		theAsteroids[astro]->setTexture(theGameTextures[randAsteroid]->getTexture());
		theAsteroids[astro]->setTextureDimensions(theGameTextures[randAsteroid]->getTWidth(), theGameTextures[randAsteroid]->getTHeight());
		theAsteroids[astro]->setSpriteCentre();
		theAsteroids[astro]->setAsteroidVelocity(glm::vec2(glm::vec2(0.0f, 0.0f)));
		theAsteroids[astro]->setActive(true);
		theAsteroids[astro]->setMdlRadius();
	}

	//Displays rocket image and sets position
	cTexture rocketTxt;
	rocketTxt.createTexture("Images\\rocketSprite2.png");
	cRocket rocketSprite;
	rocketSprite.attachInputMgr(theInputMgr); //Attach the input manager to the sprite
	rocketSprite.setSpritePos(glm::vec2(230.0f, 380.0f));
	rocketSprite.setTexture(rocketTxt.getTexture());
	rocketSprite.setTextureDimensions(rocketTxt.getTWidth(), rocketTxt.getTHeight());
	rocketSprite.setSpriteCentre();
	rocketSprite.setRocketVelocity(glm::vec2(0.0f, 0.0f));

	// Attach sound manager to rocket sprite
	rocketSprite.attachSoundMgr(theSoundMgr);


	// Load game sounds
	// Load Sound
	LPCSTR gameSounds[3] = { "Audio/ParanoidAndroid.wav", "Audio/shot007.wav", "Audio/explosion2.wav" };

	theSoundMgr->add("Theme", gameSounds[0]);
	theSoundMgr->add("Shot", gameSounds[1]);
	theSoundMgr->add("Explosion", gameSounds[2]);

	//Allows menu state to be used and exited with button 
	gameState theGameState = MENU;
	btnTypes theBtnType = EXIT;
	bool respawn = false;
	string strScore = "";

	//This is the mainloop, we render frames until isRunning returns false
	while (pgmWNDMgr->isWNDRunning())
    {
		pgmWNDMgr->processWNDEvents(); //Process any window events

        //We get the time that passed since the last frame
		float elapsedTime = pgmWNDMgr->getElapsedSeconds();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		switch (theGameState)
		{
		//Setup for menu scene state
		case MENU:
			//Displays menu scene background
			spriteStartBkgd.render();

			//Button positioning and rendering
			startButton.setSpritePos(glm::vec2(400.0f, 300.0f));
			exitButton.setSpritePos(glm::vec2(400.0f, 400.0f));
			startButton.render();
			exitButton.render();

			//Provides button function to alter state
			theGameState = startButton.update(theGameState, PLAYING);
			exitButton.update();

			//Provides button function to exit application
			if (exitButton.getClicked())
			{
				SendMessage(pgmWNDMgr->getWNDHandle(), WM_CLOSE, NULL, NULL);
			}

			//Provides button function to start game with empty values
			if (startButton.getClicked())
			{
				respawn = true;
				score = 0;
				destroyedEnemies = 0;
				rocketSprite.setSpritePos(glm::vec2(230.0f, 380.0f));
			}
			break;

		//Setup for game scene state
		case PLAYING:

			//Displays game scene background
			spriteBkgd.render();

			//Button positioning and rendering
			exitButton.setSpritePos(glm::vec2(740.0f, 575.0f));
			exitButton.render();
			theGameState = exitButton.update(theGameState, END);

			//Respawns enemies after game is replayed
			if (respawn)
			{
				if (theAsteroids.size() > 0)
				{
					theAsteroids.clear();
				}

				for (int astro = 0; astro < 5; astro++)
				{
					theAsteroids.push_back(new cAsteroid);
					theAsteroids[astro]->setSpritePos(glm::vec2(1000.0f, 0 + (rand() % 750)));
					theAsteroids[astro]->setSpriteTranslation(glm::vec2(-50.0f, 0.0f));
					int randAsteroid = rand() % 4;
					theAsteroids[astro]->setTexture(theGameTextures[randAsteroid]->getTexture());
					theAsteroids[astro]->setTextureDimensions(theGameTextures[randAsteroid]->getTWidth(), theGameTextures[randAsteroid]->getTHeight());
					theAsteroids[astro]->setSpriteCentre();
					theAsteroids[astro]->setAsteroidVelocity(glm::vec2(glm::vec2(0.0f, 0.0f)));
					theAsteroids[astro]->setActive(true);
					theAsteroids[astro]->setMdlRadius();
				}
				respawn = false;
			}

				{
					//Sets when to include and remove enemies
					vector<cAsteroid*>::iterator asteroidIterator = theAsteroids.begin();
					while (asteroidIterator != theAsteroids.end())
					{
						if ((*asteroidIterator)->isActive() == false)
						{
							asteroidIterator = theAsteroids.erase(asteroidIterator);
							destroyedEnemies++;
							score += 100;
						}
						else
						{
							(*asteroidIterator)->update(elapsedTime);
							(*asteroidIterator)->render();
							++asteroidIterator;
						}
					}
				}

			//Ends game when all enemies are removed
			if (destroyedEnemies == 5)
			{
				theGameState = (theGameState, END);
			}

			//Renders rocket real time
			rocketSprite.render();
			rocketSprite.update(elapsedTime);

			//Displays game title
			outputMsg = {"Descent"};
			theFontMgr->getFont("Space")->printText(outputMsg.c_str(), FTPoint(10, 25, 0.0f));

			strScore = {"Score " + to_string(score)};
			theFontMgr->getFont("Space")->printText(strScore.c_str(), FTPoint(10, 50, 0.0f));

			theInputMgr->clearBuffers(theInputMgr->KEYS_DOWN_BUFFER | theInputMgr->KEYS_PRESSED_BUFFER | theInputMgr->MOUSE_BUFFER);
			break;

		//Setup for end scene state
		case END:
			//Displays end scene background
			spriteEndBkgd.render();

			//Displays score
			strScore = { "Score " + to_string(score) };
			theFontMgr->getFont("Space")->printText(strScore.c_str(), FTPoint(415, 300, 0.0f));

			//Sets button input to false
			replayButton.setClicked(false);
			menuButton.setClicked(false);
			exitButton.setClicked(false);

			//Button positioning and rendering
			replayButton.setSpritePos(glm::vec2(400.0f, 300.0f));
			menuButton.setSpritePos(glm::vec2(400.0f, 400.0f));
			exitButton.setSpritePos(glm::vec2(400.0f, 500.0f));
			replayButton.render();
			menuButton.render();
			exitButton.render();

			//Provides button function to alter state
			theGameState = replayButton.update(theGameState, PLAYING);
			theGameState = menuButton.update(theGameState, MENU);
			exitButton.update();

			//Provides button function to exit application
			if (exitButton.getClicked())
			{
				SendMessage(pgmWNDMgr->getWNDHandle(), WM_CLOSE, NULL, NULL);
			}

			//Resets values when replayed
			if (replayButton.getClicked())
			{
				respawn = true;
				score = 0;
				destroyedEnemies = 0;
				rocketSprite.setSpritePos(glm::vec2(230.0f, 380.0f));
			}
			break;
		}

		pgmWNDMgr->swapBuffers();
    }

	theOGLWnd.shutdown(); //Free any resources
	pgmWNDMgr->destroyWND(); //Destroy the program window

    return 0; //Return success
}
