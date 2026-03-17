-- Example script that inherits from ScriptBase
PrintMessage = ScriptBase:new({
	name = "PrintMessageScript",
	enabled = true,
	message = "Hello from Lua!",
	test_var = 1.0,
})

function PrintMessage:OnStart()
	print("[Lua] Script started: " .. self.name)
end

function PrintMessage:OnUpdate(dt)
	if not self.enabled then
		return
	end
	---@class TransformComponent
	local tc = Scene.GetComponent(self.gameObject.index, ComponentID.Transform)

	if Input.GetKeyDown(KeyCode.G) then
		print("[Lua] Update (" .. tostring(1884) .. "): " .. self.message)
		self.gameObject.name = "amongus"
		tc.LocalPosition.x = tc.LocalPosition.x + 1
	end
end

function PrintMessage:OnDestroy()
	print("[Lua] Script destroyed: " .. self.name)
end

function PrintMessage:OnFixedUpdate(dt)
	-- print("OOGA BOOGA FIXED UPDATE")
end

return PrintMessage
