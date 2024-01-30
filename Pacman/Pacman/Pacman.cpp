// Name:  Ayaan Qazi
// Pacman Project

#include <raylib.h>
#include <iostream>
#include <string>
#include <ctime>

std::string difficultyLevelLabel = "Easy"; // Showing default difficulty level

bool showMenu = true;
bool playerWon = false;
bool gameOver = false;
bool teleporterCooldownActive = false;

time_t startTime;
time_t lastTeleportTime;
const float teleporterCooldownDuration = 2.0f; // Setting a telporter cooldown inorder to avoid bugs
int score = 0;

Sound startSound;
Sound gameplaySound;
Sound gameoverSound;
Sound gamewonSound;

const int TILE_SIZE = 30;
const int MAP_WIDTH = 21;
const int MAP_HEIGHT = 15;

// The array represents the map, 0 is empty spot, 1 is for walls, 2 for coins and 3 for teleporters.
int map[MAP_HEIGHT][MAP_WIDTH] =
{
   {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
   {1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1},
   {1, 2, 1, 1, 1, 2, 1, 2, 1, 2, 2, 2, 1, 2, 1, 2, 1, 1, 1, 2, 1},
   {1, 2, 1, 2, 2, 2, 1, 2, 1, 1, 1, 1, 1, 2, 1, 2, 2, 2, 1, 2, 1},
   {1, 2, 1, 2, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 2, 1, 2, 1},
   {1, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 1},
   {1, 1, 1, 1, 2, 2, 1, 2, 2, 0, 0, 0, 2, 2, 1, 2, 2, 1, 1, 1, 1},
   {3, 2, 2, 2, 2, 2, 1, 2, 2, 0, 0, 0, 2, 2, 1, 2, 2, 2, 2, 2, 3},
   {1, 1, 1, 1, 2, 2, 1, 2, 2, 0, 0, 0, 2, 2, 1, 2, 2, 1, 1, 1, 1},
   {1, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 1},
   {1, 2, 1, 2, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 2, 1, 2, 1},
   {1, 2, 1, 2, 2, 2, 1, 2, 1, 1, 1, 1, 1, 2, 1, 2, 2, 2, 1, 2, 1},
   {1, 2, 1, 1, 1, 2, 1, 2, 1, 2, 2, 2, 1, 2, 1, 2, 1, 1, 1, 2, 1},
   {1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1},
   {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
};

// Declaring initial map (we will save this for replay button later)
int initialMap[MAP_HEIGHT][MAP_WIDTH];

Rectangle pacmanRect;
Vector2 pacmanPos;
Vector2 pacmanVelocity;

Texture2D pacmanUp;
Texture2D pacmanDown;
Texture2D pacmanRight;
Texture2D pacmanLeft;
float pacmanSpeed = 2.0f;

Vector2 ghostPos;
Vector2 ghostVelocity;
Texture2D ghostTextureRight;
Texture2D ghostTextureLeft;
Texture2D currentGhostTexture;

Vector2 ghost2Pos;
Vector2 ghost2Velocity;
Texture2D ghost2TextureRight;
Texture2D ghost2TextureLeft;
Texture2D currentGhost2Texture;
float ghostSpeed = 1.5f;

void Initialize() {
	InitWindow(MAP_WIDTH * TILE_SIZE, MAP_HEIGHT * TILE_SIZE, "Pacman");
	SetTargetFPS(60);

	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			initialMap[i][j] = map[i][j]; // Saving initial map for retry button
		}
	}
	// Loading multiple textures for when pacman is facing each direction
	pacmanUp = LoadTexture("assets//pac2up.png");
	pacmanDown = LoadTexture("assets//pac2down.png");
	pacmanRight = LoadTexture("assets//pac2right.png");
	pacmanLeft = LoadTexture("assets//pac2left.png");

	// Setting the starting point of pacman to bottom-centre
	pacmanPos = { static_cast<float>((GetScreenWidth() - TILE_SIZE) / 2 + 5),
		static_cast<float>((GetScreenHeight() - TILE_SIZE) - TILE_SIZE / 2 - TILE_SIZE / 2) };
	pacmanRect = { pacmanPos.x, pacmanPos.y, TILE_SIZE - 8.0, TILE_SIZE - 2.5 };

	// Loading first ghost's right face and left face texture
	ghostTextureRight = LoadTexture("assets//ghostr1.png");
	ghostTextureLeft = LoadTexture("assets//ghostl1.png");
	currentGhostTexture = ghostTextureRight;

	// Loading second ghost's right face and left face texture
	ghost2TextureRight = LoadTexture("assets//ghostr2.png");
	ghost2TextureLeft = LoadTexture("assets//ghostl2.png");
	currentGhost2Texture = ghost2TextureRight;

	// Setting the starting point of the first ghost to one tile left to the center of the map
	ghostPos = { static_cast<float>(((GetScreenWidth() - TILE_SIZE) / 2) - TILE_SIZE), static_cast<float>((GetScreenHeight() - TILE_SIZE) / 2) };
	ghostVelocity = { ghostSpeed, 0 };

	// Setting the starting point of the second ghost to one tile right to the centre of the map
	ghost2Pos = { static_cast<float>(((GetScreenWidth() - TILE_SIZE) / 2) + TILE_SIZE), static_cast<float>((GetScreenHeight() - TILE_SIZE) / 2) };
	ghost2Velocity = { ghostSpeed, 0 };

	// Randomizing the initial direction of the ghost
	int randomDirection = GetRandomValue(0, 3); // 0: up, 1: down, 2: left, 3: right

	// Note that UPWARD is Negative Y (This is the case in most graphic libraries)
	if (randomDirection == 0) {
		ghostVelocity = { 0, -ghostSpeed }; // Applying 0 speed in the x-Axis, and ghostSpeed in the Negative y-Axis
	}
	else if (randomDirection == 1) {
		ghostVelocity = { 0, ghostSpeed }; // Applying 0 speed in the x-Axis, and ghostSpeed in the Positive y-Axis
	}
	else if (randomDirection == 2) {
		ghostVelocity = { -ghostSpeed, 0 }; // Applying ghostSpeed in the Negative x-Axis, and 0 speed in y-Axis
	}
	else {
		ghostVelocity = { ghostSpeed, 0 }; // Applying ghostSpeed in the Positive x-Axis, and 0 speed in y-Axis
	}
}

