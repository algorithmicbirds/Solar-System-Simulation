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
#include <celestial_body.h>
#include <raycaster.h>
#include <collision_detector.h>

Camera camera(glm::vec3(0.0f, 0.0f, 20.0f), 0.0f, 0.0f, 45.0f);

float deltaTime = 0.0f;
float lastFrame = 0.0f;

glm::vec3 calculateOrbitalVelocity(float centralMass, float distance) {
  return glm::vec3(0.0f, sqrt(gravitationalConstant * centralMass / distance),
                   0.0f);
}

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

  CelestialBody centralBody(5.0f, 10000.0f, glm::vec3(0.0f), glm::vec3(0.0f));
  float distanceBuffer = 10.0f;

  CelestialBody body1(
      2.0f, 500.0f, glm::vec3(50.0f + distanceBuffer, 0.0f, 0.0f),
      calculateOrbitalVelocity(centralBody.mass, 50.0f + distanceBuffer));
  CelestialBody body2(
      2.0f, 500.0f, glm::vec3(150.0f + distanceBuffer, 0.0f, 0.0f),
      calculateOrbitalVelocity(centralBody.mass, 150.0f + distanceBuffer));
  CelestialBody body3(
      2.0f, 500.0f, glm::vec3(250.0f + distanceBuffer, 0.0f, 0.0f),
      calculateOrbitalVelocity(centralBody.mass, 250.0f + distanceBuffer));

  std::vector<CelestialBody> bodies = {centralBody, body1, body2, body3};

  RayCaster rayCaster(&camera);
  CollisionDetector collisionDetector(&rayCaster);

  while (!windowManager.shouldClose()) {
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    windowManager.processInput(deltaTime);

    glm::vec3 totalGravitationalForce(0.0f);
    for (const auto &body : bodies) {
      totalGravitationalForce += camera.calculateGravitationalForce(body);
    }

    camera.position += totalGravitationalForce * deltaTime;

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

    for (auto &body : bodies) {
      glm::vec3 totalForce(0.0f);

      for (const auto &otherBody : bodies) {
        if (&body != &otherBody) {
          totalForce += body.calculateGravitationalForce(otherBody);
        }
      }

      body.updateBody(deltaTime, totalForce);
    }

    for (const auto &body : bodies) {
      modelShader.use();
      glm::mat4 model = glm::mat4(1.0f);
      model = glm::translate(model, body.position);
      model = glm::scale(model, glm::vec3(body.radius));
      modelShader.setMat4("model", model);
      for (Mesh &mesh : modelMeshes) {
        mesh.Draw(modelShader);
      }
    }

    /** check for collision with the terrain or celestial bodies using the
     falling ray **/
    collisionDetector.checkCollisionWithTerrain(500.0f, 400.0f, projection,
                                                view, bodies);

    windowManager.swapBuffers();
    windowManager.pollEvents();
  }

  return 0;
}
