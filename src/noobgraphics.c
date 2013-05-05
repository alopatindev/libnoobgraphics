#include <noobgraphics.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

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
static void ng_convert_color(unsigned int rgba_color,
                             GLfloat* r, GLfloat* g, GLfloat* b, GLfloat* a);

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
    ng_rgba_color = -1;
    ng_dt = 0;
    ng_window_width = width;
    ng_window_height = height;

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_ALPHA);
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

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glAlphaFunc(GL_GREATER, 0.01);
    //glEnable(GL_ALPHA_TEST);

    ng_on_reshape(width, height);
    ng_on_clear_and_render();
    glutPostRedisplay();

    ng_set_color(0);
    glutMainLoop();
}

void ng_force_redraw()
{
    glutPostRedisplay();
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
        "  vec2 coords = vec2(coord2d * 2.0) - vec2(1.0, 1.0);"
        "  gl_Position = vec4(coords, 0.0, 1.0);"
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

    GLfloat c[4];
    ng_convert_color(rgba_color, c, c+1, c+2, c+3);

    glUniform4fv(ng_uniform_color, 1, c);
}

void ng_convert_color(unsigned int rgba_color,
                      GLfloat* r, GLfloat* g, GLfloat* b, GLfloat* a)
{
    int ir = rgba_color >> 24;
    int ig = (rgba_color << 8) >> 24;
    int ib = (rgba_color << 16) >> 24;
    int ia = (rgba_color << 24) >> 24;
    *r = ir / 255.0f;
    *g = ig / 255.0f;
    *b = ib / 255.0f;
    *a = ia / 255.0f;
}

void ng_draw_line(int x0, int y0, int x1, int y1, int width)
{
    glUseProgram(ng_program);
    glEnableVertexAttribArray(ng_attribute_coord2d);

    GLfloat ww = (GLfloat)ng_window_width;
    GLfloat wh = (GLfloat)ng_window_height;

    GLfloat x0f = x0 / ww;
    GLfloat y0f = y0 / wh;
    GLfloat x1f = x1 / ww;
    GLfloat y1f = y1 / wh;

    GLfloat verts[] = {
        x0f, y0f,
        x1f, y1f
    };

    glVertexAttribPointer(ng_attribute_coord2d, 2, GL_FLOAT, GL_FALSE, 0, verts);

    glLineWidth((GLfloat)width);
    glDrawArrays(GL_LINES, 0, 2);

    glDisableVertexAttribArray(ng_attribute_coord2d);
}

void ng_draw_rectangle(int x0, int y0, int x1, int y1)
{
    glUseProgram(ng_program);
    glEnableVertexAttribArray(ng_attribute_coord2d);

    GLfloat ww = (GLfloat)ng_window_width;
    GLfloat wh = (GLfloat)ng_window_height;

    GLfloat x0f = x0 / ww;
    GLfloat y0f = y0 / wh;
    GLfloat x1f = x1 / ww;
    GLfloat y1f = y1 / wh;

    GLfloat verts[] = {
        x0f, y0f,
        x0f, y1f,
        x1f, y1f,
        x1f, y0f,
    };

    glVertexAttribPointer(ng_attribute_coord2d, 2, GL_FLOAT, GL_FALSE, 0, verts);

    glDrawArrays(GL_QUADS, 0, 4);
    glDisableVertexAttribArray(ng_attribute_coord2d);
}

void ng_draw_text(int x, int y, const char* text)
{
    GLfloat r, g, b, a;
    void* font = GLUT_BITMAP_9_BY_15;
    glRasterPos2f(x, y);
    size_t len, i;
    len = (size_t) strlen(text);
    for (i = 0; i < len; i++)
        glutBitmapCharacter(font, text[i]);
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
    ng_keyboard_state = RELEASED;
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
