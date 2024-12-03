#pragma once

#include <string>
#include <cstdlib>
#include <vector>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include "Types.hpp"

// Strings
const std::string windowClassName = "SparkWindowClass";
const std::string windowName = "Spark";
// Cursor
const int mouseLockedAreaSize2 = 2;
const unsigned int mouseRawInputBufferSize = 48;
// Colors
const COLOR whiteColor(1.0f, 1.0f, 1.0f, 1.0f);
const COLOR lightGrayColor(0.75f, 0.75f, 0.75f, 1.0f);
const COLOR mediumGrayColor(0.5f, 0.5f, 0.5f, 1.0f);
const COLOR darkGrayColor(0.25f, 0.25f, 0.25f, 1.0f);
const COLOR blackColor(0.0f, 0.0f, 0.0f, 1.0f);
const COLOR emptyColor(0.0f, 0.0f, 0.0f, 0.0f);
const COLOR greenColor(0.0f, 1.0f, 0.0f, 1.0f);
const COLOR cyanColor(0.0f, 1.0f, 1.0f, 1.0f);
const COLOR checkpointColor(0.0f, 1.0f, 1.0f, 1.0f);
const COLOR gemColor(1.0f, 1.0f, 1.0f, 1.0f);
const COLOR lampColor(1.0f, 1.0f, 1.0f, 1.0f);
const COLOR blackScreenColor(0.0f, 0.0f, 0.0f, 0.25f);
const float blackColor_float[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
// PNG
const int PNGsigBytes = 8;
// Shader Input Layout
const int iedSize = 6;
const D3D11_INPUT_ELEMENT_DESC ied[iedSize] = {
	// POSITION - 0 BYTES
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	// NORMAL - 12 BYTES
	{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	// TANGENT - 24 BYTES
	{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	// BINORMAL - 36 BYTES
	{ "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	// COLOR - 48 BYTES
	{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 48, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	// TEXCOORD - 64 BYTES
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 64, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	// TOTAL - 72 BYTES
};
// Graphics Settings
const int numAntialiasingSamples = 8;
const int samplerMaxAnisotropy = 16;
const int shadowMapResolution = 8192;
// Player
const double playerRadius_Z = 0.125;
const double playerRadius_Z_Center = playerRadius_Z * 0.5;
const double playerRadius_XY = 0.125;
// Collision
const double collisionPrecheckRadius = playerRadius_XY;
// Respawn
const double respawnThreshold_Z = -0.125;
// Platforms
const float platformZOffset = -0.05f;
const unsigned int numVerticesPerPlatform = 24;
const unsigned int numIndicesPerPlatform = 36;
const float platform_dx2 = 0.5;
const float platform_dy2 = 0.5;
// Upwind
const float upwindTopRange = 0.0625f;
// Gems
const float gemThetaAngle = DirectX::XM_PIDIV4;
const double itemMagnetThreshold = 0.75;
const double itemMagnetThresholdSquared = itemMagnetThreshold * itemMagnetThreshold;
const double itemCollectionRadius_Z = playerRadius_Z * 0.5;
const double itemCollectionRadius_XY = playerRadius_XY * 0.5;
const double gemRotateTimeRelative = 4.0;
const double gemCollectTimeRelative = 0.75;
// Checkpoints
const double checkpointRadius_XY = 0.25;
const double checkpointHeight = 0.02;
const double checkpointActivationRadius_Z = playerRadius_Z + checkpointHeight;
const double checkpointActivationRadius_XY = playerRadius_XY + checkpointRadius_XY;
// Camera
const float fov = DirectX::XM_PIDIV2;
const float nearZ = 0.01f;
const float farZ = 1024.0f;
const double cameraDistanceNormal = 1.0;
const float upYValue = 1.0f;
const double cameraMinAlpha = 15;
const double cameraMaxAlpha = 120;
const double epsilon_RTI = 0.00001;
const double cameraAddShift = 0.05;
// Sqrt2
const double sqrt2 = sqrt(2);
const double invSqrt2 = 1.0 / sqrt2;
// Player Movement
const double subpartLength = 0.05;
const double movementSpeeds_XY[2] = { 1.5 / 64 , 3.0 / 64 };
const double movement_scale_fps_nominator = 75;
// Mouse, Joystick Movement
const int movementBufferSize = 2;
const double mouseScaleX = 1.0 / 20.0;
const double mouseScaleY = 1.0 / 40.0;
const double joyLeftScaleX = 1.0;
const double joyLeftScaleY = 1.0;
const double joyRightScaleX = 2.5;
const double joyRightScaleY = 1.5;
const double inputJoystickTh1 = 0.25 * 0x7FFF;
const double inputJoystickTh2 = 0.75 * 0x7FFF;
const double inputJoystickThDist = inputJoystickTh2 - inputJoystickTh1;
// Collision Check
const int maxCollisionChecks = 5;
const double collisionCheckBufferX = playerRadius_XY;
const double collisionCheckBufferY = playerRadius_XY;
const double collisionCheckBufferZ = playerRadius_Z;
const double shiftEpsilon = 1.0 / 32768;
// Jump Function
const double glide_ratio = 0.25;
const double high_jump_ratio = 1.75;
const double stomp_ratio = 0.5;
const double z_max_normal_jump = 1.5;
const double z_max_glide = z_max_normal_jump * glide_ratio;
const double z_max_high_jump = z_max_normal_jump * high_jump_ratio;
const double z_max_stomp = z_max_normal_jump;
const double t_h_ratio = 0.50;
const double t_q_ratio = 0.25;
const double t_h_high_jump_ratio = t_h_ratio * high_jump_ratio;
const double t_h_stomp_ratio = t_h_ratio * stomp_ratio;
const double t_q_stomp_ratio = t_q_ratio * stomp_ratio;
// Text
const int gamepadCharOffsetY = -4;
const int numMaxCharsPerText = 64;
const int num_startMenuScreenText_parts = 12;
const int num_manualScreenText_parts = 39;
const int num_creditsScreenText_parts = 7;
const int num_pausedScreenText_parts = 9;
const int num_gemsScreenText_parts = 2;
const int num_checkpointsScreenText_parts = 2;
const int num_textboxes = 3;
// Materials
const Material materialStatic;
const Material materialPlatform;
const Material materialPlayer;
// Light Types
const int numLightTypes = 5;
const int gem_LightIndex = 0;
const int checkpointOff_LightIndex = 1;
const int checkpointOn_Active_LightIndex = 2;
const int checkpointOn_Inactive_LightIndex = 3;
const int lamp_LightIndex = 4;
LightType allLightTypes[numLightTypes] = {
	// 0 - Gem
	LightType(
		0.00, 0.00, 0.00, 0.00, // Ambient
		0.00, 1.00, 0.00, 1.00, // Diffuse
		0.00, 1.00, 0.00, 1.00, // Specular
		0.5f // Range
	),
	// 1 - Checkpoint (Off)
	LightType(
		0.00, 0.00, 0.00, 0.00 , // Ambient
		1.00, 0.00, 0.00, 1.00, // Diffuse
		1.00, 0.00, 0.00, 1.00, // Specular
		0.5f // Range
	),
	// 2 - Checkpoint (On - Active)
	LightType(
		0.00, 0.00, 0.00, 0.00 , // Ambient
		0.00, 1.00, 1.00, 1.00 , // Diffuse
		0.00, 1.00, 1.00, 1.00 , // Specular
		0.5f // Range
	),
	// 3 - Checkpoint (On - Inactive)
	LightType(
		0.00, 0.00, 0.00, 0.00 , // Ambient
		0.00, 0.50, 0.50, 0.50 , // Diffuse
		0.00, 0.50, 0.50, 0.50 , // Specular
		0.5f // Range
	),
	// 4 - Lamp
	LightType(
		0.00, 0.00, 0.00, 0.00 , // Ambient
		1.00, 1.00, 1.00, 1.00 , // Diffuse
		1.00, 1.00, 1.00, 1.00 , // Specular
		15.0f // Range
	),
};