#include <assert.h>
#include "../budgie/oop.h"
#include "aapp.h"

#include <stdbool.h>

#include "raylib.h"

#include "raymath.h"

#include "rlgl.h"
#define RLIGHTS_IMPLEMENTATION
#include "rlights.h"

#if defined(PLATFORM_DESKTOP)
    #define GLSL_VERSION            330
#else   // PLATFORM_ANDROID, PLATFORM_WEB
    #define GLSL_VERSION            100
#endif

#include <stdlib.h>
#include <omp.h>

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include "camera.h"

#include "../budgie/precision.h"


int SCREEN_WIDTH;
int SCREEN_HEIGHT;
float HALF_SCREEN_WIDTH;
float HALF_SCREEN_HEIGHT;

// keep the physics fixed time in step with the render frame
// rate which we don't know in advance
static float frameTime = 0.0;
static buReal physics_duration = 0.0;
static const buReal physics_delta = 1.0 / 600.0;
static const int max_physics_steps = 100;
static Camera3D camera = {0};
static Light lights[MAX_LIGHTS] = { 0 };
static Shader shader = {0};

static const char *getTitle(Application *self) {
    return "title";
}

static void init(Application *self) {
}

static void initGraphics(Application *self) {
    printf("initGraphics:enter\n");
    bool showWireframe = false;
    size_t frameCounter = 0;
    size_t CELL_SIZE = 50;
    printf("Linked Raylib version: %s\n", RAYLIB_VERSION);
    const int glslVer = rlGetVersion();
    printf("GL version: %i\n", glslVer);

    //SetConfigFlags(FLAG_FULLSCREEN_MODE);
    InitWindow(1600, 1200, INSTANCE_METHOD(self, getTitle));

    // Get the primary monitor's resolution before window creation
    int monitor = GetCurrentMonitor();
    SCREEN_HEIGHT = GetScreenHeight();
    SCREEN_WIDTH = GetScreenWidth();
    printf("Monitor %d: %d x %d\n", GetCurrentMonitor(), GetMonitorWidth(monitor), GetMonitorHeight(monitor));
    HALF_SCREEN_HEIGHT = SCREEN_HEIGHT/2.0f;
    HALF_SCREEN_WIDTH = SCREEN_WIDTH/2.0f;

    SetTargetFPS(60);

    INSTANCE_METHOD(self, setView);

    // Load basic lighting shader
    shader = LoadShader("../src/demos/lighting.vs","../src/demos/lighting.fs");
    shader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(shader, "viewPos");
    // NOTE: "matModel" location name is automatically assigned on shader loading, 
    // no need to get the location again if using that uniform name
    
    // Ambient light level (some basic lighting)
    int ambientLoc = GetShaderLocation(shader, "ambient");
    SetShaderValue(shader, ambientLoc, (float[4]){ 0.1f, 0.1f, 0.1f, 1.0f }, SHADER_UNIFORM_VEC4);

    // Create lights
    lights[0] = CreateLight(LIGHT_POINT, (Vector3){ -HALF_SCREEN_WIDTH, 200, -HALF_SCREEN_HEIGHT }, Vector3Zero(), YELLOW, shader);
    lights[1] = CreateLight(LIGHT_POINT, (Vector3){ HALF_SCREEN_WIDTH, 200, HALF_SCREEN_HEIGHT }, Vector3Zero(), RED, shader);
    lights[2] = CreateLight(LIGHT_POINT, (Vector3){ -HALF_SCREEN_WIDTH, 200, HALF_SCREEN_HEIGHT }, Vector3Zero(), GREEN, shader);
    lights[3] = CreateLight(LIGHT_POINT, (Vector3){ HALF_SCREEN_WIDTH, 200, -HALF_SCREEN_HEIGHT }, Vector3Zero(), BLUE, shader);
    printf("initGraphics:leave\n");
}

static void setView(Application *self) {
    camera.position = (Vector3){ 0.0f, 0.0f, 0.0f};  // Positioned out along +Z axis
    camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };       // Looking at the quad at origin
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };           // Standard up direction
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;
}

