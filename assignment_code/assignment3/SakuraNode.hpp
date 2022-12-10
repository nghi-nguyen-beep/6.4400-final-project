#ifndef SAKURA_NODE_H_
#define SAKURA_NODE_H_
#include "gloo/SceneNode.hpp"
#include "ParticleState.hpp"
#include "ParticleSystemBase.hpp"
#include "gloo/VertexObject.hpp"
#include "gloo/shaders/ShaderProgram.hpp"
#include "IntegratorBase.hpp"
#include "IntegratorFactory.hpp"
#include "ForwardEulerIntegrator.hpp"
#include "SakuraSystem.hpp"

namespace GLOO {
class SakuraNode : public SceneNode {
 public:
    SakuraNode(IntegratorType type, float step_size);
    void Update(double delta_time) override;

 private:
   void ResetSphere();
   void AddPetal();
   void LoadMeshFile(const std::string& filename);
   float step_size_;
   std::vector<glm::vec3> original_positions;
   ParticleState state;
   std::shared_ptr<VertexObject> sphere_mesh_;
   std::shared_ptr<ShaderProgram> shader_;
   SakuraSystem sakura;
   std::vector<SceneNode*> nodes;
   std::unique_ptr<IntegratorBase<SakuraSystem, ParticleState>> integrator;
   std::shared_ptr<VertexObject> sakura_mesh_;

};
}  // namespace GLOO

#endif
