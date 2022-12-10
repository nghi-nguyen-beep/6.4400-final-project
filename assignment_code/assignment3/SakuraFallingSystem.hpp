#ifndef SAKURA_FALLING_SYSTEM_H_
#define SAKURA_FALLING_SYSTEM_H_

#include "ParticleState.hpp"
#include "ParticleSystemBase.hpp"
#include "gloo/SceneNode.hpp"
#include "gloo/VertexObject.hpp"

namespace GLOO {
class SakuraFallingSystem : public ParticleSystemBase {
 public:
   ParticleState ComputeTimeDerivative(const ParticleState& state, float time) const override;
   void AddMass(float m);
   void AddWind(bool b);
 private:
    float k = 0.03f;
    std::vector<float> masses;
    bool wind = false;

};
}  // namespace GLOO

#endif
