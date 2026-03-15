#ifndef EHAZ_INPUSYSTEM_BINDINGS_HPP
#define EHAZ_INPUSYSTEM_BINDINGS_HPP

#include "Core/Input/InputSystem.hpp"
#include "Core/Input/MouseCodes.hpp"
#include "sol/sol.hpp"
namespace eHaz {

inline void RegisterInputSystemBindings(sol::state &p_ssLua) {

  sol::table l_stInputTable = p_ssLua.create_named_table("Input");

  l_stInputTable.set_function("GetKeyDown", [](uint32_t Key) {
    return InputSystem::m_pInstance->GetKeyDown(Key);
  });

  l_stInputTable.set_function("GetKeyUp", [](uint32_t Key) {
    return InputSystem::m_pInstance->GetKeyUp(Key);
  });

  l_stInputTable.set_function("GetKeyPressed", [](uint32_t Key) {
    return InputSystem::m_pInstance->GetKeyPressed(Key);
  });

  l_stInputTable.set_function("GetKeyReleased", [](uint32_t Key) {
    return InputSystem::m_pInstance->GetKeyReleased(Key);
  });

  l_stInputTable.set_function(
      "GetMouseX", []() { return InputSystem::m_pInstance->GetMouseX(); });
  l_stInputTable.set_function(
      "GetMouseY", []() { return InputSystem::m_pInstance->GetMouseY(); });

  l_stInputTable.set_function("GetMouseRelativeX", []() {
    return InputSystem::m_pInstance->GetMouseRelativeX();
  });

  l_stInputTable.set_function("GetMouseRelativeY", []() {
    return InputSystem::m_pInstance->GetMouseRelativeY();
  });

  l_stInputTable.set_function("GetMouseScrollX", []() {
    return InputSystem::m_pInstance->GetMouseScrollX();
  });

  l_stInputTable.set_function("GetMouseScrollY", []() {
    return InputSystem::m_pInstance->GetMouseScrollY();
  });

  l_stInputTable.set_function("IsMouseButtonPressed", [](int Key) {
    return InputSystem::m_pInstance->isMouseButtonPressed(
        static_cast<MouseCode>(Key));
  });
  l_stInputTable.set_function("IsMouseButtonDown", [](int Key) {
    return InputSystem::m_pInstance->isMouseButtonDown(
        static_cast<MouseCode>(Key));
  });
  l_stInputTable.set_function("IsMouseButtonUp", [](int Key) {
    return InputSystem::m_pInstance->isMouseButtonUp(
        static_cast<MouseCode>(Key));
  });
}

} // namespace eHaz

#endif
