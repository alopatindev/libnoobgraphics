#include <noobgraphics.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

#define FIELD_WIDTH 10
#define FIELD_HEIGHT 15
#define FIGURE_WIDTH 4

#define FIGURE_TYPES_NUMBER (sizeof(figure_types) / sizeof(figure_types[0]))

#define CELL_WIDTH (WINDOW_HEIGHT / FIELD_HEIGHT)
#define CELL_DELIMETER 5

#define CELL_NONE_COLOR 0x000055FF
#define CELL_BRICK_COLOR 0x990000FF
#define CELL_WALL_COLOR 0x006600FF
#define GAME_STEP_SPEED_MS 500

#define TRUE 1
#define FALSE 0

enum CellType
{
    None,
    Brick,
    Wall
};

static enum CellType figure_types[][FIGURE_WIDTH][FIGURE_WIDTH] = {
    {
        {0,0,0,0},
        {0,1,0,0},
        {0,1,0,0},
        {0,1,1,0}
    },
    {
        {0,1,0,0},
        {0,1,0,0},
        {0,1,0,0},
        {0,1,0,0}
    },
    {
        {0,0,1,0},
        {0,1,1,0},
        {0,1,0,0},
        {0,0,0,0}
    }
};

enum RotationType
{
    Initial,
    DontRotate,
    ClockWise,
    CounterClockWise
};

struct Figure
{
    int type;
    enum RotationType rotation;
    int x;
    int y;
    enum CellType cells[FIGURE_WIDTH][FIGURE_WIDTH];
} figure;

enum CellType field[FIELD_WIDTH][FIELD_HEIGHT];

enum Direction
{
    Down,
    Left,
    Right
};

int is_figure_collided(enum Direction direction)
{
    int xf, yf;
    for (xf = 0; xf < FIGURE_WIDTH; ++xf)
    {
        for (yf = 0; yf < FIGURE_WIDTH; ++yf)
        {
            if (figure.cells[xf][yf] == Brick)
            {
                switch (direction)
                {
                case Down:
                    if (figure.y + yf + 1 >= FIELD_HEIGHT ||
                        field[figure.x + xf][figure.y + yf + 1] == Wall)
                        return TRUE;
                    break;
                case Left:
                    if (figure.x + xf - 1 < 0)
                        return TRUE;
                    if (field[figure.x + xf - 1][figure.y + yf] == Wall)
                        return TRUE;
                    break;
                case Right:
                    if (figure.x + xf + 1 >= FIELD_WIDTH)
                        return TRUE;
                    if (field[figure.x + xf + 1][figure.y + yf] == Wall)
                        return TRUE;
                    break;
                }
            }
        }
    }

    return FALSE;
}

int can_rotate(enum CellType new_cells[FIGURE_WIDTH][FIGURE_WIDTH])
{
    int xf, yf;

    for (xf = 0; xf < FIGURE_WIDTH; ++xf)
    {
        for (yf = 0; yf < FIGURE_WIDTH; ++yf)
        {
            if (new_cells[xf][yf] == Brick &&
                    (
                        figure.x + xf - 1 < 0 ||
                        figure.x + xf + 1 >= FIELD_WIDTH ||
                        field[figure.x + xf][figure.y + yf] != None
                    )
               )
               return FALSE;
        }
    }
    return TRUE;
}

void update_figure(int x, int y, int type, enum RotationType rotation, int force)
{
    figure.x = x;
    figure.y = y;
    
    int xf, yf;
    static enum CellType new_cells[FIGURE_WIDTH][FIGURE_WIDTH];

    if (force || type != figure.type || rotation != figure.rotation)
    {
        figure.type = type;
        figure.rotation = rotation;

        switch (rotation)
        {
        case Initial:
            for (xf = 0; xf < FIGURE_WIDTH; ++xf)
            {
                for (yf = 0; yf < FIGURE_WIDTH; ++yf)
                    new_cells[xf][yf] = figure_types[type][yf][xf];
            }
            break;
        case ClockWise:
            {
                static enum CellType cells[FIGURE_WIDTH][FIGURE_WIDTH];
                for (xf = 0; xf < FIGURE_WIDTH; ++xf)
                {
                    for (yf = 0; yf < FIGURE_WIDTH; ++yf)
                        cells[xf][yf] = figure.cells[xf][yf];
                }
                for (xf = 0; xf < FIGURE_WIDTH; ++xf)
                {
                    for (yf = 0; yf < FIGURE_WIDTH; ++yf)
                        new_cells[xf][yf] = cells[yf][FIGURE_WIDTH-xf-1];
                }
                break;
            }
        case CounterClockWise:
            {
                static enum CellType cells[FIGURE_WIDTH][FIGURE_WIDTH];
                for (xf = 0; xf < FIGURE_WIDTH; ++xf)
                {
                    for (yf = 0; yf < FIGURE_WIDTH; ++yf)
                        cells[xf][yf] = figure.cells[xf][yf];
                }
                for (xf = 0; xf < FIGURE_WIDTH; ++xf)
                {
                    for (yf = 0; yf < FIGURE_WIDTH; ++yf)
                        new_cells[xf][yf] = cells[FIGURE_WIDTH-yf-1][xf];
                }
                break;
            }
        case DontRotate:
            break;
        }
    }

    figure.rotation = DontRotate;

    // removing current floating bricks
    for (xf = 0; xf < FIELD_WIDTH; ++xf)
    {
        for (yf = 0; yf < FIELD_HEIGHT; ++yf)
        {
            if (field[xf][yf] == Brick)
                field[xf][yf] = None;
        }
    }

    // checking if we can rotate at all
    int can_rot = can_rotate(new_cells);

    // finally update field
    for (xf = 0; xf < FIGURE_WIDTH; ++xf)
    {
        for (yf = 0; yf < FIGURE_WIDTH; ++yf)
        {
            enum CellType *c = &field[xf + x][yf + y];
            if (*c == None)
            {
                if (can_rot)
                    figure.cells[xf][yf] = new_cells[xf][yf];
                *c = figure.cells[xf][yf];
            }
        }
    }
}

