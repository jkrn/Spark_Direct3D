// Constants
#include "../ShaderConstants.hpp"
static const float depth_offset = -0.00001f;
static const float SMAP_SIZE = 8192.0f;
static const float DX = 1.0f / SMAP_SIZE;
static const int pcf_filter_size_2 = 4;
static const float pcf_filter_border = ((float)pcf_filter_size_2) - 0.5f;

//----------------------------------------------------------
// Register Slots
//----------------------------------------------------------

// Textures
Texture2D GlyphAtlasTexture : register(t0);
Texture2D LevelTextureRGBA : register(t1);
Texture2D LevelTextureNORM : register(t2);
Texture2D PlayerTextureRGBA : register(t3);
Texture2D SkyTextureRGBA : register(t4);
Texture2D ShadowMap : register(t5);

// Sampler
sampler Sampler : register(s0);
SamplerComparisonState Sampler_shadowmap : register(s1);

//----------------------------------------------------------
// Costum Structs
//----------------------------------------------------------

struct LightType{
	// ------------------------
	float4 Ambient; // 16 byte
	// ------------------------
	float4 Diffuse; // 16 byte
	// ------------------------
	float4 Specular; // 16 byte
	// ------------------------
	float LightRange; // 4 byte
	float padding_000; // 4 byte
	float padding_001; // 4 byte
	float padding_002; // 4 byte
	// ------------------------
};

struct Material{
	// ------------------------
	float4 Ambient; // 16 byte
	// ------------------------
	float4 Diffuse; // 16 byte
	// ------------------------
	float4 Specular; // 16 byte
	// ------------------------
};

struct RectPlatform{
	// ------------------------
	float xPos; // 4 byte
	float yPos; // 4 byte
	float dx2; // 4 byte
	float dy2; // 4 byte
	// ------------------------
	float z; // 4 byte
	float padding_000; // 4 byte
	float padding_001; // 4 byte
	float padding_002; // 4 byte
	// ------------------------
};

struct Gem{
	// ------------------------
	float3 Position; // 12 byte
	float spinDirection; // 4 byte
	// ------------------------
	float phiAngle; // 4 byte
	float thetaAngle; // 4 byte
	uint lightTypeIdx; // 4 byte
	uint isCollected; // 4 byte
	// ------------------------
};

struct Checkpoint{
	// ------------------------
	float3 Position; // 12 byte
	uint lightTypeIdx; // 4 byte
	// ------------------------
	uint isActive; // 4 byte
	uint padding_000; // 4 byte
	uint padding_001; // 4 byte
	uint padding_002; // 4 byte
	// ------------------------
};

struct Upwind {
	// ------------------------
	float xPos; // 4 byte
	float yPos; // 4 byte
	float zPos; // 4 byte
	float width; // 4 byte
	// ------------------------
	float widthSquared; // 4 byte
	float height; // 4 byte
	uint isActive; // 4 byte
	uint padding_000; // 4 byte
	// ------------------------
};

struct Lamp{
	// ------------------------
	float3 Position; // 12 byte
	uint lightTypeIdx; // 4 byte
	// ------------------------
};

struct Textbox{
	// ------------------------
	float xPos; // 4 byte
	float yPos; // 4 byte
	float dx2; // 4 byte
	float dy2; // 4 byte
	// ------------------------
};

//----------------------------------------------------------
// Constant Buffer
//----------------------------------------------------------

cbuffer ConstantBufferCamera : register(b0)
{
	matrix world; // 4 * 16 byte
	matrix view; // 4 * 16 byte
	matrix projection; // 4 * 16 byte
	matrix view_shadowmap; // 4 * 16 byte
	matrix projection_shadowmap; // 4 * 16 byte
};

cbuffer ConstantBufferPerFrame : register(b1)
{
	// ------------------------
	Gem gemsArray[NUM_MAX_GEMS]; // 2 * NUM_MAX_GEMS * 16 byte
	Checkpoint checkpointArray[NUM_MAX_CHECKPOINTS]; // NUM_MAX_CHECKPOINTS * 16 byte
	Upwind upwindArray[NUM_MAX_UPWINDS]; // 2 * NUM_MAX_UPWINDS * 16 byte
	// ------------------------
	float4 eyePosition; // 16 byte
	// ------------------------
	float playerX; // 4 byte
	float playerY; // 4 byte
	float playerZ; // 4 byte
	float playerAngle; // 4 byte
	// ------------------------
	float playerContourZ; // 4 byte
	int gameStatus; // 4 byte
	int isLevelStarted; // 4 byte
	int isPlayerNotOnGround; // 4 byte
	// ------------------------
}

cbuffer ConstantBufferPerLevel : register(b2)
{
	// ------------------------
	LightType lightTypesArray[NUM_MAX_LIGHT_TYPES]; // 4 * NUM_MAX_LIGHT_TYPES * 16 byte
	Lamp lampsArray[NUM_MAX_LAMPS]; // NUM_MAX_LAMPS * 16 byte
	RectPlatform rectPlatformArray[NUM_MAX_PLATFORMS]; // 2 * NUM_MAX_PLATFORMS * 16 byte
	Textbox textboxArray[NUM_MAX_TEXTBOXES]; // NUM_MAX_TEXTBOXES * 16 byte
	Material materialStatic; // 3 * 16 byte
	Material materialPlatform; // 3 * 16 byte
	Material materialPlayer; // 3 * 16 byte
	// ------------------------
	int numLightTypes; // 4 byte
	int numLamps; // 4 byte
	int numGems; // 4 byte
	int numCheckpoints; // 4 byte
	// ------------------------
	int numUpwinds; // 4 byte
	int numRectPlatforms; // 4 byte
	int numTextboxes; // 4 byte
	float playerRadius; // 4 byte
	// ------------------------
	float aspectRatio; // 4 byte
	uint padding_000; // 4 byte
	uint padding_001; // 4 byte
	uint padding_002; // 4 byte
	// ------------------------
}

