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
static void ng_log_shader(const char* tag, GLuint i);

static int ng_mouse_x;
static int ng_mouse_y;
static int ng_mouse_button;
static int ng_mouse_state;
static unsigned char ng_keyboard_key;
static int ng_keyboard_state;
static unsigned int ng_rgba_color;
static int ng_dt;
static int ng_window_width;
static int ng_window_height;
static GLuint ng_program;
static GLint ng_attribute_coord2d;
static GLint ng_uniform_color;

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

    ng_on_reshape(width, height);
    ng_on_clear_and_render();
    glutPostRedisplay();

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
    if (width <= 0) width = 1;
    if (height <= 0) height = 1;
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
        "attribute vec2 coord2d;"
        "void main(void) {"
        "  gl_Position = vec4(coord2d, 0.0, 1.0);"
        "}";
    glShaderSource(vs, 1, &vs_source, NULL);
    glCompileShader(vs);
    glGetShaderiv(vs, GL_COMPILE_STATUS, &result);
    if (!result)
    {
        ng_log_shader("vertex shader", vs);
        return 0;
    }

    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    const char *fs_source =
        //"#version 120\n"
        "uniform vec4 color;"
        "void main(void) {"
        "  gl_FragColor = color;"
        "}";

    glShaderSource(fs, 1, &fs_source, NULL);
    glCompileShader(fs);
    glGetShaderiv(fs, GL_COMPILE_STATUS, &result);
    if (!result)
    {
        ng_log_shader("fragment shader", fs);
        return 0;
    }

    ng_program = glCreateProgram();
    glAttachShader(ng_program, vs);
    glAttachShader(ng_program, fs);
    glLinkProgram(ng_program);
    glGetProgramiv(ng_program, GL_LINK_STATUS, &result);
    if (!result)
    {
        fprintf(stderr, "glLinkProgram failed\n");
        return 0;
    }

    ng_attribute_coord2d = glGetAttribLocation(ng_program, "coord2d");
    ng_uniform_color = glGetUniformLocation(ng_program, "color");
    if (ng_attribute_coord2d == -1 || ng_uniform_color == -1)
    {
        fprintf(stderr, "shader variables issue\n");
        return 0;
    }

    return 1;
}

void ng_free_resources()
{
    glDeleteProgram(ng_program);
}

void ng_log_shader(const char* tag, GLuint i)
{
    static const GLsizei MAXLEN = 1 << 12;
    GLsizei len = 0;
    char log[MAXLEN];
    glGetShaderInfoLog(i, MAXLEN, &len, log);
    if (len != 0)
        fprintf(stderr, "%s: %s\n", tag, log);
}

void ng_set_color(unsigned int rgba_color)
{
    if (ng_rgba_color == rgba_color)
        return;

    ng_rgba_color = rgba_color;

    int r = rgba_color >> 24;
    int g = (rgba_color << 8) >> 24;
    int b = (rgba_color << 16) >> 24;
    int a = (rgba_color << 24) >> 24;
    GLfloat c[] = {
        r / 255.0f,
        g / 255.0f,
        b / 255.0f,
        a / 255.0f
    };

    glUniform4fv(ng_uniform_color, 1, c);
}

void ng_draw_line(int x0, int y0, int x1, int y1, int width)
{
}

void ng_draw_circle(int x, int y, int radius)
{
}

void ng_draw_square(int x0, int y0, int x1, int y1)
{
    glUseProgram(ng_program);
    glEnableVertexAttribArray(ng_attribute_coord2d);
    GLfloat verts[] = {
        0.0, 0.0,
        0.0, 1.0,
        1.0, 1.0,
        1.0, 0.0,
    };
    /*float x0f = x0 / ng_window_width;
    float y0f = y0 / ng_window_height;
    float x1f = x1 / ng_window_width;
    float y1f = y1 / ng_window_height;
    GLfloat verts[] = {
        x0f, y0f,
        x0f, y1f,
        x1f, y1f,
        x1f, y0f,
    };*/
    glVertexAttribPointer(
        ng_attribute_coord2d, // attribute
        2,                    // number of elements per vertex, here (x,y)
        GL_FLOAT,             // the type of each element
        GL_FALSE,             // take our values as-is
        0,                    // no extra data between each position
        verts                 // pointer to the C array
    );

    glDrawArrays(GL_QUADS, 0, 4);
    glDisableVertexAttribArray(ng_attribute_coord2d);
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
