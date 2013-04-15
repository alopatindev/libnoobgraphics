#include <noobgraphics.h>
#include <GL/glew.h>
#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>

static void nb_on_mouse_input(int button, int state, int x, int y);
static void nb_on_mouse_move(int x, int y);
static void nb_on_keyboard_press(unsigned char key, int x, int y);
static void nb_on_keyboard_release(unsigned char key, int x, int y);
static void (*nb_on_update_dt)(int dt);
static void nb_on_update();
static int nb_init_resources();
static void nb_free_resources();

static int nb_mouse_x;
static int nb_mouse_y;
static int nb_mouse_button;
static int nb_mouse_state;
static unsigned char nb_keyboard_key;
static int nb_keyboard_state;
static int nb_rgba_color;
static int nb_dt;
static GLuint nb_program;

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
    nb_rgba_color = 0;
    nb_dt = 0;

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
    glutInitWindowSize(width, height);
    glutCreateWindow(title);

    atexit(nb_free_resources);

    GLenum glew_status = glewInit();
    if (glew_status != GLEW_OK)
    {
        fprintf(stderr, "Error: %s\n", glewGetErrorString(glew_status));
        return;
    }

    int result = nb_init_resources();

    nb_on_update_dt = update_func;
    glutIdleFunc(nb_on_update);
    glutKeyboardFunc(nb_on_keyboard_press);
    glutKeyboardUpFunc(nb_on_keyboard_release);
    glutMouseFunc(nb_on_mouse_input);
    glutMotionFunc(nb_on_mouse_move);
    glutPassiveMotionFunc(nb_on_mouse_move);
    glutDisplayFunc(render_func);

    glutMainLoop();
}

int nb_init_resources()
{
    GLint result = GL_FALSE;

    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    const char *vs_source =
        //"#version 120\n"  // OpenGL 2.1
        "attribute vec2 coord2d;                  "
        "void main(void) {                        "
        "  gl_Position = vec4(coord2d, 0.0, 1.0); "
        "}";
    glShaderSource(vs, 1, &vs_source, NULL);
    glCompileShader(vs);
    glGetShaderiv(vs, GL_COMPILE_STATUS, &result);
    if (!result)
    {
        fprintf(stderr, "Error in vertex shader\n");
        return 0;
    }
    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    const char *fs_source =
        "#version 120           \n"
        "void main(void) {        "
        "  gl_FragColor[0] = 0.0; "
        "  gl_FragColor[1] = 0.0; "
        "  gl_FragColor[2] = 1.0; "
        "}";
    glShaderSource(fs, 1, &fs_source, NULL);
    glCompileShader(fs);
    glGetShaderiv(fs, GL_COMPILE_STATUS, &result);
    if (!result) {
        fprintf(stderr, "Error in fragment shader\n");
        return 0;
    }

    nb_program = glCreateProgram();
    glAttachShader(nb_program, vs);
    glAttachShader(nb_program, fs);
    glLinkProgram(nb_program);
    glGetProgramiv(nb_program, GL_LINK_STATUS, &result);
    if (!result) {
        fprintf(stderr, "glLinkProgram failed\n");
        return 0;
    }
}

void nb_free_resources()
{
    glDeleteProgram(nb_program);
}

void nb_set_color(int rgba_color)
{
    nb_rgba_color = rgba_color;
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

void nb_on_mouse_move(int x, int y)
{
    nb_mouse_x = x;
    nb_mouse_y = y;
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

void nb_on_update()
{
    static int time_base = -1;
    int time = glutGet(GLUT_ELAPSED_TIME);
    if (time_base < 0)
        time_base = time;

    nb_on_update_dt(nb_dt);

    nb_dt = time - time_base;
    time_base = time;
}
