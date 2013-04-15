#include <noobgraphics.h>
#include <stdio.h>

void on_update(int dt)
{
    /*int x, y, button, state;
    ng_get_mouse(&x, &y, &button, &state);
    printf("%d %d %d %d\n", x, y, button, state);*/
}

void on_render()
{
    ng_set_color(0xff0000FF);
    ng_draw_square(30, 30, 400, 300);
    ng_set_color(0x00aa0055);
    ng_draw_square(30, 30, 500, 50);

    ng_set_color(0x00ff00FF);
    ng_draw_line(0, 0, 400, 300, 30);
}

int main()
{
    ng_init_graphics(800, 600, "Hi, there!", on_update, on_render);
    return 0;
}
