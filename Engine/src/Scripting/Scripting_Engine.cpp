#include "Components.hpp"
#include "Core/Application.hpp"
#include "Core/AssetSystem/Asset.hpp"
#include "Core/AssetSystem/AssetSystem.hpp"
#include "Scene.hpp"
#include "Script_Fields.hpp"
#include "glm/fwd.hpp"
#include "glm/gtc/quaternion.hpp"
#include <SDL3/SDL_log.h>
#include <Scripting_InputSystem_Bindings.hpp>
#include <Scripting_Keycode_Bindings.hpp>
#include <Scripting_Physics_Bindings.hpp>
#include <boost/mpl/assert.hpp>
#include <memory>
#include <utility>

//clang-format off
extern "C" {
#include <lua.h>

#include <lauxlib.h>
#include <lualib.h>
}

#include <sol/sol.hpp>

//clang-format on
#include <Engine/include/Scripting/Scripting_Engine.hpp>

namespace eHaz {

CScriptingEngine *CScriptingEngine::s_pInstance = nullptr;

void CScriptingEngine::Initialize() {

  s_pInstance = this;

  m_ssLua = sol::state();

  m_ssLua.open_libraries(sol::lib::base);

  RegisterTypes();
  RegisterInputKeys(m_ssLua);
  RegisterInputSystemBindings(m_ssLua);
  RegisterPhysicsSystemBindings(m_ssLua);

  m_slrScriptClassInterface = m_ssLua.load(m_strScriptClassInterfaceSource);

  if (!m_slrScriptClassInterface.valid()) {
    sol::error err = m_slrScriptClassInterface;
    SDL_Log("LUA ERROR: %s", err.what());
  }

  sol::protected_function l_spfInitInterface = m_slrScriptClassInterface;
  l_spfInitInterface();

  m_ssLua.set_function("print",
                       [](const std::string &msg) { SDL_Log(msg.c_str()); });
}

void CScriptingEngine::LoadScript(ScriptHandle p_shHandle) {

  // TODO: add a preprocesor to load compiled scripts during runtime instead od
  // .lua files

  if (m_umCompiledScripts.contains(p_shHandle))
    return;

  const SScriptAsset *l_asScript =
      CAssetSystem::m_pInstance->GetScript(p_shHandle);

  if (!l_asScript) {
    SDL_Log("Error: Invalid asset handle: index: %u generation: %u \n",
            p_shHandle.index, p_shHandle.generation);

    return;
  }

  sol::load_result l_slrCompiledScript =
      m_ssLua.load_file(l_asScript->m_strPath);
  m_umCompiledScripts[p_shHandle] = std::move(l_slrCompiledScript);
}
void CScriptingEngine::Clear() { m_umCompiledScripts.clear(); }
void CScriptingEngine::ReloadScript(ScriptHandle p_shHandle) {

  const SScriptAsset *l_asScript =
      CAssetSystem::m_pInstance->GetScript(p_shHandle);

  if (!l_asScript) {
    SDL_Log("Error: Invalid asset handle: index: %u generation: %u \n",
            p_shHandle.index, p_shHandle.generation);

    return;
  }

  sol::load_result l_slrCompiledScript =
      m_ssLua.load_file(l_asScript->m_strPath);

  m_umCompiledScripts.erase(p_shHandle);
  m_umCompiledScripts[p_shHandle] = std::move(l_slrCompiledScript);
}
void CScriptingEngine::ValidateData(ScriptComponent &p_scComponent) {

  if (!p_scComponent.m_stTableInstance.valid()) {
    p_scComponent.m_stTableInstance = CreateInstance(p_scComponent.m_shHandle);
  }

  sol::table &l_stInstance = p_scComponent.m_stTableInstance;

  for (SScriptField &field : p_scComponent.m_vFields) {

    l_stInstance[field.m_strFieldName] = field.m_varValue;
  }
}
void CScriptingEngine::CallOnDestroy(sol::table p_stData) {

  sol::protected_function l_spfDestroy = p_stData["OnDestroy"];

  if (l_spfDestroy.valid())
    l_spfDestroy(p_stData);
}
sol::table CScriptingEngine::CreateInstance(ScriptHandle p_shHandle) {

  if (!m_umCompiledScripts.contains(p_shHandle))
    LoadScript(p_shHandle);

  auto &compiled = m_umCompiledScripts[p_shHandle];

  sol::protected_function_result result = compiled();

  if (!result.valid()) {
    sol::error err = result;
    SDL_Log("Script ERROR: %s", err.what());
    return sol::nil;
  }

  sol::table scriptTable = result;

  return scriptTable;
}
void CScriptingEngine::ExtractTableValues(ScriptComponent &p_scComponent) {
  p_scComponent.m_vFields.clear();
  for (auto &pair : p_scComponent.m_stTableInstance) {

    sol::object key = pair.first;
    sol::object value = pair.second;

    if (key.get_type() != sol::type::string)
      continue;

    std::string name = key.as<std::string>();
    if (value.is<uint32_t>())
      p_scComponent.m_vFields.push_back(
          {p_scComponent.m_shHandle, name, value.as<uint32_t>()});
    if (value.is<int>())
      p_scComponent.m_vFields.push_back(
          {p_scComponent.m_shHandle, name, value.as<int>()});

    else if (value.is<float>())
      p_scComponent.m_vFields.push_back(
          {p_scComponent.m_shHandle, name, value.as<float>()});

    else if (value.is<double>())
      p_scComponent.m_vFields.push_back(
          {p_scComponent.m_shHandle, name, value.as<double>()});

    else if (value.is<bool>())
      p_scComponent.m_vFields.push_back(
          {p_scComponent.m_shHandle, name, value.as<bool>()});

    else if (value.is<std::string>())
      p_scComponent.m_vFields.push_back(
          {p_scComponent.m_shHandle, name, value.as<std::string>()});
  };
}
void CScriptingEngine::CallOnCreate(sol::table p_stScript)

{

  sol::protected_function l_spfCreate = p_stScript["OnCreate"];
  if (l_spfCreate.valid())
    l_spfCreate(p_stScript);
}
void CScriptingEngine::UpdateScript(sol::table p_stScript) {

  float l_fDeltaTime = eHaz_Core::Application::instance->GetDeltaTime();

  sol::protected_function l_spfUpdate = p_stScript["OnUpdate"];

  if (l_spfUpdate.valid())
    l_spfUpdate(p_stScript, l_fDeltaTime);
}
void CScriptingEngine::FixedUpdateScript(sol::table p_stScript,
                                         float p_fDeltaTime) {

  sol::protected_function l_spfUpdate = p_stScript["OnFixedUpdate"];

  if (l_spfUpdate.valid())
    l_spfUpdate(p_stScript, p_fDeltaTime);
}
void CScriptingEngine::Destroy() {}

//clang-format off
void CScriptingEngine::RegisterTypes() {
  m_ssLua.new_usertype<glm::vec3>(
      "Vector3",

      sol::constructors<glm::vec3(), glm::vec3(float, float, float)>(), "x",
      &glm::vec3::x, "y", &glm::vec3::y, "z", &glm::vec3::z, "Length",
      [](const glm::vec3 &v) { return v.length(); }

  );

  m_ssLua.new_usertype<glm::vec4>(
      "Vector4",

      sol::constructors<glm::vec4(), glm::vec4(float, float, float, float)>(),
      "x", &glm::vec4::x, "y", &glm::vec4::y, "z", &glm::vec4::z, "w",
      &glm::vec4::w, "Length", [](const glm::vec4 &v) { return v.length(); }

  );

  m_ssLua.new_usertype<glm::quat>(
      "Quaternion",

      sol::constructors<glm::quat(), glm::quat(float, float, float, float)>(),
      "x", &glm::quat::x, "y", &glm::quat::y, "z", &glm::quat::z, "w",
      &glm::quat::w, "Length", [](const glm::quat &v) { return v.length(); }

  );

  m_ssLua.new_usertype<glm::mat4>(
      "mat4",
      // constructors
      sol::constructors<glm::mat4(), glm::mat4(const glm::mat4 &)>(),

      // access elements via [row][col] style is complicated; we expose as flat
      // array if needed
      "identity", []() { return glm::mat4(1.0f); },

      // static helpers
      "translate",
      [](const glm::mat4 &m, const glm::vec3 &v) {
        return glm::translate(m, v);
      },
      "rotate",
      [](const glm::mat4 &m, float angle_rad, const glm::vec3 &axis) {
        return glm::rotate(m, angle_rad, axis);
      },
      "rotate_quat",
      [](const glm::mat4 &m, const glm::quat &q) {
        return m * glm::mat4_cast(q);
      },
      "scale",
      [](const glm::mat4 &m, const glm::vec3 &v) { return glm::scale(m, v); },

      // multiplication with another mat4
      sol::meta_function::multiplication,
      [](const glm::mat4 &a, const glm::mat4 &b) { return a * b; },

      // multiplication with vec4
      "mul_vec4", [](const glm::mat4 &m, const glm::vec4 &v) { return m * v; });

  m_ssLua.new_usertype<glm::mat3>(
      "mat3",
      // Constructors
      sol::constructors<glm::mat3(), glm::mat3(const glm::mat3 &)>(),

      // Methods
      "identity", []() { return glm::mat3(1.0f); },

      // Rotate method: multiplies by quaternion rotation
      "rotate_quat",
      [](glm::mat3 &m, const glm::quat &q) {
        m = m * glm::mat3_cast(q);
        return m; // return updated matrix for chaining
      },

      // Multiply another mat3
      "mul",
      [](const glm::mat3 &m, const glm::mat3 &other) { return m * other; });

  m_ssLua.new_usertype<TransformComponent>(
      "TransformComponent", "World Position",
      &TransformComponent::worldPosition, "World Scale",
      &TransformComponent::worldScale, "World Rotation",
      &TransformComponent::worldRotation,

      "Local Position", &TransformComponent::localPosition, "Local Scale",
      &TransformComponent::localScale, "Local Rotation",
      &TransformComponent::localRotation);

  m_ssLua.new_usertype<CameraComponent>(
      "CameraComponent", "IsActiveCamera",
      [](CameraComponent &cc) { return cc.m_bActiveCamera; },
      "UseCustomAspectRatio", &CameraComponent::m_bUseCustomAspectRatio,
      "AspectX", &CameraComponent::m_iAspectRatio1, "AspectY",
      &CameraComponent::m_iAspectRatio2, "FOV", &CameraComponent::m_fFOV,
      "Near_Plane", &CameraComponent::m_fNearPlane, "Far_Plane",
      &CameraComponent::m_fFarPlane

  );

  m_ssLua.new_usertype<SScriptField>(
      "ScriptField", sol::constructors<SScriptField()>(), "Name",
      &SScriptField::m_strFieldName, "Value",
      sol::property([](SScriptField &f) { return f.m_varValue; }, // getter
                    [](SScriptField &f, sol::object obj) {        // setter
                      if (obj.is<int>())
                        f.m_varValue = obj.as<int>();
                      else if (obj.is<float>())
                        f.m_varValue = obj.as<float>();
                      else if (obj.is<bool>())
                        f.m_varValue = obj.as<bool>();
                      else if (obj.is<std::string>())
                        f.m_varValue = obj.as<std::string>();
                      else if (obj.is<char>())
                        f.m_varValue = obj.as<char>();
                      else if (obj.is<uint32_t>())
                        f.m_varValue = obj.as<uint32_t>();
                      else if (obj.is<double>())
                        f.m_varValue = obj.as<double>();
                    }));

  m_ssLua.new_usertype<ScriptComponent>("ScriptComponent", "Fields",
                                        &ScriptComponent::m_vFields

  );
  m_ssLua.new_enum<JPH::EMotionType>(
      "EMotionType", // Lua-visible enum name
      {{"Static", JPH::EMotionType::Static},
       {"Kinematic", JPH::EMotionType::Kinematic},
       {"Dynamic", JPH::EMotionType::Dynamic}});

  m_ssLua.new_enum<EProjectionType>(
      "EProjectionType", {{"Perspective", EProjectionType::Perspective},
                          {"Orthographics", EProjectionType::Orthographics}});

  m_ssLua.new_usertype<RigidBodyComponent>(
      "RigidBodyComponent",                      // Lua type name
      sol::constructors<RigidBodyComponent()>(), // default constructor
      // Motion / ID
      "MotionType", &RigidBodyComponent::m_jmtMotionType, "Layer",
      &RigidBodyComponent::m_uiLayer,
      // Physical Properties
      "Mass", &RigidBodyComponent::m_Mass, "Friction",
      &RigidBodyComponent::m_Friction, "Restitution",
      &RigidBodyComponent::m_Restitution, "LinearDamping",
      &RigidBodyComponent::m_LinearDamping, "AngularDamping",
      &RigidBodyComponent::m_AngularDamping,
      // Motion Constraints
      "LockPosX", &RigidBodyComponent::m_LockPositionX, "LockPosY",
      &RigidBodyComponent::m_LockPositionY, "LockPosZ",
      &RigidBodyComponent::m_LockPositionZ, "LockRotX",
      &RigidBodyComponent::m_LockRotationX, "LockRotY",
      &RigidBodyComponent::m_LockRotationY, "LockRotZ",
      &RigidBodyComponent::m_LockRotationZ, "IsSensor",
      &RigidBodyComponent::m_IsSensor, "StartActive",
      &RigidBodyComponent::m_StartActive,
      // Collider / Transform
      "ColliderOffset", &RigidBodyComponent::m_v3ColliderPositionOffset,
      "Description", &RigidBodyComponent::m_bdDescription, "OwnerID",
      &RigidBodyComponent::m_uiSceneObjectOwnerID);

  m_ssLua.new_enum<eHaz::ComponentID>(
      "ComponentID", {{"None", eHaz::ComponentID::None},
                      {"Transform", eHaz::ComponentID::Transform},
                      {"Model", eHaz::ComponentID::Model},
                      {"Rigidbody", eHaz::ComponentID::Rigidbody},
                      {"Camera", eHaz::ComponentID::Camera},
                      {"Animator", eHaz::ComponentID::Animator},
                      {"Script", eHaz::ComponentID::Script}});

  // Expose GameObject to Lua
  m_ssLua.new_usertype<eHaz::GameObject>(
      "GameObject",
      // Constructors
      sol::constructors<eHaz::GameObject()>(),

      // Fields
      "name", &eHaz::GameObject::name, "alive", &eHaz::GameObject::alive,
      "isStatic", &eHaz::GameObject::isStatic, "isVisible",
      &eHaz::GameObject::isVisible, "componentMask",
      &eHaz::GameObject::componentMask, "isLight",
      &eHaz::GameObject::m_bIsLight, "octreeIndex",
      &eHaz::GameObject::m_uiOctreeIndex, "updateOctree",
      &eHaz::GameObject::m_bUbdateOctree, "index", &eHaz::GameObject::index,
      "parent", &eHaz::GameObject::parent, "children",
      &eHaz::GameObject::children,

      "HasComponentFlag", &eHaz::GameObject::HasComponentFlag, "GetChildren",
      &eHaz::GameObject::GetChildren, "GetParent", &eHaz::GameObject::GetParent,

      // Comparison operator
      sol::meta_function::equal_to, &eHaz::GameObject::operator==);
}

//clang-format on
void CScriptingEngine::RegisterSceneFunctions() {

  sol::table l_stSceneTable = m_ssLua.create_named_table("Scene");

  l_stSceneTable.set_function(
      "HasComponent", [&](uint32_t p_uiObjectID, ComponentID p_cidComponent) {
        eHaz_Core::Application::instance->getActiveScene().HasComponent(
            p_uiObjectID, p_cidComponent);
      });

  l_stSceneTable.set_function(
      "GetComponent",
      [&](uint32_t p_uiObjectID, ComponentID p_cidComponent) -> sol::object {
        Scene &l_sScene = eHaz_Core::Application::instance->getActiveScene();

        switch (p_cidComponent) {
        case ComponentID::Camera: {
          auto *comp = l_sScene.TryGetComponent<CameraComponent>(p_uiObjectID);
          if (!comp)
            return sol::nil;
          return sol::make_reference(m_ssLua.lua_state(), *comp);
        }
        case ComponentID::Rigidbody: {
          auto *comp =
              l_sScene.TryGetComponent<RigidBodyComponent>(p_uiObjectID);
          if (!comp)
            return sol::nil;
          return sol::make_reference(m_ssLua.lua_state(), *comp);
        }
        case ComponentID::Script: {
          auto *comp = l_sScene.TryGetComponent<ScriptComponent>(p_uiObjectID);
          if (!comp)
            return sol::nil;
          return sol::make_reference(m_ssLua.lua_state(), *comp);
        }
        case ComponentID::Transform: {
          auto *comp =
              l_sScene.TryGetComponent<TransformComponent>(p_uiObjectID);
          if (!comp)
            return sol::nil;
          return sol::make_reference(m_ssLua.lua_state(), *comp);
        }
        default:
          return sol::nil;
        }
      });

  l_stSceneTable.set_function(
      "GetGameObject", [&](uint32_t p_uiObjectID) -> sol::object {
        Scene &l_sScene = eHaz_Core::Application::instance->getActiveScene();
        if (l_sScene.scene_graph.IsValid(p_uiObjectID)) {
          auto &object = l_sScene.scene_graph.GetObject(p_uiObjectID);

          return sol::make_reference(m_ssLua.lua_state(), object);
        } else {
          return sol::nil;
        }
      });
}
} // namespace eHaz
