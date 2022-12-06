#ifndef TRUNK_NODE_H_
#define TRUNK_NODE_H_
#include "gloo/SceneNode.hpp"

#include "ParticleState.hpp"
#include "ParticleSystemBase.hpp"
#include "gloo/VertexObject.hpp"
#include "gloo/shaders/ShaderProgram.hpp"
#include "IntegratorBase.hpp"
#include "IntegratorFactory.hpp"
#include "ForwardEulerIntegrator.hpp"
#include "TreeSystem.hpp"

namespace GLOO {
class TrunkNode : public SceneNode {
 public:
    TrunkNode(IntegratorType type, float step_size);
    void Update(double delta_time) override;

 private:
   void ResetSphere();
   float step_size_;
   std::vector<glm::vec3> original_positions;
   ParticleState state;
   std::shared_ptr<VertexObject> sphere_mesh_;
   std::shared_ptr<ShaderProgram> shader_;
   TreeSystem ss;
   std::vector<SceneNode*> nodes;
   std::unique_ptr<IntegratorBase<TreeSystem, ParticleState>> integrator;
};
}  // namespace GLOO

#endif
