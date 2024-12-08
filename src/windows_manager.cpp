#include <window_manager.h>
#include <iostream>

// Initialize static members
Camera *WindowManager::activeCamera = nullptr;
float WindowManager::lastX = 0.0f;
float WindowManager::lastY = 0.0f;
bool WindowManager::firstMouse = true;

WindowManager::WindowManager(int width, int height, const char *title,
                             Camera &cam)
    : camera(cam) {
  if (!glfwInit()) {
    std::cerr << "Failed to initialize GLFW!" << std::endl;
    std::exit(-1);
  }

  window = glfwCreateWindow(width, height, title, nullptr, nullptr);
  if (!window) {
    std::cerr << "Failed to create GLFW window!" << std::endl;
    glfwTerminate();
    std::exit(-1);
  }

  glfwMakeContextCurrent(window);
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cerr << "Failed to initialize GLAD!" << std::endl;
    std::exit(-1);
  }

  glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
  glfwSetCursorPosCallback(window, mouseCallback);
  glfwSetScrollCallback(window, scrollCallback);

  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  activeCamera = &camera;
  lastX = width / 2.0f;
  lastY = height / 2.0f;
}

WindowManager::~WindowManager() {
  glfwDestroyWindow(window);
  glfwTerminate();
}

bool WindowManager::shouldClose() const {
  return glfwWindowShouldClose(window);
}

void WindowManager::processInput(float deltaTime) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);

  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {

    camera.processKeyboard(0, deltaTime);
    std::cout << "w registered" << std::endl;
  }
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    camera.processKeyboard(1, deltaTime); // Backward
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    camera.processKeyboard(2, deltaTime); // Left
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    camera.processKeyboard(3, deltaTime); // Right
}

void WindowManager::pollEvents() const { glfwPollEvents(); }

void WindowManager::swapBuffers() const { glfwSwapBuffers(window); }

void WindowManager::framebufferSizeCallback(GLFWwindow *window, int width,
                                            int height) {
  glViewport(0, 0, width, height);
}

void WindowManager::mouseCallback(GLFWwindow *window, double xpos,
                                  double ypos) {
  if (firstMouse) {
    lastX = xpos;
    lastY = ypos;
    firstMouse = false;
  }

  float xoffset = xpos - lastX;
  float yoffset = lastY - ypos; // Reversed since y-coordinates go bottom-to-top
  lastX = xpos;
  lastY = ypos;

  activeCamera->processMouseMovement(xoffset, yoffset);
}

void WindowManager::scrollCallback(GLFWwindow *window, double xoffset,
                                   double yoffset) {
  activeCamera->processMouseScroll(yoffset);
}
