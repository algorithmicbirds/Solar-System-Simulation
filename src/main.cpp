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
#include <globals.h>

Camera camera(glm::vec3(100.0f, 4.0f, 0.0f), 0.0f, 0.0f, 45.0f);

float deltaTime = 0.0f;
float lastFrame = 0.0f;

glm::vec3 calculateOrbitalVelocity(float centralMass, float distance) {
  return glm::vec3(0.0f, sqrt(gravitationalConstant * centralMass / distance),
                   0.0f);
}

int main() {
  // Set up the window manager with the camera
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

  CelestialBody centralBody(25.0f, 1000000.0f, glm::vec3(0.0f),
                            glm::vec3(0.0f));
  float distanceBuffer = 100.0f;

  CelestialBody body1(
      20.0f, 5000.0f, glm::vec3(50.0f + distanceBuffer, 0.0f, 0.0f),
      calculateOrbitalVelocity(centralBody.mass, 50.0f + distanceBuffer));
  CelestialBody body2(
      15.0f, 5000.0f, glm::vec3(150.0f + distanceBuffer, 0.0f, 0.0f),
      calculateOrbitalVelocity(centralBody.mass, 150.0f + distanceBuffer));
  CelestialBody body3(
      10.0f, 5000.0f, glm::vec3(250.0f + distanceBuffer, 0.0f, 0.0f),
      calculateOrbitalVelocity(centralBody.mass, 250.0f + distanceBuffer));

  std::vector<CelestialBody> bodies = {centralBody, body1, body2, body3};

  RayCaster rayCaster(&camera);
  CollisionDetector collisionDetector(&rayCaster);

  // Main game loop
  while (!windowManager.shouldClose()) {
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    windowManager.processInput(deltaTime);

    glm::vec3 totalGravitationalForce(0.0f);
    for (const auto &body : bodies) {
      totalGravitationalForce += camera.calculateGravitationalForce(body);
    }

    // Move the camera based on gravitational forces acting on it
    camera.position += totalGravitationalForce * deltaTime;

    // Resolve any camera collisions
    collisionDetector.resolveCameraCollisions(camera, bodies);

    view = camera.getViewMatrix();

    // Clear the screen and enable depth testing
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    // Render the starfield
    starfieldShader.use();
    starfieldShader.setMat4("projection", projection);
    starfieldShader.setMat4("view", view);
    starfield1.render(starfieldShader, projection, view);

    modelShader.use();
    modelShader.setMat4("projection", projection);
    modelShader.setMat4("view", view);

    // Update and render celestial bodies
    for (auto &body : bodies) {
      glm::vec3 totalForce(0.0f);
      for (const auto &otherBody : bodies) {
        if (&body != &otherBody) {
          totalForce += body.calculateGravitationalForce(otherBody);
        }
      }

      body.updateBody(deltaTime, totalForce);
    }

    // Render the celestial bodies with their updated positions and sizes
    for (const auto &body : bodies) {
      modelShader.use();
      glm::mat4 model = glm::mat4(1.0f);
      model = glm::translate(model, body.position);

      model = glm::scale(
          model, glm::vec3(body.radius)); // Directly use the radius to scale
      modelShader.setMat4("model", model);

      // Draw the meshes
      for (Mesh &mesh : modelMeshes) {
        mesh.Draw(modelShader);
      }
    }

    // Swap buffers and poll events
    windowManager.swapBuffers();
    windowManager.pollEvents();
  }

  return 0;
}
