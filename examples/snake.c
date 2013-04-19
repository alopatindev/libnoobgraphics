#include <noobgraphics.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

#define FIELD_WIDTH 10
#define FIELD_HEIGHT 15

#define CELL_WIDTH (WINDOW_HEIGHT / FIELD_HEIGHT)
#define CELL_DELIMETER 5

#define CELL_NONE_COLOR 0x000055FF
#define CELL_SNAKE_COLOR 0x990000FF
#define CELL_FOOD_COLOR 0x006600FF
#define SNAKE_STEP_SPEED_MS 300

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
    struct SnakeCell* prev;
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
void render_cell(int x, int y, unsigned int color);
void render_field();
void on_render();

void init_game();
void destroy_game();
void turn_snake(enum Direction direction);
void update_snake();
enum CellType get_snake_collision(int* x_next, int* y_next);
struct SnakeCell* alloc_cell();
void make_food();

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
//                ng_force_redraw();
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

void on_render()
{
    switch (game_state)
    {
    case MainMenu:
        render_mainmenu();
        break;
    case Game:
        render_field();
        break;
    }
}

void turn_snake(enum Direction direction)
{
    if ((snake.direction == Left && direction == Right) ||
        (snake.direction == Right && direction == Left) ||
        (snake.direction == Top && direction == Bottom) ||
        (snake.direction == Bottom && direction == Top))
        return;
    snake.direction = direction;
}

enum CellType get_snake_collision(int* x_next, int* y_next)
{
    enum CellType t;

    *x_next = snake.head->x;
    *y_next = snake.head->y;

    switch (snake.direction)
    {
    case Left:
        (*x_next)--;
        if (*x_next < 0)
            return Wall;
        break;
    case Right:
        (*x_next)++;
        if (*x_next >= FIELD_WIDTH)
            return Wall;
        break;
    case Top:
        (*y_next)++;
        if (*y_next >= FIELD_HEIGHT)
            return Wall;
        break;
    case Bottom:
        (*y_next)--;
        if (*y_next < 0)
            return Wall;
        break;
    }

    t = field[*x_next][*y_next];

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
    int x_next, y_next;
    enum CellType ct = get_snake_collision(&x_next, &y_next);

    switch (ct)
    {
    case Wall:
        game_state = MainMenu;
        destroy_game();
        init_game();
        break;
    case Food:
        {
            struct SnakeCell* c = alloc_cell(x_next, y_next);
            snake.head->next = c;
            c->prev = snake.head;
            snake.head = c;

            field[snake.head->x][snake.head->y] = Snake;

            make_food();
            break;
        }
    case None:
        {
            field[snake.tail->x][snake.tail->y] = None;

            // replace tail with head
            struct SnakeCell* new_tail = snake.tail->next;
            snake.head->next = snake.tail;
            snake.tail->prev = snake.head;
            snake.tail->next = NULL;
            snake.head = snake.tail;
            snake.tail = new_tail;
            snake.tail->prev = NULL;

            snake.head->x = x_next;
            snake.head->y = y_next;

            field[snake.head->x][snake.head->y] = Snake;

            break;
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
    c->prev = NULL;
    return c;
}

void make_food()
{
    int x, y;

    do
    {
        x = rand() % FIELD_WIDTH;
        y = rand() % FIELD_HEIGHT;
    } while (field[x][y] != None);

    field[x][y] = Food;
}

void init_game()
{
    atexit(destroy_game);

    memset(field, None, sizeof(field));
    snake.direction = Right;

    snake.head = alloc_cell(1, FIELD_HEIGHT - 2);
    snake.tail = alloc_cell(0, FIELD_HEIGHT - 2);
    snake.head->prev = snake.tail;
    snake.tail->next = snake.head;

    srand(time(NULL));
    make_food();
}

void destroy_game()
{
    if (snake.head == snake.tail && snake.head != NULL)
    {
        free(snake.head);
    }
    else
    {
        while (snake.tail)
        {
            struct SnakeCell* c = snake.tail->next;
            free(snake.tail);
            snake.tail = c;
        }
    }

    snake.head = NULL;
    snake.tail = NULL;
}

int main()
{
    init_game();
    ng_init_graphics(WINDOW_WIDTH, WINDOW_HEIGHT, "Snake", on_update, on_render);
    return 0;
}
