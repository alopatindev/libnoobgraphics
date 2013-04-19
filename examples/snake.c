#include <noobgraphics.h>
#include <stdio.h>
#include <string.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

#define FIELD_WIDTH 10
#define FIELD_HEIGHT 15

#define CELL_WIDTH (WINDOW_HEIGHT / FIELD_HEIGHT)
#define CELL_DELIMETER 5

int field[FIELD_WIDTH][FIELD_HEIGHT];

void update_keyboard();
void on_update(int dt);
void render_mainmenu();
void render_game();
void on_render();

void turn_left();
void turn_right();
void turn_top();
void turn_bottom();

int main();

enum GameState
{
    MainMenu,
    Game
} game_state = MainMenu;

void update_keyboard()
{
    unsigned char key;
    int kstate;
    ng_get_keyboard(&key, &kstate);

    switch (game_state)
    {
    case MainMenu:
        {
            if (key == 13) // Enter
            {
                game_state = Game;
                ng_force_redraw();
            }
            break;
        }
    case Game:
        {
            switch (key)
            {
            case 'a':
                turn_left();
                break;
            case 'd':
                turn_right();
                break;
            case 'w':
                turn_top();
                break;
            case 's':
                turn_bottom();
                break;
            default:
                break;
            }
            break;
        }
    }
}

void on_update(int dt)
{
    update_keyboard();
}

void render_mainmenu()
{
    // FIXME: wtf am i doing here, lol? fix ng_draw_text
    ng_set_color(0x00FF00FF);
    ng_draw_line(-WINDOW_WIDTH / 2, 0, 800, 30, 3);
    ng_set_color(0x00FFFFFF);
    ng_draw_text(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2, "Press Enter to start");
}

void render_game()
{
    int x, y;
    const int xoffset = WINDOW_WIDTH / 2 - (CELL_WIDTH * FIELD_WIDTH) / 2;

    for (x = 0; x < FIELD_WIDTH; ++x)
    {
        for (y = 0; y < FIELD_HEIGHT; ++y)
        {
            ng_set_color(0x000055FF);
            ng_draw_rectangle(x * CELL_WIDTH + xoffset,
                              y * CELL_WIDTH,
                              (x + 1) * CELL_WIDTH - CELL_DELIMETER + xoffset,
                              (y + 1) * CELL_WIDTH - CELL_DELIMETER);
        }
    }
}

void on_render()
{
    switch (game_state)
    {
    case MainMenu:
        render_mainmenu();
        break;
    case Game:
        render_game();
        break;
    }
}

void turn_left()
{
}

void turn_right()
{
}

void turn_top()
{
}

void turn_bottom()
{
}

int main()
{
    memset(field, 0, sizeof(field));
    ng_init_graphics(WINDOW_WIDTH, WINDOW_HEIGHT, "Snake", on_update, on_render);
    return 0;
}
