#include "ParticleSystemBase.hpp"
#include "ParticleState.hpp"
#include <fstream>
#include "gloo/utils.hpp"
#include "gloo/InputManager.hpp"
#include "gloo/MeshLoader.hpp"
#include "gloo/Renderer.hpp"
#include "gloo/debug/PrimitiveFactory.hpp"
#include "gloo/components/RenderingComponent.hpp"
#include "gloo/components/ShadingComponent.hpp"
#include "gloo/components/MaterialComponent.hpp"
#include "gloo/shaders/PhongShader.hpp"
#include "SakuraSystem.hpp"
#include "IntegratorBase.hpp"
#include "IntegratorFactory.hpp"
#include "IntegratorType.hpp"
#include "ForwardEulerIntegrator.hpp"
#include "SakuraNode.hpp"


namespace GLOO {

SakuraNode::SakuraNode(IntegratorType type, float step_size) {
    step_size_ = step_size;
    shader_ = std::make_shared<PhongShader>();
    sphere_mesh_ = PrimitiveFactory::CreateSphere(0.1f, 25, 25);
    integrator = IntegratorFactory::CreateIntegrator<SakuraSystem, ParticleState>(type);
    LoadMeshFile("Flower.obj");
    AddPetal();
}

void SakuraNode::AddPetal() {
    
    auto node = make_unique<SceneNode>();
    node->CreateComponent<ShadingComponent>(shader_);
    node->CreateComponent<RenderingComponent>(sakura_mesh_);
    node->CreateComponent<MaterialComponent>(std::make_shared<Material>(Material::GetDefault()));
    
    Material &material_color = node->GetComponentPtr<MaterialComponent>()->GetMaterial();
    auto color = glm::vec3(0.95f, 0.69f, 0.79f);
    material_color.SetAmbientColor(color);
    material_color.SetDiffuseColor(color); 

    float px = .4f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/(.7f - (.4f))));
    float py = .4f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/(.7f - (.4f))));
    float pz = -1.f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/(1.f - (-1.f))));

    float vx = static_cast <float> (rand()) / static_cast <float> (RAND_MAX / 0.5f);
    float vy = static_cast <float> (rand()) / static_cast <float> (RAND_MAX / 0.5f);
    float vz = static_cast <float> (rand()) / static_cast <float> (RAND_MAX / 0.5f);

    float lifetime = 4.f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/(10.f - (4.f))));

    float rx = 0.f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/(90.f - (0.f))));
    float ry = 0.f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/(90.f - (0.f))));
    float rz = 0.f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/(90.f - (0.f))));

    float random_size = .05f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/(.1f - (.05f))));
    float random_weight = .05f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/(.15f - (.05f))));

    sakura.AddMass(random_weight);
    node->GetTransform().SetScale(glm::vec3(random_size));
    node->GetTransform().SetRotation(glm::quat(glm::vec3(rx, ry, rz)));
    node->GetTransform().SetPosition(glm::vec3(px, py, pz));

    ////////// ADDING PETAL TO ALL SYSTEMS //////////////////////
    state.velocities.push_back(glm::vec3(vx, vy, vz));
    state.positions.push_back(glm::vec3(px, py, pz));
    state.lifetimes.push_back(lifetime);
    nodes.push_back(node.get());
    AddChild(std::move(node));
    ////////////////////////////////////////////////////////////
}

void SakuraNode::LoadMeshFile(const std::string& filename) {
  std::shared_ptr<VertexObject> vtx_obj =
      MeshLoader::Import(filename).vertex_obj;

  sakura_mesh_ = vtx_obj;
  std::vector<unsigned int> indices = sakura_mesh_->GetIndices();
  std::vector<glm::vec3> positions = sakura_mesh_->GetPositions();
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
    
    normal_sums[indices[i]] += face_normal;
    normal_sums[indices[i+1]] += face_normal;
    normal_sums[indices[i+2]] += face_normal;

    face_num[indices[i]] += 1;
    face_num[indices[i+1]] += 1;
    face_num[indices[i+2]] += 1;
  }
    
  for (int i = 0; i < positions.size(); i++) {
    normal_sums[i] = 1.f / face_num[i] * normal_sums[i];
  }
  sakura_mesh_->UpdateNormals(make_unique<std::vector<glm::vec3>>(normal_sums));
}

void SakuraNode::Update(double delta_time) {

    static bool prev_released = true;
    if (InputManager::GetInstance().IsKeyPressed('R')) {
        if (prev_released) {
            AddPetal();
        }
    }

    float time = 0.f; 

    while (time + step_size_ < delta_time) {
        state = integrator->Integrate(sakura, state, time, step_size_);
        for (int j = 0; j < nodes.size(); j++) {
            nodes[j]->GetTransform().SetPosition(state.positions[j]);
            auto current_quat = nodes[j]->GetTransform().GetRotation();
            auto rx = 0.f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/(.1f - (0.f))));
            auto ry = 0.f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/(.03f - (0.f))));
            auto rz = .0f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/(.2f - (.0f))));
            nodes[j]->GetTransform().SetRotation(current_quat * glm::quat(glm::vec3(rx, ry, 0.f)));

            state.lifetimes[j] -= step_size_;
        }  
        time += step_size_;
    } 

    if (time < delta_time) {
        float difference = delta_time - time;
        state = integrator->Integrate(sakura, state, time, difference);
        for (int j = 0; j < nodes.size(); j++) {
            nodes[j]->GetTransform().SetPosition(state.positions[j]);
            auto current_quat = nodes[j]->GetTransform().GetRotation();
            auto rx = 0.f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/(.1f - (0.f))));
            auto ry = 0.f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/(.03f - (0.f))));
            auto rz = .0f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/(.2f - (.0f))));
            nodes[j]->GetTransform().SetRotation(current_quat * glm::quat(glm::vec3(rx, ry, 0.f)));

            state.lifetimes[j] -= step_size_;
        }  
    }

    std::vector<int> index_of_dead;
    for (int j = 0; j < nodes.size(); j++) {
        if (state.lifetimes[j] < 0.f) {
            index_of_dead.push_back(j);
        }
    } 

    // removing expired particles
    for (int j = 0; j < index_of_dead.size(); j++) {
        assert(state.lifetimes.size() == state.positions.size() && 
            state.lifetimes.size() == state.velocities.size() && 
            state.lifetimes.size() == nodes.size() && 
            state.lifetimes.size() == nodes.size() &&
            state.lifetimes.size() == this->GetChildrenCount());
        
        auto size = state.lifetimes.size();
        auto ind = index_of_dead[j];
        if (ind < size) { // idk why this case happens 
            state.velocities.erase(state.velocities.begin() + ind); 
            state.positions.erase(state.positions.begin() + ind);
            state.lifetimes.erase(state.lifetimes.begin() + ind);
            nodes.erase(nodes.begin() + ind);
            RemoveChild(ind); // seems really hacky lol
        }

 
}
}
}