#include "Game.h"
#include "Utils.h"
#include "data/DataCenter.h"
#include "data/OperationCenter.h"
#include "data/SoundCenter.h"
#include "data/ImageCenter.h"
#include "data/FontCenter.h"
#include "Player.h"
/*-----I2P Revise start-----*/
#include "Hero.h"
#include "Tank.h"
#include "Obstacle.h"
#include "./shapes/Point.h"
/*-----I2P Revise end-----*/
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_acodec.h>
#include <vector>
#include <cstring>

// fixed settings
constexpr char game_icon_img_path[] = "./assets/image/game_icon.png";
constexpr char game_start_sound_path[] = "./assets/sound/bgm.wav";
constexpr char background_img_path[] = "./assets/image/background.jpg";
constexpr char initial_img_path[] = "./assets/image/initial.jpg";
constexpr char modechoose_img_path[] = "./assets/image/ModeChoose.jpg";
constexpr char pause_img_path[] = "./assets/image/pause.jpg";
constexpr char white_img_path[] = "./assets/image/white.jpg";
constexpr char background_sound_path[] = "./assets/sound/BackgroundMusic.ogg";


/**
 * @brief Game entry.
 * @details The function processes all allegro events and update the event state to a generic data storage (i.e. DataCenter).
 * For timer event, the game_update and game_draw function will be called if and only if the current is timer.
 */
void Game::execute()
{
	DataCenter *DC = DataCenter::get_instance();
	// main game loop
	bool run = true;
	while (run)
	{
		// process all events here
		al_wait_for_event(event_queue, &event);

		if (name_input_active) {
            if (event.type == ALLEGRO_EVENT_KEY_CHAR) {
                char key = event.keyboard.unichar;

                // 處理特殊鍵（退格和完成輸入）
                if (key == '\b') { // Backspace
                    if (name_input_player == 1 && !player1_name.empty()) {
                        player1_name.pop_back();
                    } else if (name_input_player == 2 && !player2_name.empty()) {
                        player2_name.pop_back();
                    }
                } else if (key == '\r') { // Enter，完成輸入
                    name_input_active = false;
                    if (name_input_player == 1) {
                        player1_name_done = true; // 玩家一完成輸入
                    }
                    if (name_input_player == 2) {
                        player2_name_done = true; // 玩家二完成輸入
                    }
                    name_input_player = 0; // 結束輸入狀態
                } else if (isprint(key)) { // 可見字符輸入
                    if (name_input_player == 1) {
                        player1_name += key;
                    } else if (name_input_player == 2) {
                        player2_name += key;
                    }
                }
            }
        }

		switch (event.type)
		{
		case ALLEGRO_EVENT_TIMER:
		{
			run &= game_update();
			game_draw();
			break;
		}
		case ALLEGRO_EVENT_DISPLAY_CLOSE:
		{ // stop game
			run = false;
			break;
		}
		case ALLEGRO_EVENT_KEY_DOWN:
		{
			DC->key_state[event.keyboard.keycode] = true;
			break;
		}
		case ALLEGRO_EVENT_KEY_UP:
		{
			DC->key_state[event.keyboard.keycode] = false;
			break;
		}
		case ALLEGRO_EVENT_MOUSE_AXES:
		{
			DC->mouse.x = event.mouse.x;
			DC->mouse.y = event.mouse.y;
			break;
		}
		case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
		{
			DC->mouse_state[event.mouse.button] = true;
			break;
		}
		case ALLEGRO_EVENT_MOUSE_BUTTON_UP:
		{
			DC->mouse_state[event.mouse.button] = false;
			break;
		}
		// 鍵盤字符輸入事件（額外用途）
        case ALLEGRO_EVENT_KEY_CHAR: {
            // 可根據需要增加自訂邏輯
            break;
        }
		default:
			break;
		}
	}
}

/**
 * @brief Initialize all allegro addons and the game body.
 * @details Only one timer is created since a game and all its data should be processed synchronously.
 */
Game::Game()
{
	DataCenter *DC = DataCenter::get_instance();
	GAME_ASSERT(al_init(), "failed to initialize allegro.");

	// initialize allegro addons
	bool addon_init = true;
	addon_init &= al_init_primitives_addon();
	addon_init &= al_init_font_addon();
	addon_init &= al_init_ttf_addon();
	addon_init &= al_init_image_addon();
	addon_init &= al_init_acodec_addon();
	GAME_ASSERT(addon_init, "failed to initialize allegro addons.");

	// initialize events
	bool event_init = true;
	event_init &= al_install_keyboard();
	event_init &= al_install_mouse();
	event_init &= al_install_audio();
	GAME_ASSERT(event_init, "failed to initialize allegro events.");

	// initialize game body
	GAME_ASSERT(
		display = al_create_display(DC->window_width, DC->window_height),
		"failed to create display.");
	GAME_ASSERT(
		timer = al_create_timer(1.0 / DC->FPS),
		"failed to create timer.");
	GAME_ASSERT(
		event_queue = al_create_event_queue(),
		"failed to create event queue.");

	debug_log("Game initialized.\n");
	game_init();
}

/**
 * @brief Initialize all auxiliary resources.
 */