//----------------------------------------------------------
// Vertex Shader Output
//----------------------------------------------------------

struct VS_OUTPUT{
	float4 Pos : SV_POSITION;
	float4 PosU : POSITION;
	float4 Normal : NORMAL;
	float4 Tangent : TANGENT;
	float4 Binormal : BINORMAL;
	float4 Color : COLOR0;
	float2 TexCoord : TEXCOORD0;
	float4 ProjTexCoord : TEXCOORD1;
};

//----------------------------------------------------------
// Vertex Shader
//----------------------------------------------------------

VS_OUTPUT VS_PLAYER(float4 Pos : POSITION, float4 Normal : NORMAL, float4 Tangent : TANGENT, float4 Binormal : BINORMAL, float4 Color : COLOR, float2 Tex : TEXCOORD){
	VS_OUTPUT output = (VS_OUTPUT)0;
	// === (1) VERTEX POSITION ===
	// Player Angle
	float alpha = playerAngle;
	float px = Pos[0];
	float py = Pos[1];
	Pos[0] = px * cos(alpha) - py * sin(alpha);
	Pos[1] = px * sin(alpha) + py * cos(alpha);
	// Player Position
	Pos[0] += playerX;
	Pos[1] += playerY;
	Pos[2] += playerZ;
	output.PosU = Pos;
	// Camera Transformation
	output.Pos = mul(Pos, world);
	output.Pos = mul(output.Pos, view);
	output.Pos = mul(output.Pos, projection);
	// === (2) VERTEX NORMAL ===
	float3 normVector = Normal.xyz;
	float nx = normVector.x;
	float ny = normVector.y;
	normVector.x = nx * cos(alpha) - ny * sin(alpha);
	normVector.y = nx * sin(alpha) + ny * cos(alpha);
	output.Normal = float4(normVector, 0.0f);
	// === (3) VERTEX TANGENT ===
	output.Tangent = Tangent;
	// === (4) VERTEX BINORMAL ===
	output.Binormal = Binormal;
	// === (5) VERTEX COLOR ===
	output.Color = Color;
	// === (6) TEX COORD ===
	output.TexCoord = Tex;
	// === (7) PROJ TEX COORD ===
	output.ProjTexCoord = mul(Pos, world);
	output.ProjTexCoord = mul(output.ProjTexCoord, view_shadowmap);
	output.ProjTexCoord = mul(output.ProjTexCoord, projection_shadowmap);
	return output;
}

VS_OUTPUT VS_PLATFORM(float4 Pos : POSITION, float4 Normal : NORMAL, float4 Tangent : TANGENT, float4 Binormal : BINORMAL, float4 Color : COLOR, float2 Tex : TEXCOORD, uint instanceID : SV_InstanceID){
	VS_OUTPUT output = (VS_OUTPUT)0;
	// === (0) INSTANCE POSITION ===
	RectPlatform currPlatform = rectPlatformArray[instanceID];
	Pos.x += currPlatform.xPos;
	Pos.y += currPlatform.yPos;
	Pos.z += currPlatform.z;
	output.PosU = Pos;
	// === (1) VERTEX POSITION ===
	// Camera Transformation
	output.Pos = mul(Pos, world);
	output.Pos = mul(output.Pos, view);
	output.Pos = mul(output.Pos, projection);
	// === (2) VERTEX NORMAL ===
	float3 normVector = Normal.xyz;
	output.Normal = float4(normVector, 0.0f);
	// === (3) VERTEX TANGENT ===
	output.Tangent = Tangent;
	// === (4) VERTEX BINORMAL ===
	output.Binormal = Binormal;
	// === (5) VERTEX COLOR ===
	output.Color = Color;
	// === (6) TEX COORD ===
	output.TexCoord = Tex;
	// === (7) PROJ TEX COORD ===
	output.ProjTexCoord = mul(Pos, world);
	output.ProjTexCoord = mul(output.ProjTexCoord, view_shadowmap);
	output.ProjTexCoord = mul(output.ProjTexCoord, projection_shadowmap);
	return output;
}

VS_OUTPUT VS_STATIC(float4 Pos : POSITION, float4 Normal : NORMAL, float4 Tangent : TANGENT, float4 Binormal : BINORMAL, float4 Color : COLOR, float2 Tex : TEXCOORD) {
	VS_OUTPUT output = (VS_OUTPUT)0;
	// === (1) VERTEX POSITION ===
	output.PosU = Pos;
	// Camera Transformation
	output.Pos = mul(Pos, world);
	output.Pos = mul(output.Pos, view);
	output.Pos = mul(output.Pos, projection);
	// === (2) VERTEX NORMAL ===
	output.Normal = float4(Normal.xyz, 0.0f);
	// === (3) VERTEX TANGENT ===
	output.Tangent = float4(Tangent.xyz, 0.0f);
	// === (4) VERTEX BINORMAL ===
	output.Binormal = float4(Binormal.xyz, 0.0f);
	// === (5) VERTEX COLOR ===
	output.Color = Color;
	// === (6) TEX COORD ===
	output.TexCoord = Tex;
	// === (7) PROJ TEX COORD ===
	output.ProjTexCoord = mul(Pos, world);
	output.ProjTexCoord = mul(output.ProjTexCoord, view_shadowmap);
	output.ProjTexCoord = mul(output.ProjTexCoord, projection_shadowmap);
	return output;
}

