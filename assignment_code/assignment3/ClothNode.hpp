#ifndef CLOTH_NODE_H_
#define CLOTH_NODE_H_
#include "gloo/SceneNode.hpp"
#include "ParticleState.hpp"
#include "ParticleSystemBase.hpp"
#include "gloo/VertexObject.hpp"
#include "gloo/shaders/ShaderProgram.hpp"
#include "IntegratorBase.hpp"
#include "IntegratorFactory.hpp"
#include "PendulumSystem.hpp"

namespace GLOO {
class ClothNode : public SceneNode {
 public:
    ClothNode(IntegratorType type, float step_size);
    void Update(double delta_time) override;

 private:
   void AddStructural();
   void AddShear();
   void AddFlex();
   void PlotCloth();
   void DrawCloth();
   void ResetNet();
   float step_size_;
   int cloth_size;
   float rl;
   std::shared_ptr<VertexObject> sphere_mesh_;
   std::shared_ptr<ShaderProgram> shader_;
   std::shared_ptr<ShaderProgram> polyline_shader_;
   PendulumSystem ps;
   std::vector<SceneNode*> nodes;
   std::shared_ptr<VertexObject> line_;
   std::vector<glm::vec3> original_positions;
   ParticleState state;
   std::unique_ptr<IntegratorBase<PendulumSystem, ParticleState>> integrator;
};
}  // namespace GLOO

#endif
