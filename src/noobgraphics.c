#include <noobgraphics.h>
#include <GL/glew.h>
#include <GL/glut.h>

void nb_init_graphics(int width,
                      int height,
                      const char* title,
                      void (*update_func)(),
                      void (*render_func)())
{
    int argc = 0;
    char** argv = NULL;

    nb_mouse_x = 0;
    nb_mouse_y = 0;
    nb_mouse_button = 0;
    nb_mouse_state = RELEASED;
    nb_keyboard_key = 0;
    nb_keyboard_state = RELEASED;

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
    glutInitWindowSize(width, height);
    glutCreateWindow(title);

    nb_on_update = update_func;
    glutIdleFunc(nb_on_update);
    glutKeyboardFunc(nb_on_keyboard_press);
    glutKeyboardUpFunc(nb_on_keyboard_release);
    glutMouseFunc(nb_on_mouse_input);
    glutDisplayFunc(render_func);

    glutMainLoop();
}

void nb_set_color(int rgba_color)
{
}

void nb_draw_line(int x0, int y0, int x1, int y1, int width)
{
}

void nb_draw_circle(int x, int y, int radius)
{
}

void nb_draw_square(int x0, int y0, int x1, int y1)
{
}

void nb_get_mouse(int* x, int* y, int* button, int* state)
{
    *x = nb_mouse_x;
    *y = nb_mouse_y;
    *button = nb_mouse_button;
    *state = nb_mouse_state;
}

void nb_get_keyboard(unsigned char* key, int* state)
{
    *key = nb_keyboard_key;
    *state = nb_keyboard_state;
}

void nb_on_mouse_input(int button, int state, int x, int y)
{
    nb_mouse_x = x;
    nb_mouse_y = y;
    nb_mouse_button = button;
    nb_mouse_state = state;
    nb_on_update();
}

void nb_on_keyboard_press(unsigned char key, int x, int y)
{
    nb_mouse_x = x;
    nb_mouse_y = y;
    nb_keyboard_key = key;
    nb_keyboard_state = PRESSED;
    nb_on_update();
}

void nb_on_keyboard_release(unsigned char key, int x, int y)
{
    nb_mouse_x = x;
    nb_mouse_y = y;
    nb_keyboard_key = key;
    nb_keyboard_state = RELEASED;
    nb_on_update();
}