void Game::game_init()
{
	DataCenter *DC = DataCenter::get_instance();
	SoundCenter *SC = SoundCenter::get_instance();
	ImageCenter *IC = ImageCenter::get_instance();
	FontCenter *FC = FontCenter::get_instance();
	// set window icon
	game_icon = IC->get(game_icon_img_path);
	al_set_display_icon(display, game_icon);

	// register events to event_queue
	al_register_event_source(event_queue, al_get_display_event_source(display));
	al_register_event_source(event_queue, al_get_keyboard_event_source());
	al_register_event_source(event_queue, al_get_mouse_event_source());
	al_register_event_source(event_queue, al_get_timer_event_source(timer));

	// init sound setting
	SC->init();

	// init font setting
	FC->init();

	ui = new UI();
	ui->init();

	/*-----I2P Revise start-----*/
    

	/*-----I2P Revise end-----*/

	// game start
	background = IC->get(initial_img_path);
	debug_log("Game state: change to INIT\n");
	state = STATE::INIT;
	al_start_timer(timer);
}

/**
 * @brief The function processes all data update.
 * @details The behavior of the whole game body is determined by its state.
 * @return Whether the game should keep running (true) or reaches the termination criteria (false).
 * @see Game::STATE
 */
bool Game::game_update()
{
	DataCenter *DC = DataCenter::get_instance();
	OperationCenter *OC = OperationCenter::get_instance();
	SoundCenter *SC = SoundCenter::get_instance();
	ImageCenter *IC = ImageCenter::get_instance();
	ALLEGRO_SAMPLE_INSTANCE *background2 = nullptr;
	
	static bool tank_is_created_at_game = false;
	static bool obstacle_is_created_at_game = false;

	switch (state)
	{
	case STATE::INIT:
	{
		static ALLEGRO_SAMPLE_INSTANCE *instance = nullptr;
		tank_is_created_at_game = false;
		obstacle_is_created_at_game = false;
		if (DC->mouse_state[1])
		{
			//instance = SC->play(game_start_sound_path, ALLEGRO_PLAYMODE_ONCE);
			//DC->level->load_level(1);
			debug_log("<Game> state: change to TANK_CHOOSE\n");
			state = STATE::TANK_CHOOSE;
			background = IC->get(white_img_path);
			DC->mouse_state[1] = false;
		}
		else
		{
			//instance = SC->play(game_start_sound_path, ALLEGRO_PLAYMODE_ONCE);
		}
		break;
	}
	case STATE::CHOOSE:
	{
		static ALLEGRO_SAMPLE_INSTANCE *instance = nullptr;
		background = IC->get(modechoose_img_path);
		tank_is_created_at_game = false;
		obstacle_is_created_at_game = false;
		if (DC->mouse_state[1] && DC->mouse.x < 800)
		{
			//instance = SC->play(game_start_sound_path, ALLEGRO_PLAYMODE_ONCE);
			//DC->level->load_level(1);
			debug_log("<Game> state: change to SCORE\n");
			background = IC->get(background_img_path);
			state = STATE::SCORE;
		}
		else if (DC->mouse_state[1] && DC->mouse.x > 800)
		{
			//instance = SC->play(game_start_sound_path, ALLEGRO_PLAYMODE_ONCE);
			//DC->level->load_level(1);
			debug_log("<Game> state: change to KILL\n");
			background = IC->get(background_img_path);
			state = STATE::KILL;
		}
		else 
		{
			//instance = SC->play(game_start_sound_path, ALLEGRO_PLAYMODE_ONCE);
		}
		break;
	}
	case STATE::SCORE:
	{
		//進入SCORE 創建TANK
		if(!tank_is_created_at_game){
			ControlScheme player1Controls = {ALLEGRO_KEY_W, ALLEGRO_KEY_LEFT};
			ControlScheme player2Controls = {ALLEGRO_KEY_UP, ALLEGRO_KEY_A};
			Point tank1Pos = {20, 450};
			Point tank2Pos = {1565, 450};
			Tank* tank1 = new Tank(tank1Pos, player1Controls);
			Tank* tank2 = new Tank(tank2Pos, player2Controls);
			// 將坦克加入 DataCenter 的 tanks 向量
			DC->tanks.push_back(tank1);
			DC->tanks.push_back(tank2);
			for (Tank* tank : DC->tanks) {tank->init();	tank->mode = 0;}
			tank_is_created_at_game = true;
			debug_log("call_tank\n");
		}
		//進入SCORE 創建SCORE MAP
		if(!obstacle_is_created_at_game){
			for (int i = 4; i <= 45; i++) {
				for (int j = 0; j <= DC->window_height-32; j += 32) {
					Point obstaclePos = {i * 32, j};
					Obstacle* obstacle = new Obstacle(obstaclePos, ObstacleState::EXIST);
					DC->obstacles.push_back(obstacle);
				}
			}
			for (Obstacle* obstacle : DC->obstacles) obstacle->init();
			obstacle_is_created_at_game = true;
		}
		state = STATE::LEVEL;
		break;
	}
	case STATE::KILL:
	{
		//進入KILL 創建TANK
		if(!tank_is_created_at_game){
			ControlScheme player1Controls = {ALLEGRO_KEY_W, ALLEGRO_KEY_LEFT};
			ControlScheme player2Controls = {ALLEGRO_KEY_UP, ALLEGRO_KEY_A};
			Point tank1Pos = {20, 434};
			Point tank2Pos = {1565, 434};
			Tank* tank1 = new Tank(tank1Pos, player1Controls);
			Tank* tank2 = new Tank(tank2Pos, player2Controls);
			// 將坦克加入 DataCenter 的 tanks 向量
			DC->tanks.push_back(tank1);
			DC->tanks.push_back(tank2);
			for (Tank* tank : DC->tanks) {tank->init(); tank->mode = 1;}
			tank_is_created_at_game = true;
			debug_log("call_tank\n");
		}
		//進入KILL 創KILL MAP
		if(!obstacle_is_created_at_game){

			//tank1 initial
			for (int i = 0; i <= 2; i++) {
				Point obstaclePos = {i * 32, 370};
				Obstacle* obstacle = new Obstacle(obstaclePos, ObstacleState::STONE);
				DC->obstacles.push_back(obstacle);
			}
			for (int i = 0; i <= 2; i++) {
				Point obstaclePos = {i * 32, 466};
				Obstacle* obstacle = new Obstacle(obstaclePos, ObstacleState::STONE);
				DC->obstacles.push_back(obstacle);
			}
			for (int i = 0; i <= 1; i++) {
				Point obstaclePos = {64, 402 + 32 * i};
				Obstacle* obstacle = new Obstacle(obstaclePos, ObstacleState::EXIST);
				DC->obstacles.push_back(obstacle);
			}
			//tank1 initial end

			//tank2 initial
			for (int i = 0; i <= 2; i++) {
				Point obstaclePos = {1568 - i * 32, 370};
				Obstacle* obstacle = new Obstacle(obstaclePos, ObstacleState::STONE);
				DC->obstacles.push_back(obstacle);
			}
			for (int i = 0; i <= 2; i++) {
				Point obstaclePos = {1568 - i * 32, 466};
				Obstacle* obstacle = new Obstacle(obstaclePos, ObstacleState::STONE);
				DC->obstacles.push_back(obstacle);
			}
			for (int i = 0; i <= 1; i++) {
				Point obstaclePos = {1504, 402 + 32 * i};
				Obstacle* obstacle = new Obstacle(obstaclePos, ObstacleState::EXIST);
				DC->obstacles.push_back(obstacle);
			}
			//tank2 initial end

			//leftup
			for (int i = 5; i <= 6; i++) {
				for (int j = 0; j <= 256; j += 32) {
					Point obstaclePos = {i * 32, j};
					Obstacle* obstacle = new Obstacle(obstaclePos, ObstacleState::EXIST);
					DC->obstacles.push_back(obstacle);
				}
			}
			for (int i = 7; i <= 10; i++) {
				for (int j = 0; j <= 192; j += 32) {
					Point obstaclePos = {i * 32, j};
					Obstacle* obstacle = new Obstacle(obstaclePos, ObstacleState::STONE);
					DC->obstacles.push_back(obstacle);
				}
				for (int j = 224; j <= 256; j += 32) {
					Point obstaclePos = {i * 32, j};
					Obstacle* obstacle = new Obstacle(obstaclePos, ObstacleState::EXIST);
					DC->obstacles.push_back(obstacle);
				}
			}
			for (int i = 11; i <= 12; i++) {
				for (int j = 0; j <= 256; j += 32) {
					Point obstaclePos = {i * 32, j};
					Obstacle* obstacle = new Obstacle(obstaclePos, ObstacleState::EXIST);
					DC->obstacles.push_back(obstacle);
				}
			}
			//leftup end

			//mid
			//mid stone
			for (int i = 0; i <= 9; i++) {
				Point obstaclePos = {624 + i * 32, 274};
				Obstacle* obstacle = new Obstacle(obstaclePos, ObstacleState::STONE);
				DC->obstacles.push_back(obstacle);
			}
			for (int j = 306; j <= 338; j += 32) {
				Point obstaclePos = {624 , j};
				Obstacle* obstacle = new Obstacle(obstaclePos, ObstacleState::STONE);
				DC->obstacles.push_back(obstacle);
			}
			for (int j = 306; j <= 338; j += 32) {
				Point obstaclePos = {912 , j};
				Obstacle* obstacle = new Obstacle(obstaclePos, ObstacleState::STONE);
				DC->obstacles.push_back(obstacle);
			}
			for (int i = 0; i <= 9; i++) {
				Point obstaclePos = {624 + i * 32, 562};
				Obstacle* obstacle = new Obstacle(obstaclePos, ObstacleState::STONE);
				DC->obstacles.push_back(obstacle);
			}
			for (int j = 498; j <= 530; j += 32) {
				Point obstaclePos = {624 , j};
				Obstacle* obstacle = new Obstacle(obstaclePos, ObstacleState::STONE);
				DC->obstacles.push_back(obstacle);
			}
			for (int j = 498; j <= 530; j += 32) {
				Point obstaclePos = {912 , j};
				Obstacle* obstacle = new Obstacle(obstaclePos, ObstacleState::STONE);
				DC->obstacles.push_back(obstacle);
			}
			//mid stone end
			//mid exist
			for (int i = 0; i <= 7; i++) {
				Point obstaclePos = {656 + i * 32, 306};
				Obstacle* obstacle = new Obstacle(obstaclePos, ObstacleState::EXIST);
				DC->obstacles.push_back(obstacle);
			}
			for (int i = 0; i <= 7; i++) {
				Point obstaclePos = {656 + i * 32, 530};
				Obstacle* obstacle = new Obstacle(obstaclePos, ObstacleState::EXIST);
				DC->obstacles.push_back(obstacle);
			}
			for (int i = 0; i <= 5; i++) {
				Point obstaclePos = {656, 338 + i * 32};
				Obstacle* obstacle = new Obstacle(obstaclePos, ObstacleState::EXIST);
				DC->obstacles.push_back(obstacle);
			}
			for (int i = 0; i <= 5; i++) {
				Point obstaclePos = {880, 338 + i * 32};
				Obstacle* obstacle = new Obstacle(obstaclePos, ObstacleState::EXIST);
				DC->obstacles.push_back(obstacle);
			}
			//mid exist end
			//mid orange
			for (int i = 0; i <= 5; i++) {
				Point obstaclePos = {688 + i * 32, 338};
				Obstacle* obstacle = new Obstacle(obstaclePos, ObstacleState::ORANGE);
				DC->obstacles.push_back(obstacle);
			}
			for (int i = 0; i <= 5; i++) {
				Point obstaclePos = {688 + i * 32, 498};
				Obstacle* obstacle = new Obstacle(obstaclePos, ObstacleState::ORANGE);
				DC->obstacles.push_back(obstacle);
			}
			for (int i = 0; i <= 3; i++) {
				Point obstaclePos = {688, 370 + i * 32};
				Obstacle* obstacle = new Obstacle(obstaclePos, ObstacleState::ORANGE);
				DC->obstacles.push_back(obstacle);
			}
			for (int i = 0; i <= 3; i++) {
				Point obstaclePos = {848, 370 + i * 32};
				Obstacle* obstacle = new Obstacle(obstaclePos, ObstacleState::ORANGE);
				DC->obstacles.push_back(obstacle);
			}
			//mid orange end
			//mid pink
			for (int i = 0; i <= 3; i++) {
				Point obstaclePos = {720 + i * 32, 370};
				Obstacle* obstacle = new Obstacle(obstaclePos, ObstacleState::PINK);
				DC->obstacles.push_back(obstacle);
			}
			for (int i = 0; i <= 3; i++) {
				Point obstaclePos = {720 + i * 32, 466};
				Obstacle* obstacle = new Obstacle(obstaclePos, ObstacleState::PINK);
				DC->obstacles.push_back(obstacle);
			}
			for (int i = 0; i <= 1; i++) {
				Point obstaclePos = {720, 402 + i * 32};
				Obstacle* obstacle = new Obstacle(obstaclePos, ObstacleState::PINK);
				DC->obstacles.push_back(obstacle);
			}
			for (int i = 0; i <= 1; i++) {
				Point obstaclePos = {816, 402 + i * 32};
				Obstacle* obstacle = new Obstacle(obstaclePos, ObstacleState::PINK);
				DC->obstacles.push_back(obstacle);
			}
			//mid pink end
			//mid blue
			for (int i = 0; i <= 1; i++) {
				for (int j = 0; j <= 1; j++) {
					Point obstaclePos = {752 + i * 32, 402 + j * 32};
					Obstacle* obstacle = new Obstacle(obstaclePos, ObstacleState::BLUE);
					DC->obstacles.push_back(obstacle);
				}
			}
			//mid blue end
			//mid end

			//rightup
			for (int i = 0; i <= 1; i++) {
				for (int j = 0; j <= 5; j++) {
					Point obstaclePos = {1136 + i * 32, j * 32};
					Obstacle* obstacle = new Obstacle(obstaclePos, ObstacleState::STONE);
					DC->obstacles.push_back(obstacle);
				}
			}
			for (int i = 0; i <= 9; i++) {
				Point obstaclePos = {1104, i * 32};
				Obstacle* obstacle = new Obstacle(obstaclePos, ObstacleState::EXIST);
				DC->obstacles.push_back(obstacle);
			}
			for (int i = 0; i <= 9; i++) {
				Point obstaclePos = {1200, i * 32};
				Obstacle* obstacle = new Obstacle(obstaclePos, ObstacleState::EXIST);
				DC->obstacles.push_back(obstacle);
			}
			for (int i = 0; i <= 1; i++) {
				for (int j = 6; j <= 9; j++) {
					Point obstaclePos = {1136 + i * 32, j * 32};
					Obstacle* obstacle = new Obstacle(obstaclePos, ObstacleState::EXIST);
					DC->obstacles.push_back(obstacle);
				}
			}
			//rightup end

			//midup 
			for (int i = 0; i <= 1; i++) {
				for (int j = 0; j <= 7; j++) {
					Point obstaclePos = {752 + i * 32, j * 32};
					Obstacle* obstacle = new Obstacle(obstaclePos, ObstacleState::EXIST);
					DC->obstacles.push_back(obstacle);
				}
			}
			//midup end

			//middown
			for (int i = 0; i <= 1; i++) {
				for (int j = 0; j <= 8; j++) {
					Point obstaclePos = {752 + i * 32, 868 - j * 32};
					Obstacle* obstacle = new Obstacle(obstaclePos, ObstacleState::EXIST);
					DC->obstacles.push_back(obstacle);
				}
			}
			//middown end

			//between init and mid
			for (int i = 0; i <= 1; i++) {
				Point obstaclePos = {360, 402 + i * 32};
				Obstacle* obstacle = new Obstacle(obstaclePos, ObstacleState::STONE);
				DC->obstacles.push_back(obstacle);
			}
			for (int i = 0; i <= 1; i++) {
				Point obstaclePos = {1208, 402 + i * 32};
				Obstacle* obstacle = new Obstacle(obstaclePos, ObstacleState::STONE);
				DC->obstacles.push_back(obstacle);
			}
			for (int i = 0; i <= 3; i++) {
				Point obstaclePos = {1240, 370 + i * 32};
				Obstacle* obstacle = new Obstacle(obstaclePos, ObstacleState::EXIST);
				DC->obstacles.push_back(obstacle);
			}
			for (int i = 0; i <= 3; i++) {
				Point obstaclePos = {1176, 370 + i * 32};
				Obstacle* obstacle = new Obstacle(obstaclePos, ObstacleState::EXIST);
				DC->obstacles.push_back(obstacle);
			}
			for (int i = 0; i <= 3; i++) {
				Point obstaclePos = {392, 370 + i * 32};
				Obstacle* obstacle = new Obstacle(obstaclePos, ObstacleState::EXIST);
				DC->obstacles.push_back(obstacle);
			}
			for (int i = 0; i <= 3; i++) {
				Point obstaclePos = {328, 370 + i * 32};
				Obstacle* obstacle = new Obstacle(obstaclePos, ObstacleState::EXIST);
				DC->obstacles.push_back(obstacle);
			}
			for (int i = 0; i <= 1; i++) {
				Point obstaclePos = {360, 370 + i * 96};
				Obstacle* obstacle = new Obstacle(obstaclePos, ObstacleState::EXIST);
				DC->obstacles.push_back(obstacle);
			}
			for (int i = 0; i <= 1; i++) {
				Point obstaclePos = {1208, 370 + i * 96};
				Obstacle* obstacle = new Obstacle(obstaclePos, ObstacleState::EXIST);
				DC->obstacles.push_back(obstacle);
			}
			//between init and mid end
			//left down
			//left down 1
			for (int i = 0; i <= 1; i++) {
				Point obstaclePos = {338 + i * 32, 594};
				Obstacle* obstacle = new Obstacle(obstaclePos, ObstacleState::STONE);
				DC->obstacles.push_back(obstacle);
			}
			for (int i = 0; i <= 1; i++) {
				Point obstaclePos = {338 + i * 32, 658};
				Obstacle* obstacle = new Obstacle(obstaclePos, ObstacleState::STONE);
				DC->obstacles.push_back(obstacle);
			}
			for(int i = 1;i <= 1;i++){
				Point obstaclePos = {338 + i * 32, 626};
				Obstacle* obstacle = new Obstacle(obstaclePos, ObstacleState::STONE);
				DC->obstacles.push_back(obstacle);
			}
			//leftdown 1 end
			//left down 2
			for (int i = 0; i <= 1; i++) {
				Point obstaclePos = {338 + (i + 7) * 32, 626};
				Obstacle* obstacle = new Obstacle(obstaclePos, ObstacleState::STONE);
				DC->obstacles.push_back(obstacle);
			}
			for (int i = 0; i <= 1; i++) {
				Point obstaclePos = {338 + (i + 7) * 32, 690};
				Obstacle* obstacle = new Obstacle(obstaclePos, ObstacleState::STONE);
				DC->obstacles.push_back(obstacle);
			}
			for(int i = 1;i <= 1;i++){
				Point obstaclePos = {338 + (i + 7) * 32, 658};
				Obstacle* obstacle = new Obstacle(obstaclePos, ObstacleState::STONE);
				DC->obstacles.push_back(obstacle);
			}
			//left down 2 end
			//left down 3
			for (int i = 0; i <= 1; i++) {
				Point obstaclePos = {338 + (i + 3) * 32, 754};
				Obstacle* obstacle = new Obstacle(obstaclePos, ObstacleState::STONE);
				DC->obstacles.push_back(obstacle);
			}
			for (int i = 0; i <= 1; i++) {
				Point obstaclePos = {338 + (i + 3) * 32, 818};
				Obstacle* obstacle = new Obstacle(obstaclePos, ObstacleState::STONE);
				DC->obstacles.push_back(obstacle);
			}
			for(int i = 1;i <= 1;i++){
				Point obstaclePos = {338 + (i + 3) * 32, 786};
				Obstacle* obstacle = new Obstacle(obstaclePos, ObstacleState::STONE);
				DC->obstacles.push_back(obstacle);
			}
			//left down 3 end
			//left down big
			for (int i = 0; i <= 3; i++) {
				for(int j = 0;j <= 1;j++){
					Point obstaclePos = {128 + i * 32, 626 + j * 32};
					Obstacle* obstacle = new Obstacle(obstaclePos, ObstacleState::STONE);
					DC->obstacles.push_back(obstacle);
				}
			}
			for (int i = 0; i <= 3; i++) {
				for(int j = 4;j <= 5;j++){
					Point obstaclePos = {128 + i * 32, 626 + j * 32};
					Obstacle* obstacle = new Obstacle(obstaclePos, ObstacleState::STONE);
					DC->obstacles.push_back(obstacle);
				}
			}
			for (int i = 2; i <= 4; i++) {
				for(int j = 2;j <= 3;j++){
					Point obstaclePos = {128 + i * 32, 626 + j * 32};
					Obstacle* obstacle = new Obstacle(obstaclePos, ObstacleState::STONE);
					DC->obstacles.push_back(obstacle);
				}
			}
			//left down big end
			//left down end
			//right down
			for (int i = 0; i <= 3; i++) {
				Point obstaclePos = {880 + i * 32, 658};
				Obstacle* obstacle = new Obstacle(obstaclePos, ObstacleState::STONE);
				DC->obstacles.push_back(obstacle);
			}
			for (int i = 0; i <= 3; i++) {
				Point obstaclePos = {880 + i * 32, 750};
				Obstacle* obstacle = new Obstacle(obstaclePos, ObstacleState::STONE);
				DC->obstacles.push_back(obstacle);
			}
			
			for (int i = 0; i <= 4; i++) {
				Point obstaclePos = {1008, 530 + i * 32};
				Obstacle* obstacle = new Obstacle(obstaclePos, ObstacleState::STONE);
				DC->obstacles.push_back(obstacle);
			}
			for (int i = 7; i <= 11; i++) {
				Point obstaclePos = {1008, 530 + i * 32};
				Obstacle* obstacle = new Obstacle(obstaclePos, ObstacleState::STONE);
				DC->obstacles.push_back(obstacle);
			}
			for (int i = 0; i <= 5; i++) {
				Point obstaclePos = {1104 + i * 32, 594};
				Obstacle* obstacle = new Obstacle(obstaclePos, ObstacleState::STONE);
				DC->obstacles.push_back(obstacle);
			}
			for (int i = 0; i <= 5; i++) {
				Point obstaclePos = {1104 + i * 32, 818};
				Obstacle* obstacle = new Obstacle(obstaclePos, ObstacleState::STONE);
				DC->obstacles.push_back(obstacle);
			}
			for (int i = 0; i <= 2; i++) {
				Point obstaclePos = {1072, 594 + i * 32};
				Obstacle* obstacle = new Obstacle(obstaclePos, ObstacleState::STONE);
				DC->obstacles.push_back(obstacle);
			}
			for (int i = 0; i <= 2; i++) {
				Point obstaclePos = {1072, 754 + i * 32};
				Obstacle* obstacle = new Obstacle(obstaclePos, ObstacleState::STONE);
				DC->obstacles.push_back(obstacle);
			}
			for (int i = 0; i <= 1; i++) {
				Point obstaclePos = {880, 690 + i * 32};
				Obstacle* obstacle = new Obstacle(obstaclePos, ObstacleState::EXIST);
				DC->obstacles.push_back(obstacle);
			}
			for (int i = 0; i <= 4; i++) {
				Point obstaclePos = {1264 + i * 32, 706};
				Obstacle* obstacle = new Obstacle(obstaclePos, ObstacleState::STONE);
				DC->obstacles.push_back(obstacle);
			}
			for (int i = 0; i <= 4; i++) {
				Point obstaclePos = {1264 + i * 32, 706};
				Obstacle* obstacle = new Obstacle(obstaclePos, ObstacleState::STONE);
				DC->obstacles.push_back(obstacle);
			}
			for (int i = 0; i <= 1; i++) {
				Point obstaclePos = {1328, 642 + i * 32};
				Obstacle* obstacle = new Obstacle(obstaclePos, ObstacleState::STONE);
				DC->obstacles.push_back(obstacle);
			}
			for (int i = 3; i <= 4; i++) {
				Point obstaclePos = {1328, 642 + i * 32};
				Obstacle* obstacle = new Obstacle(obstaclePos, ObstacleState::STONE);
				DC->obstacles.push_back(obstacle);
			}
			//right down end
			for (Obstacle* obstacle : DC->obstacles) {
				obstacle->init();
				obstacle->mode = 1;
			}
			obstacle_is_created_at_game = true;
		}
		mode = 1;
		state = STATE::LEVEL;
		break;
	}
	case STATE::LEVEL:
	{
		if (DC->key_state[ALLEGRO_KEY_P] && !DC->prev_key_state[ALLEGRO_KEY_P])
		{
			//SC->toggle_playing(background2);
			debug_log("<Game> state: change to PAUSE\n");
			state = STATE::PAUSE;
		}
		/*if (DC->level->remain_monsters() == 0 && DC->monsters.size() == 0)
		{
			debug_log("<Game> state: change to END\n");
			state = STATE::END;
		}*/
		if (DC->player->HP == 0)
		{
			debug_log("<Game> state: change to END\n");
			state = STATE::END;
		}
		static bool BGM_played = false;
		if (!BGM_played)
		{
			background2 = SC->play(background_sound_path, ALLEGRO_PLAYMODE_LOOP);
			BGM_played = true;
		}
		if (mode == 0) {
			for (Tank* tank : DC->tanks) {
				if (tank->num_gem >= 10) {
					state = STATE::KILL_END; // 一方獲得10個寶石，遊戲結束
				}
			}
		} else {
			for (Tank* tank : DC->tanks) {
				if (tank->get_state() == TankState::DEAD) {
					state = STATE::KILL_END;
				}
			};
			break;
		}
	}
	case STATE::PAUSE:
	{
		//background = IC->get(pause_img_path);
		if (DC->key_state[ALLEGRO_KEY_P] && !DC->prev_key_state[ALLEGRO_KEY_P])
		{
			//SC->toggle_playing(background2);
			//background = IC->get(pause_img_path);
			debug_log("<Game> state: change to LEVEL\n");
			state = STATE::LEVEL;
		}
		if (DC->key_state[ALLEGRO_KEY_R])
		{
			//instance = SC->play(game_start_sound_path, ALLEGRO_PLAYMODE_ONCE);
			//DC->level->load_level(1);
			for (Tank* tank : DC->tanks) {
            	delete tank; // 釋放坦克物件的記憶體
			}
			DC->tanks.clear(); // 清空坦克向量

			// 刪除障礙物
			for (Obstacle* obstacle : DC->obstacles) {
				delete obstacle; // 釋放障礙物物件的記憶體
			}
			DC->obstacles.clear(); // 清空障礙物向量
			debug_log("<Game> state: change to TANK_CHOOSE\n");
			//background = IC->get(modechoose_img_path);
			background = IC->get(white_img_path);
			state = STATE::TANK_CHOOSE;
		}
		else if (DC->key_state[ALLEGRO_KEY_E])
		{
			//instance = SC->play(game_start_sound_path, ALLEGRO_PLAYMODE_ONCE);
			//DC->level->load_level(1);
			debug_log("<Game> state: change to END\n");
			//background = IC->get(background_img_path);
			state = STATE::END;
		}
		break;
	}
	case STATE::END:
	{
		debug_log("<Game> state: change to END\n");
		//state = STATE::CHOOSE;
		return false;
	}
	case STATE::KILL_END: //一方被殺
	{
		for (Tank* tank : DC->tanks) {
            delete tank; // 釋放坦克物件的記憶體
		}
		DC->tanks.clear(); // 清空坦克向量

			// 刪除障礙物
		for (Obstacle* obstacle : DC->obstacles) {
			delete obstacle; // 釋放障礙物物件的記憶體
		}
		DC->obstacles.clear(); // 清空障礙物向量
		background = IC->get(pause_img_path);
		//重來 OR 結束
		if(DC->key_state[ALLEGRO_KEY_R] || (DC->mouse_state[1]) && DC->mouse.x < 800){
			DC->mouse_state[1] = false;
			state = STATE::CHOOSE;
		}
		else if(DC->key_state[ALLEGRO_KEY_E] || (DC->mouse_state[1]) && DC->mouse.x > 800){
			DC->mouse_state[1] = false;
			state = STATE::END;
		}
		break;
	}
	case STATE::TANK_CHOOSE:
	{
    // 處理角色選擇邏輯
		
    	/*for (auto &tank : choose_tank) {
        	// 檢查滑鼠是否懸停於角色縮圖範圍內
        	if (DC->mouse.x >= tank.x && DC->mouse.x <= tank.x + tank.width &&
           		DC->mouse.y >= tank.y && DC->mouse.y <= tank.y + tank.height) {
            	tank.is_hovered = true;  // 滑鼠懸停
        	} else {
            	tank.is_hovered = false; // 滑鼠未懸停
        	}*/

        // 處理玩家選擇角色
        if (DC->mouse_state[1] && !DC->prev_mouse_state[1] /*&& tank.is_hovered*/) {
            if (player_turn == 1) { // 玩家1的回合
                //click = SC->play(click_sound_path, ALLEGRO_PLAYMODE_ONCE);
                //player1_tank = tank; // 記錄玩家1選擇的角色
                //debug_log("<Game> Player 1 selected tank %d\n", tank.number);

                // 啟動玩家1名稱輸入
                name_input_active = true;
                name_input_player = 1;
                player_turn = 2; // 切換到玩家2
            } else if (player_turn == 2 && player1_name_done) { // 玩家2的回合
                //click = SC->play(click_sound_path, ALLEGRO_PLAYMODE_ONCE);
                //player2_tank = tank; // 記錄玩家2選擇的角色
                //debug_log("<Game> Player 2 selected tank %d\n", tank.number);

                // 啟動玩家2名稱輸入
                name_input_active = true;
                name_input_player = 2;
                player_turn = 3; // 所有玩家選擇完成
				//DC->mouse_state[1] = false;
				
            }
        }
    }
	//if(player2_name_done) background = IC->get(white_img_path);
    // 檢查是否按下開始按鈕並完成名稱輸入
    if (player_turn == 3 && !name_input_active && player2_name_done) {
        // 確認角色選擇並初始化遊戲角色
        //apply_character_selection();
		background = IC->get(modechoose_img_path);
        // 輸出玩家名稱
        debug_log("Player 1 Name: %s\n", player1_name.c_str());
        debug_log("Player 2 Name: %s\n", player2_name.c_str());
		if(DC->mouse_state[1]){
			debug_log("<Game> state: change to CHOOSE\n");
			state = STATE::CHOOSE;
		}
    }
	//background = IC->get(modechoose_img_path);
    break;
	}
	
	// If the game is not paused, we should progress update.
	if (state != STATE::PAUSE)
	{
		DC->player->update();
		SC->update();
		ui->update();
		/*-----I2P Revise start-----*/
		// DC->hero->update();
		for (Tank* tank : DC->tanks) tank->update();
		for (auto it = DC->bullets.begin(); it != DC->bullets.end(); ) {
        	(*it)->update();
        	if ((*it)->get_fly_dist() <= 0) it = DC->bullets.erase(it);
        	else ++it;
    	}
		// Maybe we need to update obstacles later
		// for (Obstacle* obstacle : DC->obstacles) {
		// 	if (obstacle->get_state() == ObstacleState::DESTROYED) {
		// 		DC->obstacles.erase(obstacle);
		// 	}
		// }
		/*-----I2P Revise end-----*/
		if (state != STATE::INIT)
		{
			//DC->level->update();
			OC->update();
		}
	}
	// game_update is finished. The states of current frame will be previous states of the next frame.
	memcpy(DC->prev_key_state, DC->key_state, sizeof(DC->key_state));
	memcpy(DC->prev_mouse_state, DC->mouse_state, sizeof(DC->mouse_state));
	return true;
}

