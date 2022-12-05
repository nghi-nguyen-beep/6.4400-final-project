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
#include "gloo/shaders/SimpleShader.hpp"
#include "PendulumSystem.hpp"
#include "IntegratorBase.hpp"
#include "IntegratorFactory.hpp"
#include "IntegratorType.hpp"
#include "ClothNode.hpp"


namespace GLOO {

ClothNode::ClothNode(IntegratorType type, float step_size) {
    step_size_ = step_size;
    cloth_size = 8; // NEEDS TO BE > 1
    shader_ = std::make_shared<PhongShader>();
    polyline_shader_ = std::make_shared<SimpleShader>();
    sphere_mesh_ = PrimitiveFactory::CreateSphere(0.02f, 25, 25);
    integrator = IntegratorFactory::CreateIntegrator<PendulumSystem, ParticleState>(type);
    line_ = std::make_shared<VertexObject>();
    rl = .4f;

    for (int i = 0; i < cloth_size; i++) {
        for (int j = 0; j < cloth_size; j++) {
            glm::vec3 pos(rl * i, rl * j, 0.f);
            if ((j == cloth_size - 1 && i == 0) || (j == cloth_size - 1 && i == cloth_size - 1)){
                ps.AddStationaryFlag(true);
            } else {
                ps.AddStationaryFlag(false);
            }
            state.positions.push_back(pos);
            original_positions.push_back(pos);
            state.velocities.push_back(glm::vec3(0.f));
            ps.AddMass(.05f);
        }
    }

    AddStructural();
    AddShear();
    AddFlex();

    PlotCloth();
    DrawCloth();
}

void ClothNode::DrawCloth(){
    auto line_node = make_unique<SceneNode>();
    line_node->CreateComponent<RenderingComponent>(line_).SetDrawMode(DrawMode::Lines);
    line_node->CreateComponent<ShadingComponent>(polyline_shader_);
    nodes.push_back(line_node.get());
    AddChild(std::move(line_node));
}

void ClothNode::PlotCloth(){
    std::unique_ptr<PositionArray> positions = make_unique<PositionArray>();
    std::unique_ptr<IndexArray> indices = make_unique<IndexArray>();

    for (int i = 0; i < cloth_size; i++) {
        for (int j = 0; j < cloth_size; j++) {
            glm::vec3 pos(0.5f * i, 0.5f * j, 0.f);
            positions->push_back(pos);
        }
    }

    for (int i = 0; i < cloth_size; i++) { // one less than net size
        for (int j = 0; j < cloth_size; j++) {
            // horizontal
            if (j < cloth_size-1) {
                indices->push_back(i*cloth_size + j);
                indices->push_back(i*cloth_size + (j+1));
            }

            // vertical
            if (i < cloth_size-1) {
                indices->push_back(i*cloth_size + j);
                indices->push_back((i+1)*cloth_size + j);
            }
        }
    }

    line_->UpdatePositions(std::move(positions));
    line_->UpdateIndices(std::move(indices));
}

void ClothNode::AddStructural() {
    for (int i = 0; i < cloth_size; i++) {
        for (int j = 0; j < cloth_size; j++) {
            std::vector<int> spring;
            std::vector<float> rest_length;
            std::vector<float> k;
            if (j - 1 >= 0) { // left
                spring.push_back(i * cloth_size + (j-1));
                rest_length.push_back(rl);
                k.push_back(5.f);
            }

            if (j + 1 < cloth_size) { // right
                spring.push_back(i * cloth_size + (j+1));
                rest_length.push_back(rl);
                k.push_back(5.f);
            }

            if (i + 1 < cloth_size) { // bottom
                spring.push_back((i+1) * cloth_size + j);
                rest_length.push_back(rl);
                k.push_back(5.f);
            }

            if (i - 1 >= 0) { // top
                spring.push_back((i-1) * cloth_size + j);
                rest_length.push_back(rl);
                k.push_back(5.f);
            }
            ps.AddSpring(spring);
            ps.AddK(k);
            ps.AddRestLength(rest_length);
        }
    }
}

void ClothNode::AddShear() {
    for (int i = 0; i < cloth_size; i++) {
        for (int j = 0; j < cloth_size; j++) {
            std::vector<int> spring;
            std::vector<float> rest_length;
            std::vector<float> k;
            if (i - 1 >= 0 && j - 1 >= 0) { // top left
                spring.push_back((i-1) * cloth_size + (j-1));
                rest_length.push_back(sqrt(pow(rl, 2) + pow(rl, 2)));
                k.push_back(4.f);
            }

            if (i - 1 >= 0 && j + 1 < cloth_size) { // top right
                spring.push_back((i-1) * cloth_size + (j+1));
                rest_length.push_back(sqrt(pow(rl, 2) + pow(rl, 2)));
                k.push_back(4.f);
            }

            if (i + 1 < cloth_size && j - 1 >= 0) { // bottom left
                spring.push_back((i+1) * cloth_size + (j-1));
                rest_length.push_back(sqrt(pow(rl, 2) + pow(rl, 2)));
                k.push_back(4.f);
            }

            if (i + 1 < cloth_size && j + 1 < cloth_size) { // bottom right
                spring.push_back((i+1) * cloth_size + (j+1));
                rest_length.push_back(sqrt(pow(rl, 2) + pow(rl, 2)));
                k.push_back(4.f);
            }

            int index = i * cloth_size + j;
            ps.AddSpringByIndex(index, spring);
            ps.AddKByIndex(index, k);
            ps.AddRestLengthByIndex(index, rest_length);
        }
    }
}

void ClothNode::AddFlex() {
    for (int i = 0; i < cloth_size; i++) {
        for (int j = 0; j < cloth_size; j++) {
            std::vector<int> spring;
            std::vector<float> rest_length;
            std::vector<float> k;
            int half_cloth = ceil(cloth_size / 2); // 5 / 2 = 3
            
            if (i < half_cloth && j < half_cloth) { // bottom left
                spring.push_back((i+2) * cloth_size + j);
                spring.push_back((i)*cloth_size + (j+2));
                rest_length.push_back(rl * 2);
                rest_length.push_back(rl * 2);
                k.push_back(4.f);
                k.push_back(4.f);
            }

            if (i < half_cloth && j >= half_cloth) { // top left
                spring.push_back((i)*cloth_size + (j-2));
                spring.push_back((i+2)*cloth_size + (j));
                rest_length.push_back(rl * 2);
                rest_length.push_back(rl * 2);
                k.push_back(4.f);
                k.push_back(4.f);
            }

            if (i >= half_cloth && j < half_cloth) { // bottom right
                spring.push_back((i-2)*cloth_size + j);
                spring.push_back((i)*cloth_size + (j + 2));
                rest_length.push_back(rl * 2);
                rest_length.push_back(rl * 2);
                k.push_back(4.f);
                k.push_back(4.f);
            }

            if (i >= half_cloth && j >= half_cloth) { // top right
                spring.push_back((i - 2)*cloth_size + j);
                spring.push_back((i)*cloth_size + (j - 2));
                rest_length.push_back(rl * 2);
                rest_length.push_back(rl * 2);
                k.push_back(2.f);
                k.push_back(2.f);
            }
            
            int index = i * cloth_size + j;
            ps.AddSpringByIndex(index, spring);
            ps.AddKByIndex(index, k);
            ps.AddRestLengthByIndex(index, rest_length);
        }
    }
}

void ClothNode::ResetNet() {
    std::unique_ptr<PositionArray> positions = make_unique<PositionArray>();
    for (int i = 0; i < original_positions.size(); i++) {
        positions->push_back(original_positions[i]);
    }
    state.positions = original_positions;
    state.velocities = std::vector<glm::vec3>(original_positions.size(), glm::vec3(0.f));
    line_->UpdatePositions(std::move(positions)); 
}

void ClothNode::Update(double delta_time) {

    static bool prev_released = true;
    if (InputManager::GetInstance().IsKeyPressed('R')) {
        if (prev_released) {
            ResetNet();
        }
    }

    float time = 0.f; 

    while (time + step_size_ < delta_time) {
        state = integrator->Integrate(ps, state, time, step_size_);
        std::unique_ptr<PositionArray> positions = make_unique<PositionArray>();
        for (int i = 0; i < state.positions.size(); i++) {
            positions->push_back(state.positions[i]);
        }
        line_->UpdatePositions(std::move(positions));   
        time += step_size_;
    } 

    if (time < delta_time) {
        float difference = delta_time - time;
        state = integrator->Integrate(ps, state, time, difference);
        std::unique_ptr<PositionArray> positions = make_unique<PositionArray>();
        for (int i = 0; i < state.positions.size(); i++) {
            positions->push_back(state.positions[i]);
        }
        line_->UpdatePositions(std::move(positions));    
    }
}
}