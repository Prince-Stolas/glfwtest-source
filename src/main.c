#define _USE_MATH_DEFINES
#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <time.h>

#include <glfw/glfw3.h>

#include "tiny_obj_c.h"
#include "stb_image.h"

#define MAX_COINS 50

GLFWmonitor* monitor;
GLFWvidmode* mode;
GLFWwindow* window;

struct gamestate {
    int line;
    float jump_start;
    bool jump;
    bool duck;
};

struct gamestate state;

bool left = false;

void error_callback(int error, const char* description) {
    fprintf(stderr, "Error: %s\n", description);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    static bool fullscreen = 0;
    if (action == GLFW_PRESS) switch (key) {
        case GLFW_KEY_F11:
            fullscreen = !fullscreen;
            if (fullscreen) glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
            else glfwSetWindowMonitor(window, NULL, mode->width/2-320, mode->height/2-240, 640, 480, 0);
            break;
        case GLFW_KEY_A:
            if (state.line > 0) state.line--;
            break;
        case GLFW_KEY_D:
            if (state.line < 2) state.line++;
            break;
        case GLFW_KEY_SPACE:
            if (!state.jump && !state.duck) {
                state.jump = 1;
                state.jump_start = (float)glfwGetTime();
            }
            break;
        case GLFW_KEY_LEFT_SHIFT:
            if (!state.jump && !state.duck) state.duck = 1;
            break;
        case GLFW_MOUSE_BUTTON_LEFT:
            left = true;
            printf("gay");
            break;
    }
    else if (action == GLFW_RELEASE) switch (key) {
        case GLFW_KEY_LEFT_SHIFT:
            state.duck = 0;
            break;
    }
}

void mouse_callback(GLFWwindow* window, int key, int action, int mods) {
    if (action == GLFW_PRESS && key == GLFW_MOUSE_BUTTON_LEFT) left = true;
}

