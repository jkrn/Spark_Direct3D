#pragma once

#include "ShaderConstants.hpp"

typedef unsigned char BYTE;

enum GameStatus { START_MENU, RUNNING, PAUSED, MANUAL, CREDITS };
enum JoyStickType { JOYSTICK_LEFT, JOYSTICK_RIGHT };
enum JumpState { ON_GROUND, JUMP_NORMAL, JUMP_SHORT, JUMP_HIGH, GLIDE, FALL_NORMAL, FALL_FINAL , STOMP , UPWIND, UPWIND_TOP };
enum InitType { FIRST_INIT , NEW_LEVEL , RESPAWN };

typedef struct COLOR {
    float r;
    float g;
    float b;
    float a;
    COLOR(float r, float g, float b, float a) : r(r), g(g), b(b), a(a) {}
    COLOR() : COLOR(0.0f, 0.0f, 0.0f, 1.0f){}
} COLOR;

class Point2int {
public:
	int x;
	int y;
	Point2int(int x, int y) : x(x), y(y) {}
	Point2int() : Point2int(0, 0) {}
};

class Point2double {
public:
	double x;
	double y;
	Point2double(double x, double y) : x(x), y(y) {}
	Point2double() : Point2double(0, 0) {}
};

class Point3 {
public:
	double x;
	double y;
	double z;
	Point3(double x, double y, double z) : x(x), y(y), z(z) {}
	Point3() : Point3(0, 0, 0) {}
};

class Unit {
public:
	Point3 p;
	double angleOffset;
	Unit(Point3 p, double angleOffset) : p(p), angleOffset(angleOffset) {}
	Unit(double angle) : angleOffset(angleOffset) {}
	Unit() : Unit(0) {}
};

class Camera {
public:
	Point3 at;
	Point3 eye;
	double alpha;
	double beta;
	Camera(double alpha, double beta) : alpha(alpha), beta(beta) {}
	Camera() : Camera(0,0) {};
};

class GemInfo {
public:
	bool isMoving;
	int steps;
	float addAngle;
	Point3 startPosition;
	GemInfo(float addAngle, Point3 startPosition) : isMoving(false), steps(0), addAngle(addAngle), startPosition(startPosition) {}
};

class RespawnInfo {
public:
	double respawnPlayerAngleOffset;
	double respawnCameraAlpha;
	double respawnCameraBeta;
	Point3 respawnPosition;
	RespawnInfo(double respawnPlayerAngleOffset, double respawnCameraAlpha, double respawnCameraBeta, Point3 respawnPosition) :
		respawnPlayerAngleOffset(respawnPlayerAngleOffset), respawnCameraAlpha(respawnCameraAlpha), respawnCameraBeta(respawnCameraBeta), respawnPosition(respawnPosition) {}
};

class ZFunction {
public:
	double t_h;
	double t_q;
	double a;
	double b;
	double c;
	double d;
	double e;
	ZFunction() : t_h(0), t_q(0), a(0), b(0), c(0), d(0), e(0) {}
};

struct MemoryReaderState {
	unsigned char* buffer;
	unsigned int bufsize;
	unsigned int current_pos;
};

struct VERTEX {
	// POSITION - 0 BYTES
	FLOAT X;
	FLOAT Y;
	FLOAT Z;
	// NORMAL - 12 BYTES
	FLOAT NX;
	FLOAT NY;
	FLOAT NZ;
	// TANGENT - 24 BYTES
	FLOAT TX;
	FLOAT TY;
	FLOAT TZ;
	// BINORMAL - 36 BYTES
	FLOAT BX;
	FLOAT BY;
	FLOAT BZ;
	// COLOR - 48 BYTES
	COLOR Color;
	// TEXCOORD - 64 BYTES
	FLOAT U;
	FLOAT V;
	// TOTAL - 72 BYTES
};

// Material
struct Material {
	DirectX::XMFLOAT4 Ambient;
	DirectX::XMFLOAT4 Diffuse;
	DirectX::XMFLOAT4 Specular;
	Material(float ambientRed, float ambientGreen, float ambientBlue, float ambientAlpha,
		float diffuseRed, float diffuseGreen, float diffuseBlue, float diffuseAlpha,
		float specularRed, float specularGreen, float specularBlue, float specularAlpha)
	{
		Ambient.x = ambientRed;
		Ambient.y = ambientGreen;
		Ambient.z = ambientBlue;
		Ambient.w = ambientAlpha;
		Diffuse.x = diffuseRed;
		Diffuse.y = diffuseGreen;
		Diffuse.z = diffuseBlue;
		Diffuse.w = diffuseAlpha;
		Specular.x = specularRed;
		Specular.y = specularGreen;
		Specular.z = specularBlue;
		Specular.w = specularAlpha;
	}
	Material() : Material(1,1,1,1,1,1,1,1,1,1,1,1) {}
};

// LightType
struct LightType {
	DirectX::XMFLOAT4 Ambient;
	DirectX::XMFLOAT4 Diffuse;
	DirectX::XMFLOAT4 Specular;
	float LightRange;
	float padding_000;
	float padding_001;
	float padding_002;
	LightType(float ambientRed, float ambientGreen, float ambientBlue, float ambientAlpha,
		float diffuseRed, float diffuseGreen, float diffuseBlue, float diffuseAlpha,
		float specularRed, float specularGreen, float specularBlue, float specularAlpha,
		float lightRangeValue)
	{
		Ambient.x = ambientRed;
		Ambient.y = ambientGreen;
		Ambient.z = ambientBlue;
		Ambient.w = ambientAlpha;
		Diffuse.x = diffuseRed;
		Diffuse.y = diffuseGreen;
		Diffuse.z = diffuseBlue;
		Diffuse.w = diffuseAlpha;
		Specular.x = specularRed;
		Specular.y = specularGreen;
		Specular.z = specularBlue;
		Specular.w = specularAlpha;
		LightRange = lightRangeValue;
		padding_000 = 0.0f;
		padding_001 = 0.0f;
		padding_002 = 0.0f;
	}
	LightType() : LightType(0,0,0,0,0,0,0,0,0,0,0,0,0) {}
};