VS_OUTPUT VS_GEM(float4 Pos : POSITION, float4 Normal : NORMAL, float4 Tangent : TANGENT, float4 Binormal : BINORMAL, float4 Color : COLOR, float2 Tex : TEXCOORD, uint instanceID : SV_InstanceID){
	VS_OUTPUT output = (VS_OUTPUT)0;
	// === (0) INSTANCE POSITION ===
	Gem gem = gemsArray[instanceID];
	if (gem.isCollected == 1) { return output; }
	// Gem Angles
	float spinDir = gem.spinDirection;
	float phi = spinDir * (gem.phiAngle);
	float theta = gem.thetaAngle;
	// Transformation
	float px = Pos.x;
	float py = Pos.y;
	float pz = Pos.z + GEM_Z_OFFSET;
	float px_t1 = px * cos(-phi) - py * sin(-phi);
	float py_t1 = px * sin(-phi) + py * cos(-phi);
	float pz_t1 = pz;
	float px_t2 = px_t1 * cos(theta) + pz_t1 * sin(theta);
	float py_t2 = py_t1;
	float pz_t2 = -px_t1 * sin(theta) + pz_t1 * cos(theta);
	float px_t3 = px_t2 * cos(phi) - py_t2 * sin(phi);
	float py_t3 = px_t2 * sin(phi) + py_t2 * cos(phi);
	float pz_t3 = pz_t2 - GEM_Z_OFFSET;
	// Gem Position
	Pos.x = px_t3 + gem.Position.x;
	Pos.y = py_t3 + gem.Position.y;
	Pos.z = pz_t3 + gem.Position.z;
	output.PosU = Pos;
	// === (1) VERTEX POSITION ===
	// Camera Transformation
	output.Pos = mul(Pos, world);
	output.Pos = mul(output.Pos, view);
	output.Pos = mul(output.Pos, projection);
	// === (2) VERTEX NORMAL ===
	float3 normVector = Normal.xyz;
	float nx = normVector.x;
	float ny = normVector.y;
	float nz = normVector.z;
	float nx_t1 = nx * cos(-phi) - ny * sin(-phi);
	float ny_t1 = nx * sin(-phi) + ny * cos(-phi);
	float nz_t1 = nz;
	float nx_t2 = nx_t1 * cos(theta) + nz_t1 * sin(theta);
	float ny_t2 = ny_t1;
	float nz_t2 = -nx_t1 * sin(theta) + nz_t1 * cos(theta);
	float nx_t3 = nx_t2 * cos(phi) - ny_t2 * sin(phi);
	float ny_t3 = nx_t2 * sin(phi) + ny_t2 * cos(phi);
	float nz_t3 = nz_t2;
	normVector.x = nx_t3;
	normVector.y = ny_t3;
	normVector.z = nz_t3;
	output.Normal = float4(normVector.xyz, 0.0f);
	// === (3) VERTEX TANGENT ===
	output.Tangent = Tangent;
	// === (4) VERTEX BINORMAL ===
	output.Binormal = Binormal;
	// === (5) VERTEX COLOR ===
	output.Color = Color;
	// === (6) TEX COORD ===
	output.TexCoord = Tex;
	// === (7) PROJ TEX COORD ===
	output.ProjTexCoord = float4(0.0f, 0.0f, 0.0f, 1.0f);
	return output;
}

VS_OUTPUT VS_CHECKPOINT(float4 Pos : POSITION, float4 Normal : NORMAL, float4 Tangent : TANGENT, float4 Binormal : BINORMAL, float4 Color : COLOR, float2 Tex : TEXCOORD, uint instanceID : SV_InstanceID){
	VS_OUTPUT output = (VS_OUTPUT)0;
	// === (0) INSTANCE POSITION ===
	Checkpoint checkpoint = checkpointArray[instanceID];
	// Checkpoint Position
	Pos.x += checkpoint.Position.x;
	Pos.y += checkpoint.Position.y;
	Pos.z += checkpoint.Position.z;
	output.PosU = Pos;
	// === (1) VERTEX POSITION ===
	// Camera Transformation
	output.Pos = mul(Pos, world);
	output.Pos = mul(output.Pos, view);
	output.Pos = mul(output.Pos, projection);
	// === (2) VERTEX NORMAL ===
	float3 normVector = Normal.xyz;
	output.Normal = float4(normVector.xyz, 0.0f);
	// === (3) VERTEX TANGENT ===
	output.Tangent = Tangent;
	// === (4) VERTEX BINORMAL ===
	output.Binormal = Binormal;
	// === (5) VERTEX COLOR ===
	output.Color = Color;
	// === (6) TEX COORD ===
	output.TexCoord = Tex;
	// === (7) PROJ TEX COORD ===
	output.ProjTexCoord = float4(0.0f, 0.0f, 0.0f, 1.0f);
	return output;
}

