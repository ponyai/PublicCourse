// Copyright @2018 Pony AI Inc. All rights reserved.

#include "common/utils/gl_support/material.h"

#include <fstream>
#include <sstream>

#include <GL/gl.h>
#include <glog/logging.h>

#include "common/utils/strings/utils.h"

namespace utils {
namespace gl_support {

std::vector<Material> Material::LoadMaterialFile(const std::string& path) {
  std::ifstream fin(path);
  CHECK(fin);

  std::vector<Material> result;
  for (std::string line; std::getline(fin, line);) {
    if (line.empty() || strings::StartWith(line, "#")) {
      // Omit empty lines and comment lines.
      continue;
    }

    std::istringstream str_stream(line);
    std::string cmd;
    str_stream >> cmd;
    if (cmd == "newmtl") {
      // Though the file format does not, we require material name to be index
      // of the material.
      int index = -1;
      str_stream >> index;
      CHECK_EQ(index, result.size());
      result.resize(index + 1);
    } else {
      CHECK(!result.empty());
      Material& material = result.back();
      if (cmd == "Kd") {
        str_stream >> material.diffuse[0] >> material.diffuse[1] >> material.diffuse[2];
      } else if (cmd == "Ka") {
        str_stream >> material.ambient[0] >> material.ambient[1] >> material.ambient[2];
      } else if (cmd == "Ks") {
        str_stream >> material.specular[0] >> material.specular[1] >> material.specular[2];
      } else if (cmd == "Ke") {
        str_stream >> material.emission[0] >> material.emission[1] >> material.emission[2];
      } else if (cmd == "Ns") {
        str_stream >> material.shininess;
      } else if (cmd == "d") {
        str_stream >> material.opacity;
      } else {
        LOG(FATAL) << "Unrecgonized command: " << cmd;
      }
    }
  }
  return result;
}

void Material::SetupOpengl() const {
  auto setup4f = [this](GLenum param_name, const float* vec) {
    float v[4] = {vec[0], vec[1], vec[2], opacity};
    glMaterialfv(GL_FRONT_AND_BACK, param_name, v);
  };

  setup4f(GL_DIFFUSE, diffuse);
  setup4f(GL_AMBIENT, ambient);
  setup4f(GL_SPECULAR, specular);
  setup4f(GL_EMISSION, emission);
  glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
}

}  // namespace gl_support
}  // namespace utils
