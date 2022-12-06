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
    AddPetal();
}

void SakuraNode::AddPetal() {
    
    auto node = make_unique<SceneNode>();
    node->CreateComponent<ShadingComponent>(shader_);
    node->CreateComponent<RenderingComponent>(sphere_mesh_);
    node->CreateComponent<MaterialComponent>(std::make_shared<Material>(Material::GetDefault()));

    Material &material_color = node->GetComponentPtr<MaterialComponent>()->GetMaterial();
    auto color = glm::vec3(0.95f, 0.69f, 0.79f);
    material_color.SetAmbientColor(color);
    material_color.SetDiffuseColor(color); 

    sakura.AddMass(0.1f);
    auto px = -1.f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/(1.f - (-1.f))));
    auto py = -1.f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/(1.f - (-1.f))));
    auto pz = -1.f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/(1.f - (-1.f))));

    auto vx = static_cast <float> (rand()) / static_cast <float> (RAND_MAX / 0.5f);
    auto vy = static_cast <float> (rand()) / static_cast <float> (RAND_MAX / 0.5f);
    auto vz = static_cast <float> (rand()) / static_cast <float> (RAND_MAX / 0.5f);

    auto lifetime = 2.f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/(6.f - (2.f))));
    state.velocities.push_back(glm::vec3(vx, vy, vz));
    state.positions.push_back(glm::vec3(px, py, pz)); // position between -1 and 1
    
    // TO DO 
    state.lifetimes.push_back(lifetime);
    node->GetTransform().SetPosition(glm::vec3(px, py, pz));
  
    nodes.push_back(node.get());
    AddChild(std::move(node));
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
        // std::cout << "node " << (nodes.size()) << std::endl;
        // std::cout << "lifetime " << (state.lifetimes.size()) << std::endl;
        // std::cout << "pos " << (state.positions.size()) << std::endl;
        // std::cout << "vel " << (state.velocities.size()) << std::endl;

        for (int j = 0; j < nodes.size(); j++) {
            nodes[j]->GetTransform().SetPosition(state.positions[j]);
            state.lifetimes[j] -= step_size_;
        }  
        time += step_size_;
    } 

    if (time < delta_time) {
        float difference = delta_time - time;
        state = integrator->Integrate(sakura, state, time, difference);
        for (int j = 0; j < nodes.size(); j++) {
            nodes[j]->GetTransform().SetPosition(state.positions[j]);
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
        nodes.erase(nodes.begin() + index_of_dead[j]);
        RemoveChild(index_of_dead[j]);
        state.velocities.erase(state.velocities.begin() + index_of_dead[j]);
        state.positions.erase(state.positions.begin() + index_of_dead[j]);
        state.lifetimes.erase(state.lifetimes.begin() + index_of_dead[j]);
    } 


}
}