VS_OUTPUT VS_UPWIND(float4 Pos : POSITION, float4 Normal : NORMAL, float4 Tangent : TANGENT, float4 Binormal : BINORMAL, float4 Color : COLOR, float2 Tex : TEXCOORD, uint instanceID : SV_InstanceID) {
	VS_OUTPUT output = (VS_OUTPUT)0;
	// === (0) INSTANCE POSITION ===
	Upwind upwind = upwindArray[instanceID];
	// Scale
	Pos.x = Pos.x * upwind.width;
	Pos.y = Pos.y * upwind.width;
	Pos.z = Pos.z * (1.0f / 256.0f);
	// Upwind Position
	Pos.x += upwind.xPos;
	Pos.y += upwind.yPos;
	Pos.z += upwind.zPos;
	output.PosU = Pos;
	// === (1) VERTEX POSITION ===
	// Camera Transformation
	output.Pos = mul(Pos, world);
	output.Pos = mul(output.Pos, view);
	output.Pos = mul(output.Pos, projection);
	// === (2) VERTEX NORMAL ===
	float3 normVector = Normal.xyz;
	output.Normal = float4(normVector.xyz, 0.0f);
	// === (3) VERTEX TANGENT ===
	output.Tangent = Tangent;
	// === (4) VERTEX BINORMAL ===
	output.Binormal = Binormal;
	// === (5) VERTEX COLOR ===
	output.Color = Color;
	// === (6) TEX COORD ===
	output.TexCoord = Tex;
	// === (7) PROJ TEX COORD ===
	output.ProjTexCoord = float4(0.0f, 0.0f, 0.0f, 1.0f);
	return output;
}

VS_OUTPUT VS_LAMP(float4 Pos : POSITION, float4 Normal : NORMAL, float4 Tangent : TANGENT, float4 Binormal : BINORMAL, float4 Color : COLOR, float2 Tex : TEXCOORD, uint instanceID : SV_InstanceID){
	VS_OUTPUT output = (VS_OUTPUT)0;
	// === (0) INSTANCE POSITION ===
	Lamp lamp = lampsArray[instanceID];
	// Lamp Position
	Pos.x += lamp.Position.x;
	Pos.y += lamp.Position.y;
	Pos.z += lamp.Position.z;
	output.PosU = Pos;
	// === (1) VERTEX POSITION ===
	// Camera Transformation
	output.Pos = mul(Pos, world);
	output.Pos = mul(output.Pos, view);
	output.Pos = mul(output.Pos, projection);
	// === (2) VERTEX NORMAL ===
	float3 normVector = Normal.xyz;
	output.Normal = float4(normVector.xyz, 0.0f);
	// === (3) VERTEX TANGENT ===
	output.Tangent = Tangent;
	// === (4) VERTEX BINORMAL ===
	output.Binormal = Binormal;
	// === (5) VERTEX COLOR ===
	output.Color = Color;
	// === (6) TEX COORD ===
	output.TexCoord = Tex;
	// === (7) PROJ TEX COORD ===
	output.ProjTexCoord = float4(0.0f, 0.0f, 0.0f, 1.0f);
	return output;
}

VS_OUTPUT VS_SKY(float4 Pos : POSITION, float4 Normal : NORMAL, float4 Tangent : TANGENT, float4 Binormal : BINORMAL, float4 Color : COLOR, float2 Tex : TEXCOORD){
	VS_OUTPUT output = (VS_OUTPUT)0;
	// === (1) VERTEX POSITION ===
	float4 PosScaled = float4(SKY_SCALE * (Pos[0]) + playerX , SKY_SCALE * (Pos[1]) + playerY, SKY_SCALE * (Pos[2]) + playerZ, Pos[3]);
	output.PosU = PosScaled;
	// Camera Transformation
	output.Pos = mul(PosScaled, world);
	output.Pos = mul(output.Pos, view);
	output.Pos = mul(output.Pos, projection);
	// === (2) VERTEX NORMAL ===
	output.Normal = float4(Normal.xyz, 0.0f);
	// === (3) VERTEX TANGENT ===
	output.Tangent = float4(Tangent.xyz, 0.0f);
	// === (4) VERTEX BINORMAL ===
	output.Binormal = float4(Binormal.xyz, 0.0f);
	// === (5) VERTEX COLOR ===
	output.Color = Color;
	// === (6) TEX COORD ===
	output.TexCoord = Tex;
	// === (7) PROJ TEX COORD ===
	output.ProjTexCoord = float4(0.0f, 0.0f, 0.0f, 1.0f);
	return output;
}

VS_OUTPUT VS_TEXT(float4 Pos : POSITION, float4 Normal : NORMAL, float4 Tangent : TANGENT, float4 Binormal : BINORMAL, float4 Color : COLOR, float2 Tex : TEXCOORD){
	VS_OUTPUT output = (VS_OUTPUT)0;
	// === (1) VERTEX POSITION ===
	output.Pos = Pos;
	output.PosU = Pos;
	// === (2) VERTEX NORMAL ===
	float3 normVector = Normal.xyz;
	output.Normal = float4(normVector, 0.0f);
	// === (3) VERTEX TANGENT ===
	output.Tangent = Tangent;
	// === (4) VERTEX BINORMAL ===
	output.Binormal = Binormal;
	// === (5) VERTEX COLOR ===
	output.Color = Color;
	// === (6) TEX COORD ===
	output.TexCoord = Tex;
	// === (7) PROJ TEX COORD ===
	output.ProjTexCoord = float4(0.0f, 0.0f, 0.0f, 1.0f);
	return output;
}

