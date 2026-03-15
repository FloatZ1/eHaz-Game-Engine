#ifndef EHAZ_SCRIPTING_PHYSICS_BINDINGS_HPP
#define EHAZ_SCRIPTING_PHYSICS_BINDINGS_HPP

#include "Components.hpp"
#include "Physics/Physics.hpp"
#include "sol/sol.hpp"
namespace eHaz {

inline void RegisterPhysicsSystemBindings(sol::state &p_ssLua) {

  sol::table l_stPhysicsTable = p_ssLua.create_named_table("Physics");

  l_stPhysicsTable.set_function(
      "AddForce", [&](RigidBodyComponent &p_rbComponent, glm::vec3 &p_v3Force) {
        PhysicsEngine::s_Instance->AddForce(p_rbComponent.m_jbidBodyID,
                                            p_v3Force);
      });

  l_stPhysicsTable.set_function(
      "AddImpulse",
      [&](RigidBodyComponent &p_rbComponent, glm::vec3 &p_v3Impulse) {
        PhysicsEngine::s_Instance->AddImpulse(p_rbComponent.m_jbidBodyID,
                                              p_v3Impulse);
      });

  l_stPhysicsTable.set_function("SetGravity", [&](glm::vec3 &p_v3Gravity) {
    PhysicsEngine::s_Instance->SetGravity(p_v3Gravity);
  });
}

} // namespace eHaz

#endif
