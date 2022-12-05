#ifndef PENDULUM_NODE_H_
#define PENDULUM_NODE_H_
#include "gloo/SceneNode.hpp"
#include "ParticleState.hpp"
#include "ParticleSystemBase.hpp"
#include "gloo/VertexObject.hpp"
#include "gloo/shaders/ShaderProgram.hpp"
#include "IntegratorBase.hpp"
#include "IntegratorFactory.hpp"
#include "ForwardEulerIntegrator.hpp"
#include "PendulumSystem.hpp"

namespace GLOO {
class PendulumNode : public SceneNode {
 public:
    PendulumNode(IntegratorType type, float step_size);
    void Update(double delta_time) override;

 private:
   void ResetPendulum();
   float step_size_;
   std::vector<glm::vec3> original_positions;
   std::shared_ptr<VertexObject> sphere_mesh_;
   std::shared_ptr<ShaderProgram> shader_;
   PendulumSystem ps;
   std::vector<SceneNode*> nodes;
   std::vector<float> masses;
   ParticleState state;
   std::unique_ptr<IntegratorBase<PendulumSystem, ParticleState>> integrator;
};
}  // namespace GLOO

#endif