VS_OUTPUT VS_BOXLAYER(float4 Pos : POSITION, float4 Normal : NORMAL, float4 Tangent : TANGENT, float4 Binormal : BINORMAL, float4 Color : COLOR, float2 Tex : TEXCOORD){
	VS_OUTPUT output = (VS_OUTPUT)0;
	// === (1) VERTEX POSITION ===
	output.Pos = Pos;
	output.PosU = Pos;
	// === (2) VERTEX NORMAL ===
	float3 normVector = Normal.xyz;
	output.Normal = float4(normVector, 0.0f);
	// === (3) VERTEX TANGENT ===
	output.Tangent = Tangent;
	// === (4) VERTEX BINORMAL ===
	output.Binormal = Binormal;
	// === (5) VERTEX COLOR ===
	output.Color = Color;
	// === (6) TEX COORD ===
	output.TexCoord = Tex;
	// === (7) PROJ TEX COORD ===
	output.ProjTexCoord = float4(0.0f, 0.0f, 0.0f, 1.0f);
	return output;
}

//----------------------------------------------------------
// Light Computation
//----------------------------------------------------------

void ComputeLampLight(int currLightIndex, float shadowFactor, float lightDir, float3 pos, float3 normal, float3 toEyeVector, out float4 diffResult, out float4 specResult){
	// Get Lamp
	Lamp lamp = lampsArray[currLightIndex];
	LightType lightType = lightTypesArray[lamp.lightTypeIdx];
	float maxRangeValue = lightType.LightRange;
	// Initialize outputs
	float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 specular = float4(0.0f, 0.0f, 0.0f, 0.0f);
	// Set diffuse / specular
	float diffuseFactor = 0;
	float specularFactor = 0;
	// Light Vector
	float3 lightVec = pos - lamp.Position.xyz;
	float distance = sqrt(dot(lightVec, lightVec));
	float3 lightVecNormal = lightDir * normalize(lightVec);
	if (distance <= maxRangeValue) {
		float distRatio = distance / maxRangeValue;
		// Diffuse Factor
		diffuseFactor = max(dot(-lightVecNormal, normal), 0.0f) * (1.0f - distRatio * distRatio);
		// Specular Factor
		float specBase = max(dot(reflect(lightVecNormal, normal), toEyeVector), 0.0f);
		if (specBase > 0.0f) { specularFactor = pow(specBase, 256) * (1.0f - distRatio); }
	}
	diffResult = max(shadowFactor , 0.5f) * diffuseFactor * lightType.Diffuse;
	specResult = shadowFactor * specularFactor * lightType.Specular;
}

void ComputeCheckpointLight(int currCheckpointIndex, float lightDir, float3 pos, float3 normal, float3 toEyeVector, out float4 diffResult, out float4 specResult){
	// Get Checkpoint
	Checkpoint checkpoint = checkpointArray[currCheckpointIndex];
	LightType lightType = lightTypesArray[checkpoint.lightTypeIdx];
	float maxRangeValue = lightType.LightRange;
	// Initialize outputs
	float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 specular = float4(0.0f, 0.0f, 0.0f, 0.0f);
	// Set diffuse / specular
	float diffuseFactor = 0;
	float specularFactor = 0;
	// Light Vector
	float3 lightVec = pos - ( checkpoint.Position.xyz + float3(0.0f, 0.0f, CHECKPOINT_Z_OFFSET) );
	float distance = sqrt(dot(lightVec, lightVec));
	float3 lightVecNormal = lightDir * normalize(lightVec * float3(0.0625f, 0.0625f, 1.0f));
	if (distance <= maxRangeValue) {
		float distRatio = distance / maxRangeValue;
		// Diffuse Factor
		diffuseFactor = max(dot(-lightVecNormal, normal), 0.0f) * (1.0f - distRatio * distRatio);
		// Specular Factor
		float specBase = max(dot(reflect(lightVecNormal, normal), toEyeVector), 0.0f);
		if (specBase > 0.0f) { specularFactor = pow(specBase, 256) * (1.0f - distRatio); }
	}
	diffResult = diffuseFactor * lightType.Diffuse;
	specResult = specularFactor * lightType.Specular;
}