void InitializeAudio() {
	InitAudioDevice();

	// Loading audio files
	startSound = LoadSound("assets//pacman_start.mp3");
	gameplaySound = LoadSound("assets//pacman_gameplay.mp3");
	gameoverSound = LoadSound("assets//pacman_over.mp3");
	gamewonSound = LoadSound("assets//pacman_win.mp3");

	// Set volume for audio
	SetSoundVolume(gameplaySound, 0.5f); // Adjust volume as needed
}

void UnloadAudio() {
	UnloadSound(startSound);
	UnloadSound(gameplaySound);
	UnloadSound(gameoverSound);
	UnloadSound(gamewonSound);

	CloseAudioDevice();
}

void PlayStartAudio() {
	PlaySound(startSound);
}

void PlayGameplayAudio() {
	PlaySound(gameplaySound);
}

void PlayGameoverAudio() {
	PlaySound(gameoverSound);
}

void PlayGamewonAudio() {
	PlaySound(gamewonSound);
}

void DifficultyLevel() {
	DrawText("1  - Easy", 10, GetScreenHeight() - 80, 20, WHITE);
	DrawText("2 - Hard", 10, GetScreenHeight() - 60, 20, WHITE);
	DrawText("3 - Impossible", 10, GetScreenHeight() - 40, 20, WHITE);

	// Concatenating two strings with the use of + operator
	DrawText(("Difficulty Level: " + difficultyLevelLabel).c_str(),
		GetScreenWidth() / 2 - MeasureText(("Difficulty Level: " + difficultyLevelLabel).c_str(), 20) / 2,
		GetScreenHeight() - 40, 20, WHITE);

	// Handling input for difficulty level selection
	if (IsKeyPressed(KEY_ONE)) {
		difficultyLevelLabel = "Easy";
		ghostSpeed = 1.5f;
	}
	else if (IsKeyPressed(KEY_TWO)) {
		difficultyLevelLabel = "Hard";
		ghostSpeed = 2.0f;
	}
	else if (IsKeyPressed(KEY_THREE)) {
		difficultyLevelLabel = "Impossible";
		ghostSpeed = 2.5f;
	}
}

