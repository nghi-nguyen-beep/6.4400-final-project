#include "SkeletonNode.hpp"

#include "gloo/utils.hpp"
#include "gloo/InputManager.hpp"
#include "gloo/MeshLoader.hpp"

#include "gloo/debug/PrimitiveFactory.hpp"
#include "gloo/components/RenderingComponent.hpp"
#include "gloo/components/ShadingComponent.hpp"
#include "gloo/components/MaterialComponent.hpp"
#include "gloo/VertexObject.hpp"
#include "gloo/shaders/ShaderProgram.hpp"
#include "gloo/shaders/PhongShader.hpp"
#include "gloo/shaders/SimpleShader.hpp"
#include "gloo/Material.hpp"

#include <fstream>
#include <iostream>
#include <string>
#include <sstream>

namespace GLOO {
SkeletonNode::SkeletonNode(const std::string& filename)
    : SceneNode(), draw_mode_(DrawMode::Skeleton) {
  std::cout << "loading all files..." << std::endl;
  
  // std::vector<EulerAngle*> linked_angles();
  
  // linked_angles_ = linked_angles;

  LoadAllFiles(filename);
  std::cout << "decorating tree..." << std::endl;
  // std::cout << skeleton_hier.size() << std::endl;

  wind_vel = glm::vec3(0.0f);
  wind_goal = glm::vec3(0.0f);

  for (int i = 0; i<skeleton_hier.size(); i++) {
    auto angle = EulerAngle();
    angle.rx = 0.0f;
    angle.ry = 0.0f;
    angle.rz = -wind_vel.z;
    if (i == 0 ) {
      angle.rz = -0.5f * wind_vel.z; // trunk is more stiff
    }
    linked_angles.push_back(angle);
    linked_angles_.push_back(&angle);
  }
  DecorateTree();

  // Force initial update.
  OnJointChanged();
  
  std::cout << "done initializing.....?" << std::endl;
  // OnJointChanged();
}



// void SkeletonNode::ToggleDrawMode() {
//   draw_mode_ =
//       draw_mode_ == DrawMode::Skeleton ? DrawMode::SSD : DrawMode::Skeleton;
    
//   // TODO: implement here toggling between skeleton mode and SSD mode.
//   // The current mode is draw_mode_;
//   // Hint: you may find SceneNode::SetActive convenient here as
//   // inactive nodes will not be picked up by the renderer.
//   if (draw_mode_ == DrawMode::Skeleton) {
//     skeleton_hier[0]->SetActive(true);
//     // mesh_node->SetActive(false);
//   } 
//   // else if (draw_mode_ == DrawMode::SSD) {
//     // skeleton_hier[0]->SetActive(false);
//     // mesh_node->SetActive(true);
//   // }
// }

void SkeletonNode::DecorateTree() {

  // std::shared_ptr<VertexObject> sphere_mesh_ = PrimitiveFactory::CreateSphere(0.015f, 25, 25);
  std::shared_ptr<VertexObject> sphere_mesh_ = PrimitiveFactory::CreateSphere(0.05f, 25, 25);
  // std::shared_ptr<VertexObject> world_mesh_ = PrimitiveFactory::CreateCylinder(300.0f, 100.0f, 50);
  // std::shared_ptr<VertexObject> world_mesh_ = PrimitiveFactory::CreateSphere(300.0f, 50, 50);
  std::shared_ptr<VertexObject> world_mesh_ = PrimitiveFactory::CreateCylinder(100.0f, 200.0f, 100);
  std::shared_ptr<VertexObject> ground_mesh_ = PrimitiveFactory::CreateCylinder(20.0f, 200.0f, 100);
  std::shared_ptr<VertexObject> trunk_sphere_mesh_ = PrimitiveFactory::CreateSphere(0.12f, 25, 25);
  // std::shared_ptr<VertexObject> cylinder_mesh_ = PrimitiveFactory::CreateCylinder(0.012f, 1.0f, 25);
  std::shared_ptr<VertexObject> branch_mesh_ = PrimitiveFactory::CreateCylinder(0.05f, 1.0f, 25);
  std::shared_ptr<VertexObject> trunk_mesh_ = PrimitiveFactory::CreateCylinder(0.12f, 1.0f, 25);
  std::shared_ptr<ShaderProgram> shader_ = std::make_shared<PhongShader>();
  auto brown_color = glm::vec3(0.59f, 0.29f, 0.0f);

  // create the ground
  auto ground_node = make_unique<SceneNode>();
  ground_node -> CreateComponent<ShadingComponent>(std::make_shared<PhongShader>());
  ground_node -> CreateComponent<RenderingComponent>(ground_mesh_);
  ground_node -> GetTransform().SetPosition(glm::vec3(100.0f, -21.07f, .0f));
  ground_node -> GetTransform().SetRotation(glm::vec3(0.0f, 0.0f, 1.0f), 1.57f);
  ground_node->CreateComponent<MaterialComponent>(std::make_shared<Material>(Material::GetDefault()));

  // Material &material_color = ground_node->GetComponentPtr<MaterialComponent>()->GetMaterial();
  auto color = glm::vec3(0.3137f, 0.4902f, 0.1647f);
  ground_node->GetComponentPtr<MaterialComponent>()->GetMaterial().SetAmbientColor(color);
  ground_node->GetComponentPtr<MaterialComponent>()->GetMaterial().SetDiffuseColor(color);
  ground_node->GetComponentPtr<MaterialComponent>()->GetMaterial().SetSpecularColor(color); 
  // ground_node->GetComponentPtr<MaterialComponent>()->GetMaterial().SetSpecularColor(color); 
  AddChild(std::move(ground_node));
  
  // create the world
  auto world_node = make_unique<SceneNode>();
  world_node -> CreateComponent<ShadingComponent>(std::make_shared<PhongShader>());
  world_node -> CreateComponent<RenderingComponent>(world_mesh_);
  world_node -> GetTransform().SetPosition(glm::vec3(100.0f, -21.1f, -110.0f));
  world_node -> GetTransform().SetRotation(glm::vec3(0.0f, 0.0f, 1.0f), 1.57f);
  world_node->CreateComponent<MaterialComponent>(std::make_shared<Material>(Material::GetDefault()));

  // material_color = world_node->GetComponentPtr<MaterialComponent>()->GetMaterial();
  auto blue_color = glm::vec3(0.7894f, 0.9075f, 1.00f);
  world_node->GetComponentPtr<MaterialComponent>()->GetMaterial().SetAmbientColor(blue_color);
  world_node->GetComponentPtr<MaterialComponent>()->GetMaterial().SetDiffuseColor(blue_color); 
  world_node->GetComponentPtr<MaterialComponent>()->GetMaterial().SetSpecularColor(blue_color); 
  AddChild(std::move(world_node));

  // std::cout << "BUT WHY 1" << std::endl;
  // for (SceneNode* joint_ptr : skeleton_hier) {
  for (int i = 0; i<skeleton_hier.size(); i++) {
    SceneNode* joint_ptr = skeleton_hier.at(i);

    
     //create sphere mesh scene node for each node in the skeleton heirarchy
    auto sphere_node = make_unique<SceneNode>();
    sphere_node -> CreateComponent<ShadingComponent>(shader_);
    
    // make a leaf mesh for end nodes or sphere meshes for all joint pointers
    if (joint_ptr->GetChildrenCount()==0){
      sphere_node -> CreateComponent<RenderingComponent>(leaves_mesh_);
      sphere_node -> GetTransform().SetPosition(glm::vec3(.0f, -.2f, .0f));
      sphere_node->CreateComponent<MaterialComponent>(std::make_shared<Material>(Material::GetDefault()));

      Material &material_color = sphere_node->GetComponentPtr<MaterialComponent>()->GetMaterial();
      auto color = glm::vec3(0.95f, 0.69f, 0.79f);
      material_color.SetAmbientColor(color);
      material_color.SetDiffuseColor(color); 
    } else {
      if (joint_ptr == skeleton_hier.at(0) || joint_ptr == skeleton_hier.at(1)) {
        sphere_node -> CreateComponent<RenderingComponent>(trunk_sphere_mesh_);
      } else {
        sphere_node -> CreateComponent<RenderingComponent>(sphere_mesh_);
      }
      sphere_node->CreateComponent<MaterialComponent>(std::make_shared<Material>(Material::GetDefault()));
      Material &material_color = sphere_node->GetComponentPtr<MaterialComponent>()->GetMaterial();
      material_color.SetAmbientColor(brown_color);
      material_color.SetDiffuseColor(brown_color); 
    }
    glm::vec3 offset = glm::vec3(.0f, -.1f, .0f);
    glm::vec3 position = joint_ptr->GetTransform().GetPosition()+offset;

    sphere_nodes_ptrs_.push_back(sphere_node.get());
    joint_ptr->AddChild(std::move(sphere_node));

    // create cylinder mesh scene node between each parent and child node in the skeleton heirarchy 
    if (joint_ptr != skeleton_hier.at(0)) {
      // std::cout << "soooo" << std::endl;
      std::vector<SceneNode*> cylinder_sections;
      // std::cout << i << "/" << tree_section_hier.size() << "/" << skeleton_hier.size() << std::endl;
      std::vector<SceneNode*> tree_section = tree_section_hier.at(i-1);
      // std::cout << "here?" << std::endl;

      for (int section = 0; section<section_num+1; section++) {
        auto cylinder_node = make_unique<SceneNode>();
        cylinder_node -> CreateComponent<ShadingComponent>(shader_);
        if (joint_ptr == skeleton_hier.at(1)) {
          cylinder_node -> CreateComponent<RenderingComponent>(trunk_mesh_);
        }
        else {
          cylinder_node -> CreateComponent<RenderingComponent>(branch_mesh_);
        }
        // std::cout << "hmmmmmmmm" << std::endl;
        // cylinder_node -> CreateComponent<RenderingComponent>(trunk_mesh_);
        cylinder_node->CreateComponent<MaterialComponent>(std::make_shared<Material>(Material::GetDefault()));
        Material &material_color = cylinder_node->GetComponentPtr<MaterialComponent>()->GetMaterial();
        material_color.SetAmbientColor(brown_color*(section/(section_num+1.0f)));
        material_color.SetDiffuseColor(brown_color*(section/(section_num+1.0f))); 

        glm::vec3 position = joint_ptr->GetTransform().GetPosition();

        // compute vector between child and parent sphere
        glm::vec3 y_axis(0.f, 1.0f, 0.f);
        // glm::vec3 parent_sphere_position = tree_section.at(section) -> GetTransform().GetPosition();
        // glm::vec3 child_sphere_position = tree_section.at(section+1) -> GetTransform().GetPosition();
        glm::vec3 two_sphere_ray = position; 

        // scale and rotate cylinder along vector between child and parent sphere
        float y_scale = glm::length(two_sphere_ray);                     // distance between two spheres
        glm::vec3 axis = glm::normalize(glm::cross(y_axis, two_sphere_ray));         // cross product between y axis vector and vector between two spheres

        float angle = acos(glm::dot(y_axis, glm::normalize(two_sphere_ray))) + 2.f*acos(0.f);
        cylinder_node -> GetTransform().SetScale(glm::vec3(1.0f, y_scale, 1.0f));
        cylinder_node -> GetTransform().SetRotation(axis, angle);
        // cylinder_node -> GetTransform().SetPosition(position);

        cylinder_child_ptrs_.push_back(cylinder_node.get());
        cylinder_sections.push_back(cylinder_node.get());
        // joint_ptr->AddChild(std::move(cylinder_node));
        tree_section.at(section+1)->AddChild(std::move(cylinder_node));

      }
      cylinder_section_ptrs.push_back(cylinder_sections);
      // std::cout << "okie!" << std::endl;
    } else {
      cylinder_child_ptrs_.push_back(nullptr);
      // std::cout << "0!" << std::endl;
    }

  }
  
  // std::cout << "BUT WHY 122" << std::endl;
}

void SkeletonNode::Update(double delta_time) {
  // Prevent multiple toggle.
  if (InputManager::GetInstance().IsKeyPressed('R')) {
      strong_wind = true;
  } else {
      strong_wind = false;
  }
  
  // if (InputManager::GetInstance().IsKeyPressed('S')) {
  //     OnJointChanged();
  // } 

  dt += delta_time;
  if (dt > 0.05) {
    dt = 0.0;
    UpdateAngles();
    OnJointChanged();
  }

  // OnJointChanged();
  // float time = 0.f; 

  // while (time + step_size_ < delta_time) {
  //     state = integrator->Integrate(ts, state, time, step_size_);
  //     for (int j = 0; j < nodes.size(); j++) {
  //         nodes[j]->GetTransform().SetPosition(state.positions[j]);
  //         if (state.positions[j][1] >= -1.f) {
  //             auto current_quat = nodes[j]->GetTransform().GetRotation();
  //             auto rx = 0.f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/(.01f - (0.f))));
  //             nodes[j]->GetTransform().SetRotation(current_quat * glm::quat(glm::vec3(rx, 0.f, 0.f)));
  //         } else {
  //             nodes[j]->GetTransform().SetRotation(glm::quat(glm::vec3(0.f, 0.f, 0.f)));
  //         }
  //         state.lifetimes[j] -= step_size_;
  //     }  
  //     time += step_size_;
  // } 

  // if (time < delta_time) {
  //     float difference = delta_time - time;
  //     state = integrator->Integrate(ts, state, time, difference);
  //     for (int j = 0; j < nodes.size(); j++) {
  //         nodes[j]->GetTransform().SetPosition(state.positions[j]);
  //         if (state.positions[j][1] >= -1.f) {
  //             auto current_quat = nodes[j]->GetTransform().GetRotation();
  //             auto rx = 0.f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/(.01f - (0.f))));
  //             nodes[j]->GetTransform().SetRotation(current_quat * glm::quat(glm::vec3(rx, 0.f, 0.f)));
  //         } else {
  //             nodes[j]->GetTransform().SetRotation(glm::quat(glm::vec3(0.f, 0.f, 0.f)));
  //         }
  //         state.lifetimes[j] -= step_size_;
  //     }  
  // }
}


void SkeletonNode::UpdateAngles() {
  wind_vel.z += (wind_goal.z - wind_vel.z)/30.0f;
  if (abs(wind_goal.z-wind_vel.z) < 0.008f) {
    if (strong_wind)
      wind_goal.z = 0.08f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/(0.03f)));
    else 
      wind_goal.z = 0.05f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/(0.03f)));
    wind_vel.z += (wind_goal.z - wind_vel.z)/60.0f;
  }


  for (int i = 0; i<skeleton_hier.size(); i++) {
    auto angle = EulerAngle();
    angle.rx = 0.0f;
    angle.ry = 0.0f;
    angle.rz = -wind_vel.z;
    if (i == 0 ) {
      angle.rz = -0.5f * wind_vel.z; // trunk is more stiff
    }
    linked_angles[i] = angle;
    linked_angles_[i] = &angle;
  }
}

