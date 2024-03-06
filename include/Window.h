#pragma once

#include "Camera.h"
#include "Cube.h"
#include "Shader.h"
#include "core.h"
#include "DOF.h"
#include "Joint.h"
#include "Skeleton.h"
#include "Skin.h"
#include "Cloth.h"
#include "Fluid.h"
#include "AnimationClip.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

class Window {
public:
    // Window Properties
    static int width;
    static int height;
    static const char* windowTitle;

    // Objects to render
    static Cube* cube;
    static Skeleton* skeleton;
    static Skin* skin;
    static AnimationClip* animation;
    static Cloth* cloth;
    static Fluid* fluid;

    static char* filename;
    static char* skinFilename;
    static char* animFilename;

    // static bool drawSkeleton;
    // static bool drawSkin;

    // Shader Program
    static GLuint shaderProgram;

    // Act as Constructors and desctructors
    static bool initializeProgram();
    static bool initializeObjects();
    static void cleanUp();

    // for the Window
    static GLFWwindow* createWindow(int width, int height);
    static void resizeCallback(GLFWwindow* window, int width, int height);

    // update and draw functions
    static void idleCallback();
    static void displayCallback(GLFWwindow*);

    // helper to reset the camera
    static void resetCamera();

    // callbacks - for interaction
    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void mouse_callback(GLFWwindow* window, int button, int action, int mods);
    static void cursor_callback(GLFWwindow* window, double currX, double currY);
};