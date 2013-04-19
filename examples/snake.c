#include <noobgraphics.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

#define FIELD_WIDTH 10
#define FIELD_HEIGHT 15

#define CELL_WIDTH (WINDOW_HEIGHT / FIELD_HEIGHT)
#define CELL_DELIMETER 5

#define CELL_NONE_COLOR 0x000055FF
#define CELL_SNAKE_COLOR 0x990000FF
#define CELL_FOOD_COLOR 0x006600FF
#define SNAKE_STEP_SPEED_MS 500

enum GameState
{
    MainMenu,
    Game
} game_state = MainMenu;

enum CellType
{
    None,
    Snake,
    Food,
    Wall
};

enum CellType field[FIELD_WIDTH][FIELD_HEIGHT];

enum Direction
{
    Left,
    Right,
    Top,
    Bottom
};

struct SnakeCell
{
    int x;
    int y;
    struct SnakeCell* next;
};

struct Snake
{
    struct SnakeCell* head;
    struct SnakeCell* tail;
    enum Direction direction;
} snake;

void update_keyboard();
void on_update(int dt);
void render_mainmenu();
void render_game();
void render_cell(int x, int y, unsigned int color);
void render_field();
void on_render();

void init_game();
void destroy_game();
void turn_snake(enum Direction direction);
void update_snake();
enum CellType get_snake_collision();
struct SnakeCell* alloc_cell();

int main();

void update_keyboard()
{
    unsigned char key;
    int kstate;
    ng_get_keyboard(&key, &kstate);

    if (kstate != PRESSED)
        return;

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
                turn_snake(Left);
                break;
            case 'd':
                turn_snake(Right);
                break;
            case 'w':
                turn_snake(Top);
                break;
            case 's':
                turn_snake(Bottom);
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

    static int snake_timer = 0;
    snake_timer += dt;
    if (snake_timer >= SNAKE_STEP_SPEED_MS)
    {
        update_snake();
        snake_timer = 0;
    }
}

void render_mainmenu()
{
    // FIXME: wtf am i doing here, lol? fix ng_draw_text
    ng_set_color(0x00FF00FF);
    ng_draw_line(-WINDOW_WIDTH / 2, 0, 800, 30, 3);
    ng_set_color(0x00FFFFFF);
    ng_draw_text(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2, "Press Enter to start");
}

void render_field()
{
    int x, y;

    for (x = 0; x < FIELD_WIDTH; ++x)
    {
        for (y = 0; y < FIELD_HEIGHT; ++y)
        {
            unsigned int color;
            switch (field[x][y])
            {
            case None:
                color = CELL_NONE_COLOR;
                break;
            case Snake:
                color = CELL_SNAKE_COLOR;
                break;
            case Food:
                color = CELL_FOOD_COLOR;
                break;
            }
            render_cell(x, y, color);
        }
    }
}

void render_cell(int x, int y, unsigned int color)
{
    static const int xoffset = WINDOW_WIDTH / 2 - (CELL_WIDTH * FIELD_WIDTH) / 2;
    ng_set_color(color);
    ng_draw_rectangle(x * CELL_WIDTH + xoffset,
                      y * CELL_WIDTH,
                      (x + 1) * CELL_WIDTH - CELL_DELIMETER + xoffset,
                      (y + 1) * CELL_WIDTH - CELL_DELIMETER);
}

void render_game()
{
    render_field();
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

void turn_snake(enum Direction direction)
{
    snake.direction = direction;
}

enum CellType get_snake_collision()
{
    enum CellType t;

    int x_next = snake.head->x;
    int y_next = snake.head->y;

    switch (snake.direction)
    {
    case Left:
        x_next--;
        if (x_next < 0)
            return Wall;
        break;
    case Right:
        x_next++;
        if (x_next >= FIELD_WIDTH)
            return Wall;
        break;
    case Top:
        y_next++;
        if (y_next >= FIELD_HEIGHT)
            return Wall;
        break;
    case Bottom:
        y_next--;
        if (y_next < 0)
            return Wall;
        break;
    }

    t = field[x_next][y_next];

    switch (t)
    {
    case Food:
        return Food;
    case Snake: // beating myself
        return Wall;
    }

    return None;
}

void update_snake()
{
    enum CellType ct = get_snake_collision();
    switch (ct)
    {
    case Wall:
        game_state = MainMenu;
        ng_force_redraw();
        return;
    case Food:
//        return;
    case None:
        {
            field[snake.tail->x][snake.tail->y] = None;

            // replace tail with head
            if (snake.head != snake.tail)
            {
                int x = snake.head->x;
                int y = snake.head->y;
                snake.head->next = snake.tail;
                snake.head = snake.head->next;
                snake.head->next = NULL;
                snake.tail = snake.tail->next;
                snake.head->x = x;
                snake.head->y = y;
            }

            switch (snake.direction)
            {
            case Left:
                snake.head->x--;
                break;
            case Right:
                snake.head->x++;
                break;
            case Top:
                snake.head->y++;
                break;
            case Bottom:
                snake.head->y--;
            }

            field[snake.head->x][snake.head->y] = Snake;
        }
    }

    ng_force_redraw();
}

struct SnakeCell* alloc_cell(int x, int y)
{
    struct SnakeCell* c = (struct SnakeCell*) malloc(sizeof(struct SnakeCell));
    c->x = x;
    c->y = y;
    c->next = NULL;
    return c;
}

void init_game()
{
    atexit(destroy_game);
    memset(field, 0, sizeof(field));
    snake.direction = Right;
    snake.head = alloc_cell(1, 1);
    snake.tail = snake.head;
    snake.tail->next = snake.head;

    field[4][4] = Food;
}

void destroy_game()
{
    if (snake.head == snake.tail && snake.head != NULL)
        free(snake.head);
    else
    {
        while (snake.tail)
        {
            struct SnakeCell* c = snake.tail->next;
            free(snake.tail);
            snake.tail = c;
        }
    }
}

int main()
{
    init_game();
    ng_init_graphics(WINDOW_WIDTH, WINDOW_HEIGHT, "Snake", on_update, on_render);
    return 0;
}