void HandleInput() {
	// Handles pacman movements according to key presses
	if (IsKeyPressed(KEY_RIGHT)) {
		pacmanVelocity = { pacmanSpeed, 0 };
	}
	else if (IsKeyPressed(KEY_LEFT)) {
		pacmanVelocity = { -pacmanSpeed, 0 };
	}
	else if (IsKeyPressed(KEY_DOWN)) {
		pacmanVelocity = { 0, pacmanSpeed };
	}
	else if (IsKeyPressed(KEY_UP)) {
		pacmanVelocity = { 0, -pacmanSpeed };
	}
}

void UpdatePacman() {
	// Calculates the next position
	float nextX = pacmanPos.x + pacmanVelocity.x;
	float nextY = pacmanPos.y + pacmanVelocity.y;

	// Calculates the center of Pac-Man
	float centerX = nextX + pacmanRect.width / 2;
	float centerY = nextY + pacmanRect.height / 2;

	// Checks for collisions with walls
	int leftTile = static_cast<int>((centerX - pacmanRect.width / 2) / TILE_SIZE);
	int rightTile = static_cast<int>((centerX + pacmanRect.width / 2 - 1) / TILE_SIZE);
	int topTile = static_cast<int>((centerY - pacmanRect.height / 2) / TILE_SIZE);
	int bottomTile = static_cast<int>((centerY + pacmanRect.height / 2 - 1) / TILE_SIZE);

	// Checking horizontal collisions (walls to the right and left of pacman)
	if (pacmanVelocity.x > 0 && rightTile < MAP_WIDTH) {
		if (map[topTile][rightTile] != 1 && map[bottomTile][rightTile] != 1) {
			pacmanPos.x = nextX;
		}
	}
	else if (pacmanVelocity.x < 0 && leftTile >= 0) {
		if (map[topTile][leftTile] != 1 && map[bottomTile][leftTile] != 1) {
			pacmanPos.x = nextX;
		}
	}

	// Checking vertical collisions (walls above and below pacman)
	if (pacmanVelocity.y > 0 && bottomTile < MAP_HEIGHT) {
		if (map[bottomTile][leftTile] != 1 && map[bottomTile][rightTile] != 1) {
			pacmanPos.y = nextY;
		}
	}
	else if (pacmanVelocity.y < 0 && topTile >= 0) {
		if (map[topTile][leftTile] != 1 && map[topTile][rightTile] != 1) {
			pacmanPos.y = nextY;
		}
	}

	// Checks for collision with coins
	int currentTileX = static_cast<int>(pacmanPos.x / TILE_SIZE);
	int currentTileY = static_cast<int>(pacmanPos.y / TILE_SIZE);

	if (map[currentTileY][currentTileX] == 2) {
		// Pacman collided with a coin, so we remove the coin and increase score
		map[currentTileY][currentTileX] = 0;
		score++;
	}
	else if (map[currentTileY][currentTileX] == 3) {
		// Pacman collided with a teleporter
		if (!teleporterCooldownActive) {
			// Finding the other teleporter and moving pacman there
			for (int i = 0; i < MAP_HEIGHT; i++) {
				for (int j = 0; j < MAP_WIDTH; j++) {
					if (map[i][j] == 3 && (i != currentTileY || j != currentTileX)) {
						pacmanPos = { static_cast<float>(j * TILE_SIZE), static_cast<float>(i * TILE_SIZE) };
						pacmanRect.x = pacmanPos.x;
						pacmanRect.y = pacmanPos.y;

						// Activate teleporter cooldown (mainly to avoid bugs)
						teleporterCooldownActive = true;
						lastTeleportTime = time(nullptr);

						break;
					}
				}
			}
		}
	}

	// Checking teleporter cooldown
	if (teleporterCooldownActive && difftime(time(nullptr), lastTeleportTime) >= teleporterCooldownDuration) {
		teleporterCooldownActive = false;
	}
	pacmanRect.x = pacmanPos.x;
	pacmanRect.y = pacmanPos.y;
}