void SkeletonNode::OnJointChanged() {
  // this method is called whenever the values of UI sliders change.
  // The new Euler angles (represented as EulerAngle struct) can be retrieved
  // from linked_angles_ (a std::vector of EulerAngle*).
  // The indices of linked_angles_ align with the order of the joints in .skel
  // files. For instance, *linked_angles_[0] corresponds to the first line of
  // the .skel file.

  // std::cout << "BUT WHY 133" << std::endl;
  for (int i = 0; i<linked_angles_.size(); i++) {
    // update joint pointer's rotation transformation 
    // std::cout << "start ? " << i << std::endl;
    
    // EulerAngle* linked_angle = linked_angles_[i];
    EulerAngle linked_ang = linked_angles[i];
    SceneNode* joint_ptr = skeleton_hier[i];
    glm::quat quat_angle(glm::vec3(linked_ang.rx, linked_ang.ry, linked_ang.rz));
    // glm::quat quat_angle(glm::vec3(linked_angle->rx, linked_angle->ry, linked_angle->rz));
    // joint_ptr -> GetTransform().SetRotation(quat_angle);
    
    // std::cout << "errorweofij <<<<<<<<<<<<<<<<<<<<<<<<<<<<?" << std::endl;
    // std::cout << linked_ang.rx << std::endl;
    // std::cout << linked_ang.ry << std::endl;
    // std::cout << linked_ang.rz << std::endl;
    // std::cout << "errorweofij <<<<<<<<<<<<<<<<<<<<<<<<<<<<?" << std::endl;

    // update bone transformations
    if (joint_ptr != skeleton_hier.at(0)) {
      // SceneNode* cylinder_node_ptr = cylinder_child_ptrs_[i];
      
      // if (cylinder_node_ptr!=nullptr) {

        // glm::vec3 position = joint_ptr -> GetTransform().GetPosition();

        // glm::vec3 two_sphere_ray = position;

        // glm::vec3 y_axis(0.f, 1.0f, 0.f);
        // glm::vec3 up = y_axis;
        // glm::vec3 axis = glm::normalize(glm::cross(up, two_sphere_ray));         // cross product between y axis vector and vector between two spheres
        // float angle = acos(glm::dot(up, glm::normalize(two_sphere_ray)))+2*acos(0.0f);

        // glm::quat quat_local_angle(cosf(angle / 2), axis.x * sinf(angle / 2),
        //                 axis.y * sinf(angle / 2), axis.z * sinf(angle / 2));
        // glm::mat3 mat_angle = glm::mat3_cast(quat_angle);
        // glm::mat3 mat_local_angle = glm::mat3_cast(quat_local_angle);
        // glm::mat3 final_mat = glm::transpose(mat_angle) * mat_local_angle;
        // glm::quat final_quat = glm::quat_cast(final_mat);
        // cylinder_node -> GetTransform().SetRotation(final_quat);


        // std::cout << "wief here done?>>>>>>>>>>>>>>>>>>>>>>>>>>>>" << std::endl;
        
        std::vector<SceneNode*> cylinder_sections = cylinder_section_ptrs.at(i-1);
        std::vector<SceneNode*> tree_section = tree_section_hier.at(i-1);
        // std::cout << "sizes: " << i << std::endl;
        // std::cout << tree_section.size()<<std::endl;
        // std::cout << cylinder_sections.size()<<std::endl;
        // std::cout << tree_section_hier.size()<<std::endl;
        // std::cout << cylinder_section_ptrs.size()<<std::endl;
        // std::cout << linked_angles_.size() << std::endl;
        for (int section = 1; section<section_num+1; section++) {
        // for (int section = section_num; section>0; section--) {
          // update all cylinder sections

          auto sphere_node = tree_section.at(section+1);
          sphere_node -> GetTransform().SetRotation(quat_angle);
          auto cylinder_node = cylinder_sections.at(section);
          auto rotation = cylinder_node -> GetTransform().GetRotation();
          // std::cout << quat_angle[0] << std::endl;
          // std::cout << quat_angle[1] << std::endl;
          // std::cout << quat_angle[2] << std::endl;
          // std::cout << quat_angle[3] << std::endl;
          // std::cout << "error here maybe???" << std::endl;

          // glm::vec3 two_sphere_ray = sphere_node->GetTransform().GetPosition();
          glm::vec3 two_sphere_ray = bind_skeleton_positions_.at(i-1).at(section+1);

          // glm::vec3 two_sphere_ray = sphere_node->GetTransform().GetWorldPosition() - tree_section.at(section)->GetTransform().GetWorldPosition();
          // std::cout << two_sphere_ray.x << std::endl;
          // std::cout << two_sphere_ray.y << std::endl;
          // std::cout << two_sphere_ray.z << std::endl;

          // scale and rotate cylinder along vector between child and parent sphere
          glm::vec3 up(0.f, 1.0f, 0.f);
          glm::vec3 axis = glm::normalize(glm::cross(up, two_sphere_ray));         // cross product between y axis vector and vector between two spheres
          float angle = acos(glm::dot(up, glm::normalize(two_sphere_ray)))+2*acos(0.0f);
          // std::cout << "error here no???:: " << angle << std::endl;

          glm::quat quat_local_angle(cosf(angle / 2.0f), axis.x * sinf(angle / 2.0f),
                          axis.y * sinf(angle / 2.0f), axis.z * sinf(angle / 2.0f));
          glm::mat3 mat_angle = glm::mat3_cast(quat_angle);
          glm::mat3 mat_local_angle = glm::mat3_cast(quat_local_angle);
          glm::mat3 final_mat = glm::transpose(mat_angle) * mat_local_angle;
          glm::quat final_quat = glm::quat_cast(final_mat);
          cylinder_node -> GetTransform().SetRotation(final_quat);
          // cylinder_node -> GetTransform().SetRotation(up, 90.0f);
          // std::cout << "error here huh???" << std::endl;
          rotation = cylinder_node -> GetTransform().GetRotation();
          // std::cout << rotation[0] << std::endl;
          // std::cout << rotation[1] << std::endl;
          // std::cout << rotation[2] << std::endl;
          // std::cout << rotation[3] << std::endl;

          // float angle = acos(glm::dot(y_axis, glm::normalize(two_sphere_ray))) + 2.f*acos(0.f);
          // cylinder_node -> GetTransform().SetRotation(axis, angle);
          // cylinder_node -> GetTransform().SetPosition(position);
          // break;
        }
        // break;
      // }
    }
    // std::cout << "error here done? " << i << std::endl;
  }
  // std::cout << "error here actually done?" << std::endl;
}

