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
constexpr char game_start_sound_path[] = "./assets/sound/growl.wav";
constexpr char background_img_path[] = "./assets/image/background.jpg";
constexpr char initial_img_path[] = "./assets/image/initial.jpg";
constexpr char modechoose_img_path[] = "./assets/image/ModeChoose.jpg";
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
	// DC->hero->init();
	// 創建兩台坦克並初始化
	ControlScheme player1Controls = {ALLEGRO_KEY_W, ALLEGRO_KEY_LEFT};
	ControlScheme player2Controls = {ALLEGRO_KEY_UP, ALLEGRO_KEY_A};
	Point tank1Pos = {20, 450};
	Point tank2Pos = {1535, 450};
    Tank* tank1 = new Tank(tank1Pos, player1Controls);
    Tank* tank2 = new Tank(tank2Pos, player2Controls);
    

    // 將坦克加入 DataCenter 的 tanks 向量
    DC->tanks.push_back(tank1);
    DC->tanks.push_back(tank2);

	// Create obstacles
	for (int i = 3; i <= 50; i++) {
		for (int j = 0; j <= DC->window_height; j += 30) {
			Point obstaclePos = {i * 30, j};
			Obstacle* obstacle = new Obstacle(obstaclePos);
			DC->obstacles.push_back(obstacle);
		}
	}


	for (Tank* tank : DC->tanks) tank->init();
	for (Obstacle* obstacle : DC->obstacles) obstacle->init();

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

	switch (state)
	{
	case STATE::INIT:
	{
		static ALLEGRO_SAMPLE_INSTANCE *instance = nullptr;
		if (DC->mouse_state[1])
		{
			instance = SC->play(game_start_sound_path, ALLEGRO_PLAYMODE_ONCE);
			//DC->level->load_level(1);
			debug_log("<Game> state: change to CHOOSE\n");
			background = IC->get(modechoose_img_path);
			state = STATE::CHOOSE;
			DC->mouse_state[1] = false;
		}
		else
		{
			instance = SC->play(game_start_sound_path, ALLEGRO_PLAYMODE_ONCE);
		}
		break;
	}
	case STATE::CHOOSE:
	{
		static ALLEGRO_SAMPLE_INSTANCE *instance = nullptr;
		if (DC->mouse_state[1] && DC->mouse.x < 800)
		{
			instance = SC->play(game_start_sound_path, ALLEGRO_PLAYMODE_ONCE);
			//DC->level->load_level(1);
			debug_log("<Game> state: change to SCORE\n");
			background = IC->get(background_img_path);
			state = STATE::SCORE;
		}
		else if (DC->mouse_state[1] && DC->mouse.x > 800)
		{
			instance = SC->play(game_start_sound_path, ALLEGRO_PLAYMODE_ONCE);
			//DC->level->load_level(1);
			debug_log("<Game> state: change to KILL\n");
			background = IC->get(background_img_path);
			state = STATE::KILL;
		}
		else 
		{
			instance = SC->play(game_start_sound_path, ALLEGRO_PLAYMODE_ONCE);
		}
		break;
	}
	case STATE::SCORE:
	{
		state = STATE::LEVEL;
	}
	case STATE::KILL:
	{
		state = STATE::LEVEL;
	}
	case STATE::LEVEL:
	{
		if (DC->key_state[ALLEGRO_KEY_P] && !DC->prev_key_state[ALLEGRO_KEY_P])
		{
			SC->toggle_playing(background2);
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
		for (Tank* tank : DC->tanks) {
			if (tank->get_state() == TankState::DEAD) {
				state = STATE::END;
			}
		};
		break;
	}
	case STATE::PAUSE:
	{
		if (DC->key_state[ALLEGRO_KEY_P] && !DC->prev_key_state[ALLEGRO_KEY_P])
		{
			SC->toggle_playing(background2);
			debug_log("<Game> state: change to LEVEL\n");
			state = STATE::LEVEL;
		}
		break;
	}
	case STATE::END:
	{
		debug_log("<Game> state: change to END\n");
		return false;
	}
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
		if (state != STATE::INIT && state != STATE::CHOOSE)
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
	}
	case STATE::CHOOSE:
	{

	}
	case STATE::SCORE:
	{

	}
	case STATE::KILL:
	{

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