#include <glad/glad.h>
#include <iostream>
#include <GLFW/glfw3.h>
#include <window_manager.h>

WindowManager::WindowManager(int width, int height, Camera *cam)
    : window(nullptr), camera(cam), firstMouse(true), lastX(width / 2.0),
      lastY(height / 2.0), windowedWidth(width), windowedHeight(height),
      isFullscreen(false) {
  initWindow(width, height);
}

WindowManager::~WindowManager() { glfwTerminate(); }

void WindowManager::initWindow(int width, int height) {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  window = glfwCreateWindow(width, height, "Solar System Simulation", NULL,
                            NULL);
  if (window == NULL) {
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
}

void WindowManager::setupCallbacks() {
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  glfwSetCursorPosCallback(window, mouse_callback);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void WindowManager::processInput(float deltaTime) {
  bool w = glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS;
  bool s = glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS;
  bool a = glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS;
  bool d = glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS;
  bool space = glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS;
  bool shift = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS;

  camera->processKeyboardInput(deltaTime, w, s, a, d, space, shift);

  if (glfwGetKey(window, GLFW_KEY_F11) == GLFW_PRESS) {
    toggleFullscreen();
  }

  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, GLFW_TRUE);
}

bool WindowManager::shouldClose() { return glfwWindowShouldClose(window); }

void WindowManager::swapBuffers() { glfwSwapBuffers(window); }

void WindowManager::pollEvents() { glfwPollEvents(); }

void WindowManager::framebuffer_size_callback(GLFWwindow *window, int width,
                                              int height) {
  glViewport(0, 0, width,
             height);
  WindowManager *wm =
      static_cast<WindowManager *>(glfwGetWindowUserPointer(window));
  wm->camera->setAspectRatio((float)width / (float)height);
}

void WindowManager::mouse_callback(GLFWwindow *window, double xpos,
                                   double ypos) {
  WindowManager *wm =
      static_cast<WindowManager *>(glfwGetWindowUserPointer(window));
  if (wm->firstMouse) {
    wm->lastX = xpos;
    wm->lastY = ypos;
    wm->firstMouse = false;
  }

  float xoffset = xpos - wm->lastX;
  float yoffset = wm->lastY - ypos;
  wm->lastX = xpos;
  wm->lastY = ypos;

  wm->camera->processMouseMovement(xoffset, yoffset);
}

void WindowManager::toggleFullscreen() {
  isFullscreen = !isFullscreen;
  if (isFullscreen) {
    glfwGetWindowSize(window, &windowedWidth, &windowedHeight);

    GLFWmonitor *monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode *mode = glfwGetVideoMode(monitor);

    glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height,
                         mode->refreshRate);
  } else {
    glfwSetWindowMonitor(window, nullptr, 100, 100, windowedWidth,
                         windowedHeight, GLFW_DONT_CARE);
  }
  int width, height;
  glfwGetFramebufferSize(window, &width, &height);
  glViewport(0, 0, width, height);

  camera->setAspectRatio((float)width / (float)height);
}