static void loop(Application *self) {
    printf("loop:enter\n");

    while (!WindowShouldClose()) {

        frameTime = GetFrameTime();
        int physics_steps = 0;
        physics_duration = GetTime(); 
        while (frameTime > physics_delta) {
            INSTANCE_METHOD(self, update, physics_delta);
            frameTime -= physics_delta;
            physics_steps++;
            if (physics_steps > max_physics_steps) {
                frameTime = 0;
                break;      
            }
        }
        physics_duration = GetTime() - physics_duration;  

        // Update camera
        UpdateCameraManual(&camera);
        // Update the shader with the camera view vector (points towards { 0.0f, 0.0f, 0.0f })
        float cameraPos[3] = { camera.position.x, camera.position.y, camera.position.z };
        SetShaderValue(shader, shader.locs[SHADER_LOC_VECTOR_VIEW], cameraPos, SHADER_UNIFORM_VEC3);

        // Check key inputs to enable/disable lights
        if (IsKeyPressed(KEY_Y)) { lights[0].enabled = !lights[0].enabled; }
        if (IsKeyPressed(KEY_R)) { lights[1].enabled = !lights[1].enabled; }
        if (IsKeyPressed(KEY_G)) { lights[2].enabled = !lights[2].enabled; }
        if (IsKeyPressed(KEY_B)) { lights[3].enabled = !lights[3].enabled; }
        
        // Update light values (actually, only enable/disable them)
        for (int i = 0; i < MAX_LIGHTS; i++) UpdateLightValues(shader, lights[i]);
        size_t Y = 10, d = 60;
        BeginDrawing();
            ClearBackground(RAYWHITE);

            BeginMode3D(camera);

                rlSetMatrixProjection(MatrixPerspective(
                    DEG2RAD * camera.fovy,
                    (float)SCREEN_WIDTH / SCREEN_HEIGHT,
                    10.0f,     // near clip
                    10000.0f   // far clip
                ));
                
                BeginShaderMode(shader);
                    DrawGrid(1000, 10.0f);
                    KeyboardKey key = GetKeyPressed();
                    INSTANCE_METHOD(self, keyboard, key);
                    if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) INSTANCE_METHOD(self, mouseButtonPressed, MOUSE_LEFT_BUTTON);
                    INSTANCE_METHOD(self, display);
                EndShaderMode();

                // Draw spheres to show where the lights are
                for (int i = 0; i < MAX_LIGHTS; i++) {
                    if (lights[i].enabled) DrawSphereEx(lights[i].position, 10.0f, 8, 8, lights[i].color);
                    else DrawSphereWires(lights[i].position, 10.0f, 8, 8, ColorAlpha(lights[i].color, 0.3f));
                }
            
            EndMode3D();

            DrawText(TextFormat("physics_steps: %d", physics_steps), 20, Y, 30, BLUE);
            DrawText(TextFormat("physics_duration: %f", physics_duration), 20, Y + d, 30, BLUE);
            DrawText(TextFormat("Current Resolution: %d x %d", SCREEN_WIDTH, SCREEN_HEIGHT), 20, Y + 2*d, 30, BLUE);
            DrawText(TextFormat("Frame Time: %0.2f ms", GetFrameTime() * 1000), 20, Y + 3*d, 30, BLUE);
            DrawFPS(GetScreenWidth() - 100, 10);
            INSTANCE_METHOD(self, display_info, Y + 4*d, d);
        EndDrawing();
    }
    CloseWindow();
    printf("loop:leave\n");
}

static void display(Application *self) {

}

static void display_info(Application *self, size_t Y, size_t d) {

}

static void deinit(Application *self) {

}

static void keyboard(Application *self, KeyboardKey key) {

}

static void mouseButtonPressed(Application *self, MouseButton button) {

}

static void update(Application *self, buReal physics_delta) {

}


// all objects of same class have the same singlton vtable object
static ApplicationVTable application_vtable = {
        .getTitle = getTitle,
        .initGraphics = initGraphics,
        .setView = setView,
        .init = init,
        .deinit = deinit,
        .loop = loop,
        .display = display,
        .display_info = display_info,
        .keyboard = keyboard,
        .mouseButtonPressed = mouseButtonPressed,
        .update = update
};

// new object
static Application *new_instance(const ApplicationClass *cls) {
    printf("new_instance: enter %p\n", cls);
    Application *app= malloc(sizeof(Application));
    assert(app);
    app->klass = cls;
    printf("new_instance: leave\n");
    return app;
}

// free object
void application_free_instance(ApplicationClass *cls, Application *self) {
    free(self);
}

const ApplicationClass *create_child_class(const ApplicationClass *cls, const char *name) {
    ApplicationClass *child = malloc(sizeof(ApplicationClass));
    ApplicationVTable *vtable = malloc(sizeof(ApplicationVTable));
    *vtable = application_vtable;
    child->vtable = vtable;
    size_t length = strlen(name);
    child->class_name = malloc((length+1)*sizeof(char));
    assert(child->class_name);
    strcpy(child->class_name, name);
    child->parent = cls;
    child->new_instance = new_instance;
    child->free = application_free_instance;
    child->create_child_class = create_child_class;
    printf("create_child_class: cls: %p\n", cls);
    printf("create_child_class: child: %p\n", child);
    return child;
}


ApplicationClass applicationClass = {
    .class_name = "Application",
    .vtable = &application_vtable,
    .new_instance = new_instance,
    .free = application_free_instance,
    .create_child_class = create_child_class,
    .parent = NULL
};

