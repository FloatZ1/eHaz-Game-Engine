
#ifndef EHAZ_SCRIPTING_ENGINE_HPP
#define EHAZ_SCRIPTING_ENGINE_HPP
//clang-format off
#include "Core/AssetSystem/Asset.hpp"
#include "Script_Fields.hpp"
#include <Components.hpp>
#include <memory>
#include <unordered_map>
#include <vector>
extern "C" {
#include <lua.h>

#include <lauxlib.h>
#include <lualib.h>
}

#include <sol/sol.hpp>

//clang-format on

namespace eHaz {

class CScriptingEngine {

public:
  static CScriptingEngine *s_pInstance;

  void Initialize();

  void ExtractTableValues(ScriptComponent &p_scComponent);

  void LoadScript(ScriptHandle p_shHandle);

  void ReloadScript(ScriptHandle p_shHandle);

  sol::table CreateInstance(ScriptHandle p_shHandle, uint32_t p_uiObjectID);

  void UpdateScript(sol::table p_stScript);

  void FixedUpdateScript(sol::table p_stScript, float p_fDeltaTime);
  void CallOnCreate(sol::table p_stScript);

  // TODO: IMPLEMENT
  void CallOnCollision(sol::table p_stScript);

  void ValidateData(ScriptComponent &p_scComponent, uint32_t p_uiObjectID);

  void CallOnDestroy(sol::table p_stData);

  sol::state &GetLuaState() { return m_ssLua; }

  void Clear();

  void Destroy();

private:
  void RegisterTypes();

  void RegisterSceneFunctions();

  sol::object GetComponent(uint32_t p_uiObjectID, ComponentID p_cidComponent,
                           sol::state &m_ssLua);

  sol::object GetGameObject(uint32_t p_uiObjectID, sol::state &m_ssLua);

  sol::state m_ssLua;

  std::unordered_map<ScriptHandle, sol::load_result> m_umCompiledScripts;

  sol::load_result m_slrScriptClassInterface;

  std::string m_strScriptClassInterfaceSource = R"lua(

  


     -- Base interface for all scripts
ScriptBase = {}

function ScriptBase:new(o)
    o = o or {}
    setmetatable(o, self)
    self.__index = self
    -- Default fields (user scripts can override)
    o.name = o.name or "Unnamed"
    o.enabled = o.enabled or true
    return o
end

-- Virtual methods
function ScriptBase:OnStart()
    -- Override in child
end

function ScriptBase:OnUpdate(dt)
    -- Override in child
end

function ScriptBase:OnFixedUpdate(dt)
end


function ScriptBase:OnDestroy()
    -- Override in child
end

-- TODO: pass the collision event data from the c++ struct     
function ScriptBase:OnCollision()
end
     


)lua";
};

} // namespace eHaz

#endif
