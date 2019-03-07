// Copyright @2018 Pony AI Inc. All rights reserved.

#include "common/utils/gl_support/mesh_model.h"

#include <fstream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

#include <glog/logging.h>

#include "common/utils/file/path.h"
#include "common/utils/strings/utils.h"

namespace utils {
namespace gl_support {

MeshModel::MeshModel(const std::string& model_file_path) { LoadFromFile(model_file_path); }

void MeshModel::Draw() const {
  if (has_normal_) {
    glEnable(GL_LIGHTING);
    glEnableClientState(GL_NORMAL_ARRAY);
  }
  glEnableClientState(GL_VERTEX_ARRAY);
  for (const FaceGroup& group : groups_) {
    if (group.material_index >= 0) {
      CHECK_LT(group.material_index, materials_.size());
      materials_[group.material_index].SetupOpengl();
    }

    CHECK_GE(group.start_index, 0);
    CHECK_LT(group.start_index, group.end_index);
    CHECK_LE(group.end_index, vertex_attributes_.size());
    const float* vertex_ptr =
        reinterpret_cast<const float*>(vertex_attributes_.data() + group.start_index);
    glVertexPointer(3, GL_FLOAT, sizeof(VertexAttributes), vertex_ptr);

    if (has_normal_) {
      glNormalPointer(GL_FLOAT, sizeof(VertexAttributes), vertex_ptr + 3);
    }
    glDrawArrays(GL_TRIANGLES, 0, group.end_index - group.start_index);
  }
  glDisableClientState(GL_VERTEX_ARRAY);
  if (has_normal_) {
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisable(GL_LIGHTING);
  }
}

void MeshModel::LoadFromFile(const std::string& model_file_path) {
  std::ifstream fin(model_file_path);
  CHECK(fin);

  std::vector<math::Vec3<float>> vertices;
  std::vector<math::Vec3<float>> normals;
  for (std::string line; std::getline(fin, line);) {
    if (line.empty() || strings::StartWith(line, "#")) {
      // Omit empty lines and comment lines.
      continue;
    }

    std::istringstream str_stream(line);
    std::string cmd;
    str_stream >> cmd;
    if (cmd == "mtllib") {
      // Though the file format does not, we require material name to be index
      // of the material.
      std::string material_file_path;
      str_stream >> material_file_path;
      material_file_path =
          file::path::Join(file::path::Dirname(model_file_path), material_file_path);
      materials_ = Material::LoadMaterialFile(material_file_path);
    } else if (cmd == "usemtl") {
      FaceGroup new_group;
      str_stream >> new_group.material_index;
      new_group.start_index = vertex_attributes_.size();
      if (!groups_.empty()) {
        groups_.back().end_index = vertex_attributes_.size();
      }
      groups_.push_back(new_group);
    } else if (cmd == "v") {
      math::Vec3<float> v;
      str_stream >> v.x >> v.y >> v.z;
      vertices.push_back(v);
    } else if (cmd == "vn") {
      math::Vec3<float> n;
      str_stream >> n.x >> n.y >> n.z;
      normals.push_back(n);
    } else if (cmd == "f") {
      const int orig_size = vertex_attributes_.size();
      vertex_attributes_.resize(orig_size + 3);

      // Only support triangular face, and do not support texture coordinate.
      for (int i = 0; i < 3; ++i) {
        VertexAttributes& va = vertex_attributes_[orig_size + i];
        std::string part;
        str_stream >> part;

        std::vector<std::string> fields = strings::Split(part, '/', true);
        const int vertex_index = std::atoi(fields[0].c_str()) - 1;
        CHECK(vertex_index >= 0 && vertex_index < vertices.size());
        va.coord = vertices[vertex_index];
        // Omit the second field (which is for texture coordinates).

        if (orig_size == 0 && i == 0) {
          // First vertex in the mesh.
          has_normal_ = fields.size() >= 3 && !fields[2].empty();
        }

        if (fields.size() >= 3 && !fields[2].empty()) {
          CHECK(has_normal_) << "Either all or none of the vertices should have normals.";
          const int normal_index = std::atoi(fields[2].c_str()) - 1;
          CHECK(normal_index >= 0 && normal_index < normals.size());
          va.normal = normals[normal_index];
        } else {
          CHECK(!has_normal_) << "Either all or none of the vertices should have normals.";
        }
      }
    } else {
      LOG(FATAL) << "Unrecognized command: " << cmd;
    }
  }

  if (groups_.empty()) {
    FaceGroup group;
    group.start_index = 0;
    group.end_index = vertex_attributes_.size();
    group.material_index = -1;
    groups_.push_back(group);
  } else {
    groups_.back().end_index = vertex_attributes_.size();
  }
}

}  // namespace gl_support
}  // namespace utils