void SkeletonNode::LinkRotationControl(const std::vector<EulerAngle*>& angles) {
  linked_angles_ = angles;
}

void SkeletonNode::LoadSkeletonFile(const std::string& path) {
  std::fstream s(path, std::ios_base::in);
  if (!s.is_open()) {
    std::cout << "failed to open " << path << '\n';
  } else {
    // // read
    // s.read(reinterpret_cast<char*>(&d), sizeof d); // binary input
    // int n;
    // std::string str;
    // if (s >> n >> str)                             // text input
    //   std::cout << "read back from file: " << d << ' ' << n << ' ' << str << '\n';

    for ( std::string line; std::getline(s, line); ) {
      // parse each line for the position displacement and parent index
      std::istringstream ss(line);
      std::string x_str;
      std::string y_str;
      std::string z_str;
      std::string parent_index_str;

      std::getline(ss, x_str, ' ');
      std::getline(ss, y_str, ' ');
      std::getline(ss, z_str, ' ');
      std::getline(ss, parent_index_str, ' ');

      float x = std::stof(x_str);
      float y = std::stof(y_str);
      float z = std::stof(z_str);
      int parent_index = std::stoi(parent_index_str);

      // std::cout << "start: " << parent_index << std::endl;
      // std::cout << x << ", " << y << ", " << z << std::endl;

      // create a scene node with position transform
      glm::vec3 position(x, y, z);
      auto node = make_unique<SceneNode>();
      node->GetTransform().SetPosition(position); // have to do it one line....???? why 
      position = node->GetTransform().GetPosition();
      // std::cout << position.x << ", " << position.y << ", " << position.z << std::endl;
      
      
      // add node as child to parent and to the hierarchical node tree
      // skeleton_hier.push_back(node.get());
      // skeleton_hier.push_back(make_unique<SceneNode>(node));
      if (parent_index != -1) {
        auto parent_node = skeleton_hier[parent_index];
        // parent_node -> AddChild(std::move(node));
        // std::cout << "BUT huhh nandeeeee " << parent_index << std::endl;
        SceneNode* last_section_node = CreateSections(parent_node, node.get());
        // TODO: change node to glm::vec3
        // std::cout << "mkay...." << std::endl;
        // last_section_node -> AddChild(std::move(node));
        skeleton_hier.push_back(last_section_node);
        node.release();
      } else {
        skeleton_hier.push_back(node.get());
        AddChild(std::move(node));     // need to release....
      }
    }
  }
}