void ComputeGemLight(int currGemIndex, float lightDir, float3 pos, float3 normal, float3 toEyeVector, out float4 diffResult, out float4 specResult){
	// Get Gem
	Gem gem = gemsArray[currGemIndex];
	if (gem.isCollected == 1) {
		diffResult = float4(0.0f, 0.0f, 0.0f, 0.0f);
		specResult = float4(0.0f, 0.0f, 0.0f, 0.0f);
		return; 
	}
	LightType lightType = lightTypesArray[gem.lightTypeIdx];
	float maxRangeValue = lightType.LightRange;
	// Gem Angles
	float spinDir = gem.spinDirection;
	float phi = spinDir * gem.phiAngle;
	float theta = gem.thetaAngle;
	float cosTheta = cos(theta);
	float sinTheta = sin(theta);
	float cosPhi = cos(phi);
	float sinPhi = sin(phi);
	// Transformation
	float3 posUn = gem.Position.xyz;
	float px_t1 = GEM_Z_OFFSET * sinTheta;
	float px_t2 = px_t1 * cosPhi;
	float py_t2 = px_t1 * sinPhi;
	float pz_t2 = GEM_Z_OFFSET * cosTheta;
	float3 lightPosition_T = float3(posUn.x + px_t2, posUn.y + py_t2, posUn.z + pz_t2 - GEM_Z_OFFSET);
	// Initialize outputs
	float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 specular = float4(0.0f, 0.0f, 0.0f, 0.0f);
	// Set diffuse / specular
	float diffuseFactor = 0;
	float specularFactor = 0;
	// Light Vector
	float3 lightVec = pos - lightPosition_T;
	float distance = length(lightVec);
	float3 lightVecNormal = lightDir * normalize(lightVec);
	if (distance <= maxRangeValue) {
		float distRatio = distance / maxRangeValue;
		// Diffuse Factor
		diffuseFactor = max(dot(-lightVecNormal, normal), 0.0f) * (1.0f - distRatio);
		// Specular Factor
		float specBase = max(dot(reflect(lightVecNormal, normal), toEyeVector), 0.0f);
		if (specBase > 0.0f) { specularFactor = pow(specBase, 256) * (1.0f - distRatio); }
	}
	diffResult = diffuseFactor * lightType.Diffuse;
	specResult = specularFactor * lightType.Specular;
}

//----------------------------------------------------------
// Distance Computation
//----------------------------------------------------------

float distPointRect(float px, float py, float cx, float cy, float dx2, float dy2, float xScale){
	float distX = abs(cx - px);
	float distY = abs(cy - py);
	if (distX > dx2 && distY > dy2) { return length(float2((distX - dx2) * xScale, distY - dy2)); }
	else if (distX > dx2 && distY <= dy2) { return (distX - dx2) * xScale; }
	else if (distX <= dx2 && distY > dy2) { return (distY - dy2); }
	return 0;
}

//----------------------------------------------------------
// Contour Computation
//----------------------------------------------------------

float4 computeCircleContour(float px, float py, float cx, float cy, float r, float maxDist) {
	float v = (1.0f - min(1.0f , abs(length(float2(px - cx, py - cy)) - r) / maxDist) );
	return float4(0.25f * v , 0.25f * v , 0.25f * v, 1.0f);
}

float4 computePlayerCircleContour(float3 posValue, float3 N_normalcheck) {
	float distZ_abs = abs(playerZ - posValue.z);
	float4 PlayerCircleContour = float4(0.0f, 0.0f, 0.0f, 1.0f);
	bool normalCheck = abs(dot(N_normalcheck, float3(0.0f, 0.0f, 1.0f))) >= 0.5f;
	bool contourZCheck = abs(posValue.z - playerContourZ) <= PLAYER_CONTOUR_Z_RANGE;
	if (isPlayerNotOnGround && normalCheck && contourZCheck && playerZ > posValue.z && length(float2(posValue.x - playerX, posValue.y - playerY)) <= (playerRadius * 1.125f)) {
		PlayerCircleContour = computeCircleContour(posValue.x, posValue.y, playerX, playerY, playerRadius, playerRadius * 0.125f);
	}
	return PlayerCircleContour;
}

//----------------------------------------------------------
// Shadow Map Sampling
//----------------------------------------------------------

float sampleShadowMap(float4 coord) {
	coord.xyz /= coord.w;
	float2 coord_param = float2( coord.x * 0.5 + 0.5, 1.0 - (coord.y * 0.5 + 0.5) );
	float sum = 0.0f;
	float norm = 0.0f;
	float x, y;
	for (x = -pcf_filter_border; x <= +pcf_filter_border; x += 1.0f) {
		for (y = -pcf_filter_border; y <= +pcf_filter_border; y += 1.0f) {
			sum += ShadowMap.SampleCmpLevelZero(Sampler_shadowmap, coord_param + float2(x * DX, y * DX), coord.z + depth_offset).r;
			norm += 1.0f;
		}
	}
	return (sum / norm);
}

//----------------------------------------------------------
// Pixel Shader
//----------------------------------------------------------

float4 PS_PLAYER(VS_OUTPUT input) : SV_Target {
	// Normal
	float3 N = normalize( float3(input.Normal[0], input.Normal[1], input.Normal[2]) );
	// Position
	float3 posValue = float3(input.PosU[0], input.PosU[1], input.PosU[2]);
	// Eye Vector
	float3 toEyeVector = normalize(eyePosition.xyz - posValue);
	// Sample Shadow Map
	float shadowFactor = sampleShadowMap(input.ProjTexCoord);
	// Result Values
	float4 ambient = float4(AMBIENT_LIGHT_R, AMBIENT_LIGHT_G, AMBIENT_LIGHT_B, AMBIENT_LIGHT_A);
	float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 specular = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 D = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 S = float4(0.0f, 0.0f, 0.0f, 0.0f);
	// Compute Lamp Lights
	for (int j = 0; j < numLamps; j++) {
		ComputeLampLight(j, shadowFactor, 1.0f, posValue, N, toEyeVector, D, S);
		diffuse += D;
		specular += S;
	}
	// Compute Checkpoint Lights
	for (int k = 0; k < numCheckpoints; k++) {
		ComputeCheckpointLight(k, 1.0f, posValue, N, toEyeVector, D, S);
		diffuse += D;
		specular += S;
	}
	// Compute Gem Lights
	for (int l = 0; l < numGems; l++) {
		ComputeGemLight(l, 1.0f, posValue, N, toEyeVector, D, S);
		diffuse += D;
		specular += S;
	}
	// Set Material
	ambient = ambient * materialPlayer.Ambient;
	diffuse = diffuse * materialPlayer.Diffuse;
	specular = specular * materialPlayer.Specular;
	// Result Color
	float4 currPosColor = PlayerTextureRGBA.Sample(Sampler, input.TexCoord) * input.Color;
	float4 resultColor = (ambient + diffuse + specular) * currPosColor;
	resultColor.a = input.Color.a;
	return resultColor;
}

