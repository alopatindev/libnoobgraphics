#ifndef NOOBGRAPHICS_H
#define NOOBGRAPHICS_H

#define MOUSE_LEFT_BUTTON GLUT_LEFT_BUTTON
#define MOUSE_RIGHT_BUTTON GLUT_RIGHT_BUTTON
#define MOUSE_MIDDLE_BUTTON GLUT_MIDDLE_BUTTON

#define RELEASED GLUT_UP
#define PRESSED GLUT_DOWN

#include <GL/glew.h>
#include <GL/glut.h>

void ng_init_graphics(int width,
                      int height,
                      const char* title,
                      void (*update_func)(int),
                      void (*render_func)());

void ng_force_redraw();

void ng_set_color(unsigned int rgba_color);

void ng_draw_line(int x0, int y0, int x1, int y1, int width);
void ng_draw_rectangle(int x0, int y0, int x1, int y1);
void ng_draw_text(int x, int y, const char* text);

void ng_get_mouse(int* x, int* y, int* button, int* state);
void ng_get_keyboard(unsigned char* key, int* state);
int ng_get_window_size(int* width, int* height);

#endif
