#include <noobgraphics.h>
#include <GL/glew.h>
#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>

static void ng_on_mouse_input(int button, int state, int x, int y);
static void ng_on_mouse_move(int x, int y);
static void ng_on_keyboard_press(unsigned char key, int x, int y);
static void ng_on_keyboard_release(unsigned char key, int x, int y);
static void (*ng_on_update_dt)(int dt);
static void ng_on_update();
static void (*ng_on_render)();
static void ng_on_clear_and_render();
static void ng_on_reshape(int width, int height);
static int ng_init_resources();
static void ng_free_resources();

static int ng_mouse_x;
static int ng_mouse_y;
static int ng_mouse_button;
static int ng_mouse_state;
static unsigned char ng_keyboard_key;
static int ng_keyboard_state;
static int ng_rgba_color;
static int ng_dt;
static int ng_window_width;
static int ng_window_height;
static GLuint ng_program;
static GLint ng_attribute_coord2d;

void ng_init_graphics(int width,
                      int height,
                      const char* title,
                      void (*update_func)(),
                      void (*render_func)())
{
    int argc = 0;
    char** argv = NULL;

    ng_mouse_x = 0;
    ng_mouse_y = 0;
    ng_mouse_button = 0;
    ng_mouse_state = RELEASED;
    ng_keyboard_key = 0;
    ng_keyboard_state = RELEASED;
    ng_rgba_color = 0;
    ng_dt = 0;
    ng_window_width = width;
    ng_window_height = height;

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
    glutInitWindowSize(width, height);
    glutCreateWindow(title);

    GLenum glew_status = glewInit();
    if (glew_status != GLEW_OK)
    {
        fprintf(stderr, "Error: %s\n", glewGetErrorString(glew_status));
        return;
    }

    int result = ng_init_resources();
    if (!result)
        return;

    atexit(ng_free_resources);

    ng_on_update_dt = update_func;
    ng_on_render = render_func;

    glutIdleFunc(ng_on_update);
    glutKeyboardFunc(ng_on_keyboard_press);
    glutKeyboardUpFunc(ng_on_keyboard_release);
    glutMouseFunc(ng_on_mouse_input);
    glutMotionFunc(ng_on_mouse_move);
    glutPassiveMotionFunc(ng_on_mouse_move);
    glutDisplayFunc(ng_on_clear_and_render);
    glutReshapeFunc(ng_on_reshape);

    glClearColor(0.0, 0.0, 0.0, 1.0);

    glutMainLoop();
}

void ng_on_clear_and_render()
{
    glClear(GL_COLOR_BUFFER_BIT);
    ng_on_render();
    glutSwapBuffers();
}

void ng_on_reshape(int width, int height)
{
    ng_window_width = width;
    ng_window_height = height;
    glClearColor(0.0, 0.0, 0.0, 1.0);
}

int ng_init_resources()
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

    ng_program = glCreateProgram();
    glAttachShader(ng_program, vs);
    glAttachShader(ng_program, fs);
    glLinkProgram(ng_program);
    glGetProgramiv(ng_program, GL_LINK_STATUS, &result);
    if (!result) {
        fprintf(stderr, "glLinkProgram failed\n");
        return 0;
    }

    const char* attribute_name = "coord2d";
    ng_attribute_coord2d = glGetAttribLocation(ng_program, attribute_name);
    if (ng_attribute_coord2d == -1) {
        fprintf(stderr, "Could not bind attribute %s\n", attribute_name);
        return 0;
    }

    return 1;
}

void ng_free_resources()
{
    glDeleteProgram(ng_program);
}

void ng_set_color(int rgba_color)
{
    ng_rgba_color = rgba_color;
}

void ng_draw_line(int x0, int y0, int x1, int y1, int width)
{
}

void ng_draw_circle(int x, int y, int radius)
{
}

void ng_draw_square(int x0, int y0, int x1, int y1)
{
}

void ng_get_mouse(int* x, int* y, int* button, int* state)
{
    *x = ng_mouse_x;
    *y = ng_mouse_y;
    *button = ng_mouse_button;
    *state = ng_mouse_state;
}

void ng_get_keyboard(unsigned char* key, int* state)
{
    *key = ng_keyboard_key;
    *state = ng_keyboard_state;
}

void ng_on_mouse_input(int button, int state, int x, int y)
{
    ng_mouse_x = x;
    ng_mouse_y = y;
    ng_mouse_button = button;
    ng_mouse_state = state;
    ng_on_update();
}

void ng_on_mouse_move(int x, int y)
{
    ng_mouse_x = x;
    ng_mouse_y = y;
    ng_on_update();
}

void ng_on_keyboard_press(unsigned char key, int x, int y)
{
    ng_mouse_x = x;
    ng_mouse_y = y;
    ng_keyboard_key = key;
    ng_keyboard_state = PRESSED;
    ng_on_update();
}

void ng_on_keyboard_release(unsigned char key, int x, int y)
{
    ng_mouse_x = x;
    ng_mouse_y = y;
    ng_keyboard_key = key;
    ng_keyboard_state = RELEASED;
    ng_on_update();
}

int ng_get_window_size(int* width, int* height)
{
    *width = ng_window_width;
    *height = ng_window_height;
}

void ng_on_update()
{
    static int time_base = -1;
    int time = glutGet(GLUT_ELAPSED_TIME);
    if (time_base < 0)
        time_base = time;

    ng_on_update_dt(ng_dt);

    ng_dt = time - time_base;
    time_base = time;
}
