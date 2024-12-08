#ifndef WINDOW_MANAGER_H
#define WINDOW_MANAGER_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <camera.h>

class WindowManager {
public:
  GLFWwindow *window;
  Camera &camera;

  WindowManager(int width, int height, const char *title, Camera &cam);
  ~WindowManager();

  bool shouldClose() const;
  void processInput(float deltaTime);
  void pollEvents() const;
  void swapBuffers() const;

  static void framebufferSizeCallback(GLFWwindow *window, int width,
                                      int height);
  static void mouseCallback(GLFWwindow *window, double xpos, double ypos);
  static void scrollCallback(GLFWwindow *window, double xoffset,
                             double yoffset);

private:
  static Camera *activeCamera;
  static float lastX;
  static float lastY;
  static bool firstMouse;
};

#endif
