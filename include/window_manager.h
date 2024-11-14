#ifndef WINDOWMANAGER_H
#define WINDOWMANAGER_H

#include <GLFW/glfw3.h>
#include <camera.h>

class WindowManager {
public:
  GLFWwindow *window;
  Camera *camera;

  WindowManager(int width, int height, Camera *cam);
  ~WindowManager();

  void processInput(float deltaTime);
  void setupCallbacks();
  bool shouldClose();
  void swapBuffers();
  void pollEvents();

private:
  double lastX, lastY;
  bool firstMouse;
  bool isFullscreen;
  int windowedWidth, windowedHeight;

  void initWindow(int width, int height);
  void toggleFullscreen();

  static void framebuffer_size_callback(GLFWwindow *window, int width,
                                        int height);
  static void mouse_callback(GLFWwindow *window, double xpos, double ypos);
};

#endif
