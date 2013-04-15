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
    ng_set_color(0xFF0000FF);
    ng_draw_rectangle(30, 10, 400, 300);
    ng_set_color(0x00AA0055);
    ng_draw_rectangle(30, 30, 500, 50);

    ng_set_color(0x00FF00FF);
    ng_draw_line(0, 0, 400, 300, 30);

    ng_draw_text(5, 5, "asdASD asdasddsgdfsg");
}

int main()
{
    ng_init_graphics(800, 600, "Hi, there!", on_update, on_render);
    return 0;
}
