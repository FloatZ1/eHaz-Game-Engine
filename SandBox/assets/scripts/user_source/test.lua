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
	---@type RigidBodyComponent
	local rb = Scene.GetComponent(self.gameObject.index, ComponentID.Rigidbody)
	if not rb then
		print("dasdasdasd")
	end
	if Input.GetKeyDown(KeyCode.D) then
		-- print("[Lua] Update (" .. tostring(1884) .. "): " .. self.message)

		local force = Vector3.new(100.0, 0.0, 0.0)
		Physics.AddImpulse(rb, force)
	end

	if Input.GetKeyDown(KeyCode.A) then
		-- print("[Lua] Update (" .. tostring(1884) .. "): " .. self.message)

		local force = Vector3.new(-100.0, 0.0, 0.0)
		Physics.AddImpulse(rb, force)
	end

	if Input.GetKeyDown(KeyCode.W) then
		-- print("[Lua] Update (" .. tostring(1884) .. "): " .. self.message)

		local force = Vector3.new(0.0, 0.0, -100.0)
		Physics.AddImpulse(rb, force)
	end

	if Input.GetKeyDown(KeyCode.S) then
		-- print("[Lua] Update (" .. tostring(1884) .. "): " .. self.message)

		local force = Vector3.new(0.0, 0.0, 100.0)
		Physics.AddImpulse(rb, force)
	end

	if Input.GetKeyDown(KeyCode.SPACE) then
		-- print("[Lua] Update (" .. tostring(1884) .. "): " .. self.message)

		local force = Vector3.new(0.0, 1000.0, 0.0)
		Physics.AddImpulse(rb, force)
	end

	if Input.GetKeyDown(KeyCode.LCTRL) then
		-- print("[Lua] Update (" .. tostring(1884) .. "): " .. self.message)

		local force = Vector3.new(0.0, -1000.0, 0.0)
		Physics.AddImpulse(rb, force)
	end
end

function PrintMessage:OnDestroy()
	print("[Lua] Script destroyed: " .. self.name)
end

function PrintMessage:OnFixedUpdate(dt)
	--  print("OOGA BOOGA FIXED UPDATE")
end

return PrintMessage
