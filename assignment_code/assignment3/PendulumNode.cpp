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
#include "PendulumSystem.hpp"
#include "IntegratorBase.hpp"
#include "IntegratorFactory.hpp"
#include "IntegratorType.hpp"
#include "PendulumNode.hpp"


namespace GLOO {

PendulumNode::PendulumNode(IntegratorType type, float step_size) {
    step_size_ = step_size;
    shader_ = std::make_shared<PhongShader>();
    sphere_mesh_ = PrimitiveFactory::CreateSphere(0.06f, 25, 25);
    integrator = IntegratorFactory::CreateIntegrator<PendulumSystem, ParticleState>(type);
    int num_particles = 5;
    float ypos = 1.f;

    for (int i = 0; i < num_particles; i++) {
        auto node = make_unique<SceneNode>();
        state.positions.push_back(glm::vec3(-1.f, ypos, 0.f));
        state.velocities.push_back(glm::vec3(0.f));
        if (i == 0) {
            ps.AddStationaryFlag(true);
        } else{
            ps.AddStationaryFlag(false);
        }
        ps.AddMass(.1f);
        std::vector<int> spring;
        std::vector<float> rest_length;
        std::vector<float> k;
        if (i == 0) {
            spring.push_back(i + 1);
            rest_length.push_back(.3f);
            k.push_back(5.f);
        } else if (i > 0 && i < num_particles - 1) {
            spring.push_back(i - 1);
            spring.push_back(i + 1);
            rest_length.push_back(.3f) ;
            rest_length.push_back(.3f);
            k.push_back(5.f);
            k.push_back(5.f);

        } else {
            spring.push_back(i - 1);
            rest_length.push_back(.3f);
            k.push_back(5.f);
        }

        ps.AddSpring(spring);
        ps.AddK(k);
        ps.AddRestLength(rest_length);
        glm::vec3 pos(-1.f, ypos, 0.f);
        node->GetTransform().SetPosition(pos);
        original_positions.push_back(pos);
        node->CreateComponent<ShadingComponent>(shader_);
        node->CreateComponent<RenderingComponent>(sphere_mesh_);
        nodes.push_back(node.get());
        AddChild(std::move(node));
        ypos -= .2f ;
    }
}

void PendulumNode::ResetPendulum() {
    for (int i = 0; i < original_positions.size(); i++) {
        nodes[i]->GetTransform().SetPosition(original_positions[i]);
    }
    state.positions = original_positions;
    state.velocities = std::vector<glm::vec3>(original_positions.size(), glm::vec3(0.f));

}

void PendulumNode::Update(double delta_time) {
    
    static bool prev_released = true;
    if (InputManager::GetInstance().IsKeyPressed('R')) {
        if (prev_released) {
            ResetPendulum();
        }
    } 

    float time = 0.f; 

    while (time + step_size_ < delta_time) {
        state = integrator->Integrate(ps, state, time, step_size_);
        for (int j = 0; j < nodes.size(); j++) {
            nodes[j]->GetTransform().SetPosition(state.positions[j]);
        }  
        time += step_size_;
    } 

    if (time < delta_time) {
        float difference = delta_time - time;
        state = integrator->Integrate(ps, state, time, difference);
        for (int j = 0; j < nodes.size(); j++) {
            nodes[j]->GetTransform().SetPosition(state.positions[j]);
        }  
    }

}
}