bool CheckCollision(Rectangle rect1, Rectangle rect2) {
	return !(rect1.x + rect1.width < rect2.x ||
		rect1.x > rect2.x + rect2.width ||
		rect1.y + rect1.height < rect2.y ||
		rect1.y > rect2.y + rect2.height);
}

void UpdateGhost1() {

	time_t currentTime = time(nullptr);
	float elapsedTime = difftime(currentTime, startTime);

	// Pausing the ghost for 3 seconds at the beginning
	if (elapsedTime < 1.0f) {
		return;  // Do nothing, ghost is paused
	}
	// Calculate distance between ghost and pacman
	float dx = pacmanPos.x - ghostPos.x;
	float dy = pacmanPos.y - ghostPos.y;

	float distanceThreshold;
	if (difficultyLevelLabel == "Easy") {
		distanceThreshold = TILE_SIZE * 3; // Determines how close pacman can get to ghost before getting chased
	}
	else if (difficultyLevelLabel == "Hard") {
		distanceThreshold = TILE_SIZE * 4;
	}
	else if (difficultyLevelLabel == "Impossible") {
		distanceThreshold = TILE_SIZE * 5;
	}
	else {
		distanceThreshold = TILE_SIZE * 3; // Default value
	}
	// If Pac-Man is close, set a target position for the ghost
	if (sqrt(dx * dx + dy * dy) < distanceThreshold) {
		float targetX = pacmanPos.x;
		float targetY = pacmanPos.y;

		// Smoothly adjust the ghost's target position
		float smoothingFactor = 1.0f;
		targetX = ghostPos.x + (targetX - ghostPos.x) * smoothingFactor;
		targetY = ghostPos.y + (targetY - ghostPos.y) * smoothingFactor;

		// Setting the ghost's velocity based on the target position
		ghostVelocity = { targetX - ghostPos.x, targetY - ghostPos.y };

		// Normalizing the velocity to maintain constant speed
		float length = sqrt(ghostVelocity.x * ghostVelocity.x + ghostVelocity.y * ghostVelocity.y);
		if (length > 0) {
			ghostVelocity.x /= length;
			ghostVelocity.y /= length;
		}
	}
	else {
		// If Pacman is far, patrol the areas without walls

		// Randomly change direction every few seconds
		if (GetRandomValue(0, 100) < 2) {
			// Find a random direction for patrolling
			int direction = GetRandomValue(0, 3);
			float speed = ghostSpeed;

			// Adjusting the ghost's velocity based on the chosen direction
			switch (direction) {
			case 0: // Up
				ghostVelocity = { 0, -speed };
				break;
			case 1: // Down
				ghostVelocity = { 0, speed };
				break;
			case 2: // Left
				ghostVelocity = { -speed, 0 };
				break;
			case 3: // Right
				ghostVelocity = { speed, 0 };
				break;
			}

			// Normalizing the velocity to maintain constant speed
			float length = sqrt(ghostVelocity.x * ghostVelocity.x + ghostVelocity.y * ghostVelocity.y);
			if (length > 0) {
				ghostVelocity.x /= length;
				ghostVelocity.y /= length;
			}
		}
	}
	// Calculating the next position of the ghost
	float nextX = ghostPos.x + ghostVelocity.x * ghostSpeed;
	float nextY = ghostPos.y + ghostVelocity.y * ghostSpeed;

	// Checking collisions with walls
	int leftTile = static_cast<int>((nextX) / TILE_SIZE);
	int rightTile = static_cast<int>((nextX + TILE_SIZE - 1) / TILE_SIZE);
	int topTile = static_cast<int>((nextY) / TILE_SIZE);
	int bottomTile = static_cast<int>((nextY + TILE_SIZE - 1) / TILE_SIZE);

	if (ghostVelocity.x > 0 && rightTile < MAP_WIDTH) {
		if (map[topTile][rightTile] != 1 && map[bottomTile][rightTile] != 1) {
			ghostPos.x = nextX;
		}
	}
	else if (ghostVelocity.x < 0 && leftTile >= 0) {
		if (map[topTile][leftTile] != 1 && map[bottomTile][leftTile] != 1) {
			ghostPos.x = nextX;
		}
	}

	if (ghostVelocity.y > 0 && bottomTile < MAP_HEIGHT) {
		if (map[topTile][rightTile] != 1 && map[bottomTile][rightTile] != 1) {
			ghostPos.y = nextY;
		}
	}
	else if (ghostVelocity.y < 0 && topTile >= 0) {
		if (map[topTile][leftTile] != 1 && map[bottomTile][leftTile] != 1) {
			ghostPos.y = nextY;
		}
	}
	// Checking for collision between Ghost1 and Ghost2
	if (CheckCollision({ ghostPos.x, ghostPos.y, TILE_SIZE - 8.0f, TILE_SIZE - 2.5f },
		{ ghost2Pos.x, ghost2Pos.y, TILE_SIZE - 8.0f, TILE_SIZE - 2.5f })) {
		// Adjust the position of Ghost1
		// You can modify this part based on your desired behavior
		ghostPos.x -= ghostVelocity.x * ghostSpeed;
		ghostPos.y -= ghostVelocity.y * ghostSpeed;
	}
}