SceneNode* SkeletonNode::CreateSections(SceneNode* parent_node, SceneNode* node) {
  // std::cout << "BUT WHY nandeeeee" << std::endl;
  glm::vec3 parent_position = parent_node->GetTransform().GetPosition();
  // std::cout << "BUT WHY wait huh" << std::endl;
  glm::vec3 node_position = node->GetTransform().GetPosition();
  // std::cout << "BUT WHY asdkfjkds" << std::endl;

  SceneNode* previous_node = parent_node;
  std::vector<SceneNode*> tree_section;
  std::vector<glm::vec3> tree_positions;
  tree_section.push_back(parent_node);
  tree_positions.push_back(parent_node->GetTransform().GetPosition());
  // std::cout << "BUT WHY ?????" << std::endl;
  for (int i = 0 ; i<=section_num; i++) {
    // std::cout << "BUT WHY " << i << std::endl;
    // glm::vec3 new_node_position = (i+1.0f)*node_position + (section_num-i-0.0f)*parent_position;
    // new_node_position = new_node_position / (section_num + 1.0f);
    glm::vec3 new_node_position = node_position / (section_num + 1.0f);

    auto new_node = make_unique<SceneNode>();
    new_node->GetTransform().SetPosition(new_node_position); 
    glm::vec3 position = new_node->GetTransform().GetPosition();
    // std::cout << position.x << ", " << position.y << ", " << position.z << std::endl;

    tree_section.push_back(new_node.get());
    tree_positions.push_back(new_node.get()->GetTransform().GetPosition());
    SceneNode* temp = new_node.get();
    previous_node->AddChild(std::move(new_node));
    previous_node = temp;
  }
  // std::cout << "BUT WHY mehhhh" << std::endl;
  // tree_section.push_back(node);
  // previous_node->AddChild(node);
  bind_skeleton_positions_.push_back(tree_positions);
  tree_section_hier.push_back(tree_section);
  return previous_node;
}

