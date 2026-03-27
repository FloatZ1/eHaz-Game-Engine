#ifndef EHAZ_SCRIPTING_PHYSICS_BINDINGS_HPP
#define EHAZ_SCRIPTING_PHYSICS_BINDINGS_HPP

#include "Components.hpp"
#include "Physics/Physics.hpp"
#include "sol/sol.hpp"
#include <SDL3/SDL_log.h>
namespace eHaz {

inline void RegisterPhysicsSystemBindings(sol::state &p_ssLua) {

  sol::table l_stPhysicsTable = p_ssLua.create_named_table("Physics");

  l_stPhysicsTable.set_function(
      "AddForce", [](RigidBodyComponent &p_rbComponent, glm::vec3 &p_v3Force) {
        PhysicsEngine::s_Instance->AddForce(p_rbComponent.m_jbidBodyID,
                                            p_v3Force);

        SDL_Log("Add Force called from script");
      });

  l_stPhysicsTable.set_function(
      "AddImpulse",
      [](RigidBodyComponent &p_rbComponent, glm::vec3 &p_v3Impulse) {
        PhysicsEngine::s_Instance->AddImpulse(p_rbComponent.m_jbidBodyID,
                                              p_v3Impulse);
        SDL_Log("Add Impulse called from script");
      });

  l_stPhysicsTable.set_function("SetGravity", [](glm::vec3 &p_v3Gravity) {
    PhysicsEngine::s_Instance->SetGravity(p_v3Gravity);

    SDL_Log("Set Gravity called from script");
  });
}

} // namespace eHaz

#endif