void DrawGhost1() {
	// Determine the direction the ghost is moving
	if (ghostVelocity.x > 0) {
		currentGhostTexture = ghostTextureRight; // Use the texture for right movement
	}
	else {
		currentGhostTexture = ghostTextureLeft;  // Use the texture for left movement
	}

	// Draw the ghost using the selected texture
	DrawTexture(currentGhostTexture, static_cast<int>(ghostPos.x), static_cast<int>(ghostPos.y), WHITE);
}

void UpdateGhost2() {
	time_t currentTime = time(nullptr);
	float elapsedTime = difftime(currentTime, startTime);

	// Pausing the second ghost for 3 seconds at the beginning
	if (elapsedTime < 3.0f) {
		return;  // Do nothing, second ghost is paused
	}

	float dx2 = pacmanPos.x - ghost2Pos.x;
	float dy2 = pacmanPos.y - ghost2Pos.y;

	float distanceThreshold; // Determines how close pacman can get to ghost before getting chased
	if (difficultyLevelLabel == "Easy") {
		distanceThreshold = TILE_SIZE * 3;
	}
	else if (difficultyLevelLabel == "Hard") {
		distanceThreshold = TILE_SIZE * 4;
	}
	else if (difficultyLevelLabel == "Impossible") {
		distanceThreshold = TILE_SIZE * 5;
	}
	else {
		distanceThreshold = TILE_SIZE * 3; // Default (which is Easy mode)
	}

	// If Pacman is close, the second ghost will start chasing Pacman
	if (sqrt(dx2 * dx2 + dy2 * dy2) < distanceThreshold) {
		float targetX2 = pacmanPos.x;
		float targetY2 = pacmanPos.y;

		// Smoothly adjust the second ghost's target position to avoid getting stuck at corners
		float smoothingFactor2 = 1.0f;
		targetX2 = ghost2Pos.x + (targetX2 - ghost2Pos.x) * smoothingFactor2;
		targetY2 = ghost2Pos.y + (targetY2 - ghost2Pos.y) * smoothingFactor2;

		// Setting the second ghost's velocity based on Pacman’s position
		ghost2Velocity = { targetX2 - ghost2Pos.x, targetY2 - ghost2Pos.y };

		// Normalizing the velocity to maintain constant speed
		float length2 = sqrt(ghost2Velocity.x * ghost2Velocity.x + ghost2Velocity.y * ghost2Velocity.y);
		if (length2 > 0) {
			ghost2Velocity.x /= length2;
			ghost2Velocity.y /= length2;
		}
	}
	else {
		// If Pacman is far, patrol the areas without walls

		// Randomly change direction every few seconds
		if (GetRandomValue(0, 100) < 2) {
			// Find a random direction for patrolling
			int direction2 = GetRandomValue(0, 3);
			float speed2 = ghostSpeed;

			// Adjust the second ghost's velocity based on the chosen direction
			switch (direction2) {
			case 0: // Up
				ghost2Velocity = { 0, -speed2 };
				break;
			case 1: // Down
				ghost2Velocity = { 0, speed2 };
				break;
			case 2: // Left
				ghost2Velocity = { -speed2, 0 };
				break;
			case 3: // Right
				ghost2Velocity = { speed2, 0 };
				break;
			}

			// Normalizing the velocity to maintain constant speed
			float length2 = sqrt(ghost2Velocity.x * ghost2Velocity.x + ghost2Velocity.y * ghost2Velocity.y);
			if (length2 > 0) {
				ghost2Velocity.x /= length2;
				ghost2Velocity.y /= length2;
			}
		}

	}

	// Calculating the next position of the second ghost
	float nextX2 = ghost2Pos.x + ghost2Velocity.x * ghostSpeed;
	float nextY2 = ghost2Pos.y + ghost2Velocity.y * ghostSpeed;

	// Checking collisions with walls
	int leftTile2 = static_cast<int>((nextX2) / TILE_SIZE);
	int rightTile2 = static_cast<int>((nextX2 + TILE_SIZE - 1) / TILE_SIZE);
	int topTile2 = static_cast<int>((nextY2) / TILE_SIZE);
	int bottomTile2 = static_cast<int>((nextY2 + TILE_SIZE - 1) / TILE_SIZE);

	if (ghost2Velocity.x > 0 && rightTile2 < MAP_WIDTH) {
		if (map[topTile2][rightTile2] != 1 && map[bottomTile2][rightTile2] != 1) {
			ghost2Pos.x = nextX2;
		}
	}
	else if (ghost2Velocity.x < 0 && leftTile2 >= 0) {
		if (map[topTile2][leftTile2] != 1 && map[bottomTile2][leftTile2] != 1) {
			ghost2Pos.x = nextX2;
		}
	}

	if (ghost2Velocity.y > 0 && bottomTile2 < MAP_HEIGHT) {
		if (map[topTile2][rightTile2] != 1 && map[bottomTile2][rightTile2] != 1) {
			ghost2Pos.y = nextY2;
		}
	}
	else if (ghost2Velocity.y < 0 && topTile2 >= 0) {
		if (map[topTile2][leftTile2] != 1 && map[bottomTile2][leftTile2] != 1) {
			ghost2Pos.y = nextY2;
		}
	}
	// Checking for collision between Ghost2 and Ghost1
	if (CheckCollision({ ghost2Pos.x, ghost2Pos.y, TILE_SIZE - 8.0f, TILE_SIZE - 2.5f },
		{ ghostPos.x, ghostPos.y, TILE_SIZE - 8.0f, TILE_SIZE - 2.5f })) {
		// Adjust the position of Ghost2
		// You can modify this part based on your desired behavior
		ghost2Pos.x -= ghost2Velocity.x * ghostSpeed;
		ghost2Pos.y -= ghost2Velocity.y * ghostSpeed;
	}
}

