// Copyright @2018 Pony AI Inc. All rights reserved.

#include "homework4/display/layer/intensity_map_layer.h"

#include "common/proto/map.pb.h"
#include "common/utils/common/defines.h"
#include "common/utils/file/file.h"
#include "common/utils/file/path.h"
#include "common/utils/gl/basic_primitives.h"

constexpr const char kLowResIntensityMapFilename[] = "map_2d.png";
constexpr const char kIntensityMapConfigFilename[] = "map_image_config.txt";

DECLARE_string(map_dir);

namespace {
std::string MapMetaInfoDir() {
  if (FLAGS_map_dir.empty()) {
    return file::path::Join(file::path::GetProjectRootPath(), "homework4", "map", "grid2");
  }  else {
    return FLAGS_map_dir;
  }

}
}

namespace utils {
namespace display {

IntensityMapLayer::IntensityMapLayer(const std::string& name) : utils::display::Layer(name) {}

void IntensityMapLayer::InitializeOnContextCreated() {
  GLuint image_id =
      static_cast<MapOpenglPainter*>(gl_painter_)
          ->LoadImage(file::path::Join(MapMetaInfoDir(), kLowResIntensityMapFilename));
  interface::map::MapImageConfig config;
  CHECK(file::ReadFileToProto(file::path::Join(MapMetaInfoDir(), kIntensityMapConfigFilename),
                              &config));
  intensity_map_images_.push_back({image_id, config});
}

void IntensityMapLayer::Draw() const { ShowIntensityMap(intensity_map_images_, 0.0); }

void IntensityMapLayer::ShowIntensityMap(
    const std::vector<utils::display::IntensityMapImage>& images,
    double intensity_map_height) const {
  std::vector<math::Vec2d> texcoords = {
      math::Vec2d(0.0, 0.0), math::Vec2d(1.0, 0.0), math::Vec2d(1.0, 1.0), math::Vec2d(0.0, 1.0),
  };
  std::vector<math::Vec3d> vertices(4);
  glEnable(GL_TEXTURE_2D);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
  for (const auto& image : images) {
    const auto& image_config = image.map_image_config;
    vertices[0] = math::Vec3d(image_config.min_x(), image_config.min_y(), intensity_map_height);
    vertices[1] = math::Vec3d(image_config.max_x(), image_config.min_y(), intensity_map_height);
    vertices[2] = math::Vec3d(image_config.max_x(), image_config.max_y(), intensity_map_height);
    vertices[3] = math::Vec3d(image_config.min_x(), image_config.max_y(), intensity_map_height);

    gl::Quads quads(context_);
    quads.SetData(utils::ConstArrayView<math::Vec3d>(vertices.data(), vertices.size()),
                  utils::ConstArrayView<math::Vec2d>(texcoords.data(), texcoords.size()));
    quads.SetTextureFlag(true);

    glBindTexture(GL_TEXTURE_2D, image.image_id);
    quads.Draw();
  }
  glDisable(GL_TEXTURE_2D);
}

}  // namespace display
}  // namespace utils
