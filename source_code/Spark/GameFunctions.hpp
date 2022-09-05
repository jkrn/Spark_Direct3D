#pragma once

#include <cmath>
#include <algorithm>
#include <fstream>
#include <Windows.h>
#include <Xinput.h>
#include "resource.h"
#include "Constants.hpp"
#include "glyphatlas/GlyphAtlas.hpp"
#include "Rendering.hpp"
#include "Map.hpp"
#include "external/include/png.h"

// === Window ===
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
bool initWindow(HINSTANCE hInstance, int nCmdShow);
// === Resources ===
void LoadFileInResource(int name, int type, DWORD& size, void * &data);
// === Vector Math ===
double getDotProduct(Point3& a, Point3& b);
double getVectorAngle(Point3& a, Point3& b);
void getCrossProduct(Point3& a, Point3& b, Point3& c);
void setVertexVectors(unsigned int numVertices, unsigned int numIndices, VERTEX* vertexValues, unsigned int* indexValues, bool useWeight, bool calculateVertexNormals);
bool getRayTriangleIntersection(Point3 orig, Point3 dir, Point3 vert0, Point3 vert1, Point3 vert2, double& t_res);
// === PNG ===
void readMemoryPNG(png_structp png_ptr, png_bytep data, png_uint_32 length);
bool getPNGTextureData(void* buffer, DWORD size, unsigned int& width, unsigned int& height, unsigned int& rowbytes, png_bytepp& rows);
bool getResourceViewFromPNG(void* buffer, DWORD size, ID3D11ShaderResourceView** textureSRV);
// === BINMESH ===
void readBINMESH(unsigned int ResourceID, uint32_t& numIndices, uint32_t& numVertices, uint32_t* & indices , VERTEX* & vertices);
// === Distance Computation ===
void GetMinEdge02(double const& a11, double const& b1, double& pr_0, double& pr_1);
void GetMinEdge12(double const& a01, double const& a11, double const& b1, double const& f10, double const& f01, double& pr_0, double& pr_1);
void GetMinInterior(double const& p0_0, double const& p0_1, double const& h0, double const& p1_0, double const& p1_1, double const& h1, double& pr_0, double& pr_1);
void getDistancePointToTriangle(double const& point_x, double const& point_y, double const& point_z, double const& tr_0_x, double const& tr_0_y, double const& tr_0_z, double const& tr_1_x, double const& tr_1_y, double const& tr_1_z, double const& tr_2_x, double const& tr_2_y, double const& tr_2_z, double& closest_x, double& closest_y, double& closest_z, double& sqrDistance);
// === Mesh Generation ===
bool generatePlayerMesh();
bool generateCheckpointMesh();
bool generateUpwindMesh();
bool generateSkyMesh();
bool generateBoxlayerMesh();
bool generateGemMesh();
bool generateLampMesh();
bool generateRectPlatformMesh();
bool generateCirclePlatformMesh();
// === D3D11 ===
bool initSwapChainDeviceAndDeviceContext();
bool initBackBufferAndDepthStencilBuffer();
void initViewports();
bool initShaders();
bool initMeshes();
bool initRenderTexts();
bool initTextures();
bool initShadowMap();
bool initSamplerState();
bool initRasterizerState();
bool initBlendState();
bool initConstantBuffer();
// === System ===
bool initSystem(HINSTANCE hInstance, int nCmdShow);
void closeSystem();
// === Game Variables ===
void resetMovements();
void initGame(InitType initType);
// === Update Game Status ===
void updateGameStatus(GameStatus nextStatus);
// === Cursor ===
void lockCursor();
void releaseCursor();
// === Process Input ===
void get_movement_from_WASD_or_LeftStick(double& joyLeftDeltaScaledX, double& joyLeftDeltaScaledY, double& moveX, double& moveY);
bool is_Esc_or_B_pressed();
bool is_Esc_or_Start_pressed();
bool is_F1_or_Y_pressed();
bool is_F2_or_X_pressed();
bool is_Space_or_A_pressed();
bool is_MouseLeft_or_LB_pressed();
bool is_MouseRight_or_RB_pressed();
bool is_Q_or_Y_pressed();
void getStickMovement(JoyStickType type, double& dx, double& dy);
void getMouseMovement(int& dx, int& dy);
void processInput();
// === Movement ===
double getInAirFunctionValue(double t, ZFunction f);
double getPlayerZMovement(double a_int);
void updatePlayerPositon();
double getplayerContourZ();
// === Upwind ===
void checkUpwind();
void updateUpwindState();
// === Respawn ===
void updateRespawn();
// === Gems ===
void updateGems();
std::wstring getCollectedGemsString();
// === Checkpoints ===
void updateCheckpoints();
std::wstring getActivatedCheckpointsString();
// === Camera ===
void adjustCameraDistance(double at_posX, double at_posY, double at_posZ, double& eye_posX, double& eye_posY, double& eye_posZ);
void updateCamera();
// === Constant Buffer ===
void updateConstantBufferContent(bool initContent);
// === Game Status Screens ===
void showStartMenuScreen();
void showManualScreen();
void showCreditsScreen();
void showPausedScreen();
void showRunningScreen();
// === Shadow Map ===
void updateShadowMapCamera();
void computeShadowMap();
// === Render Target ===
void setCameraRenderTarget();
void setShadowMapRenderTarget();
// === Viewport ===
void setCameraViewport();
void setShadowMapViewport();
// === Camera Matrices ===
void setCameraMatricesForShadowMap();
void setCameraMatricesForPlayerCamera();
// === Clear Frame ===
void clearCameraFrame();
void clearShadowMapFrame();
// === Present Frame ===
void presentFrame();
// === Render Frame ===
void renderFrameStartMenu();
void renderFrameManual();
void renderFrameCredits();
void renderFramePaused();
void renderFrameRunning();
void renderFrameBoxlayer();