/**
 * @brief Draw the whole game and objects.
 */
void Game::game_draw()
{
	DataCenter *DC = DataCenter::get_instance();
	OperationCenter *OC = OperationCenter::get_instance();
	FontCenter *FC = FontCenter::get_instance();

	// Flush the screen first.
	al_clear_to_color(al_map_rgb(100, 100, 100));
	if (state != STATE::END)
	{
		// background
		al_draw_bitmap(background, 0, 0, 0);
		if (DC->game_field_length < DC->window_width)
			al_draw_filled_rectangle(
				DC->game_field_length, 0,
				DC->window_width, DC->window_height,
				al_map_rgb(100, 100, 100));
		if (DC->game_field_length < DC->window_height)
			al_draw_filled_rectangle(
				0, DC->game_field_length,
				DC->window_width, DC->window_height,
				al_map_rgb(100, 100, 100));
		// user interface
		if (state != STATE::INIT /*&& state != STATE::CHOOSE*/)
		{
			//DC->level->draw();
			/*-----I2P Revise start-----*/
			// DC->hero->draw();
			for (Obstacle* obstacle : DC->obstacles) obstacle->draw();
			for (Tank* tank : DC->tanks) tank->draw();
			for (auto &bullet : DC->bullets) {bullet->draw();}
			/*-----I2P Revise end-----*/
			ui->draw();
			OC->draw();
		}
	}
	switch (state)
	{
	case STATE::INIT:
	{
		break;
	}
	case STATE::CHOOSE:
	{
		break;
	}
	case STATE::SCORE:
	{
		break;
	}
	case STATE::KILL:
	{
		break;
	}
	case STATE::LEVEL:
	{
		break;
	}
	case STATE::PAUSE:
	{
		// game layout cover
		al_draw_filled_rectangle(0, 0, DC->window_width, DC->window_height, al_map_rgba(50, 50, 50, 64));
		al_draw_text(
			FC->caviar_dreams[FontSize::LARGE], al_map_rgb(255, 255, 255),
			DC->window_width / 2., DC->window_height / 2.,
			ALLEGRO_ALIGN_CENTRE, "GAME PAUSED");
		break;
	}
	case STATE::END:
	{
		break;
	}
	case STATE::KILL_END:
	{
		break;
	}
	case STATE::TANK_CHOOSE: {
			// 繪製玩家1選擇的角色高亮
            /*for (auto &character : select_character) {

				//選中的高亮留著
				if (player1_character.number == character.number) 
					al_draw_filled_rectangle(character.x, character.y, character.x + character.width, character.y + character.height / 2, al_map_rgba(169, 169, 169, 0.8));
				if (player2_character.number == character.number)
					al_draw_filled_rectangle(character.x, character.y + character.height / 2, character.x + character.width, character.y + character.height, al_map_rgba(247, 247, 1, 0.8));
                //選中的高亮留著

				if (character.is_hovered) {
                    if (player_turn == 1) {
                        // 玩家1高亮 (白色，角色上半部)
                        al_draw_filled_rectangle(character.x, character.y, character.x + character.width, character.y + character.height / 2, al_map_rgba(169, 169, 169, 0.8));
                    } else if (player_turn == 2) {
                        // 玩家2高亮 (灰色，角色下半部)
                        al_draw_filled_rectangle(character.x, character.y + character.height / 2, character.x + character.width, character.y + character.height, al_map_rgba(247, 247, 1, 0.8));
                    }
                    al_draw_bitmap(character.character, character.x, character.y, 0);
                } else {
                    al_draw_bitmap(character.character, character.x, character.y, 0);
                }
            }*/
			//if (player_turn == 3 && player2_name_done) {
			//	al_draw_bitmap(playbtn, play_button.x1, play_button.y1, 0);
			//}

			al_draw_text(
                FC->caviar_dreams[FontSize::LARGE], al_map_rgb(255, 255, 255),
                30, DC->window_height-FontSize::LARGE - 10,
                ALLEGRO_ALIGN_LEFT, "BACK");
			
			if (player_turn == 1 && !name_input_active) {
				al_draw_text(	
                FC->caviar_dreams[FontSize::LARGE], al_map_rgb(0, 0, 0),
                DC->window_width/2, DC->window_height/2,
                ALLEGRO_ALIGN_CENTER, "Player 1 TURN");	
			} else if (player_turn == 2 && !name_input_active) {
				al_draw_text(
                FC->caviar_dreams[FontSize::LARGE], al_map_rgb(0, 0, 0),
                DC->window_width/2, DC->window_height/2,
                ALLEGRO_ALIGN_CENTER, "Player 2 TURN");
			}

			// 如果正在輸入名稱，繪製輸入框和提示文字
    		if (name_input_active) {
        		float box_width = 400;
        		float box_height = 50;

        		// 輸入框位置（正中間）
        		float box_x = DC->window_width / 2 - box_width / 2;
        		float box_y = DC->window_height / 2 - box_height / 2;

        		// 提示文字位置
        		float text_x = DC->window_width / 2;
        		float text_y = box_y - 60;

        		// 繪製背景框
        		al_draw_filled_rectangle(box_x, box_y, box_x + box_width, box_y + box_height, al_map_rgb(0, 0, 0));
        		al_draw_rectangle(box_x, box_y, box_x + box_width, box_y + box_height, al_map_rgb(255, 255, 255), 2);

        		// 繪製提示文字
        		al_draw_text(
            		FC->caviar_dreams[FontSize::LARGE], al_map_rgb(255, 255, 255),
            		text_x, text_y, ALLEGRO_ALIGN_CENTER, "NickName");

        		// 繪製輸入的名稱
        		std::string current_name = (name_input_player == 1) ? player1_name : player2_name;
        		al_draw_text(
            		FC->caviar_dreams[FontSize::LARGE], al_map_rgb(255, 255, 255),
            		DC->window_width / 2, box_y + 10, ALLEGRO_ALIGN_CENTER,
            		current_name.c_str());
				
				al_draw_text(
            		FC->caviar_dreams[FontSize::LARGE], al_map_rgb(0, 0, 0),
            		text_x, box_y + 60, ALLEGRO_ALIGN_CENTER, "PRESS ENTER");
    		}

    		al_draw_text(
        		FC->caviar_dreams[FontSize::LARGE], al_map_rgb(255, 255, 255),
        		30, DC->window_height - FontSize::LARGE - 10,
        		ALLEGRO_ALIGN_LEFT, "BACK");

			break;
		}
	}
	al_flip_display();
}

Game::~Game()
{
	al_destroy_display(display);
	al_destroy_timer(timer);
	al_destroy_event_queue(event_queue);
}