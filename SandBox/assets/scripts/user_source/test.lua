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

	if Input.GetKeyDown(KeyCode.G) then
		-- print("[Lua] Update (" .. tostring(1884) .. "): " .. self.message)

		---@type RigidBodyComponent
		local rb = Scene.GetComponent(self.gameObject.index, ComponentID.Rigidbody)
		if not rb then
			print("dasdasdasd")
		end
		local force = Vector3.new(100.0, 100.0, 0.0)
		Physics.AddForce(rb, force)
	end
end

function PrintMessage:OnDestroy()
	print("[Lua] Script destroyed: " .. self.name)
end

function PrintMessage:OnFixedUpdate(dt)
	print("OOGA BOOGA FIXED UPDATE")
end

return PrintMessage
