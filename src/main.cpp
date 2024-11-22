#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <shader_loader.h>
#include <cstdlib>
#include <ctime>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
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

  Shader starfieldShader(RESOURCES_PATH "shaders/starfield.vert",
                         RESOURCES_PATH "shaders/starfield.frag");
  Shader modelShader(RESOURCES_PATH "shaders/sphere.vert",
                     RESOURCES_PATH "shaders/sphere.frag");

  Starfield starfield1(5000, 5000.0f);

  std::vector<Mesh> modelMeshes =
      ModelLoader::loadModel(RESOURCES_PATH "models/testSphere.glb");

  windowManager.setupCallbacks();

  glm::mat4 view;
  glm::mat4 projection = glm::perspective(
      glm::radians(45.0f), (float)SCR_WIDTH / SCR_HEIGHT, 0.1f, 10000.0f);

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

    modelShader.use();
    modelShader.setMat4("projection", projection);
    modelShader.setMat4("view", view);

    for (Mesh &mesh : modelMeshes) {
      glm::mat4 model = glm::mat4(1.0f);
      model = glm::translate(model, glm::vec3(0.0f, 0.0f, -5.0f));
      model = glm::scale(model, glm::vec3(1.0f));

      modelShader.use();
      modelShader.setMat4("model", model);
      mesh.Draw(modelShader);
    }

    windowManager.swapBuffers();
    windowManager.pollEvents();
  }

  return 0;
}
