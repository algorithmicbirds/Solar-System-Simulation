#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <shader_loader.h>
#include <cstdlib>
#include <ctime>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <sphere.h>
#include <camera.h>
#include <starfield.h>
#include <window_manager.h>
#include <model_loader.h>
#include <vector>
#include <string>

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

Camera camera(glm::vec3(0.0f, 0.0f, 20.0f));

float deltaTime = 0.0f;
float lastFrame = 0.0f;

int main() {
  WindowManager windowManager(SCR_WIDTH, SCR_HEIGHT, &camera);
  glfwSetWindowUserPointer(windowManager.window, &windowManager);

  Shader starfieldShader(RESOURCES_PATH "starfield.vert",
                         RESOURCES_PATH "starfield.frag");
  Shader sphereShader(RESOURCES_PATH "sphere.vert",
                      RESOURCES_PATH "sphere.frag");

  Sphere sphere1(1.0f, 16, 32, &sphereShader);
  Sphere sphere2(1.0f, 16, 32, &sphereShader);

  glm::mat4 projection = glm::perspective(
      glm::radians(45.0f), (float)SCR_WIDTH / SCR_HEIGHT, 0.1f, 10000.0f);
  Starfield starfield1(5000, 5000.0f);
  Starfield starfield2(2000, 10.0f);

  std::vector<Mesh> modelMeshes =
      ModelLoader::loadModel(RESOURCES_PATH "models/testSphere.glb");

  windowManager.setupCallbacks();

  glm::mat4 view;
  while (!windowManager.shouldClose()) {
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    windowManager.processInput(deltaTime);

    view = camera.getViewMatrix();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    starfieldShader.use();
    starfieldShader.setMat4("projection", projection);
    starfieldShader.setMat4("view", view);
    starfield1.render(starfieldShader, projection, view);

    sphereShader.use();
    sphereShader.setMat4("projection", projection);
    sphereShader.setMat4("view", view);

    sphere1.render(projection, view, glm::vec3(-2.0f, 0.0f, -5.0f));
    sphere2.render(projection, view, glm::vec3(2.0f, 0.0f, -5.0f));

    for (Mesh &mesh : modelMeshes) {
      glm::mat4 model = glm::mat4(1.0f);
      model = glm::translate(model, glm::vec3(0.0f, 0.0f, -10.0f));
      model = glm::scale(model, glm::vec3(0.5f));

      sphereShader.use();
      sphereShader.setMat4("model", model);
      mesh.Draw(sphereShader);
    }

    windowManager.swapBuffers();
    windowManager.pollEvents();
  }

  return 0;
}