void SkeletonNode::UpdateSections() {
  for(std::vector<SceneNode*> tree_section : tree_section_hier) {
    SceneNode* parent_node = tree_section.at(0);
    SceneNode* node = tree_section.at(section_num + 1);
    glm::vec3 parent_position = parent_node->GetTransform().GetPosition();
    glm::vec3 node_position = node->GetTransform().GetPosition();

    SceneNode* previous_node = parent_node;
    std::vector<SceneNode*> tree_section;
    tree_section.push_back(parent_node);
    for (int i = 0 ; i<section_num; i++) {
      SceneNode* cur_node = tree_section.at(i+1);
      glm::vec3 cur_node_position = (i+1.0f)*node_position + (section_num-i-0.0f)*parent_position;
      cur_node_position = cur_node_position / (section_num + 1.0f);

      cur_node->GetTransform().SetPosition(cur_node_position); 
    }
  }
}

void SkeletonNode::LoadMeshFile(const std::string& filename) {
  std::cout << "start loading mesh" << std::endl;
  std::shared_ptr<VertexObject> vtx_obj =
      MeshLoader::Import(filename).vertex_obj;

  MeshData mesh_data =
      MeshLoader::Import(filename);
  // SetAmbientToDiffuse(mesh_data);

  auto test = mesh_data.groups.size();
  std::cout<<test<<std::endl;
  // material_ = mesh_data.groups.at(0).material;
  // group_node->CreateComponent<MaterialComponent>(material);
  // root.AddChild(std::move(group_node));
      
  std::cout << "tx downsh" << std::endl;

  leaves_mesh_ = vtx_obj;
  // leaves_mesh_ = mesh_data.vertex_obj.get();
  // bind_skeleton_positions_ = leaves_mesh_->GetPositions();

  // compute per face normal
  std::vector<unsigned int> indices = leaves_mesh_->GetIndices();
  std::vector<glm::vec3> positions = leaves_mesh_->GetPositions();
  std::vector<glm::vec3> normal_sums;
  normal_sums.resize(positions.size(), glm::vec3(0.f));
  std::vector<unsigned int> face_num;
  face_num.resize(positions.size(), 0);

  for(int i = 0; i<indices.size(); i = i + 3 ) {
    glm::vec3 v1 = positions[indices[i]];
    glm::vec3 v2 = positions[indices[i+1]];
    glm::vec3 v3 = positions[indices[i+2]];

    glm::vec3 edge1 = glm::normalize(v2-v1);
    glm::vec3 edge2 = glm::normalize(v3-v1);
    glm::vec3 face_normal = glm::cross(edge1, edge2);
    
    // compute per vertex normal 

    normal_sums[indices[i]] += face_normal;
    normal_sums[indices[i+1]] += face_normal;
    normal_sums[indices[i+2]] += face_normal;

    face_num[indices[i]] += 1;
    face_num[indices[i+1]] += 1;
    face_num[indices[i+2]] += 1;
  }
  
  std::cout << "midpoint mesh" << std::endl;
  
  for (int i = 0; i<positions.size(); i++) {
    normal_sums[i] = 1.f / face_num[i] * normal_sums[i];
  }

  
  std::cout << "rendering loading mesh" << std::endl;

  // add to vertex objeect
  leaves_mesh_->UpdateNormals(make_unique<std::vector<glm::vec3>>(normal_sums));
  // auto node = make_unique<SceneNode>();
  // mesh_node = node.get();
  // node -> CreateComponent<RenderingComponent>(leaves_mesh_);
  // std::shared_ptr<ShaderProgram> shader_ = std::make_shared<PhongShader>();
  // node -> CreateComponent<ShadingComponent>(shader_);
  // node -> SetActive(true);
  // AddChild(std::move(node));

  // std::cout << "mesh added!!! yayyyy ~"  << std::endl;
}

// void SkeletonNode::LoadAttachmentWeights(const std::string& path) {
//   // TODO: load attachment weights. test / make sure it works
//   std::fstream s(path, std::ios_base::in);
//   if (!s.is_open()) {
//     std::cout << "failed to open " << path << '\n';
//   } else {
//     for ( std::string line; std::getline(s, line); ) {
//       // parse each line for a row of weights and add rows to attach_weights_
//       std::istringstream ss(line);
//       std::vector<float> weight_row;
//       for (std::string weight; std::getline(ss, weight, ' '); ) {
//         weight_row.push_back(std::stof(weight));
//       }
//       attach_weights_.push_back(weight_row);
//     }
//   }
// }

void SkeletonNode::LoadAllFiles(const std::string& prefix) {
  std::string prefix_full = GetAssetDir() + prefix;
  LoadSkeletonFile(prefix_full + ".skel");
  LoadMeshFile("Cornus alba Baton Rouge.obj");
  // LoadAttachmentWeights(prefix_full + ".attach");
}
}  // namespace GLOO
