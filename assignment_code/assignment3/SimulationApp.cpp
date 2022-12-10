#include "SimulationApp.hpp"

#include "glm/gtx/string_cast.hpp"

#include "gloo/shaders/PhongShader.hpp"
#include "gloo/components/RenderingComponent.hpp"
#include "gloo/components/ShadingComponent.hpp"
#include "gloo/components/CameraComponent.hpp"
#include "gloo/components/LightComponent.hpp"
#include "gloo/components/MaterialComponent.hpp"
#include "gloo/MeshLoader.hpp"
#include "gloo/lights/PointLight.hpp"
#include "gloo/lights/AmbientLight.hpp"
#include "gloo/lights/DirectionalLight.hpp"
#include "gloo/cameras/ArcBallCameraNode.hpp"
#include "gloo/debug/AxisNode.hpp"
#include "CircleNode.hpp"
#include "PendulumNode.hpp"
#include "ClothNode.hpp"
#include "SakuraNode.hpp"
// #include "TreeNode.hpp"
#include "SkeletonNode.hpp"



namespace GLOO {
SimulationApp::SimulationApp(const std::string& app_name,
                             glm::ivec2 window_size,
                             IntegratorType integrator_type,
                             float integration_step)
    : Application(app_name, window_size),
      integrator_type_(integrator_type),
      integration_step_(integration_step) {
  // TODO: remove the following two lines and use integrator type and step to
  // create integrators; the lines below exist only to suppress compiler
  // warnings.
  UNUSED(integrator_type_);
  UNUSED(integration_step_);
}

void SimulationApp::SetupScene() {
  SceneNode& root = scene_->GetRootNode();

  auto camera_node = make_unique<ArcBallCameraNode>(45.f, 0.75f, 5.0f);
  scene_->ActivateCamera(camera_node->GetComponentPtr<CameraComponent>());
  root.AddChild(std::move(camera_node));

  root.AddChild(make_unique<AxisNode>('A'));

  auto ambient_light = std::make_shared<AmbientLight>();
  ambient_light->SetAmbientColor(glm::vec3(0.2f));
  root.CreateComponent<LightComponent>(ambient_light);

  auto point_light = std::make_shared<PointLight>();
  point_light->SetDiffuseColor(glm::vec3(0.8f, 0.8f, 0.8f));
  point_light->SetSpecularColor(glm::vec3(1.0f, 1.0f, 1.0f));
  point_light->SetAttenuation(glm::vec3(1.0f, 0.09f, 0.032f));
  auto point_light_node = make_unique<SceneNode>();
  point_light_node->CreateComponent<LightComponent>(point_light);
  point_light_node->GetTransform().SetPosition(glm::vec3(0.0f, 2.0f, 4.f));
  root.AddChild(std::move(point_light_node));

  // change to directional light
  auto sun_light = std::make_shared<DirectionalLight>();
  sun_light->SetDiffuseColor(glm::vec3(0.6f, 0.6f, 0.6f));
  sun_light->SetSpecularColor(glm::vec3(0.6f, 0.6f, 0.6f));
  sun_light->SetDirection(glm::vec3(-1.0f, -0.8f, -1.0f));
  auto sun_light_node = make_unique<SceneNode>();
  sun_light_node->CreateComponent<LightComponent>(sun_light);
  root.AddChild(std::move(sun_light_node));

  root.AddChild(make_unique<SakuraNode>(integrator_type_, integration_step_));
  // root.AddChild(make_unique<CircleNode>(integrator_type_, integration_step_));
  // root.AddChild(make_unique<PendulumNode>(integrator_type_, integration_step_));
  // root.AddChild(make_unique<ClothNode>(integrator_type_, integration_step_));
  // root.AddChild(make_unique<TreeNode>(integrator_type_, integration_step_));

  // heirarchical tree tree skeletonrepresentation
  // auto skeletal_node = make_unique<SkeletonNode>("Model1");
  auto skeletal_node = make_unique<SkeletonNode>("Tree");
  // skeletal_node_ptr_ = skeletal_node.get();
  root.AddChild(std::move(skeletal_node));

}
}  // namespace GLOO
