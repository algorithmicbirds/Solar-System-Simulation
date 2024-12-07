#include <glad/glad.h>
#include <iostream>
#include <GLFW/glfw3.h>
#include <window_manager.h>
#include <globals.h>

float deltaTime2 = 0.0f;
WindowManager::WindowManager(int width, int height, Camera *cam)
    : window(nullptr), camera(cam), firstMouse(true), lastX(width / 2.0),
      lastY(height / 2.0), windowedWidth(width), windowedHeight(height),
      isFullscreen(false) {
  initWindow(width, height);
}

WindowManager::~WindowManager() { glfwTerminate(); }

void WindowManager::initWindow(int width, int height) {
  if (!glfwInit()) {
    std::cerr << "Failed to initialize GLFW" << std::endl;
    exit(-1);
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  window =
      glfwCreateWindow(width, height, "Solar System Simulation", NULL, NULL);
  if (window == nullptr) {
    std::cerr << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    exit(-1);
  }

  glfwMakeContextCurrent(window);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cerr << "Failed to initialize GLAD" << std::endl;
    exit(-1);
  }


  camera->setAspectRatio((float)width / (float)height);
  
  glfwSetWindowUserPointer(window, this);
}

void key_callback(GLFWwindow *window, int key, int scancode, int action,
                  int mods);

void WindowManager::setupCallbacks() {

  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  glfwSetCursorPosCallback(window, mouse_callback);
  glfwSetKeyCallback(window, key_callback); 
  glfwSetInputMode(window, GLFW_CURSOR,
                   GLFW_CURSOR_DISABLED); 
}


void WindowManager::processInput(float deltaTime) {
 
  bool w = glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS;
  bool s = glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS;
  bool a = glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS;
  bool d = glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS;
  bool space = glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS;
  bool shift = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS;

  camera->processKeyboardInput(deltaTime, w, s, a, d, space, shift);

  // Close the window if the escape key is pressed
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, GLFW_TRUE);
}

bool WindowManager::shouldClose() { return glfwWindowShouldClose(window); }

void WindowManager::swapBuffers() { glfwSwapBuffers(window); }

void WindowManager::pollEvents() { glfwPollEvents(); }

// GLFW framebuffer size callback function
void WindowManager::framebuffer_size_callback(GLFWwindow *window, int width,
                                              int height) {
  glViewport(0, 0, width, height);
  WindowManager *wm =
      static_cast<WindowManager *>(glfwGetWindowUserPointer(window));
  wm->camera->setAspectRatio((float)width / (float)height);
}

// GLFW mouse callback function for FPS-style camera
void WindowManager::mouse_callback(GLFWwindow *window, double xpos,
                                   double ypos) {
  WindowManager *wm =
      static_cast<WindowManager *>(glfwGetWindowUserPointer(window));

  // Handle the first mouse movement for initialization
  if (wm->firstMouse) {
    wm->lastX = xpos;
    wm->lastY = ypos;
    wm->firstMouse = false;
  }

  // Calculate the mouse movement offsets
  float xoffset = xpos - wm->lastX;
  float yoffset = wm->lastY - ypos; // Invert y-axis to match FPS style

 
  wm->camera->processMouseMovement(xoffset, yoffset);

  // Update the last mouse positions
  wm->lastX = xpos;
  wm->lastY = ypos;
}

void key_callback(GLFWwindow *window, int key, int scancode, int action,
                  int mods) {
  WindowManager *wm =
      static_cast<WindowManager *>(glfwGetWindowUserPointer(window));

  if (action == GLFW_PRESS || action == GLFW_REPEAT) {
    bool w = glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS;
    bool s = glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS;
    bool a = glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS;
    bool d = glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS;
    bool space = glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS;
    bool shift = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS;

    wm->camera->processKeyboardInput(deltaTime2, w, s, a, d, space, shift);
  }
}