void DrawGhost2() {
	// Determine the direction the second ghost is moving
	if (ghost2Velocity.x > 0) {
		currentGhost2Texture = ghost2TextureRight; // Use the texture for right movement
	}
	else {
		currentGhost2Texture = ghost2TextureLeft;  // Use the texture for left movement
	}

	// Draw the second ghost using the selected texture
	DrawTexture(currentGhost2Texture, static_cast<int>(ghost2Pos.x), static_cast<int>(ghost2Pos.y), WHITE);
}

void DrawCoin(int i, int j) {
	Rectangle rect = { j * TILE_SIZE, i * TILE_SIZE, TILE_SIZE, TILE_SIZE };
	DrawRectangleRec(rect, BLACK);
	DrawCircle(j * TILE_SIZE + TILE_SIZE / 2, i * TILE_SIZE + TILE_SIZE / 2, TILE_SIZE / 6, YELLOW);
}

void DrawTeleporter(int i, int j) {
	Rectangle rect = { j * TILE_SIZE, i * TILE_SIZE, TILE_SIZE, TILE_SIZE };
	DrawRectangleRec(rect, BLACK);
	DrawCircle(j * TILE_SIZE + TILE_SIZE / 2, i * TILE_SIZE + TILE_SIZE / 2, TILE_SIZE / 4, BLACK);
}

