#include "Game.hpp"
#include "Map.hpp"
#include "TextureManager.hpp"

SDL_Renderer* Game::renderer = nullptr;
TTF_Font* Game::font = nullptr;
SDL_Color Color = { 144, 164, 174 };

Map* playerMap = nullptr;
Map* enemyMap = nullptr;

Vector2D mousePos;

bool win = false;
Ship ships[5];
int shipPrepCount = 0;
int shipStartEndCounter = 0;
int shipPlacement[4];
int prepErrorCode = 0;

Data data{ 0,0 };
Vector2D inputVectorStart{ -1, -1 };
Vector2D inputVectorEnd{ -1, -1 };


Game::Game() {

}

Game::~Game() {

}

void Game::init(const char* title, int xpos, int ypos, int width, int height, bool fullscreen) {
	int flags = 0;

	if (fullscreen) {
		flags = SDL_WINDOW_FULLSCREEN;
	}

	if (SDL_Init(SDL_INIT_EVERYTHING) == 0) {
		window = SDL_CreateWindow(title, xpos, ypos, width, height, flags);

		renderer = SDL_CreateRenderer(window, -1, 0);

		if (renderer) {
			SDL_SetRenderDrawColor(renderer, 32, 32, 32, 255);
		}

		TTF_Init();
		font = TTF_OpenFont("Ubuntu.ttf", 24);

		isRunning = true;
		gameState = GameState::Begin;
		playerMap = new Map(0, 0);
		enemyMap = new Map(448, 0);
		ships[0] = Ship{ "Anyahaj�", inputVectorStart, inputVectorStart, 5 , false };
		ships[1] = Ship{ "Csatahaj�", inputVectorStart, inputVectorStart, 4 , false };
		ships[2] = Ship{ "Cirk�l�", inputVectorStart, inputVectorStart, 3 , false };
		ships[3] = Ship{ "Tengeralattj�r�", inputVectorStart, inputVectorStart, 3 , false };
		ships[4] = Ship{ "Rombol�", inputVectorStart, inputVectorStart, 2 , false };
		
	}else {
		isRunning = false;
	}
}

void Game::handleEvents() {
	SDL_Event event;
	SDL_PollEvent(&event);

	SDL_GetMouseState(&mousePos.x, &mousePos.y);

	switch (gameState) {
		case GameState::Begin:
			if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
				gameState = GameState::Prep;
			}
			break;
		case GameState::Prep:
			
			if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
				//ha start {-1,-1}
				if (inputVectorStart.x == -1 && inputVectorStart.y == -1) {
					// olvassuk be az eger pozt
					inputVectorStart.x = mousePos.x;
					inputVectorStart.y = mousePos.y;
					std::cout << inputVectorStart.x << "  " << inputVectorStart.y << std::endl;
					// convert to coord
					playerMap->ScreenToMapCoord(inputVectorStart, inputVectorStart);

					std::cout <<"start: "<< inputVectorStart.x << "  " << inputVectorStart.y << std::endl;
					shipStartEndCounter++;
				}
				//ha start m�r nem {-1, -1}, ha end {-1, -1}
				else if (inputVectorEnd.x == -1 && inputVectorEnd.y == -1) {
					// olvassuk be az eger pozt
					inputVectorEnd.x = mousePos.x;
					inputVectorEnd.y = mousePos.y;
					std::cout << inputVectorEnd.x << "  " << inputVectorEnd.y << std::endl;
					// convert to coord
					playerMap->ScreenToMapCoord(inputVectorEnd, inputVectorEnd);

					std::cout << "end: " << inputVectorEnd.x << "  " << inputVectorEnd.y << std::endl;
					// validak egymashoz kepest ?
					
					prepErrorCode = playerMap->ValidOrientation(inputVectorStart, inputVectorEnd);
					if(prepErrorCode == 0)
						prepErrorCode = playerMap->ValidLength(ships[shipPrepCount].length, inputVectorStart, inputVectorEnd);
					if (prepErrorCode == 0)
						prepErrorCode = playerMap->ValidPlacement(inputVectorStart, inputVectorEnd);
						
					std::cout << "megvan mindketto" << std::endl;

					if (prepErrorCode == 0) {
						shipStartEndCounter++;
						shipPrepCount++;
						std::cout << "shipprepcount: " << shipPrepCount << std::endl;
						inputVectorStart.x = inputVectorStart.y = inputVectorEnd.x = inputVectorEnd.y = -1;
					}
					else {
						shipStartEndCounter = 0;
						inputVectorStart.x = inputVectorStart.y = inputVectorEnd.x = inputVectorEnd.y = -1;
					}
					
				}
				
				//ha setfield ok mehet tov�bb

				if (shipPrepCount == 5 && prepErrorCode == 0) {
					gameState = GameState::Play;
				}
			}
			break;
		case GameState::Play:
			if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
				gameState = GameState::End;
			}
			break;
		case GameState::End:
			if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
				isRunning = false;
			}
			break;
		default:

			break;
	}


	if(event.type == SDL_QUIT)
	{
		isRunning = false;
	}
}

void Game::update() {

}

void Game::render() {
	SDL_RenderClear(renderer);
	
	switch (gameState) {
	case GameState::Begin:
		TextureManager::DrawText(342, 20, "Torpedo", Color);
		TextureManager::DrawText(240, 70, "Kattints a folytat�shoz!", Color);
		break;
	case GameState::Prep:
		TextureManager::DrawText(420, 20, "Helyezd el haj�idat!", Color);
		if (!ships[shipPrepCount].placed && shipPrepCount <= 4) {
			TextureManager::DrawText(480, 70, ships[shipPrepCount].name, Color);
			if (shipStartEndCounter % 2 == 0) {
				TextureManager::DrawText(400, 110, "Add meg a kezd�poz�ci�t!", Color);
			}
			else {
				TextureManager::DrawText(400, 110, "Add meg a v�gpoz�ci�t!", Color);
			}

			if (prepErrorCode == 1) {
				TextureManager::DrawText(400, 150, "Az elhelyez�s hib�s!", Color);
			}
			else if (prepErrorCode == 2) {
				TextureManager::DrawText(400, 150, "A terulet nem szabad", Color);
			}
			else if (prepErrorCode == 4) {
				TextureManager::DrawText(400, 150, "A meret kicsi!", Color);
			}
			else if (prepErrorCode == 5) {
				TextureManager::DrawText(400, 150, "A meret nagy!", Color);
			}
		}
		if (shipPrepCount > 4) {
			TextureManager::DrawText(400, 70, "Kattints a folytat�shoz!", Color);
		}
		
		
		
		playerMap->DrawMap(mousePos.x, mousePos.y);
		
		break;
	case GameState::Play:
		SDL_GetMouseState(&mousePos.x, &mousePos.y);
		playerMap->DrawMap(mousePos.x, mousePos.y);
		enemyMap->DrawMap(mousePos.x, mousePos.y);
		break;
	case GameState::End:
		if (win) {
			TextureManager::DrawText(318, 20, "Gratul�lok!", Color);
		}else{
			TextureManager::DrawText(270, 20, "Majd legk�zelebb...", Color);
		}
		break;
	default:
		break;
	}
	SDL_RenderPresent(renderer);
}

void Game::clean() {
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	SDL_Quit();
}

bool Game::running() {
	return isRunning;
}


