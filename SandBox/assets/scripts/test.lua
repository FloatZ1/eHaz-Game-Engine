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

	print("[Lua] Update (" .. tostring(dt) .. "): " .. self.message)
end

function PrintMessage:OnDestroy()
	print("[Lua] Script destroyed: " .. self.name)
end

function PrintMessage:OnFixedUpdate(dt)
	print("OOGA BOOGA FIXED UPDATE")
end

return PrintMessage