bool gameWon() {
	// if all coins get eaten, game is won by the player.
	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			if (map[i][j] == 2) {
				return false;  // When there is at least one coin remaining
			}
		}
	}
	playerWon = true;  // All coins have been eaten
	return true;
}

void Retry() {
	// Reset the map to its initial state
	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			map[i][j] = initialMap[i][j];
		}
	}
	Initialize();

	// Reset other game-related variables
	StopSound(gamewonSound);
	PlayStartAudio();
	PlayGameplayAudio();
	playerWon = false;
	gameOver = false;
	startTime = time(nullptr);
	score = 0;
	pacmanVelocity = { 0.0f, 0.0f };

}

void DrawYouWin() {

	BeginDrawing();
	ClearBackground(BLACK);

	DrawText("You Win!", GetScreenWidth() / 2 - MeasureText("You Win!", 40) / 2, GetScreenHeight() / 2 - 60, 40, GREEN);
	DrawText("Press Enter to Replay", GetScreenWidth() / 2 - MeasureText("Press Enter to Replay", 20) / 2, GetScreenHeight() / 2, 20, GREEN);
	DrawText("Press ESC to Quit", GetScreenWidth() / 2 - MeasureText("Press ESC to Quit", 20) / 2, GetScreenHeight() / 2 + 40, 20, RED);
	DrawText(("Score: " + std::to_string(score)).c_str(), 10, 10, 20, WHITE);
	DrawText("Created by ~ Ayaan Qazi", GetScreenWidth() - MeasureText("Created by ~ Ayaan Qazi", 20) - 10, 10, 20, PURPLE);
	EndDrawing();

	// Handling input for retry and quit buttons
	if (IsKeyPressed(KEY_ENTER)) {
		CloseWindow();
		// Retry the game		
		Retry();
	}
	else if (IsKeyPressed(KEY_ESCAPE)) {
		// Quit the game
		CloseWindow();
		exit(0);
	}
	DifficultyLevel();

}

void DrawGame() {
	// If all coins are eaten, display the "You Win" screen
	if (gameWon()) {
		DrawYouWin();
	}
	else {
		BeginDrawing();
		ClearBackground(BLACK);  // Setting background color to black

		// Drawing map according to map array
		for (int i = 0; i < MAP_HEIGHT; i++) {
			for (int j = 0; j < MAP_WIDTH; j++) {
				Rectangle rect = { j * TILE_SIZE, i * TILE_SIZE, TILE_SIZE, TILE_SIZE };
				if (map[i][j] == 1) {
					DrawRectangleRec(rect, BLUE); // Walls
				}
				else if (map[i][j] == 2) {
					DrawCoin(i, j);
				}
				else if (map[i][j] == 3) {
					DrawTeleporter(i, j);
				}
			}
		}

		// Choosing appropriate texture for pacman based on its direction
		Texture2D currentTexture = pacmanRight;
		if (pacmanVelocity.x > 0) {
			currentTexture = pacmanRight;
		}
		else if (pacmanVelocity.x < 0) {
			currentTexture = pacmanLeft;
		}
		else if (pacmanVelocity.y > 0) {
			currentTexture = pacmanDown;
		}
		else if (pacmanVelocity.y < 0) {
			currentTexture = pacmanUp;
		}
		DrawTexture(currentTexture, static_cast<int>(pacmanRect.x), static_cast<int>(pacmanRect.y), WHITE);

		DrawText(("Score: " + std::to_string(score)).c_str(), 10, 10, 20, WHITE);

		// Calculates teleporters cooldown time (this is to avoid bugs)
		float remainingCooldown = teleporterCooldownDuration - difftime(time(nullptr), lastTeleportTime);
		if (remainingCooldown < 0) {
			remainingCooldown = 0;
		}


		// Drawing the ghost after drawing Pacman and coins
		DrawGhost1();
		DrawGhost2();

		EndDrawing();
	}
}

