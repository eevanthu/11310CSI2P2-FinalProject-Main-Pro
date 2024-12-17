#ifndef GAME_H_INCLUDED
#define GAME_H_INCLUDED

#include <allegro5/allegro.h>
#include "UI.h"

/**
 * @brief Main class that runs the whole game.
 * @details All game procedures must be processed through this class.
 */
class Game
{
public:
	void execute();
public:
	Game();
	~Game();
	void game_init();
	bool game_update();
	void game_draw();
private:
	/**
	 * @brief States of the game process in game_update.
	 * @see Game::game_update()
	 */
	enum class STATE {
		INIT, // -> LEVEL
		CHOOSE, //CHOOSE MODE
		SCORE, //SCORE MODE
		KILL, //KILL MODE
		LEVEL, // -> PAUSE, END
		PAUSE, // -> LEVEL
		END,
		KILL_END, // 一方被殺，選離開or重開
		TANK_CHOOSE // 選坦克+名字
	};
	STATE state;
	ALLEGRO_EVENT event;
	ALLEGRO_BITMAP *game_icon;
	ALLEGRO_BITMAP *background;
private:

struct ChooseTank {     // 選角色
        ALLEGRO_BITMAP *tank;  // 圖片
        double x, y;                   // 圖片的顯示位置
        double width, height;          // 圖片的寬度和高度
        bool is_hovered;            // 判斷是否被滑鼠懸停
        int number;
        int choosed = 0;
    };
    // 選角色
    std::vector<ChooseTank> choose_tank;

    ChooseTank player1_tank, player2_tank;
    int player_turn = 1;   

    std::string player1_name = ""; // 玩家一名稱
    std::string player2_name = ""; // 玩家二名稱
    bool name_input_active = false; // 是否正在輸入名稱
    int name_input_player = 0; // 當前輸入名稱的玩家 (1 或 2)
    bool player1_name_done = false; // 玩家一名稱是否完成輸入
    bool player2_name_done = false; // 玩家一名稱是否完成輸入
    int typeChar;
	ALLEGRO_DISPLAY *display;
	ALLEGRO_TIMER *timer;
	ALLEGRO_EVENT_QUEUE *event_queue;
	UI *ui;
};

#endif
