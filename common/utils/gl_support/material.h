// Copyright @2018 Pony AI Inc. All rights reserved.

#pragma once

#include <string>
#include <vector>

namespace utils {
namespace gl_support {

struct Material {
  float diffuse[3] = {0.0f, 0.0f, 0.0f};
  float ambient[3] = {0.0f, 0.0f, 0.0f};
  float specular[3] = {0.0f, 0.0f, 0.0f};
  float emission[3] = {0.0f, 0.0f, 0.0f};
  float shininess = 0.0f;
  float opacity = 1.0f;

  // A more restricted .mtl file loader:
  // * Each material's name is required to be the index of the material (though .mtl file format
  //   does not require that).
  // * Material does not allow to have a texture map.
  static std::vector<Material> LoadMaterialFile(const std::string& path);

  void SetupOpengl() const;
};

}  // namespace gl_support
}  // namespace utils