float4 PS_PLATFORM(VS_OUTPUT input) : SV_Target {
	// Normal, Tangent, Binormal
	float3 N = normalize(float3(input.Normal[0], input.Normal[1], input.Normal[2]));
	float3 T = normalize(float3(input.Tangent[0], input.Tangent[1], input.Tangent[2]));
	float3 B = normalize(float3(input.Binormal[0], input.Binormal[1], input.Binormal[2]));
	float3 N_normalcheck = N;
	// Adjust Normal
	N = normalize(mul(2.0f * ((LevelTextureNORM.Sample(Sampler, input.TexCoord)).xyz) - 1.0f , float3x3(T, B, N)));
	// Position
	float3 posValue = float3(input.PosU[0], input.PosU[1], input.PosU[2]);
	// Eye Vector
	float3 toEyeVector = normalize(eyePosition.xyz - posValue);
	// Player Circle Contour
	float4 PlayerCircleContour = computePlayerCircleContour(posValue.xyz, N_normalcheck);
	// Sample Shadow Map
	float shadowFactor = sampleShadowMap(input.ProjTexCoord);
	// Result Values
	float4 ambient = float4(AMBIENT_LIGHT_R, AMBIENT_LIGHT_G, AMBIENT_LIGHT_B, AMBIENT_LIGHT_A);
	float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 specular = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 D = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 S = float4(0.0f, 0.0f, 0.0f, 0.0f);
	// Compute Lamp Lights
	for (int j = 0; j < numLamps; j++) {
		ComputeLampLight(j, shadowFactor, 1.0f, posValue, N, toEyeVector, D, S);
		diffuse += D;
		specular += S;
	}
	// Compute Checkpoint Lights
	for (int k = 0; k < numCheckpoints; k++) {
		ComputeCheckpointLight(k, 1.0f, posValue, N, toEyeVector, D, S);
		diffuse += D;
		specular += S;
	}
	// Compute Gem Lights
	for (int l = 0; l < numGems; l++) {
		ComputeGemLight(l, 1.0f, posValue, N, toEyeVector, D, S);
		diffuse += D;
		specular += S;
	}
	// Set Material
	ambient = ambient * materialPlatform.Ambient;
	diffuse = diffuse * materialPlatform.Diffuse;
	specular = specular * materialPlatform.Specular;
	// Result Color
	float4 currPosColor = LevelTextureRGBA.Sample(Sampler, input.TexCoord) * input.Color;
	float4 resultColor = (ambient + diffuse + specular) * currPosColor + PlayerCircleContour;
	resultColor.a = input.Color.a;
	return resultColor;
}

float4 PS_STATIC(VS_OUTPUT input) : SV_Target{
	// Normal, Tangent, Binormal
	float3 N = normalize(float3(input.Normal[0], input.Normal[1], input.Normal[2]));
	float3 N_normalcheck = N;
	// Position
	float3 posValue = float3(input.PosU[0], input.PosU[1], input.PosU[2]);
	// Eye Vector
	float3 toEyeVector = normalize(eyePosition.xyz - posValue);
	// Player Circle Contour
	float4 PlayerCircleContour = computePlayerCircleContour(posValue.xyz, N_normalcheck);
	// Sample Shadow Map
	float shadowFactor = sampleShadowMap(input.ProjTexCoord);
	// Result Values
	float4 ambient = float4(AMBIENT_LIGHT_R, AMBIENT_LIGHT_G, AMBIENT_LIGHT_B, AMBIENT_LIGHT_A);
	float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 specular = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 D = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 S = float4(0.0f, 0.0f, 0.0f, 0.0f);
	// Compute Lamp Lights
	for (int j = 0; j < numLamps; j++) {
		ComputeLampLight(j, shadowFactor, 1.0f, posValue, N, toEyeVector, D, S);
		diffuse += D;
		specular += S;
	}
	// Compute Checkpoint Lights
	for (int k = 0; k < numCheckpoints; k++) {
		ComputeCheckpointLight(k, 1.0f, posValue, N, toEyeVector, D, S);
		diffuse += D;
		specular += S;
	}
	// Compute Gem Lights
	for (int l = 0; l < numGems; l++) {
		ComputeGemLight(l, 1.0f, posValue, N, toEyeVector, D, S);
		diffuse += D;
		specular += S;
	}
	// Set Material
	ambient = ambient * materialStatic.Ambient;
	diffuse = diffuse * materialStatic.Diffuse;
	specular = specular * materialStatic.Specular;
	// Result Color
	float4 resultColor = (ambient + diffuse + specular) * input.Color + PlayerCircleContour;
	resultColor.a = input.Color.a;
	return resultColor;
}