void rotate_clockwise()
{
    update_figure(figure.x, figure.y, figure.type, ClockWise, FALSE);
}

void rotate_counterclockwise()
{
    update_figure(figure.x, figure.y, figure.type, CounterClockWise, FALSE);
}

void set_next_figure()
{
    // current bricks convert into the wall
    int xf, yf;
    for (xf = 0; xf < FIELD_WIDTH; ++xf)
    {
        for (yf = 0; yf < FIELD_HEIGHT; ++yf)
        {
            if (field[xf][yf] == Brick)
                field[xf][yf] = Wall;
        }
    }

    // randomizing next figure
    // FIXME
    update_figure(FIELD_WIDTH / 2 - FIGURE_WIDTH / 2,
                  0,
                  rand() % FIGURE_TYPES_NUMBER,
                  Initial,
                  TRUE);
    int lr = rand() % 2;
    int rr = rand() % 2;
    int i;
    if (lr > 0)
    {
        for (i = 0; i < lr; ++i)
            rotate_clockwise();
    } else if (rr > 0) {
        for (i = 0; i < rr; ++i)
            rotate_counterclockwise();
    }
}

void destroy_game()
{
}

void init_game()
{
    atexit(destroy_game);
    memset(field, None, sizeof(field));
    memset(&figure, 0, sizeof(figure));
    srand(time(NULL));
    set_next_figure();
}

void render_cell(int x, int y, unsigned int color)
{
    static const int xoffset = WINDOW_WIDTH / 2 - (CELL_WIDTH * FIELD_WIDTH) / 2;
    y = FIELD_HEIGHT - y - 1;
    ng_set_color(color);
    ng_draw_rectangle(x * CELL_WIDTH + xoffset,
                      y * CELL_WIDTH,
                      (x + 1) * CELL_WIDTH - CELL_DELIMETER + xoffset,
                      (y + 1) * CELL_WIDTH - CELL_DELIMETER);
}

void on_render()
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
            case Brick:
                color = CELL_BRICK_COLOR;
                break;
            case Wall:
                color = CELL_WALL_COLOR;
                break;
            }
            render_cell(x, y, color);
        }
    }
}

void go(enum Direction direction)
{
    if (is_figure_collided(direction))
    {
        if (direction == Down)
            set_next_figure();
    }
    else
    {
        switch (direction)
        {
        case Down:
            update_figure(figure.x, figure.y+1, figure.type, DontRotate, TRUE);
            break;
        case Left:
            update_figure(figure.x-1, figure.y, figure.type, DontRotate, TRUE);
            break;
        case Right:
            update_figure(figure.x+1, figure.y, figure.type, DontRotate, TRUE);
            break;
        }
    }

    ng_force_redraw();
}

void update_keyboard()
{
    unsigned char key;
    int kstate;
    ng_get_keyboard(&key, &kstate);

    if (kstate != PRESSED)
        return;

    switch (key)
    {
    case 'g':
    case 'w':
        rotate_clockwise();
        ng_force_redraw();
        break;
    case 'h':
        rotate_counterclockwise();
        ng_force_redraw();
        break;
    case 's':
        go(Down);
        break;
    case 'a':
        go(Left);
        break;
    case 'd':
        go(Right);
        break;
    }
}

void on_update(int dt)
{
    update_keyboard();

    static int figure_timer = 0;
    figure_timer += dt;
    if (figure_timer >= GAME_STEP_SPEED_MS)
    {
        figure_timer = 0;
        go(Down);
    }
}

int main()
{
    init_game();
    ng_init_graphics(WINDOW_WIDTH, WINDOW_HEIGHT,
                     "Tetris", on_update, on_render);
    return 0;
}
