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
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <vector>
#include <string>
#include <iostream>

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

Camera camera(glm::vec3(0.0f, 0.0f, 20.0f));

float deltaTime = 0.0f;
float lastFrame = 0.0f;

struct Mesh {
  GLuint VAO, VBO, EBO;
  std::vector<GLfloat> vertices;
  std::vector<GLuint> indices;

  Mesh(const std::vector<GLfloat> &verts, const std::vector<GLuint> &inds) {
    vertices = verts;
    indices = inds;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat),
                 &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint),
                 &indices[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat),
                          (GLvoid *)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat),
                          (GLvoid *)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat),
                          (GLvoid *)(6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
  }

  void Draw(Shader &shader) {
    shader.use();
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
  }
};

std::vector<Mesh> loadModel(const std::string &path) {
  Assimp::Importer importer;
  const aiScene *scene =
      importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

  std::vector<Mesh> meshes;
  if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE ||
      !scene->mRootNode) {
    std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
    return meshes;
  }

  aiNode *rootNode = scene->mRootNode;
  aiMesh *mesh;
  aiMaterial *material;
  aiTexture *texture;

  for (unsigned int i = 0; i < scene->mNumMeshes; i++) {
    mesh = scene->mMeshes[i];
    std::vector<GLfloat> vertices;
    std::vector<GLuint> indices;

    for (unsigned int j = 0; j < mesh->mNumVertices; j++) {
      aiVector3D position = mesh->mVertices[j];
      aiVector3D normal = mesh->mNormals[j];
      aiVector3D texCoords = mesh->mTextureCoords[0]
                                 ? mesh->mTextureCoords[0][j]
                                 : aiVector3D(0.0f, 0.0f, 0.0f);

      vertices.push_back(position.x);
      vertices.push_back(position.y);
      vertices.push_back(position.z);
      vertices.push_back(normal.x);
      vertices.push_back(normal.y);
      vertices.push_back(normal.z);
      vertices.push_back(texCoords.x);
      vertices.push_back(texCoords.y);
    }

    for (unsigned int j = 0; j < mesh->mNumFaces; j++) {
      aiFace face = mesh->mFaces[j];
      for (unsigned int k = 0; k < face.mNumIndices; k++) {
        indices.push_back(face.mIndices[k]);
      }
    }

    meshes.push_back(Mesh(vertices, indices));
  }

  return meshes;
}

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
      loadModel(RESOURCES_PATH "models/testSphere.glb");

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
