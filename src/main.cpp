#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <shader_loader.h>
#include <cstdlib>
#include <ctime>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <model_loader.h>
#include <vector>
#include <string>
#include <btBulletDynamicsCommon.h>
#include <iostream>
#include <globals.h>
#include <starfield.h>
#include <camera.h>
#include <window_manager.h>

btBroadphaseInterface *broadphase;
btDefaultCollisionConfiguration *collisionConfig;
btCollisionDispatcher *dispatcher;
btSequentialImpulseConstraintSolver *solver;
btDiscreteDynamicsWorld *dynamicsWorld;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

Camera camera(glm::vec3(-1.0f, 2.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f,
              0.0f);

btRigidBody *cameraRigidBody;

void initPhysics() {
  broadphase = new btDbvtBroadphase();
  collisionConfig = new btDefaultCollisionConfiguration();
  dispatcher = new btCollisionDispatcher(collisionConfig);
  solver = new btSequentialImpulseConstraintSolver();
  dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver,
                                              collisionConfig);
  dynamicsWorld->setGravity(btVector3(0, 0.0f, 0));

  btCapsuleShape *capsuleShape = new btCapsuleShape(0.5f, 1.8f);
  btDefaultMotionState *cameraMotionState =
      new btDefaultMotionState(btTransform(
          btQuaternion(0, 0, 0, 1),
          btVector3(camera.position.x, camera.position.y, camera.position.z)));
  btScalar mass = 80.0f;
  btVector3 inertia(0, 0, 0);
  capsuleShape->calculateLocalInertia(mass, inertia);

  btRigidBody::btRigidBodyConstructionInfo cameraRigidBodyCI(
      mass, cameraMotionState, capsuleShape, inertia);
  cameraRigidBody = new btRigidBody(cameraRigidBodyCI);
  dynamicsWorld->addRigidBody(cameraRigidBody);
}

void cleanupPhysics() {
  dynamicsWorld->removeRigidBody(cameraRigidBody);
  delete cameraRigidBody->getMotionState();
  delete cameraRigidBody;
  delete dynamicsWorld;
  delete solver;
  delete dispatcher;
  delete collisionConfig;
  delete broadphase;
}

int main() {
  initPhysics();
  WindowManager windowManager(SCR_WIDTH, SCR_HEIGHT, "Solar Sim", camera);
  Shader modelShader(RESOURCES_PATH "shaders/sphere.vert",
                     RESOURCES_PATH "shaders/sphere.frag");
  Shader starfieldShader(RESOURCES_PATH "shaders/starfield.vert",
                         RESOURCES_PATH "shaders/starfield.frag");

  std::cout << camera.position.x << std::endl;
  std::vector<Mesh> modelMeshes =
      ModelLoader::loadModel(RESOURCES_PATH "models/sphere.glb");

  Starfield starfield1(10000, 40000.0f);

  while (!windowManager.shouldClose()) {
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    windowManager.processInput(deltaTime);

    dynamicsWorld->stepSimulation(deltaTime, 10);

    btTransform trans;
    cameraRigidBody->getMotionState()->getWorldTransform(trans);
    camera.position =
        glm::vec3(trans.getOrigin().getX(), trans.getOrigin().getY(),
                  trans.getOrigin().getZ());

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    glm::mat4 projection = camera.getProjectionMatrix(SCR_WIDTH, SCR_HEIGHT);
    glm::mat4 view = camera.getViewMatrix();

    starfield1.render(starfieldShader, projection, view);

    for (Mesh &mesh : modelMeshes) {
      glm::mat4 model = glm::mat4(1.0f);
      model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
      model = glm::scale(model, glm::vec3(1.0f));

      modelShader.use();
      modelShader.setMat4("projection", projection);
      modelShader.setMat4("view", view);
      modelShader.setMat4("model", model);

      mesh.Draw(modelShader);
    }

    windowManager.swapBuffers();
    windowManager.pollEvents();
  }

  cleanupPhysics();
  return 0;
}
