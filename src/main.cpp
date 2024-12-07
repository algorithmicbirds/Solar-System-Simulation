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
  WindowManager windowManager(SCR_WIDTH, SCR_HEIGHT, &camera);
  glfwSetWindowUserPointer(windowManager.window, &windowManager);

  Shader starfieldShader(RESOURCES_PATH "shaders/starfield.vert",
                         RESOURCES_PATH "shaders/starfield.frag");
  Shader modelShader(RESOURCES_PATH "shaders/sphere.vert",
                     RESOURCES_PATH "shaders/sphere.frag");
  Shader centralBodyShader(RESOURCES_PATH "shaders/centralBody.vert", 
      RESOURCES_PATH "shaders/centralBody.frag");

  Starfield starfield1(10000, 40000.0f);
  std::vector<Mesh> modelMeshes =
      ModelLoader::loadModel(RESOURCES_PATH "models/sphere.glb");

  windowManager.setupCallbacks();

  glm::mat4 view;
  glm::mat4 projection = glm::perspective(
      glm::radians(45.0f), (float)SCR_WIDTH / SCR_HEIGHT, 0.1f, 50000.0f);

  CelestialBody centralBody(500.0f, 1000000.0f, glm::vec3(0.0f),
                            glm::vec3(0.0f));
  float distanceBuffer = 3000.0f;

  CelestialBody body1(
      180.0f, 30000.0f, glm::vec3(1000.0f + distanceBuffer, 0.0f, 0.0f),
      calculateOrbitalVelocity(centralBody.mass, 5000.0f + distanceBuffer));
  CelestialBody body2(
      200.0f, 30000.0f, glm::vec3(2000.0f + distanceBuffer, 0.0f, 0.0f),
      calculateOrbitalVelocity(centralBody.mass, 15000.0f + distanceBuffer));
  CelestialBody body3(
      130.0f, 30000.0f, glm::vec3(3000.0f + distanceBuffer, 0.0f, 0.0f),
      calculateOrbitalVelocity(centralBody.mass, 25000.0f + distanceBuffer));

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

    collisionDetector.resolveCameraCollisions(camera, bodies);

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
      if (&body == &centralBody) {
        centralBodyShader.use();

        glm::vec3 lightPos = body.position; 
        glm::vec3 lightColor(3.0f, 5.0f, 1.0f); 

        centralBodyShader.setVec3("lightPos", lightPos);
        centralBodyShader.setVec3("lightColor", lightColor);
        centralBodyShader.setVec3("viewPos",
                                  camera.position); 
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, body.position);
        model = glm::scale(model,
                           glm::vec3(body.radius)); 
        centralBodyShader.setMat4("model", model);

        for (Mesh &mesh : modelMeshes) {
          mesh.Draw(centralBodyShader);
        }
      } else {
        // Render other bodies with the regular shader
        modelShader.use();
        modelShader.setMat4("projection", projection);
        modelShader.setMat4("view", view);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, body.position);
        model = glm::scale(model, glm::vec3(body.radius));
        modelShader.setMat4("model", model);

        for (Mesh &mesh : modelMeshes) {
          mesh.Draw(modelShader);
        }
      }
    }



    windowManager.swapBuffers();
    windowManager.pollEvents();
  }

  return 0;
}
