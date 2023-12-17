

#include <realm_io/pcl_export.h>

#include <opencv2/imgproc.hpp>

#include <pcl/io/ply_io.h>
#include <pcl/conversions.h>
#include <pcl/point_types.h>

#include <realm_core/cv_grid_map.h>

namespace realm
{
namespace io
{

void saveElevationPointsToPLY(const CvGridMap &map,
                              const std::string &ele_layer_name,
                              const std::string &normals_layer_name,
                              const std::string &color_layer_name,
                              const std::string &mask_layer_name,
                              const std::string &directory,
                              const std::string &name)
{
  std::string filename = (directory + "/" + name + ".ply");
  saveElevationPoints(map, ele_layer_name, normals_layer_name, color_layer_name, mask_layer_name, filename, "ply");
}

void saveElevationPoints(const CvGridMap &map,
                         const std::string &ele_layer_name,
                         const std::string &normals_layer_name,
                         const std::string &color_layer_name,
                         const std::string &mask_layer_name,
                         const std::string &filename,
                         const std::string &suffix)
{
  assert(map.exists(ele_layer_name));
  assert(normals_layer_name.empty() ? map.exists(normals_layer_name) : true);
  assert(map.exists(mask_layer_name));

  if (normals_layer_name.empty())
    saveElevationPointsRGB(map, ele_layer_name, color_layer_name, mask_layer_name, filename, suffix);
  else
    saveElevationPointsRGBNormal(map, ele_layer_name, normals_layer_name, color_layer_name, mask_layer_name, filename, suffix);
}

void saveElevationPointsRGB(const CvGridMap &map,
                            const std::string &ele_layer_name,
                            const std::string &color_layer_name,
                            const std::string &mask_layer_name,
                            const std::string &filename,
                            const std::string &suffix)
{
  assert(map.exists(ele_layer_name));
  assert(map.exists(color_layer_name));
  assert(map.exists(mask_layer_name));

  cv::Size2i size = map.size();
  auto n = static_cast<uint32_t>(size.width*size.height);

  cv::Mat elevation = map[ele_layer_name];
  cv::Mat mask = (elevation == elevation);
  cv::Mat color = map[color_layer_name];
  if (color.type() == CV_8UC4)
    cv::cvtColor(color, color, cv::ColorConversionCodes::COLOR_BGRA2BGR);

  // Fill in the cloud data
  pcl::PointCloud<pcl::PointXYZRGB> cloud;
  cloud.width    = n;
  cloud.height   = 1;
  cloud.is_dense = false;
  cloud.points.reserve(cloud.width * cloud.height);

  for (int r = 0; r < size.height; ++r)
    for (int c = 0; c < size.width; ++c)
    {
      // Skip invalid grid elements
      if (mask.at<uchar>(r, c) == 0)
        continue;

      cv::Point3d pt = map.atPosition3d(r, c, "elevation");
      cv::Vec3b bgr = color.at<cv::Vec3b>(r, c);

      pcl::PointXYZRGB pt_rgb;
      pt_rgb.x = static_cast<float>(pt.x);
      pt_rgb.y = static_cast<float>(pt.y);
      pt_rgb.z = static_cast<float>(pt.z);
      pt_rgb.r = bgr[2];
      pt_rgb.g = bgr[1];
      pt_rgb.b = bgr[0];

      cloud.push_back(pt_rgb);
    }
  cloud.resize(cloud.size());

  if (suffix == "ply")
    pcl::io::savePLYFileBinary(filename, cloud);
}

void saveElevationPointsRGBNormal(const CvGridMap &map,
                                  const std::string &ele_layer_name,
                                  const std::string &normals_layer_name,
                                  const std::string &color_layer_name,
                                  const std::string &mask_layer_name,
                                  const std::string &filename,
                                  const std::string &suffix)
{
  assert(map.exists(ele_layer_name));
  assert(map.exists(normals_layer_name));
  assert(map.exists(color_layer_name));
  assert(map.exists(mask_layer_name));

  cv::Size2i size = map.size();
  auto n = static_cast<uint32_t>(size.width*size.height);

  cv::Mat elevation = map[ele_layer_name];
  cv::Mat elevation_normals = map[normals_layer_name];
  // cv::Mat mask = map[mask_layer_name];
  cv::Mat mask = (elevation == elevation);
  cv::Mat color = map[color_layer_name];
  if (color.type() == CV_8UC4)
    cv::cvtColor(color, color, cv::ColorConversionCodes::COLOR_BGRA2BGR);

  // Fill in the cloud data
  pcl::PointCloud<pcl::PointXYZRGBNormal> cloud;
  cloud.width    = n;
  cloud.height   = 1;
  cloud.is_dense = false;
  cloud.points.reserve(cloud.width * cloud.height);

  for (int r = 0; r < size.height; ++r)
    for (int c = 0; c < size.width; ++c)
    {
      // Skip invalid grid elements
      if (mask.at<uchar>(r, c) == 0)
        continue;

      cv::Point3d pt = map.atPosition3d(r, c, "elevation");
      cv::Vec3f normal = elevation_normals.at<cv::Vec3f>(r, c);
      cv::Vec3b bgr = color.at<cv::Vec3b>(r, c);

      pcl::PointXYZRGBNormal pt_rgb;
      pt_rgb.x = static_cast<float>(pt.x);
      pt_rgb.y = static_cast<float>(pt.y);
      pt_rgb.z = static_cast<float>(pt.z);
      pt_rgb.r = bgr[2];
      pt_rgb.g = bgr[1];
      pt_rgb.b = bgr[0];
      pt_rgb.normal_x = normal[0];
      pt_rgb.normal_y = normal[1];
      pt_rgb.normal_z = normal[2];

      cloud.push_back(pt_rgb);
    }
  cloud.resize(cloud.size());

  if (suffix == "ply")
    pcl::io::savePLYFileBinary(filename, cloud);
}

void saveElevationMeshToPLY(const CvGridMap &map,
                            const std::vector<cv::Point2i> &vertices,
                            const std::string &ele_layer_name,
                            const std::string &normal_layer_name,
                            const std::string &color_layer_name,
                            const std::string &mask_layer_name,
                            const std::string &directory,
                            const std::string &name)
{
  std::string filename = (directory + "/" + name + ".ply");
  saveElevationMeshToPLY(map, vertices, ele_layer_name, normal_layer_name, color_layer_name, mask_layer_name, filename);
}

void saveElevationMeshToPLY(const CvGridMap &map,
                            const std::vector<cv::Point2i> &vertices,
                            const std::string &ele_layer_name,
                            const std::string &normal_layer_name,
                            const std::string &color_layer_name,
                            const std::string &mask_layer_name,
                            const std::string &filename)
{
  assert(map.exists(ele_layer_name));
  assert(map.exists(normal_layer_name));
  assert(map.exists(color_layer_name));
  assert(map.exists(mask_layer_name));

  cv::Size2i size = map.size();
  auto n = static_cast<uint32_t>(size.width*size.height);

  cv::Mat elevation = map[ele_layer_name];
  cv::Mat elevation_normals = map[normal_layer_name];
  // cv::Mat mask = map[mask_layer_name];
  cv::Mat mask = (elevation == elevation);

  cv::Mat color = map[color_layer_name];
  if (color.type() == CV_8UC4)
    cv::cvtColor(color, color, cv::ColorConversionCodes::COLOR_BGRA2BGR);

  // Fill in the cloud data
  pcl::PointCloud<pcl::PointXYZRGBNormal> cloud;
  cloud.width    = n;
  cloud.height   = 1;
  cloud.is_dense = false;
  cloud.points.reserve(cloud.width * cloud.height);

  std::unordered_map<int, size_t> rc_to_idx;
  for (int r = 0; r < size.height; ++r)
    for (int c = 0; c < size.width; ++c)
    {
      // Skip invalid grid elements
      if (mask.at<uchar>(r, c) == 0)
        continue;

      cv::Point3d pt = map.atPosition3d(r, c, "elevation");
      cv::Vec3f normal = elevation_normals.at<cv::Vec3f>(r, c);
      cv::Vec3b bgr = color.at<cv::Vec3b>(r, c);

      pcl::PointXYZRGBNormal pt_rgb;
      pt_rgb.x = static_cast<float>(pt.x);
      pt_rgb.y = static_cast<float>(pt.y);
      pt_rgb.z = static_cast<float>(pt.z);
      pt_rgb.r = bgr[2];
      pt_rgb.g = bgr[1];
      pt_rgb.b = bgr[0];
      pt_rgb.normal_x = normal[0];
      pt_rgb.normal_y = normal[1];
      pt_rgb.normal_z = normal[2];

      cloud.push_back(pt_rgb);

      int hash = r*size.width+c;
      rc_to_idx[hash] = cloud.size()-1;
    }

  size_t count = 0;
  std::vector<pcl::Vertices> polygons(vertices.size()/3);
  for (size_t i = 0; i < vertices.size(); i+=3, ++count)
    for (uint8_t j = 0; j < 3; ++j)
    {
      int hash = vertices[i+j].y*size.width + vertices[i+j].x;
      polygons[count].vertices.push_back(rc_to_idx[hash]);
    }

  pcl::PolygonMesh mesh;
  pcl::toPCLPointCloud2(cloud, mesh.cloud);
  mesh.polygons = polygons;

  pcl::io::savePLYFileBinary(filename, mesh);
}

}
}
