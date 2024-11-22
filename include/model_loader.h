#ifndef MODEL_LOADER
#define MODEL_LOADER

#include <string>
#include <vector>
#include <glad/glad.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <shader_loader.h>

struct Mesh {
  GLuint VAO, VBO, EBO;
  std::vector<GLfloat> vertices;
  std::vector<GLuint> indices;

  Mesh(const std::vector<GLfloat> &verts, const std::vector<GLuint> &inds);
  void Draw(Shader &shader);
};

class ModelLoader {
public:
  static std::vector<Mesh> loadModel(const std::string &path);

private:
  static void processNode(aiNode *node, const aiScene *scene,
                          std::vector<Mesh> &meshes);
  static Mesh processMesh(aiMesh *mesh, const aiScene *scene);
};

#endif
