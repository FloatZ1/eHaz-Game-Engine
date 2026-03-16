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
	--  print(self.gameObject.index)
	--  transform = Scene.GetComponent(self.gameObject.index, ComponentID.Transform)
	--   print(transform)
	--	print("aaaaa")
	if Input.GetKeyDown(KeyCode.G) then
		print("[Lua] Update (" .. tostring(1884) .. "): " .. self.message)
	end
end

function PrintMessage:OnDestroy()
	print("[Lua] Script destroyed: " .. self.name)
end

function PrintMessage:OnFixedUpdate(dt)
	-- print("OOGA BOOGA FIXED UPDATE")
end

return PrintMessage
