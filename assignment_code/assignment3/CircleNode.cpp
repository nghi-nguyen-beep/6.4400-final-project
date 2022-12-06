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
#include "SimpleSystem.hpp"
#include "IntegratorBase.hpp"
#include "IntegratorFactory.hpp"
#include "IntegratorType.hpp"
#include "ForwardEulerIntegrator.hpp"
#include "CircleNode.hpp"


namespace GLOO {

CircleNode::CircleNode(IntegratorType type, float step_size) {
    step_size_ = step_size;
    shader_ = std::make_shared<PhongShader>();
    sphere_mesh_ = PrimitiveFactory::CreateSphere(0.1f, 25, 25);
    integrator = IntegratorFactory::CreateIntegrator<SimpleSystem, ParticleState>(type);
}

void CircleNode::ResetSphere() {
    nodes[0]->GetTransform().SetPosition(original_positions[0]);
    state.positions = original_positions;
    state.velocities = std::vector<glm::vec3>(original_positions.size(), glm::vec3(0.f));
}

void CircleNode::Update(double delta_time) {

    static bool prev_released = true;
    if (InputManager::GetInstance().IsKeyPressed('R')) {
        if (prev_released) {
            ResetSphere();
        }
    }

    float time = 0.f; 

    while (time + step_size_ < delta_time) {
        state = integrator->Integrate(ss, state, time, step_size_);
        nodes[0]->GetTransform().SetPosition(state.positions[0]);
        time += step_size_;
    } 

    if (time < delta_time) {
        float difference = delta_time - time;
        state = integrator->Integrate(ss, state, time, difference);
        nodes[0]->GetTransform().SetPosition(state.positions[0]);  
    }
}
}