float4 PS_GEM(VS_OUTPUT input) : SV_Target {
	// Normal
	float3 normValue = float3(input.Normal[0], input.Normal[1], input.Normal[2]);
	normValue = normalize(normValue);
	// Position
	float3 posValue = float3(input.PosU[0], input.PosU[1], input.PosU[2]);
	// Eye Vector
	float3 toEyeVector = normalize(eyePosition.xyz - posValue);
	// Result Values
	float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 D = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 S = float4(0.0f, 0.0f, 0.0f, 0.0f);
	// Compute Gem Lights
	for (int k = 0; k < numGems; k++) {
		ComputeGemLight(k, -1.0f, posValue, normValue, toEyeVector, D, S);
		diffuse += D;
	}
	// Result Color
	float4 resultColor = diffuse * input.Color;
	resultColor.a = input.Color.a;
	return resultColor;
}

float4 PS_CHECKPOINT(VS_OUTPUT input) : SV_Target {
	// Normal
	float3 normValue = float3(input.Normal[0], input.Normal[1], input.Normal[2]);
	normValue = normalize(normValue);
	// Position
	float3 posValue = float3(input.PosU[0], input.PosU[1], input.PosU[2]);
	// Eye Vector
	float3 toEyeVector = normalize(eyePosition.xyz - posValue);
	// Result Values
	float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 D = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 S = float4(0.0f, 0.0f, 0.0f, 0.0f);
	// Compute Checkpoint Lights
	for (int k = 0; k < numCheckpoints; k++) {
		ComputeCheckpointLight(k, -1.0f, posValue, normValue, toEyeVector, D, S);
		diffuse += D;
	}
	// Result Color
	float4 resultColor = diffuse * input.Color;
	resultColor.a = input.Color.a;
	return resultColor;
}

float4 PS_UPWIND(VS_OUTPUT input) : SV_Target{
	// Normal
	float3 normValue = float3(input.Normal[0], input.Normal[1], input.Normal[2]);
	normValue = normalize(normValue);
	// Active flag
	float upwindActive = 0.0f;
	if (numUpwinds > 0) {
		upwindActive = (float)upwindArray[0].isActive;
	}
	// Position
	float3 posValue = float3(input.PosU[0], input.PosU[1], input.PosU[2]);
	// Eye Vector
	float3 toEyeVector = normalize(eyePosition.xyz - posValue);
	// Result Values
	float4 diffuse = float4(1.0f, 1.0f, 1.0f, 1.0f);
	float4 D = float4(1.0f, 1.0f, 1.0f, 1.0f);
	float4 S = float4(1.0f, 1.0f, 1.0f, 1.0f);
	// Result Color
	float4 resultColor = diffuse * input.Color;
	resultColor.a = input.Color.a * 0.25f * upwindActive;
	return resultColor;
}

float4 PS_LAMP(VS_OUTPUT input) : SV_Target {
	// Normal
	float3 normValue = float3(input.Normal[0], input.Normal[1], input.Normal[2]);
	normValue = normalize(normValue);
	// Position
	float3 posValue = float3(input.PosU[0], input.PosU[1], input.PosU[2]);
	// Eye Vector
	float3 toEyeVector = normalize(eyePosition.xyz - posValue);
	// Result Values
	float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 D = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 S = float4(0.0f, 0.0f, 0.0f, 0.0f);
	// Compute Lamp Lights
	for (int k = 0; k < numLamps; k++) {
		ComputeLampLight(k, 1.0f, -1.0f, posValue, normValue, toEyeVector, D, S);
		diffuse += D;
	}
	// Result Color
	float4 resultColor = diffuse * input.Color;
	resultColor.a = input.Color.a;
	return resultColor;
}

float4 PS_SKY(VS_OUTPUT input) : SV_Target {
	float4 resultColor = SkyTextureRGBA.Sample(Sampler, input.TexCoord) * input.Color;
	return resultColor;
}

float4 PS_TEXT(VS_OUTPUT input) : SV_Target {
	float4 resultColor = GlyphAtlasTexture.Sample(Sampler, input.TexCoord) * input.Color;
	return resultColor;
}

float4 PS_BOXLAYER(VS_OUTPUT input) : SV_Target {
	// Position
	float2 posValue = float2(input.PosU[0], input.PosU[1]);
	// Textbox
	float textboxFactor = 0.0f;
	if (gameStatus > 0 && isLevelStarted) {
		for (int i = 0; i < numTextboxes; i++) {
			if ( (i == 0) || (i == gameStatus - 1) ) {
				Textbox currTextbox = textboxArray[i];
				float distX = abs(posValue.x - currTextbox.xPos);
				float distY = abs(posValue.y - currTextbox.yPos);
				if (distX <= (currTextbox.dx2 + TEXTBOX_FADE_OFF_RANGE_CHECK) && distY <= (currTextbox.dy2 + TEXTBOX_FADE_OFF_RANGE_CHECK)) {
					textboxFactor = TEXTBOX_ALPHA * (1.0 - min(distPointRect(posValue.x, posValue.y, currTextbox.xPos, currTextbox.yPos, currTextbox.dx2, currTextbox.dy2, aspectRatio), TEXTBOX_FADE_OFF_RANGE) * TEXTBOX_FADE_OFF_RANGE_INV);
				}
			}
		}
	}
	// Result Color
	float4 resultColor = float4(0.0, 0.0, 0.0, textboxFactor);
	return resultColor;
}