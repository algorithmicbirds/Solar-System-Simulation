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
#include <btBulletDynamicsCommon.h>

// Bullet Physics components
btBroadphaseInterface *broadphase;
btDefaultCollisionConfiguration *collisionConfig;
btCollisionDispatcher *dispatcher;
btSequentialImpulseConstraintSolver *solver;
btDiscreteDynamicsWorld *dynamicsWorld;

Camera camera(glm::vec3(100.0f, 4.0f, 0.0f), 0.0f, 0.0f, 45.0f);

float deltaTime = 0.0f;
float lastFrame = 0.0f;

void initPhysics() {
  broadphase = new btDbvtBroadphase();
  collisionConfig = new btDefaultCollisionConfiguration();
  dispatcher = new btCollisionDispatcher(collisionConfig);
  solver = new btSequentialImpulseConstraintSolver();
  dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver,
                                              collisionConfig);
  dynamicsWorld->setGravity(btVector3(0, 0, 0)); // No uniform gravity
}

void cleanupPhysics() {
  delete dynamicsWorld;
  delete solver;
  delete dispatcher;
  delete collisionConfig;
  delete broadphase;
}

// Celestial Body and gravitational force calculation
btRigidBody *createCelestialBody(float mass, float radius,
                                 const glm::vec3 &position) {
  btCollisionShape *shape = new btSphereShape(radius);
  btDefaultMotionState *motionState = new btDefaultMotionState(btTransform(
      btQuaternion(0, 0, 0, 1), btVector3(position.x, position.y, position.z)));
  btVector3 inertia(0, 0, 0);
  if (mass > 0.0f)
    shape->calculateLocalInertia(mass, inertia);

  btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, shape,
                                                  inertia);
  btRigidBody *body = new btRigidBody(rbInfo);

  dynamicsWorld->addRigidBody(body);
  return body;
}

void applyGravitationalForces(std::vector<btRigidBody *> &bodies) {
  const float G = 6.67430e-11f; // Gravitational constant (m^3 kg^-1 s^-2)

  for (size_t i = 0; i < bodies.size(); ++i) {
    btRigidBody *bodyA = bodies[i];
    btVector3 force(0, 0, 0);

    for (size_t j = 0; j < bodies.size(); ++j) {
      if (i == j)
        continue;

      btRigidBody *bodyB = bodies[j];
      btVector3 posA = bodyA->getCenterOfMassPosition();
      btVector3 posB = bodyB->getCenterOfMassPosition();

      btVector3 direction = posB - posA;
      float distance = direction.length();
      if (distance > 1e-3) { // Avoid division by zero
        float massA = bodyA->getMass();
        float massB = bodyB->getMass();
        float forceMagnitude = (G * massA * massB) / (distance * distance);
        force += direction.normalized() * forceMagnitude;
      }
    }

    bodyA->applyCentralForce(force);
  }
}

int main() {
  // Initialize Bullet Physics
  initPhysics();

  // Set up GLFW window
  WindowManager windowManager(SCR_WIDTH, SCR_HEIGHT, &camera);
  glfwSetWindowUserPointer(windowManager.window, &windowManager);

  // Load shaders
  Shader starfieldShader(RESOURCES_PATH "shaders/starfield.vert",
                         RESOURCES_PATH "shaders/starfield.frag");
  Shader modelShader(RESOURCES_PATH "shaders/sphere.vert",
                     RESOURCES_PATH "shaders/sphere.frag");
  Shader centralBodyShader(RESOURCES_PATH "shaders/centralBody.vert",
                           RESOURCES_PATH "shaders/centralBody.frag");

  // Set up starfield and models
  Starfield starfield1(10000, 40000.0f);
  std::vector<Mesh> modelMeshes =
      ModelLoader::loadModel(RESOURCES_PATH "models/sphere.glb");

  // Set up camera and window
  windowManager.setupCallbacks();
  glm::mat4 view;
  glm::mat4 projection = glm::perspective(
      glm::radians(45.0f), (float)SCR_WIDTH / SCR_HEIGHT, 0.1f, 50000.0f);

  // Create celestial bodies
  std::vector<btRigidBody *> celestialBodies;
  celestialBodies.push_back(createCelestialBody(
      1e6, 500.0f, glm::vec3(0.0f, 0.0f, 0.0f))); // Central body
  celestialBodies.push_back(createCelestialBody(
      1e3, 180.0f, glm::vec3(5000.0f, 0.0f, 0.0f))); // Orbiting body 1
  celestialBodies.push_back(createCelestialBody(
      1e3, 200.0f, glm::vec3(15000.0f, 0.0f, 0.0f))); // Orbiting body 2
  celestialBodies.push_back(createCelestialBody(
      1e3, 130.0f, glm::vec3(25000.0f, 0.0f, 0.0f))); // Orbiting body 3

  // Main loop
  while (!windowManager.shouldClose()) {
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    windowManager.processInput(deltaTime);

    // Apply custom gravitational forces
    applyGravitationalForces(celestialBodies);

    // Step the physics simulation
    dynamicsWorld->stepSimulation(deltaTime, 10);

    // Render celestial bodies
    view = camera.getViewMatrix();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    for (btRigidBody *body : celestialBodies) {
      btTransform transform;
      body->getMotionState()->getWorldTransform(transform);
      btVector3 pos = transform.getOrigin();

      glm::mat4 model = glm::mat4(1.0f);
      model = glm::translate(model, glm::vec3(pos.x(), pos.y(), pos.z()));
      model =
          glm::scale(model, glm::vec3(500.0f)); // Adjust for the body's radius

      modelShader.use();
      modelShader.setMat4("projection", projection);
      modelShader.setMat4("view", view);
      modelShader.setMat4("model", model);

      for (Mesh &mesh : modelMeshes) {
        mesh.Draw(modelShader);
      }
    }

    windowManager.swapBuffers();
    windowManager.pollEvents();
  }

  // Clean up physics
  cleanupPhysics();
  return 0;
}