int main() {
    srand(time(NULL));

    tobj_scene_f yaris, coin;
    tobj_load_config cfg = tobj_default_config();   // triangulate = true
    tobj_diag diag = {0};

    stbi_set_flip_vertically_on_load(1);

    if (tobj_load_obj_from_file_f(&yaris, "assets/yaris.obj", &cfg, &diag) != TOBJ_OK) {
        if (diag.err) fprintf(stderr, "%s\n", diag.err);
        return 1;
    }

    if (tobj_load_obj_from_file_f(&coin, "assets/coin.obj", &cfg, &diag) != TOBJ_OK) {
        if (diag.err) fprintf(stderr, "%s\n", diag.err);
        return 1;
    }




    state = (struct gamestate){
        .line = 1,
        .jump_start = 0,
        .jump = 0,
        .duck = 0
    };

    struct coin {
        int line;
        float zoffs;
        bool high;
    };

    struct coin coins[MAX_COINS] = {{
        .line = 0,
        .zoffs = 0,
        .high = 0
    }};
    int coin_i = 0;




    if (!glfwInit())
        glfwTerminate();

    glfwSetErrorCallback(error_callback);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 1);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_ANY_PROFILE);

    monitor = glfwGetPrimaryMonitor();
    mode = glfwGetVideoMode(monitor);

    //glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    window = glfwCreateWindow(mode->width, mode->height, "W Game", monitor, NULL);
    if (!window)
        glfwTerminate();

    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_callback);


    unsigned int textures[3];
    glGenTextures(3, textures);

    int twidth, theight, nrChannels;
    unsigned char *data;

    //tex 1
    glBindTexture(GL_TEXTURE_2D, textures[0]);
    data = stbi_load("assets/tex.png", &twidth, &theight, &nrChannels, 4);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);


    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, twidth, theight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    } else {
        fprintf(stderr, "%s\n", diag.err);
        return 1;
    }

    stbi_image_free(data);

    //tex 2
    glBindTexture(GL_TEXTURE_2D, textures[1]);
    data = stbi_load("assets/road.jpg", &twidth, &theight, &nrChannels, 4);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);


    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, twidth, theight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    } else {
        fprintf(stderr, "%s\n", diag.err);
        return 1;
    }

    stbi_image_free(data);

    //tex 2
    glBindTexture(GL_TEXTURE_2D, textures[2]);
    data = stbi_load("assets/grass.jpg", &twidth, &theight, &nrChannels, 4);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);


    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, twidth, theight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    } else {
        fprintf(stderr, "%s\n", diag.err);
        return 1;
    }

    stbi_image_free(data);
    

    glClearColor(0, 0.8f, 1, 1);

    glEnable(GL_NORMALIZE);
    glEnable(GL_FOG);


    float fog_color[] = { 0, 0.8f, 1.0f };
    glFogi(GL_FOG_MODE, GL_LINEAR);
    glFogfv(GL_FOG_COLOR, fog_color);
    glFogf(GL_FOG_START, 15);
    glFogf(GL_FOG_END, 50);

    GLfloat min_lighting[] = {0.5, 0.5, 0.5, 1.0f};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, min_lighting);

    float light_col1[] = { 1.0f, 0, 0 };
    float light_col2[] = { 0, 0, 1.0f };
    float light_col3[] = { 0, 1.0f, 0 };
    float light_col4[] = { 1.0f, 1.0f, 1.0f };
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_col1);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, light_col2);
    glLightfv(GL_LIGHT2, GL_DIFFUSE, light_col3);
    glLightfv(GL_LIGHT3, GL_DIFFUSE, light_col4);

    float light_position[] = { 1.0f, 1.0f, 1.0f, 0.0f };
    glLightfv(GL_LIGHT3, GL_POSITION, light_position);


    

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    int width, height;
    float ratio;

    float dt = 0;
    float lastTime = 0;

    float fovDeg = 60;
    float fovRad = fovDeg * (M_PI / 180.0f);

    float dtc = 0;

    float coin_spawn_timer = 0.2;



    float click_start = 0;
    int clicks;



    while (!glfwWindowShouldClose(window)) {
        float time = (float)glfwGetTime();
        dt = time-lastTime;
        lastTime = time;

        dtc += dt;

        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);
        ratio = (float)width / (float)height;


        if (left) clicks++;
        if (left&&click_start==0) click_start = time;
        left = 0;

        //printf("clicks: %d\nt-cs: %f\n", clicks, time-click_start);

        float cps = clicks/(time-click_start);

        char* buffer = malloc(255);
        snprintf(buffer, sizeof(buffer), "%f", cps);
        glfwSetWindowTitle(window, buffer);






        coin_spawn_timer -= dt;

        if (coin_spawn_timer<=0 && coin_i < MAX_COINS-1) {
            if (rand()%3==0)
                coins[coin_i++] = (struct coin){
                    .line = rand()%3,
                    .high = rand()%5==0,
                    .zoffs = 45
                };
            coin_spawn_timer = 0.2;
        }

        for (int i=0;i<coin_i;i++) coins[i].zoffs-=0.3f;

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        double top, bottom, left, right;
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();

        top = tanf(fovRad/2.0f);
        bottom = -top;
        right = top*ratio;
        left = -right;

        glFrustum(left, right, bottom, top, 1, 100);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glRotatef(dtc*100, 0, 1, 0);
        float light_position1[] = { 2.0f, 1.0f, 2.0f, 0.0f };
        float light_position2[] = { -2.0f, 1.0f, 2.0f, 0.0f };
        float light_position3[] = { 0, 1.0f, -2.0f, 0.0f };
        glLightfv(GL_LIGHT0, GL_POSITION, light_position1);
        glLightfv(GL_LIGHT1, GL_POSITION, light_position2);
        glLightfv(GL_LIGHT2, GL_POSITION, light_position3);

        glLoadIdentity();

        glRotatef(atanf(0.3f)*(180.0f/M_PI), 1, 0, 0);
        glTranslatef(0, -5, -7);

        //glRotatef(dtc*20, 0, -1, 0);
        glRotatef(180, 0, -1, 0);

        glPushMatrix();
        //glTranslatef(0, 1, 0);
        //glRotatef(dtc*300, 1, 0, 0);
        //glTranslatef(0, -1, 0);

        float jump_height = 2*sinf((time-state.jump_start)*5);
        if (jump_height<=0 && time-state.jump_start != 0) state.jump = 0;

        glTranslatef(2-state.line*2, (state.jump?jump_height:0)-(state.duck?0:0), 0); //line + jump + duck

        glRotatef(90, 1, 0, 0);
        glScalef(.2*cosf(dtc*10)+1, 1, 1.2-.2*cosf(dtc*10)); //dance
        glScalef(state.duck?1.5:1, 1, state.duck?0.5:1); //duck


        glDisable(GL_LIGHT3);
        glEnable(GL_LIGHT0);
        glEnable(GL_LIGHT1);
        glEnable(GL_LIGHT2);
        glEnable(GL_LIGHTING);
        glEnable(GL_TEXTURE_2D);

        glBindTexture(GL_TEXTURE_2D, textures[0]);
        glBegin(GL_TRIANGLES);
            for (size_t s = 0; s < yaris.num_shapes; s++) {
              const tobj_mesh_f *mesh = &yaris.shapes[s].mesh;
              for (size_t i = 0; i < mesh->num_indices; i++) {
                tobj_index idx = mesh->indices[i];

                if (idx.normal_index >= 0) {
                    float nx = yaris.attrib.normals.ptr[3 * idx.normal_index + 0];
                    float ny = yaris.attrib.normals.ptr[3 * idx.normal_index + 1];
                    float nz = yaris.attrib.normals.ptr[3 * idx.normal_index + 2];
                    glNormal3f(nx, ny, nz);
                }

                float x = yaris.attrib.vertices.ptr[3 * idx.vertex_index + 0];
                float y = yaris.attrib.vertices.ptr[3 * idx.vertex_index + 1];
                float z = yaris.attrib.vertices.ptr[3 * idx.vertex_index + 2];

                if (idx.texcoord_index >= 0) {
                    float u = yaris.attrib.texcoords.ptr[2 * idx.texcoord_index + 0];
                    float v = yaris.attrib.texcoords.ptr[2 * idx.texcoord_index + 1];

                    glTexCoord2f(u, v);
                }

                //glColor3f((i%3==0)?1:0, (i%3==1)?1:0, (i%3==2)?1:0);
                glVertex3f(x, y, z);
              }
            }
        glEnd();


        glPopMatrix();
        glPushMatrix();

        glEnable(GL_LIGHT3);
        glDisable(GL_LIGHT0);
        glDisable(GL_LIGHT1);
        glDisable(GL_LIGHT2);
        glDisable(GL_TEXTURE_2D);
        glEnable(GL_COLOR_MATERIAL);
        glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

        for (int i=0;i<coin_i;i++) {
            glPopMatrix();
            glPushMatrix();

            glTranslatef(2-coins[i].line*2, 1+coins[i].high, coins[i].zoffs);
            glScalef(15, 15, 15);
            glRotatef(fmodf(dtc*100, 360), 0, 1, 0);
            glRotatef(90, 1, 0, 0);
            
            glBegin(GL_TRIANGLES);
                for (size_t s = 0; s < coin.num_shapes; s++) {
                  const tobj_mesh_f *mesh = &coin.shapes[s].mesh;
                  for (size_t i = 0; i < mesh->num_indices; i++) {
                    tobj_index idx = mesh->indices[i];

                    if (idx.normal_index >= 0) {
                        float nx = coin.attrib.normals.ptr[3 * idx.normal_index + 0];
                        float ny = coin.attrib.normals.ptr[3 * idx.normal_index + 1];
                        float nz = coin.attrib.normals.ptr[3 * idx.normal_index + 2];
                        glNormal3f(nx, ny, nz);
                    }

                    float x = coin.attrib.vertices.ptr[3 * idx.vertex_index + 0];
                    float y = coin.attrib.vertices.ptr[3 * idx.vertex_index + 1];
                    float z = coin.attrib.vertices.ptr[3 * idx.vertex_index + 2];

                    glColor3f(1, 1, 0);
                    glVertex3f(x, y, z);
                  }
                }
            glEnd();
        }
        
        glColor3f(1, 1, 1);



        glPopMatrix();
        glPushMatrix();

        glRotatef(90, 0, 1, 0);
        glScalef(3, 1, 3);
        glTranslatef(fmodf(dtc*10, 2), 0, 0);

        glDisable(GL_LIGHTING);
        glDisable(GL_COLOR_MATERIAL);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, textures[1]);
        glBegin(GL_TRIANGLES);
            glTexCoord2f(0, 0);
            glVertex3f(-100, 0, -1);
            glTexCoord2f(200, 1);
            glVertex3f(100, 0, 1);
            glTexCoord2f(200, 0);
            glVertex3f(100, 0, -1);

            glTexCoord2f(200, 1);
            glVertex3f(100, 0, 1);
            glTexCoord2f(0, 0);
            glVertex3f(-100, 0, -1);
            glTexCoord2f(0, 1);
            glVertex3f(-100, 0, 1);
        glEnd();

        glPopMatrix();
        
        glScalef(3, 1, 3);
        glTranslatef(0, 0, -fmodf(dtc*10, 2));

        glBindTexture(GL_TEXTURE_2D, textures[2]);
        glBegin(GL_TRIANGLES);
            glTexCoord2f(0, 0);
            glVertex3f(-50, -0.01f, -50);
            glTexCoord2f(100, 100);
            glVertex3f(50, -0.01f, 50);
            glTexCoord2f(100, 0);
            glVertex3f(50, -0.01f, -50);

            glTexCoord2f(100, 100);
            glVertex3f(50, -0.01f, 50);
            glTexCoord2f(0, 0);
            glVertex3f(-50, -0.01f, -50);
            glTexCoord2f(0, 100);
            glVertex3f(-50, -0.01f, 50);
        glEnd();


        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();

    return 0;
}