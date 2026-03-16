---@meta

--------------------------------------------------
-- Math Types
--------------------------------------------------

---@class Vector3
---@field x number
---@field y number
---@field z number
local Vector3 = {}

---@return number
function Vector3:Length() end

---@class Vector4
---@field x number
---@field y number
---@field z number
---@field w number
local Vector4 = {}

---@return number
function Vector4:Length() end

---@class Quaternion
---@field x number
---@field y number
---@field z number
---@field w number
local Quaternion = {}

---@return number
function Quaternion:Length() end

---@class mat4
local mat4 = {}

---@return mat4
function mat4.identity() end

---@param m mat4
---@param v Vector3
---@return mat4
function mat4.translate(m, v) end

---@param m mat4
---@param angle number
---@param axis Vector3
---@return mat4
function mat4.rotate(m, angle, axis) end

---@param m mat4
---@param q Quaternion
---@return mat4
function mat4.rotate_quat(m, q) end

---@param m mat4
---@param v Vector3
---@return mat4
function mat4.scale(m, v) end

---@param v Vector4
---@return Vector4
function mat4:mul_vec4(v) end

---@class mat3
local mat3 = {}

---@return mat3
function mat3.identity() end

---@param q Quaternion
---@return mat3
function mat3:rotate_quat(q) end

---@param other mat3
---@return mat3
function mat3:mul(other) end

--------------------------------------------------
-- Components
--------------------------------------------------

---@class TransformComponent
---@field ["WorldPosition"] Vector3
---@field ["WorldScale"] Vector3
---@field ["WorldRotation"] Quaternion
---@field ["LocalPosition"] Vector3
---@field ["LocalScale"] Vector3
---@field ["LocalRotation"] Quaternion
local TransformComponent = {}

---@class CameraComponent
---@field IsActiveCamera boolean
---@field UseCustomAspectRatio boolean
---@field AspectX integer
---@field AspectY integer
---@field FOV number
---@field Near_Plane number
---@field Far_Plane number
local CameraComponent = {}

---@class ScriptField
---@field Name string
---@field Value any
local ScriptField = {}

---@class ScriptComponent
---@field Fields ScriptField[]
local ScriptComponent = {}

---@class RigidBodyComponent
---@field MotionType EMotionType
---@field Layer integer
---@field Mass number
---@field Friction number
---@field Restitution number
---@field LinearDamping number
---@field AngularDamping number
---@field LockPosX boolean
---@field LockPosY boolean
---@field LockPosZ boolean
---@field LockRotX boolean
---@field LockRotY boolean
---@field LockRotZ boolean
---@field IsSensor boolean
---@field StartActive boolean
---@field ColliderOffset Vector3
---@field Description any
---@field OwnerID integer
local RigidBodyComponent = {}

--------------------------------------------------
-- GameObject
--------------------------------------------------

---@class GameObject
---@field name string
---@field alive boolean
---@field isStatic boolean
---@field isVisible boolean
---@field componentMask integer
---@field isLight boolean
---@field octreeIndex integer
---@field updateOctree boolean
---@field index integer
---@field parent integer
---@field children integer[]
local GameObject = {}

---@param component ComponentID
---@return boolean
function GameObject:HasComponentFlag(component) end

---@return GameObject[]
function GameObject:GetChildren() end

---@return GameObject
function GameObject:GetParent() end

--------------------------------------------------
-- Scene API
--------------------------------------------------

Scene = {}

---@param objectID integer
---@param component ComponentID
---@return boolean
function Scene.HasComponent(objectID, component) end

---@param objectID integer
---@param component ComponentID
---@return any
function Scene.GetComponent(objectID, component) end

---@param objectID integer
---@return GameObject
function Scene.GetGameObject(objectID) end

--------------------------------------------------
-- Input
--------------------------------------------------

Input = {}

---@param key integer
---@return boolean
function Input.GetKeyDown(key) end

---@param key integer
---@return boolean
function Input.GetKeyUp(key) end

---@param key integer
---@return boolean
function Input.GetKeyPressed(key) end

---@param key integer
---@return boolean
function Input.GetKeyReleased(key) end

---@return number
function Input.GetMouseX() end

---@return number
function Input.GetMouseY() end

---@return number
function Input.GetMouseRelativeX() end

---@return number
function Input.GetMouseRelativeY() end

---@return number
function Input.GetMouseScrollX() end

---@return number
function Input.GetMouseScrollY() end

---@param key integer
---@return boolean
function Input.IsMouseButtonPressed(key) end

---@param key integer
---@return boolean
function Input.IsMouseButtonDown(key) end

