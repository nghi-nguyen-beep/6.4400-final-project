#include "ParticleSystemBase.hpp"
#include "ParticleState.hpp"
#include "PendulumSystem.hpp"
#include "gloo/SceneNode.hpp"
#include "gloo/VertexObject.hpp"


namespace GLOO {

ParticleState PendulumSystem::ComputeTimeDerivative(const ParticleState& state, float time) const {
    ParticleState delta;
    for (int i = 0; i < state.positions.size(); i++) {
        if (stationary[i]) {
            delta.positions.push_back(glm::vec3(0.f));
            delta.velocities.push_back(glm::vec3(0.f));
        } else {
            glm::vec3 gravity(0.f, -masses[i] * kGravity, 0.f);

            glm::vec3 drag_force(state.velocities[i] * -k);

            glm::vec3 spring_force(0.f);

            for (int j = 0; j < springs[i].size(); j++) {

                glm::vec3 d(state.positions[i] - state.positions[springs[i][j]]);
                float stretch = glm::length(d) - rest_length[i][j];
                spring_force += (-spring_k[i][j] * stretch) * glm::normalize(d);
            }
            glm::vec3 dx(state.velocities[i]);
            glm::vec3 dv((gravity + drag_force + spring_force) / masses[i]); // sum of all forces
            delta.positions.push_back(dx);
            delta.velocities.push_back(dv);
        }
    }
    return delta;
}

void PendulumSystem::AddMass(float m) {
    masses.push_back(m);
}

void PendulumSystem::AddStationaryFlag(bool b) {
    stationary.push_back(b);
}

void PendulumSystem::AddSpring(std::vector<int> s) {
    springs.push_back(s);
}

void PendulumSystem::AddK(std::vector<float> s) {
    spring_k.push_back(s);
}

void PendulumSystem::AddRestLength(std::vector<float> s) {
    rest_length.push_back(s);
}

// ASSUMES CORRESPONDING VECTORS HAVE ALREADY BEEN INITIALIZED
void PendulumSystem::AddSpringByIndex(int i, std::vector<int> s) {
    for (int j = 0; j < s.size(); j++) {
        springs[i].push_back(s[j]);
    }
}

void PendulumSystem::AddKByIndex(int i, std::vector<float> s) {
    for (int j = 0; j < s.size(); j++) {
        spring_k[i].push_back(s[j]);
    }
}

void PendulumSystem::AddRestLengthByIndex(int i, std::vector<float> s) {
    for (int j = 0; j < s.size(); j++) {
        rest_length[i].push_back(s[j]);
    }
}
}