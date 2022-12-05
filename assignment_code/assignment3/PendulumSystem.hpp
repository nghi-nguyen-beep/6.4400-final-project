#ifndef PENDULUM_SYSTEM_H_
#define PENDULUM_SYSTEM_H_

#include "ParticleState.hpp"
#include "ParticleSystemBase.hpp"
#include "gloo/SceneNode.hpp"
#include "gloo/VertexObject.hpp"


namespace GLOO {
class PendulumSystem : public ParticleSystemBase {
public:
   void AddMass(float m);
   void AddSpring(std::vector<int> s);
   void AddK(std::vector<float> k);
   void AddRestLength(std::vector<float> r);
   void AddStationaryFlag(bool b);
   void AddSpringByIndex(int i, std::vector<int> s);
   void AddKByIndex(int i, std::vector<float> s);
   void AddRestLengthByIndex(int i, std::vector<float> s);
   ParticleState ComputeTimeDerivative(const ParticleState& state, float time) const override;
 
private:
    std::vector<float> masses;
    std::vector<std::vector<int>> springs;
    float k = 0.03f;
    std::vector<std::vector<float>> spring_k;
    std::vector<std::vector<float>> rest_length;
    std::vector<bool> stationary;
    std::vector<glm::vec3> spring_forces;

};
}  // namespace GLOO

#endif