---@param key integer
---@return boolean
function Input.IsMouseButtonUp(key) end

--------------------------------------------------
-- Physics
--------------------------------------------------

Physics = {}

---@param body RigidBodyComponent
---@param force Vector3
function Physics.AddForce(body, force) end

---@param body RigidBodyComponent
---@param impulse Vector3
function Physics.AddImpulse(body, impulse) end

---@param gravity Vector3
function Physics.SetGravity(gravity) end

--------------------------------------------------
-- Enums
--------------------------------------------------

---@enum EMotionType
EMotionType = {
	Static = 0,
	Kinematic = 1,
	Dynamic = 2,
}

---@enum EProjectionType
EProjectionType = {
	Perspective = 0,
	Orthographics = 1,
}

---@enum ComponentID
ComponentID = {
	None = 0,
	Transform = 1,
	Model = 2,
	Rigidbody = 3,
	Camera = 4,
	Animator = 5,
	Script = 6,
}

---@class ScriptBase
---@field gameObject GameObject
---@field ObjectID integer
---@field name string
---@field enabled boolean

---@param dt number
function ScriptBase:OnUpdate(dt) end

---@param dt number
function ScriptBase:OnFixedUpdate(dt) end

function ScriptBase:OnStart() end

function ScriptBase:OnDestroy() end

function ScriptBase:OnCollision() end

---@class KeyCode
KeyCode = {}

-- Mouse buttons
KeyCode.LEFT_MOUSE_BUTTON = 0
KeyCode.RIGHT_MOUSE_BUTTON = 1
KeyCode.MIDDLE_MOUSE_BUTTON = 2
KeyCode.X1_MOUSE_BUTTON = 3
KeyCode.X2_MOUSE_BUTTON = 4

-- Common keys
KeyCode.UNKNOWN = 5
KeyCode.RETURN = 6
KeyCode.ESCAPE = 7
KeyCode.BACKSPACE = 8
KeyCode.TAB = 9
KeyCode.SPACE = 10

KeyCode.EXCLAIM = 11
KeyCode.DBLAPOSTROPHE = 12
KeyCode.HASH = 13
KeyCode.DOLLAR = 14
KeyCode.PERCENT = 15
KeyCode.AMPERSAND = 16
KeyCode.APOSTROPHE = 17

KeyCode.LEFTPAREN = 18
KeyCode.RIGHTPAREN = 19
KeyCode.ASTERISK = 20
KeyCode.PLUS = 21
KeyCode.COMMA = 22
KeyCode.MINUS = 23
KeyCode.PERIOD = 24
KeyCode.SLASH = 25

-- Number keys
KeyCode._0 = 26
KeyCode._1 = 27
KeyCode._2 = 28
KeyCode._3 = 29
KeyCode._4 = 30
KeyCode._5 = 31
KeyCode._6 = 32
KeyCode._7 = 33
KeyCode._8 = 34
KeyCode._9 = 35

-- Letters
KeyCode.A = 36
KeyCode.B = 37
KeyCode.C = 38
KeyCode.D = 39
KeyCode.E = 40
KeyCode.F = 41
KeyCode.G = 42
KeyCode.H = 43
KeyCode.I = 44
KeyCode.J = 45
KeyCode.K = 46
KeyCode.L = 47
KeyCode.M = 48
KeyCode.N = 49
KeyCode.O = 50
KeyCode.P = 51
KeyCode.Q = 52
KeyCode.R = 53
KeyCode.S = 54
KeyCode.T = 55
KeyCode.U = 56
KeyCode.V = 57
KeyCode.W = 58
KeyCode.X = 59
KeyCode.Y = 60
KeyCode.Z = 61

-- Function keys
KeyCode.F1 = 62
KeyCode.F2 = 63
KeyCode.F3 = 64
KeyCode.F4 = 65
KeyCode.F5 = 66
KeyCode.F6 = 67
KeyCode.F7 = 68
KeyCode.F8 = 69
KeyCode.F9 = 70
KeyCode.F10 = 71
KeyCode.F11 = 72
KeyCode.F12 = 73

-- Arrows
KeyCode.UP = 74
KeyCode.DOWN = 75
KeyCode.LEFT = 76
KeyCode.RIGHT = 77

-- Modifiers
KeyCode.LCTRL = 78
KeyCode.RCTRL = 79
KeyCode.LSHIFT = 80
KeyCode.RSHIFT = 81
KeyCode.LALT = 82
KeyCode.RALT = 83
KeyCode.LGUI = 84
KeyCode.RGUI = 85

-- Delete
KeyCode.DELETE = 86
