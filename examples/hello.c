#include <noobgraphics.h>

void on_update()
{
}

void on_render()
{
}

int main()
{
    nb_init_graphics(800, 600, "Hi, there!", on_update, on_render);
    while (1)
    {
        nb_set_color(0x00FF00FF);
        nb_draw_square(30, 30, 400, 300);
        nb_set_color(0xFF0000FF);
        nb_draw_line(0, 0, 400, 300, 30);
    }
    nb_destroy_graphics();
    return 0;
}