void DisplayGameOverScreen() {
	// Displaying the game over screen
	BeginDrawing();
	ClearBackground(BLACK);
	DrawText("Game Over!", GetScreenWidth() / 2 - MeasureText("Game Over!", 40) / 2, GetScreenHeight() / 2 - 55, 40, RED);
	DrawText("Press Enter to Retry", GetScreenWidth() / 2 - MeasureText("Press Enter to Retry", 20) / 2, GetScreenHeight() / 2 + 10, 20, GREEN);
	DrawText("Press ESC to Quit", GetScreenWidth() / 2 - MeasureText("Press ESC to Quit", 20) / 2, GetScreenHeight() / 2 + 60, 20, RED);
	DrawText(("Score: " + std::to_string(score)).c_str(), 10, 10, 20, WHITE);
	DrawText("Created by ~ Ayaan Qazi", GetScreenWidth() - MeasureText("Created by ~ Ayaan Qazi", 20) - 10, 10, 20, PURPLE);

	EndDrawing();

	// Handling input for retry and quit buttons
	if (IsKeyPressed(KEY_ENTER)) {
		CloseWindow();
		// Retry the game		
		Retry();
	}
	else if (IsKeyPressed(KEY_ESCAPE)) {
		// Quit the game
		CloseWindow();
		exit(0);
	}
	DifficultyLevel();
}

void GameOver() {
	gameOver = true;

	StopSound(gameplaySound);
	StopSound(startSound);
	PlayGameoverAudio();
	DisplayGameOverScreen();
}

void DrawMenu() {
	BeginDrawing();
	ClearBackground(BLACK);

	DrawText("Pacman Game", GetScreenWidth() / 2 - MeasureText("Pacman Game", 40) / 2, GetScreenHeight() / 4 + 40, 40, BLUE);
	DrawText("Press Enter to Start", GetScreenWidth() / 2 - MeasureText("Press Enter to Start", 20) / 2, GetScreenHeight() / 2, 20, GREEN);
	DrawText("Press ESC to Quit", GetScreenWidth() / 2 - MeasureText("Press ESC to Quit", 20) / 2, GetScreenHeight() / 2 + 40, 20, RED);
	DrawText("Created by ~ Ayaan Qazi", GetScreenWidth() - MeasureText("Created by ~ Ayaan Qazi", 20) - 10, 10, 20, PURPLE);

	if (IsKeyPressed(KEY_ENTER)) {
		showMenu = false;
		startTime = time(nullptr);

		// Play start audio when the game begins
		PlayStartAudio();
		PlayGameplayAudio();
	}
	else if (IsKeyPressed(KEY_ESCAPE)) {
		CloseWindow();
	}
	EndDrawing();
}

int main() {
	Initialize();
	InitializeAudio();

	while (!WindowShouldClose()) {
		if (showMenu) {
			DrawMenu();
			DifficultyLevel();
		}
		else if (!gameOver && !playerWon) {
			// Condition to check if the game is not won

			HandleInput();
			UpdatePacman();
			UpdateGhost1();
			UpdateGhost2();

			// Checking for collision between Pacman and Ghost1
			if (CheckCollision(pacmanRect, { ghostPos.x, ghostPos.y, TILE_SIZE - 8.0f, TILE_SIZE - 2.5f })) {
				// Pacman and Ghost1 collided, handle game over
				GameOver();
			}

			// Checking for collision between Pacman and Ghost2
			if (CheckCollision(pacmanRect, { ghost2Pos.x, ghost2Pos.y, TILE_SIZE - 8.0f, TILE_SIZE - 2.5f })) {
				// Pacman and Ghost2 collided, handle game over
				GameOver();
			}

			DrawGame();
			DrawGhost1();
			DrawGhost2();

			// Check if the game is won
			if (gameWon()) {
				playerWon = true;

				StopSound(gameplaySound);

				// Play game won audio
				PlayGamewonAudio();
			}
		}
		else if (playerWon) {
			DrawYouWin();
		}
		else {
			DisplayGameOverScreen();
		}
	}

	UnloadAudio();
	CloseWindow();
	return 0;
}

