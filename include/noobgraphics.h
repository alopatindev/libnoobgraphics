#ifndef NOOBGRAPHICS_H
#define NOOBGRAPHICS_H

#define MOUSE_LEFT_BUTTON GLUT_LEFT_BUTTON
#define MOUSE_RIGHT_BUTTON GLUT_RIGHT_BUTTON
#define MOUSE_MIDDLE_BUTTON GLUT_MIDDLE_BUTTON

#define RELEASED GLUT_UP
#define PRESSED GLUT_DOWN

void nb_init_graphics(int width,
                      int height,
                      const char* title,
                      void (*update_func)(),
                      void (*render_func)());

void nb_set_color(int rgba_color);

void nb_draw_line(int x0, int y0, int x1, int y1, int width);
void nb_draw_circle(int x, int y, int radius);
void nb_draw_square(int x0, int y0, int x1, int y1);
void nb_draw_text(int x, int y, const char* text); // TODO: size?

void nb_get_mouse(int* x, int* y, int* button, int* state);
void nb_get_keyboard(unsigned char* key, int* state);

static void nb_on_mouse_input(int button, int state, int x, int y);
static void nb_on_keyboard_press(unsigned char key, int x, int y);
static void nb_on_keyboard_release(unsigned char key, int x, int y);
static void (*nb_on_update)();

static int nb_mouse_x;
static int nb_mouse_y;
static int nb_mouse_button;
static int nb_mouse_state;
static unsigned char nb_keyboard_key;
static int nb_keyboard_state;

// TODO: pause, dt?

#endif
