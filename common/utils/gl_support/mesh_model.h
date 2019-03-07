// Copyright @2018 Pony AI Inc. All rights reserved.

#pragma once

#include <string>
#include <vector>

#include <GL/gl.h>

#include "common/utils/common/defines.h"
#include "common/utils/gl_support/material.h"
#include "common/utils/math/vec3d.h"

namespace utils {
namespace gl_support {

class MeshModel {
 public:
  MeshModel() = default;
  explicit MeshModel(const std::string& model_file_path);
  virtual ~MeshModel() = default;

  std::vector<Material>* mutable_materials() { return &materials_; }

  void LoadFromFile(const std::string& model_file_path);

  void Draw() const;

 private:
  struct VertexAttributes {
    math::Vec3<float> coord;
    math::Vec3<float> normal;
  };
  static_assert(sizeof(VertexAttributes) == sizeof(float) * 6,
                "VertexAttributes should be compact.");

  // Groups of faces organized by material.
  struct FaceGroup {
    int start_index = -1;
    int end_index = -1;
    int material_index = -1;  // -1 means no material is assigned.
  };

  std::vector<Material> materials_;
  std::vector<VertexAttributes> vertex_attributes_;
  std::vector<FaceGroup> groups_;

  bool has_normal_ = false;

  friend class MeshModelTest_NoMaterial_Test;
  friend class MeshModelTest_WithNormalAndMaterial_Test;

  DISALLOW_COPY_AND_ASSIGN(MeshModel);
};

}  // namespace gl_support
}  // namespace utils
