#include "GameFunctions.hpp"

using namespace std;
using namespace DirectX;

// === SYSTEM VARIABLES ===
// D3D11 Resoruces
D3D11_VIEWPORT g_camera_viewport = {};
D3D11_VIEWPORT g_shadowmap_viewport = {};
IDXGISwapChain* g_swapchain = NULL;
ID3D11Device* g_dev = NULL;
ID3D11DeviceContext* g_devcon = NULL;
ID3D11InputLayout* g_pLayout = NULL;
ID3D11RenderTargetView* g_backbuffer = NULL;
ID3D11Texture2D* g_pDSBuffer = NULL;
ID3D11DepthStencilState* g_pDSState = NULL;
ID3D11DepthStencilView* g_pDSView = NULL;
ID3D11Texture2D* g_depthMap = NULL;
ID3D11DepthStencilView* g_depthMapDSV = NULL;
ID3D11ShaderResourceView* g_depthMapSRV = NULL;
ID3D11SamplerState* g_pSamplerState_texture = NULL;
ID3D11SamplerState* g_pSamplerState_shadowmap = NULL;
ID3D11RasterizerState* g_pRasterizerState = NULL;
ID3D11BlendState* g_pBlendState_default = NULL;
ID3D11BlendState* g_pBlendState_alphaToCoverage = NULL;
// Vertex Shaders
ID3D11VertexShader* g_pVS_PLAYER = NULL;
ID3D11VertexShader* g_pVS_PLATFORM = NULL;
ID3D11VertexShader* g_pVS_STATIC = NULL;
ID3D11VertexShader* g_pVS_GEM = NULL;
ID3D11VertexShader* g_pVS_CHECKPOINT = NULL;
ID3D11VertexShader* g_pVS_UPWIND = NULL;
ID3D11VertexShader* g_pVS_LAMP = NULL;
ID3D11VertexShader* g_pVS_SKY = NULL;
ID3D11VertexShader* g_pVS_TEXT = NULL;
ID3D11VertexShader* g_pVS_BOXLAYER = NULL;
// Pixel Shaders
ID3D11PixelShader* g_pPS_PLAYER = NULL;
ID3D11PixelShader* g_pPS_PLATFORM = NULL;
ID3D11PixelShader* g_pPS_STATIC = NULL;
ID3D11PixelShader* g_pPS_GEM = NULL;
ID3D11PixelShader* g_pPS_CHECKPOINT = NULL;
ID3D11PixelShader* g_pPS_UPWIND = NULL;
ID3D11PixelShader* g_pPS_LAMP = NULL;
ID3D11PixelShader* g_pPS_SKY = NULL;
ID3D11PixelShader* g_pPS_TEXT = NULL;
ID3D11PixelShader* g_pPS_BOXLAYER = NULL;
// Textures
ID3D11ShaderResourceView* g_glyphAtlasTexture_SRV = NULL;
ID3D11ShaderResourceView* g_levelTexture_RGBA_SRV = NULL;
ID3D11ShaderResourceView* g_levelTexture_NORM_SRV = NULL;
ID3D11ShaderResourceView* g_playerTexture_RGBA_SRV = NULL;
ID3D11ShaderResourceView* g_skyTexture_RGBA_SRV = NULL;
// Constant Buffers
ConstantBufferCamera g_cb_camera;
ConstantBufferPerFrame g_cb_perFrame;
ConstantBufferPerLevel g_cb_perLevel;
ID3D11Buffer* g_pCBuffer_camera = NULL;
ID3D11Buffer* g_pCBuffer_perFrame = NULL;
ID3D11Buffer* g_pCBuffer_perLevel = NULL;
// Meshes
Mesh g_playerMesh;
Mesh g_checkpointMesh;
Mesh g_upwindMesh;
Mesh g_gemMesh;
Mesh g_lampMesh;
Mesh g_skyMesh;
Mesh g_boxlayerMesh;
Mesh g_rectPlatformMesh;
Mesh g_circlePlatformMesh;
// Render Texts
RenderText g_startMenuScreenText_array[num_startMenuScreenText_parts];
RenderText g_manualScreenText_array[num_manualScreenText_parts];
RenderText g_creditsScreenText_array[num_creditsScreenText_parts];
RenderText g_pausedScreenText_array[num_pausedScreenText_parts];
RenderText g_gemsRenderText_array[num_gemsScreenText_parts];
RenderText g_checkpointsRenderText_array[num_checkpointsScreenText_parts];
// Textboxes
Textbox g_textbox_array[num_textboxes];
// isRunning, isLevelStarted
bool g_isRunning = true;
bool g_isLevelStarted = false;
// Game Status
GameStatus g_gameStatus = START_MENU;
// Window
HWND g_hwnd;
int g_windowWidthInPixel = 0;
int g_windowHeightInPixel = 0;
// FPS
int g_fps = 0;
int g_gemRotateTimeInFrames = 0;
int g_gemCollectTimeInFrames = 0;
int g_attackLengthInFrames = 0;
double g_movement_scale_fps = 0;

// === MOVEMENT VARIABLES ===
// Movement
Point3 g_moveVec;
bool g_isMoving = false;
// Jumping
ZFunction g_normal_jump;
ZFunction g_short_jump;
ZFunction g_high_jump;
ZFunction g_glide;
ZFunction g_stomp;
ZFunction g_upwind;
JumpState g_jumpState = ON_GROUND;
Point3 g_lastReferencePoint;
int g_inAirFrames = 0;
int g_inAttackFrames = 0;
// Upwind
bool g_isInUpwind = false;
bool g_isInUpwindTop = false;
// Camera
XMMATRIX g_World = DirectX::XMMatrixIdentity();
XMMATRIX g_View;
XMFLOAT4 g_Eye;
XMMATRIX g_Projection;
// Shadowmap
XMMATRIX g_View_shadowmap;
XMMATRIX g_Projection_shadowmap;

// === CONTROL VARIABLES ===
// Keyboard, Mouse
bool g_EscapePressed = false;
bool g_SpacePressed = false;
bool g_MouseLeftPressed = false;
bool g_MouseRightPressed = false;
bool g_QPressed = false;
bool g_F1Pressed = false;
bool g_F2Pressed = false;
long g_event_mouseMoveX = 0;
long g_event_mouseMoveY = 0;
vector<Point2int> g_lastMouseMovements;
// Gamepad
bool g_isGamepadConnected = false;
XINPUT_STATE g_gamepadState = {};
vector<Point2double> g_lastLeftStickMovements;
vector<Point2double> g_lastRightStickMovements;

// === PLAYER, CAMERA, MAP VARIABLES ===
Unit g_player;
Camera g_camera;
Map g_map;

// === Window ===

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
		// Close Window
		case WM_CLOSE: {
			DestroyWindow(hwnd);
			break;
		}
		// Destroy Window
		case WM_DESTROY: {
			PostQuitMessage(0);
			break;
		}
		// Lost Focus
		case WM_KILLFOCUS: {
			// Pause Game
			if (g_gameStatus == RUNNING) { updateGameStatus(PAUSED); }
		}
		 // Input
		case WM_INPUT: {
			UINT RawInputBufferSize = mouseRawInputBufferSize;
			BYTE RawInputBuffer[mouseRawInputBufferSize];
			GetRawInputData((HRAWINPUT)lParam, RID_INPUT, (LPVOID)RawInputBuffer, &RawInputBufferSize, sizeof(RAWINPUTHEADER));
			RAWINPUT* raw = (RAWINPUT*)RawInputBuffer;
			if (raw->header.dwType == RIM_TYPEMOUSE) {
				// Save Movement
				g_event_mouseMoveX += raw->data.mouse.lLastX;
				g_event_mouseMoveY += raw->data.mouse.lLastY;
			}
			break;
		}
		// Default
		default: {
			return DefWindowProc(hwnd, msg, wParam, lParam);
		}
	}
	return 0;
}

bool initWindow(HINSTANCE hInstance, int nCmdShow) {
	// Get FPS
	DEVMODE DevMode = { 0 };
	EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &DevMode);
	g_fps = (int)(DevMode.dmDisplayFrequency);
	if (g_fps == 0) { return false; }
	// Movement Scale
	g_movement_scale_fps = movement_scale_fps_nominator / ((double)g_fps);
	// Window Size
	g_windowWidthInPixel = DevMode.dmPelsWidth;
	g_windowHeightInPixel = DevMode.dmPelsHeight;
	if (g_windowWidthInPixel == 0) { return false; }
	if (g_windowHeightInPixel == 0) { return false; }
	g_Projection = XMMatrixPerspectiveFovRH(fov, ((float)g_windowWidthInPixel / (float)g_windowHeightInPixel), nearZ, farZ);
	// Init Start Menu Render Texts
	g_startMenuScreenText_array[0].init((double)g_windowWidthInPixel, (double)g_windowHeightInPixel, 370, 250, wstring(L"Start"), whiteColor);
	g_startMenuScreenText_array[1].init((double)g_windowWidthInPixel, (double)g_windowHeightInPixel, 370, 250 + 2 * atlasHeight, wstring(L"Manual"), whiteColor);
	g_startMenuScreenText_array[2].init((double)g_windowWidthInPixel, (double)g_windowHeightInPixel, 370, 250 + 3 * atlasHeight, wstring(L"Credits"), whiteColor);
	g_startMenuScreenText_array[3].init((double)g_windowWidthInPixel, (double)g_windowHeightInPixel, 370, 250 + 4 * atlasHeight, wstring(L"Exit"), whiteColor);
	g_startMenuScreenText_array[4].init((double)g_windowWidthInPixel, (double)g_windowHeightInPixel, 250, 250, wstring(L"[Space]"), whiteColor);
	g_startMenuScreenText_array[5].init((double)g_windowWidthInPixel, (double)g_windowHeightInPixel, 250, 250 + 2 * atlasHeight, wstring(L"[F1]"), whiteColor);
	g_startMenuScreenText_array[6].init((double)g_windowWidthInPixel, (double)g_windowHeightInPixel, 250, 250 + 3 * atlasHeight, wstring(L"[F2]"), whiteColor);
	g_startMenuScreenText_array[7].init((double)g_windowWidthInPixel, (double)g_windowHeightInPixel, 250, 250 + 4 * atlasHeight, wstring(L"[Esc]"), whiteColor);
	g_startMenuScreenText_array[8].init((double)g_windowWidthInPixel, (double)g_windowHeightInPixel, 250, 250 + gamepadCharOffsetY, wstring(1 , gamepad_A_char), whiteColor);
	g_startMenuScreenText_array[9].init((double)g_windowWidthInPixel, (double)g_windowHeightInPixel, 250, 250 + 2 * atlasHeight + gamepadCharOffsetY, wstring(1 , gamepad_Y_char), whiteColor);
	g_startMenuScreenText_array[10].init((double)g_windowWidthInPixel, (double)g_windowHeightInPixel, 250, 250 + 3 * atlasHeight + gamepadCharOffsetY, wstring(1, gamepad_X_char), whiteColor);
	g_startMenuScreenText_array[11].init((double)g_windowWidthInPixel, (double)g_windowHeightInPixel, 250, 250 + 4 * atlasHeight + gamepadCharOffsetY, wstring(1 , gamepad_B_char), whiteColor);
	// Init Manual Render Texts
	g_manualScreenText_array[0].init((double)g_windowWidthInPixel, (double)g_windowHeightInPixel, 250, 250, wstring(L"[Esc]"), whiteColor);
	g_manualScreenText_array[1].init((double)g_windowWidthInPixel, (double)g_windowHeightInPixel, 250, 250 + gamepadCharOffsetY, wstring(1, gamepad_B_char), whiteColor);
	g_manualScreenText_array[2].init((double)g_windowWidthInPixel, (double)g_windowHeightInPixel, 370, 250, wstring(L"Back"), whiteColor);
	g_manualScreenText_array[3].init((double)g_windowWidthInPixel, (double)g_windowHeightInPixel, 250, 250 + 2 * atlasHeight, wstring(L"In-Game Controls:"), greenColor);
	g_manualScreenText_array[4].init((double)g_windowWidthInPixel, (double)g_windowHeightInPixel, 250, 250 + 3 * atlasHeight, wstring(L"Move Camera"), whiteColor);
	g_manualScreenText_array[5].init((double)g_windowWidthInPixel, (double)g_windowHeightInPixel, 250, 250 + 4 * atlasHeight, wstring(L"Move Spark"), whiteColor);
	g_manualScreenText_array[6].init((double)g_windowWidthInPixel, (double)g_windowHeightInPixel, 250, 250 + 5 * atlasHeight, wstring(L"Dash Move"), whiteColor);
	g_manualScreenText_array[7].init((double)g_windowWidthInPixel, (double)g_windowHeightInPixel, 250, 250 + 6 * atlasHeight, wstring(L"Jump"), whiteColor);
	g_manualScreenText_array[8].init((double)g_windowWidthInPixel, (double)g_windowHeightInPixel, 250, 250 + 7 * atlasHeight, wstring(L"High Jump"), whiteColor);
	g_manualScreenText_array[9].init((double)g_windowWidthInPixel, (double)g_windowHeightInPixel, 250, 250 + 8 * atlasHeight, wstring(L"Glide"), whiteColor);
	g_manualScreenText_array[10].init((double)g_windowWidthInPixel, (double)g_windowHeightInPixel, 250, 250 + 9 * atlasHeight, wstring(L"Stomp"), whiteColor);
	g_manualScreenText_array[11].init((double)g_windowWidthInPixel, (double)g_windowHeightInPixel, 250, 250 + 10 * atlasHeight, wstring(L"Load Checkpoint"), whiteColor);
	g_manualScreenText_array[12].init((double)g_windowWidthInPixel, (double)g_windowHeightInPixel, 250, 250 + 11 * atlasHeight, wstring(L"Pause Game"), whiteColor);
	g_manualScreenText_array[13].init((double)g_windowWidthInPixel, (double)g_windowHeightInPixel, 250, 250 + 13 * atlasHeight, wstring(L"Goal:"), greenColor);
	g_manualScreenText_array[14].init((double)g_windowWidthInPixel, (double)g_windowHeightInPixel, 250, 250 + 14 * atlasHeight, wstring(L"Collect all gems and activate all checkpoints."), whiteColor);
	g_manualScreenText_array[15].init((double)g_windowWidthInPixel, (double)g_windowHeightInPixel, 470, 250 + 6 * atlasHeight, wstring(L"(On Ground)"), lightGrayColor);
	g_manualScreenText_array[16].init((double)g_windowWidthInPixel, (double)g_windowHeightInPixel, 470, 250 + 7 * atlasHeight, wstring(L"(On Ground)"), lightGrayColor);
	g_manualScreenText_array[17].init((double)g_windowWidthInPixel, (double)g_windowHeightInPixel, 470, 250 + 8 * atlasHeight, wstring(L"(In Air)"), lightGrayColor);
	g_manualScreenText_array[18].init((double)g_windowWidthInPixel, (double)g_windowHeightInPixel, 470, 250 + 9 * atlasHeight, wstring(L"(In Air)"), lightGrayColor);
	g_manualScreenText_array[19].init((double)g_windowWidthInPixel, (double)g_windowHeightInPixel, 650, 250 + 2 * atlasHeight, wstring(1, symbol_Mouse_Keyboard_char), greenColor);
	g_manualScreenText_array[20].init((double)g_windowWidthInPixel, (double)g_windowHeightInPixel, 650, 250 + 3 * atlasHeight, wstring(1, symbol_Mouse_char), whiteColor);
	g_manualScreenText_array[21].init((double)g_windowWidthInPixel, (double)g_windowHeightInPixel, 650, 250 + 4 * atlasHeight, wstring(L"[WASD]"), whiteColor);
	g_manualScreenText_array[22].init((double)g_windowWidthInPixel, (double)g_windowHeightInPixel, 650, 250 + 5 * atlasHeight, wstring(1, symbol_Mouse_Right_char) + wstring(L"+ [WASD]"), whiteColor);
	g_manualScreenText_array[23].init((double)g_windowWidthInPixel, (double)g_windowHeightInPixel, 650, 250 + 6 * atlasHeight, wstring(L"[Space]"), whiteColor);
	g_manualScreenText_array[24].init((double)g_windowWidthInPixel, (double)g_windowHeightInPixel, 650, 250 + 7 * atlasHeight, wstring(1, symbol_Mouse_Left_char) + wstring(L"+ [Space]"), whiteColor);
	g_manualScreenText_array[25].init((double)g_windowWidthInPixel, (double)g_windowHeightInPixel, 650, 250 + 8 * atlasHeight, wstring(L"[Space]"), whiteColor);
	g_manualScreenText_array[26].init((double)g_windowWidthInPixel, (double)g_windowHeightInPixel, 650, 250 + 9 * atlasHeight, wstring(1, symbol_Mouse_Left_char), whiteColor);
	g_manualScreenText_array[27].init((double)g_windowWidthInPixel, (double)g_windowHeightInPixel, 650, 250 + 10 * atlasHeight, wstring(L"[Q]"), whiteColor);
	g_manualScreenText_array[28].init((double)g_windowWidthInPixel, (double)g_windowHeightInPixel, 650, 250 + 11 * atlasHeight, wstring(L"[Esc]"), whiteColor);
	g_manualScreenText_array[29].init((double)g_windowWidthInPixel, (double)g_windowHeightInPixel, 836, 250 + 2 * atlasHeight, wstring(1, symbol_Gamepad_char), greenColor);
	g_manualScreenText_array[30].init((double)g_windowWidthInPixel, (double)g_windowHeightInPixel, 840, 250 + 3 * atlasHeight + gamepadCharOffsetY, wstring(1, gamepad_RStick_char), whiteColor);
	g_manualScreenText_array[31].init((double)g_windowWidthInPixel, (double)g_windowHeightInPixel, 840, 250 + 4 * atlasHeight + gamepadCharOffsetY, wstring(1, gamepad_LStick_char), whiteColor);
	g_manualScreenText_array[32].init((double)g_windowWidthInPixel, (double)g_windowHeightInPixel, 840, 250 + 5 * atlasHeight + gamepadCharOffsetY, wstring(1, gamepad_RB_char) + wstring(L" + ") + wstring(1, gamepad_LStick_char), whiteColor);
	g_manualScreenText_array[33].init((double)g_windowWidthInPixel, (double)g_windowHeightInPixel, 840, 250 + 6 * atlasHeight + gamepadCharOffsetY, wstring(1, gamepad_A_char), whiteColor);
	g_manualScreenText_array[34].init((double)g_windowWidthInPixel, (double)g_windowHeightInPixel, 840, 250 + 7 * atlasHeight + gamepadCharOffsetY, wstring(1, gamepad_LB_char) + wstring(L" + ") + wstring(1, gamepad_A_char), whiteColor);
	g_manualScreenText_array[35].init((double)g_windowWidthInPixel, (double)g_windowHeightInPixel, 840, 250 + 8 * atlasHeight + gamepadCharOffsetY, wstring(1, gamepad_A_char), whiteColor);
	g_manualScreenText_array[36].init((double)g_windowWidthInPixel, (double)g_windowHeightInPixel, 840, 250 + 9 * atlasHeight + gamepadCharOffsetY, wstring(1, gamepad_LB_char), whiteColor);
	g_manualScreenText_array[37].init((double)g_windowWidthInPixel, (double)g_windowHeightInPixel, 840, 250 + 10 * atlasHeight + gamepadCharOffsetY, wstring(1, gamepad_Y_char), whiteColor);
	g_manualScreenText_array[38].init((double)g_windowWidthInPixel, (double)g_windowHeightInPixel, 840, 250 + 11 * atlasHeight + gamepadCharOffsetY, wstring(1, gamepad_Start_char), whiteColor);
	// Init Credits Render Texts
	g_creditsScreenText_array[0].init((double)g_windowWidthInPixel, (double)g_windowHeightInPixel, 370, 250, wstring(L"Back"), whiteColor);
	g_creditsScreenText_array[1].init((double)g_windowWidthInPixel, (double)g_windowHeightInPixel, 250, 250 + 2 * atlasHeight, wstring(L"Made by:"), greenColor);
	g_creditsScreenText_array[2].init((double)g_windowWidthInPixel, (double)g_windowHeightInPixel, 250, 250 + 3 * atlasHeight, wstring(L"Jens Krenzin (jens_krenzin@t-online.de)"), whiteColor);
	g_creditsScreenText_array[3].init((double)g_windowWidthInPixel, (double)g_windowHeightInPixel, 250, 250 + 5 * atlasHeight, wstring(L"Third-Party Libraries:"), greenColor);
	g_creditsScreenText_array[4].init((double)g_windowWidthInPixel, (double)g_windowHeightInPixel, 250, 250 + 6 * atlasHeight, wstring(L"Direct3D 11, XInput, libpng, zlib"), whiteColor);
	g_creditsScreenText_array[5].init((double)g_windowWidthInPixel, (double)g_windowHeightInPixel, 250, 250, wstring(L"[Esc]"), whiteColor);
	g_creditsScreenText_array[6].init((double)g_windowWidthInPixel, (double)g_windowHeightInPixel, 250, 250 + gamepadCharOffsetY, wstring(1, gamepad_B_char), whiteColor);
	// Init Paused Render Texts
	g_pausedScreenText_array[0].init((double)g_windowWidthInPixel, (double)g_windowHeightInPixel, 370, 250, wstring(L"Resume"), whiteColor);
	g_pausedScreenText_array[1].init((double)g_windowWidthInPixel, (double)g_windowHeightInPixel, 370, 250 + 1 * atlasHeight, wstring(L"Manual"), whiteColor);
	g_pausedScreenText_array[2].init((double)g_windowWidthInPixel, (double)g_windowHeightInPixel, 370, 250 + 2 * atlasHeight, wstring(L"Start Menu"), whiteColor);
	g_pausedScreenText_array[3].init((double)g_windowWidthInPixel, (double)g_windowHeightInPixel, 250, 250 , wstring(L"[Space]"), whiteColor);
	g_pausedScreenText_array[4].init((double)g_windowWidthInPixel, (double)g_windowHeightInPixel, 250, 250 + 1 * atlasHeight, wstring(L"[F1]"), whiteColor);
	g_pausedScreenText_array[5].init((double)g_windowWidthInPixel, (double)g_windowHeightInPixel, 250, 250 + 2 * atlasHeight, wstring(L"[Esc]"), whiteColor);
	g_pausedScreenText_array[6].init((double)g_windowWidthInPixel, (double)g_windowHeightInPixel, 250, 250 + gamepadCharOffsetY, wstring(1, gamepad_A_char), whiteColor);
	g_pausedScreenText_array[7].init((double)g_windowWidthInPixel, (double)g_windowHeightInPixel, 250, 250 + 1 * atlasHeight + gamepadCharOffsetY, wstring(1, gamepad_Y_char), whiteColor);
	g_pausedScreenText_array[8].init((double)g_windowWidthInPixel, (double)g_windowHeightInPixel, 250, 250 + 2 * atlasHeight + gamepadCharOffsetY, wstring(1, gamepad_B_char), whiteColor);
	// Init Gems Render Text
	g_gemsRenderText_array[0].init((double)g_windowWidthInPixel, (double)g_windowHeightInPixel, 30, 30, wstring(1, gameicon_Gem_char), greenColor);
	g_gemsRenderText_array[1].init((double)g_windowWidthInPixel, (double)g_windowHeightInPixel, 70, 34, 7);
	// Init Checkpoints Render Text
	g_checkpointsRenderText_array[0].init((double)g_windowWidthInPixel, (double)g_windowHeightInPixel, 150, 30, wstring(1, gameicon_Checkpoint_char), cyanColor);
	g_checkpointsRenderText_array[1].init((double)g_windowWidthInPixel, (double)g_windowHeightInPixel, 190, 34, 3);
	// Init Textboxes
	g_textbox_array[0].init((double)g_windowWidthInPixel, (double)g_windowHeightInPixel, 30,  30,  215, atlasHeight - 3);
	g_textbox_array[1].init((double)g_windowWidthInPixel, (double)g_windowHeightInPixel, 250, 249, 273, 3 * atlasHeight - 2);
	g_textbox_array[2].init((double)g_windowWidthInPixel, (double)g_windowHeightInPixel, 250, 249, 680, 15 * atlasHeight - 2);
	// Gems
	g_gemRotateTimeInFrames = (int)(gemRotateTimeRelative * g_fps);
	g_gemCollectTimeInFrames = (int)(gemCollectTimeRelative * g_fps);
	// Attack
	g_attackLengthInFrames = (int)(attack_ratio * g_fps);
	// Jumping States
	double t_h = t_h_ratio * g_fps;
	double t_h_high_jump = t_h_high_jump_ratio * g_fps;
	double t_h_stomp = t_h_stomp_ratio * g_fps;
	double t_q = t_q_ratio * g_fps;
	double t_q_stomp = t_q_stomp_ratio  * g_fps;
	g_normal_jump.a = z_max_normal_jump / t_h;
	g_normal_jump.b = -g_normal_jump.a / (2 * t_q);
	g_normal_jump.d = 2 * g_normal_jump.b * t_q;
	g_normal_jump.e = z_max_normal_jump - g_normal_jump.d * t_h;
	g_normal_jump.c = g_normal_jump.a * (t_h - t_q) - g_normal_jump.b * t_q * t_q;
	g_normal_jump.t_h = t_h;
	g_normal_jump.t_q = t_q;
	g_high_jump.a = z_max_high_jump / t_h_high_jump;
	g_high_jump.b = -g_high_jump.a / (2 * t_q);
	g_high_jump.d = 2 * g_high_jump.b * t_q;
	g_high_jump.e = z_max_high_jump - g_high_jump.d * t_h_high_jump;
	g_high_jump.c = g_high_jump.a * (t_h_high_jump - t_q) - g_high_jump.b * t_q * t_q;
	g_high_jump.t_h = t_h_high_jump;
	g_high_jump.t_q = t_q;
	g_glide.a = z_max_glide / t_h;
	g_glide.b = -g_glide.a / (2 * t_q);
	g_glide.d = 2 * g_glide.b * t_q;
	g_glide.e = z_max_glide - g_glide.d * t_h;
	g_glide.c = g_glide.a * (t_h - t_q) - g_glide.b * t_q * t_q;
	g_glide.t_h = t_h;
	g_glide.t_q = t_q;
	g_stomp.a = z_max_stomp / t_h_stomp;
	g_stomp.b = -g_stomp.a / (2 * t_q_stomp);
	g_stomp.d = 2 * g_stomp.b * t_q_stomp;
	g_stomp.e = z_max_normal_jump - g_stomp.d * t_h_stomp;
	g_stomp.c = g_stomp.a * (t_h_stomp - t_q_stomp) - g_stomp.b * t_q_stomp * t_q_stomp;
	g_stomp.t_h = t_h_stomp;
	g_stomp.t_q = t_q_stomp;
	g_upwind.a = 2 * z_max_normal_jump / t_h;
	// Register Window Class
	WNDCLASSEX wc;
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = 0;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(ID_ICON));
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = windowClassName.c_str();
	wc.hIconSm = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(ID_ICON));
	if (!RegisterClassEx(&wc)) { return false; }
	// Set Client Size
	RECT wr = { 0, 0, g_windowWidthInPixel, g_windowHeightInPixel };
	// Adjust Window Size
	int windowFlags = (WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX);
	AdjustWindowRectEx(&wr, windowFlags, FALSE, WS_EX_CLIENTEDGE);
	// Create Window
	g_hwnd = CreateWindowEx(WS_EX_CLIENTEDGE, windowClassName.c_str(), windowName.c_str(), windowFlags, 0, 0, g_windowWidthInPixel, g_windowHeightInPixel, NULL, NULL, hInstance, NULL);
	if (g_hwnd == NULL) { return false; }
	// Register Raw Input Device - Mouse
	RAWINPUTDEVICE Rid;
	Rid.usUsagePage = 1;
	Rid.usUsage = 2;
	Rid.dwFlags = 0;
	Rid.hwndTarget = 0;
	if (RegisterRawInputDevices(&Rid, 1, sizeof(RAWINPUTDEVICE)) == FALSE) { return false; }
	// Show Window
	ShowWindow(g_hwnd, nCmdShow);
	// Update Window
	UpdateWindow(g_hwnd);
	return true;
}

// === Resources ===

void LoadFileInResource(int name, int type, DWORD& size, void*& data) {
	HMODULE handle = GetModuleHandle(NULL);
	HRSRC rc = FindResource(handle, MAKEINTRESOURCE(name), MAKEINTRESOURCE(type));
	HGLOBAL rcData = LoadResource(handle, rc);
	size = SizeofResource(handle, rc);
	data = LockResource(rcData);
}

// === Vector Math ===

double getPointToPointDist(double x1, double y1, double z1, double x2, double y2, double z2) {
	double dx = x2 - x1;
	double dy = y2 - y1;
	double dz = z2 - z1;
	return std::sqrt(dx * dx + dy * dy + dz * dz);
}

double getDotProduct(Point3& a, Point3& b) {
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

double getVectorAngle(Point3& a, Point3& b) {
	return std::acos(getDotProduct(a, b) / (sqrt(a.x * a.x + a.y * a.y + a.z * a.z) * sqrt(b.x * b.x + b.y * b.y + b.z * b.z)));
}

void getCrossProduct(Point3& a, Point3& b, Point3& c) {
	c.x = (a.y * b.z - a.z * b.y);
	c.y = (a.z * b.x - a.x * b.z);
	c.z = (a.x * b.y - a.y * b.x);
}

void setVertexVectors(unsigned int numVertices, unsigned int numIndices, VERTEX* vertexValues, unsigned int* indexValues, bool useWeight, bool calculateVertexNormals) {
	// Initialize Vectors
	if (calculateVertexNormals) {
		for (unsigned int i = 0; i < numVertices; i++) {
			vertexValues[i].NX = 0.0f;
			vertexValues[i].NY = 0.0f;
			vertexValues[i].NZ = 0.0f;
			vertexValues[i].TX = 0.0f;
			vertexValues[i].TY = 0.0f;
			vertexValues[i].TZ = 0.0f;
			vertexValues[i].BX = 0.0f;
			vertexValues[i].BY = 0.0f;
			vertexValues[i].BZ = 0.0f;
		}
	}
	// Iterate over triangles
	unsigned int numTriangles = numIndices / 3;
	for (unsigned int triangleID = 0; triangleID < numTriangles; triangleID++) {
		// Get Triangle Indices
		unsigned int index0 = indexValues[triangleID * 3 + 0];
		unsigned int index1 = indexValues[triangleID * 3 + 1];
		unsigned int index2 = indexValues[triangleID * 3 + 2];
		// Get Triangle vertices
		VERTEX& v0 = vertexValues[index0];
		VERTEX& v1 = vertexValues[index1];
		VERTEX& v2 = vertexValues[index2];
		// Get triangle vectors: v0 center
		Point3 edge01(v1.X - v0.X, v1.Y - v0.Y, v1.Z - v0.Z);
		Point3 edge02(v2.X - v0.X, v2.Y - v0.Y, v2.Z - v0.Z);
		// Get triangle vectors: v1 center
		Point3 edge12(v2.X - v1.X, v2.Y - v1.Y, v2.Z - v1.Z);
		Point3 edge10(v0.X - v1.X, v0.Y - v1.Y, v0.Z - v1.Z);
		// Get triangle vectors: v2 center
		Point3 edge20(v0.X - v2.X, v0.Y - v2.Y, v0.Z - v2.Z);
		Point3 edge21(v1.X - v2.X, v1.Y - v2.Y, v1.Z - v2.Z);
		// Get UV differences
		double dU1 = v1.U - v0.U;
		double dV1 = v1.V - v0.V;
		double dU2 = v2.U - v0.U;
		double dV2 = v2.V - v0.V;
		double tangentNorm = (1.0 / (dU1 * dV2 - dV1 * dU2));
		// --- (1) Normal Vector ---
		if (calculateVertexNormals) {
			// Get cross product: V0 center
			Point3 f;
			getCrossProduct(edge02, edge01, f);
			// Get area
			double area = sqrt(f.x * f.x + f.y * f.y + f.z * f.z);
			// Get normal vector
			Point3 fn(f.x / area, f.y / area, f.z / area);
			// Get angles for v0,v1,v2 as center
			double angle_v0 = getVectorAngle(edge01, edge02);
			double angle_v1 = getVectorAngle(edge12, edge10);
			double angle_v2 = getVectorAngle(edge20, edge21);
			// Add vertex normal
			if (useWeight) {
				v0.NX += (float)(fn.x * area * angle_v0);
				v0.NY += (float)(fn.y * area * angle_v0);
				v0.NZ += (float)(fn.z * area * angle_v0);
				v1.NX += (float)(fn.x * area * angle_v1);
				v1.NY += (float)(fn.y * area * angle_v1);
				v1.NZ += (float)(fn.z * area * angle_v1);
				v2.NX += (float)(fn.x * area * angle_v2);
				v2.NY += (float)(fn.y * area * angle_v2);
				v2.NZ += (float)(fn.z * area * angle_v2);
			}
			else {
				v0.NX += (float)(fn.x);
				v0.NY += (float)(fn.y);
				v0.NZ += (float)(fn.z);
				v1.NX += (float)(fn.x);
				v1.NY += (float)(fn.y);
				v1.NZ += (float)(fn.z);
				v2.NX += (float)(fn.x);
				v2.NY += (float)(fn.y);
				v2.NZ += (float)(fn.z);
			}
		}
		// --- (2) Tangent Vector ---
		Point3 t( (edge01.x * dV2 - edge02.x * dV1) * tangentNorm, (edge01.y * dV2 - edge02.y * dV1) * tangentNorm, (edge01.z * dV2 - edge02.z * dV1) * tangentNorm );
		// Add vertex tangent
		v0.TX += (float)(t.x);
		v0.TY += (float)(t.y);
		v0.TZ += (float)(t.z);
		v1.TX += (float)(t.x);
		v1.TY += (float)(t.y);
		v1.TZ += (float)(t.z);
		v2.TX += (float)(t.x);
		v2.TY += (float)(t.y);
		v2.TZ += (float)(t.z);
		// --- (3) Bitangent Vector ---
		Point3 b( (edge02.x * dU1 - edge01.x * dU2) * tangentNorm, (edge02.y * dU1 - edge01.y * dU2) * tangentNorm, (edge02.z * dU1 - edge01.z * dU2) * tangentNorm );
		// Add vertex bitangent
		v0.BX += (float)(b.x);
		v0.BY += (float)(b.y);
		v0.BZ += (float)(b.z);
		v1.BX += (float)(b.x);
		v1.BY += (float)(b.y);
		v1.BZ += (float)(b.z);
		v2.BX += (float)(b.x);
		v2.BY += (float)(b.y);
		v2.BZ += (float)(b.z);
	}
	// Normalize all vectors
	double dx, dy, dz, n, dxn, dyn, dzn;
	for (unsigned int i = 0; i < numVertices; i++) {
		// --- (1) Normal Vector ---
		if (calculateVertexNormals) {
			dx = (double)(vertexValues[i].NX);
			dy = (double)(vertexValues[i].NY);
			dz = (double)(vertexValues[i].NZ);
			n = sqrt(dx * dx + dy * dy + dz * dz);
			dxn = dx / n;
			dyn = dy / n;
			dzn = dz / n;
			vertexValues[i].NX = (float)dxn;
			vertexValues[i].NY = (float)dyn;
			vertexValues[i].NZ = (float)dzn;
		}
		// --- (2) Tangent Vector ---
		dx = (double)(vertexValues[i].TX);
		dy = (double)(vertexValues[i].TY);
		dz = (double)(vertexValues[i].TZ);
		n = sqrt(dx * dx + dy * dy + dz * dz);
		dxn = dx / n;
		dyn = dy / n;
		dzn = dz / n;
		vertexValues[i].TX = (float)dxn;
		vertexValues[i].TY = (float)dyn;
		vertexValues[i].TZ = (float)dzn;
		// --- (3) Bitangent Vector ---
		dx = (double)(vertexValues[i].BX);
		dy = (double)(vertexValues[i].BY);
		dz = (double)(vertexValues[i].BZ);
		n = sqrt(dx * dx + dy * dy + dz * dz);
		dxn = dx / n;
		dyn = dy / n;
		dzn = dz / n;
		vertexValues[i].BX = (float)dxn;
		vertexValues[i].BY = (float)dyn;
		vertexValues[i].BZ = (float)dzn;
	}
}

bool getRayTriangleIntersection(Point3 orig, Point3 dir, Point3 vert0, Point3 vert1, Point3 vert2, double& t_res) {
	Point3 edge1(vert1.x - vert0.x, vert1.y - vert0.y, vert1.z - vert0.z);
	Point3 edge2(vert2.x - vert0.x, vert2.y - vert0.y, vert2.z - vert0.z);
	Point3 pvec;
	getCrossProduct(dir, edge2, pvec);
	double det = getDotProduct(edge1, pvec);
	if (det < epsilon_RTI) { return false; }
	Point3 tvec(orig.x - vert0.x, orig.y - vert0.y, orig.z - vert0.z);
	double u = getDotProduct(tvec, pvec);
	if (u < 0.0 || u > det) { return false; }
	Point3 qvec;
	getCrossProduct(tvec, edge1, qvec);
	double v = getDotProduct(dir, qvec);
	if (v < 0.0 || (u + v) > det) { return false; }
	double t = getDotProduct(edge2, qvec);
	double inv_det = 1.0 / det;
	t_res = t * inv_det;
	return true;
}

// === PNG ===

void readMemoryPNG(png_structp png_ptr, png_bytep data, png_uint_32 length) {
	MemoryReaderState* f = (MemoryReaderState*)png_get_io_ptr(png_ptr);
	memcpy(data, f->buffer + f->current_pos, length);
	f->current_pos += length;
}

bool getPNGTextureData(void* buffer, DWORD size, unsigned int& width, unsigned int& height, unsigned int& rowbytes, png_bytepp& rows) {
	png_structp	png_ptr;
	png_infop info_ptr;
	int bit_depth;
	int color_type;
	int interlace_method;
	int compression_method;
	int filter_method;
	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!png_ptr) { return false; }
	info_ptr = png_create_info_struct(png_ptr);
	if (!png_ptr) { return false; }
	MemoryReaderState memory_reader_state;
	memory_reader_state.buffer = (png_bytep)buffer;
	memory_reader_state.bufsize = size;
	memory_reader_state.current_pos = PNGsigBytes;
	png_set_read_fn(png_ptr, &memory_reader_state, (png_rw_ptr)readMemoryPNG);
	png_set_sig_bytes(png_ptr, PNGsigBytes);
	png_read_png(png_ptr, info_ptr, 0, 0);
	png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, &interlace_method, &compression_method, &filter_method);
	rows = png_get_rows(png_ptr, info_ptr);
	rowbytes = (unsigned int)png_get_rowbytes(png_ptr, info_ptr);
	return true;
}

bool getResourceViewFromPNG(void* buffer, DWORD size, ID3D11ShaderResourceView** textureSRV) {
	ID3D11Texture2D* texture = NULL;
	D3D11_TEXTURE2D_DESC textureDesc;
	png_byte* textureData = NULL;
	unsigned int width = 0;
	unsigned int height = 0;
	unsigned int rowbytes = 0;
	png_bytepp rows = 0;
	if (!getPNGTextureData(buffer, size, width, height, rowbytes, rows)) { return false; }
	textureData = new png_byte[rowbytes * height];
	unsigned int i = 0;
	for (unsigned int y = 0; y < height; y++) {
		png_bytep row = rows[y];
		for (unsigned int x = 0; x < width; x++) {
			png_bytep px = &(row[x * 4]);
			textureData[i * 4 + 0] = px[0];
			textureData[i * 4 + 1] = px[1];
			textureData[i * 4 + 2] = px[2];
			textureData[i * 4 + 3] = px[3];
			i++;
		}
	}
	textureDesc.Width = width;
	textureDesc.Height = height;
	textureDesc.MipLevels = 0;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
	D3D11_SUBRESOURCE_DATA initialTextureData;
	initialTextureData.pSysMem = textureData;
	initialTextureData.SysMemPitch = rowbytes;
	initialTextureData.SysMemSlicePitch = 0;
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = -1;
	if (FAILED(g_dev->CreateTexture2D(&textureDesc, NULL, &texture))) { return false; }
	g_devcon->UpdateSubresource(texture, 0, NULL, textureData, rowbytes, 0);
	if (FAILED(g_dev->CreateShaderResourceView(texture, &srvDesc, textureSRV))) { return false; }
	g_devcon->GenerateMips(*textureSRV);
	if (texture != NULL) { texture->Release(); texture = NULL; }
	delete[] textureData;
	return true;
}

// === BINMESH ===

void readBINMESH(unsigned int ResourceID, uint32_t& numIndices, uint32_t& numVertices, uint32_t*& indices, VERTEX*& vertices) {
	// Load Resource
	DWORD size = 0;
	void* data = NULL;
	LoadFileInResource(ResourceID, BINMESH, size, data);
	// Get Pointer for Sizes and Indices
	uint32_t* d0_uint = (uint32_t*)data;
	BYTE* d0_byte = (BYTE*)data;
	// Get Sizes
	numIndices = d0_uint[0];
	numVertices = d0_uint[1];
	// Get Indices
	indices = new uint32_t[numIndices];
	for (unsigned int i = 0; i < numIndices; i++) { indices[i] = d0_uint[i + 2]; }
	// Get Pointer for Vertices
	VERTEX* d1_vertex = (VERTEX*)((BYTE*)(d0_byte + (2 + numIndices) * sizeof(uint32_t)));
	// Get Vertices
	vertices = new VERTEX[numVertices];
	for (unsigned int i = 0; i < numVertices; i++) { vertices[i] = d1_vertex[i]; }
}

// === Distance Computation ===

void GetMinEdge02(double const& a11, double const& b1, double& pr_0, double& pr_1) {
	pr_0 = 0.0;
	if (b1 >= 0.0) { pr_1 = 0.0; }
	else if (a11 + b1 <= 0.0) { pr_1 = 0.0; }
	else { pr_1 = -b1 / a11; }
}
void GetMinEdge12(double const& a01, double const& a11, double const& b1, double const& f10, double const& f01, double& pr_0, double& pr_1) {
	double h0 = a01 + b1 - f10;
	if (h0 >= 0.0) { pr_1 = 0.0; }
	else {
		double h1 = a11 + b1 - f01;
		if (h1 <= 0.0) { pr_1 = 1.0; }
		else { pr_1 = h0 / (h0 - h1); }
	}
	pr_0 = 1.0 - pr_1;
}
void GetMinInterior(double const& p0_0, double const& p0_1, double const& h0, double const& p1_0, double const& p1_1, double const& h1, double& pr_0, double& pr_1) {
	double z = h0 / (h0 - h1);
	pr_0 = (1.0 - z) * p0_0 + z * p1_0;
	pr_1 = (1.0 - z) * p0_1 + z * p1_1;
}
void getDistancePointToTriangle(double const& point_x, double const& point_y, double const& point_z, double const& tr_0_x, double const& tr_0_y, double const& tr_0_z, double const& tr_1_x, double const& tr_1_y, double const& tr_1_z, double const& tr_2_x, double const& tr_2_y, double const& tr_2_z, double& closest_x, double& closest_y, double& closest_z, double& sqrDistance) {
	double diff_x = point_x - tr_0_x;
	double diff_y = point_y - tr_0_y;
	double diff_z = point_z - tr_0_z;
	double edge0_x = tr_1_x - tr_0_x;
	double edge0_y = tr_1_y - tr_0_y;
	double edge0_z = tr_1_z - tr_0_z;
	double edge1_x = tr_2_x - tr_0_x;
	double edge1_y = tr_2_y - tr_0_y;
	double edge1_z = tr_2_z - tr_0_z;
	double a00 = edge0_x * edge0_x + edge0_y * edge0_y + edge0_z * edge0_z;
	double a01 = edge0_x * edge1_x + edge0_y * edge1_y + edge0_z * edge1_z;
	double a11 = edge1_x * edge1_x + edge1_y * edge1_y + edge1_z * edge1_z;
	double b0 = -diff_x * edge0_x - diff_y * edge0_y - diff_z * edge0_z;
	double b1 = -diff_x * edge1_x - diff_y * edge1_y - diff_z * edge1_z;
	double f00 = b0;
	double f10 = b0 + a00;
	double f01 = b0 + a01;
	double p0_0, p0_1, p1_0, p1_1, p_0, p_1;
	double dt1, h0, h1;
	if (f00 >= 0.0) {
		if (f01 >= 0.0) { GetMinEdge02(a11, b1, p_0, p_1); }
		else {
			p0_0 = 0.0;
			p0_1 = f00 / (f00 - f01);
			p1_0 = f01 / (f01 - f10);
			p1_1 = 1.0 - p1_0;
			dt1 = p1_1 - p0_1;
			h0 = dt1 * (a11 * p0_1 + b1);
			if (h0 >= 0.0) { GetMinEdge02(a11, b1, p_0, p_1); }
			else {
				h1 = dt1 * (a01 * p1_0 + a11 * p1_1 + b1);
				if (h1 <= 0.0) { GetMinEdge12(a01, a11, b1, f10, f01, p_0, p_1); }
				else { GetMinInterior(p0_0, p0_1, h0, p1_0, p1_1, h1, p_0, p_1); }
			}
		}
	}
	else if (f01 <= 0.0) {
		if (f10 <= 0.0) { GetMinEdge12(a01, a11, b1, f10, f01, p_0, p_1); }
		else {
			p0_0 = f00 / (f00 - f10);
			p0_1 = 0.0;
			p1_0 = f01 / (f01 - f10);
			p1_1 = 1.0 - p1_0;
			h0 = p1_1 * (a01 * p0_0 + b1);
			if (h0 >= 0.0) { p_0 = p0_0; p_1 = p0_1; }
			else {
				h1 = p1_1 * (a01 * p1_0 + a11 * p1_1 + b1);
				if (h1 <= 0.0) { GetMinEdge12(a01, a11, b1, f10, f01, p_0, p_1); }
				else { GetMinInterior(p0_0, p0_1, h0, p1_0, p1_1, h1, p_0, p_1); }
			}
		}
	}
	else if (f10 <= 0.0) {
		p0_0 = 0.0;
		p0_1 = f00 / (f00 - f01);
		p1_0 = f01 / (f01 - f10);
		p1_1 = 1.0 - p1_0;
		dt1 = p1_1 - p0_1;
		h0 = dt1 * (a11 * p0_1 + b1);
		if (h0 >= 0.0) { GetMinEdge02(a11, b1, p_0, p_1); }
		else {
			h1 = dt1 * (a01 * p1_0 + a11 * p1_1 + b1);
			if (h1 <= 0.0) { GetMinEdge12(a01, a11, b1, f10, f01, p_0, p_1); }
			else { GetMinInterior(p0_0, p0_1, h0, p1_0, p1_1, h1, p_0, p_1); }
		}
	}
	else {
		p0_0 = f00 / (f00 - f10);
		p0_1 = 0.0;
		p1_0 = 0.0;
		p1_1 = f00 / (f00 - f01);
		h0 = p1_1 * (a01 * p0_0 + b1);
		if (h0 >= 0.0) { p_0 = p0_0; p_1 = p0_1; }
		else {
			h1 = p1_1 * (a11 * p1_1 + b1);
			if (h1 <= 0.0) { GetMinEdge02(a11, b1, p_0, p_1); }
			else { GetMinInterior(p0_0, p0_1, h0, p1_0, p1_1, h1, p_0, p_1); }
		}
	}
	closest_x = tr_0_x + p_0 * edge0_x + p_1 * edge1_x;
	closest_y = tr_0_y + p_0 * edge0_y + p_1 * edge1_y;
	closest_z = tr_0_z + p_0 * edge0_z + p_1 * edge1_z;
	diff_x = point_x - closest_x;
	diff_y = point_y - closest_y;
	diff_z = point_z - closest_z;
	sqrDistance = diff_x * diff_x + diff_y * diff_y + diff_z * diff_z;
}

// === Mesh Generation ===

bool generatePlayerMesh() {
	uint32_t numIndices = 0;
	uint32_t numVertices = 0;
	uint32_t* indices = NULL;
	VERTEX* vertices = NULL;
	readBINMESH(ID_PLAYER_MESH, numIndices, numVertices, indices, vertices);
	// Set Vertex Vectors
	setVertexVectors(numVertices, numIndices, vertices, indices, true, false);
	// Set Content
	g_playerMesh.setContentFromHeapMemory(numVertices, numIndices, vertices, indices, false);
	if (!g_playerMesh.initBuffers(g_dev, g_devcon)) { return false; }
	return true;
}

bool generateCheckpointMesh() {
	uint32_t numIndices = 0;
	uint32_t numVertices = 0;
	uint32_t* indices = NULL;
	VERTEX* vertices = NULL;
	readBINMESH(ID_CHECKPOINT_MESH, numIndices, numVertices, indices, vertices);
	// Set Vertex Vectors
	setVertexVectors(numVertices, numIndices, vertices, indices, false, true);
	// Set Content
	g_checkpointMesh.setContentFromHeapMemory(numVertices, numIndices, vertices, indices, false);
	if (!g_checkpointMesh.initBuffers(g_dev, g_devcon)) { return false; }
	return true;
}

bool generateUpwindMesh() {
	uint32_t numIndices = 0;
	uint32_t numVertices = 0;
	uint32_t* indices = NULL;
	VERTEX* vertices = NULL;
	readBINMESH(ID_UPWIND_MESH, numIndices, numVertices, indices, vertices);
	// Set Vertex Vectors
	setVertexVectors(numVertices, numIndices, vertices, indices, false, true);
	// Set Content
	g_upwindMesh.setContentFromHeapMemory(numVertices, numIndices, vertices, indices, false);
	if (!g_upwindMesh.initBuffers(g_dev, g_devcon)) { return false; }
	return true;
}

bool generateSkyMesh() {
	uint32_t numIndices = 0;
	uint32_t numVertices = 0;
	uint32_t* indices = NULL;
	VERTEX* vertices = NULL;
	readBINMESH(ID_SKY_MESH, numIndices, numVertices, indices, vertices);
	// Set Vertex Vectors
	setVertexVectors(numVertices, numIndices, vertices, indices, true, false);
	// Set Content
	g_skyMesh.setContentFromHeapMemory(numVertices, numIndices, vertices, indices, false);
	if (!g_skyMesh.initBuffers(g_dev, g_devcon)) { return false; }
	return true;
}

bool generateBoxlayerMesh() {
	const unsigned int numVerticesBoxlayer = 4;
	const unsigned int numIndicesBoxlayer = 6;
	// Vertices
	VERTEX* verticesBoxlayer = new VERTEX[numVerticesBoxlayer];
	verticesBoxlayer[0] = { -1, +1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, emptyColor, 0, 0 };
	verticesBoxlayer[1] = { +1, +1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, emptyColor, 1, 0 };
	verticesBoxlayer[2] = { +1, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, emptyColor, 1, 1 };
	verticesBoxlayer[3] = { -1, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, emptyColor, 0, 1 };
	// Indices
	unsigned int i = 0;
	unsigned int* indicesBoxlayer = new unsigned int[numIndicesBoxlayer];
	indicesBoxlayer[i++] = 0; indicesBoxlayer[i++] = 1; indicesBoxlayer[i++] = 3;
	indicesBoxlayer[i++] = 1; indicesBoxlayer[i++] = 2; indicesBoxlayer[i++] = 3;
	// Set Vertex Vectors
	setVertexVectors(numVerticesBoxlayer, numIndicesBoxlayer, verticesBoxlayer, indicesBoxlayer, true, true);
	// Set Content
	g_boxlayerMesh.setContentFromHeapMemory(numVerticesBoxlayer, numIndicesBoxlayer, verticesBoxlayer, indicesBoxlayer, false);
	if (!g_boxlayerMesh.initBuffers(g_dev, g_devcon)) { return false; }
	return true;
}

bool generateGemMesh() {
	const unsigned int numVerticesGem = 18;
	const unsigned int numIndicesGem = 96;
	const double r1 = 0.5;
	const double r2 = 0.75;
	const double h1 = 1.0;
	const double h2 = 0.75;
	const double n2 = 0.125;
	// Vertices
	VERTEX* verticesGem = new VERTEX[numVerticesGem];
	// Mid Point
	verticesGem[0] = { (float)(0 * n2), (float)(0 * n2), (float)((1 - 0.5) * n2), 0, 0, 0, 0, 0, 0, 0, 0, 0, gemColor, 0, 0 };
	// Inner Circle
	verticesGem[1] = { (float)(0 * r1 * n2), (float)(1 * r1 * n2), (float)((h1 - 0.5) * n2), 0, 0, 0, 0, 0, 0, 0, 0, 0, gemColor, 0, 0 };
	verticesGem[2] = { (float)(invSqrt2 * r1 * n2), (float)(invSqrt2 * r1 * n2), (float)((h1 - 0.5) * n2), 0, 0, 0, 0, 0, 0, 0, 0, 0, gemColor, 0, 0 };
	verticesGem[3] = { (float)(1 * r1 * n2), (float)(0 * r1 * n2), (float)((h1 - 0.5) * n2), 0, 0, 0, 0, 0, 0, 0, 0, 0, gemColor, 0, 0 };
	verticesGem[4] = { (float)(invSqrt2 * r1 * n2), (float)(-invSqrt2 * r1 * n2), (float)((h1 - 0.5) * n2), 0, 0, 0, 0, 0, 0, 0, 0, 0, gemColor, 0, 0 };
	verticesGem[5] = { (float)(0 * r1 * n2), (float)(-1 * r1 * n2), (float)((h1 - 0.5) * n2), 0, 0, 0, 0, 0, 0, 0, 0, 0, gemColor, 0, 0 };
	verticesGem[6] = { (float)(-invSqrt2 * r1 * n2), (float)(-invSqrt2 * r1 * n2), (float)((h1 - 0.5) * n2), 0, 0, 0, 0, 0, 0, 0, 0, 0, gemColor, 0, 0 };
	verticesGem[7] = { (float)(-1 * r1 * n2), (float)(0 * r1 * n2), (float)((h1 - 0.5) * n2), 0, 0, 0, 0, 0, 0, 0, 0, 0, gemColor, 0, 0 };
	verticesGem[8] = { (float)(-invSqrt2 * r1 * n2), (float)(invSqrt2 * r1 * n2), (float)((h1 - 0.5) * n2), 0, 0, 0, 0, 0, 0, 0, 0, 0, gemColor, 0, 0};
	// Outer Circle
	verticesGem[9] = { (float)(0 * r2 * n2), (float)(1 * r2 * n2), (float)((h2 - 0.5) * n2), 0, 0, 0, 0, 0, 0, 0, 0, 0, gemColor, 0, 0 };
	verticesGem[10] = { (float)(invSqrt2 * r2 * n2), (float)(invSqrt2 * r2 * n2), (float)((h2 - 0.5) * n2), 0, 0, 0, 0, 0, 0, 0, 0, 0, gemColor, 0, 0 };
	verticesGem[11] = { (float)(1 * r2 * n2), (float)(0 * r2 * n2), (float)((h2 - 0.5) * n2), 0, 0, 0, 0, 0, 0, 0, 0, 0, gemColor, 0, 0 };
	verticesGem[12] = { (float)(invSqrt2 * r2 * n2), (float)(-invSqrt2 * r2 * n2), (float)((h2 - 0.5) * n2), 0, 0, 0, 0, 0, 0, 0, 0, 0, gemColor, 0, 0 };
	verticesGem[13] = { (float)(0 * r2 * n2), (float)(-1 * r2 * n2), (float)((h2 - 0.5) * n2), 0, 0, 0, 0, 0, 0, 0, 0, 0, gemColor, 0, 0 };
	verticesGem[14] = { (float)(-invSqrt2 * r2 * n2), (float)(-invSqrt2 * r2 * n2), (float)((h2 - 0.5) * n2), 0, 0, 0, 0, 0, 0, 0, 0, 0, gemColor, 0, 0 };
	verticesGem[15] = { (float)(-1 * r2 * n2), (float)(0 * r2 * n2), (float)((h2 - 0.5) * n2), 0, 0, 0, 0, 0, 0, 0, 0, 0, gemColor, 0, 0 };
	verticesGem[16] = { (float)(-invSqrt2 * r2 * n2), (float)(invSqrt2 * r2 * n2), (float)((h2 - 0.5) * n2), 0, 0, 0, 0, 0, 0, 0, 0, 0, gemColor, 0, 0 };
	// Bottom
	verticesGem[17] = { (float)(0 * n2), (float)(0 * n2), (float)((0 - 0.5) * n2), 0, 0, 0, 0, 0, 0, 0, 0, 0, gemColor, 0, 0 };
	// Indices
	unsigned int i = 0;
	unsigned int* indicesGem = new unsigned int[numIndicesGem];
	// Inner Circle
	indicesGem[i++] = 0; indicesGem[i++] = 1; indicesGem[i++] = 2;
	indicesGem[i++] = 0; indicesGem[i++] = 2; indicesGem[i++] = 3;
	indicesGem[i++] = 0; indicesGem[i++] = 3; indicesGem[i++] = 4;
	indicesGem[i++] = 0; indicesGem[i++] = 4; indicesGem[i++] = 5;
	indicesGem[i++] = 0; indicesGem[i++] = 5; indicesGem[i++] = 6;
	indicesGem[i++] = 0; indicesGem[i++] = 6; indicesGem[i++] = 7;
	indicesGem[i++] = 0; indicesGem[i++] = 7; indicesGem[i++] = 8;
	indicesGem[i++] = 0; indicesGem[i++] = 8; indicesGem[i++] = 1;
	// Outer Circle
	indicesGem[i++] = 1; indicesGem[i++] = 9; indicesGem[i++] = 2;
	indicesGem[i++] = 2; indicesGem[i++] = 9; indicesGem[i++] = 10;
	indicesGem[i++] = 2; indicesGem[i++] = 10; indicesGem[i++] = 3;
	indicesGem[i++] = 3; indicesGem[i++] = 10; indicesGem[i++] = 11;
	indicesGem[i++] = 3; indicesGem[i++] = 11; indicesGem[i++] = 4;
	indicesGem[i++] = 4; indicesGem[i++] = 11; indicesGem[i++] = 12;
	indicesGem[i++] = 4; indicesGem[i++] = 12; indicesGem[i++] = 5;
	indicesGem[i++] = 5; indicesGem[i++] = 12; indicesGem[i++] = 13;
	indicesGem[i++] = 5; indicesGem[i++] = 13; indicesGem[i++] = 6;
	indicesGem[i++] = 6; indicesGem[i++] = 13; indicesGem[i++] = 14;
	indicesGem[i++] = 6; indicesGem[i++] = 14; indicesGem[i++] = 7;
	indicesGem[i++] = 7; indicesGem[i++] = 14; indicesGem[i++] = 15;
	indicesGem[i++] = 7; indicesGem[i++] = 15; indicesGem[i++] = 8;
	indicesGem[i++] = 8; indicesGem[i++] = 15; indicesGem[i++] = 16;
	indicesGem[i++] = 8; indicesGem[i++] = 16; indicesGem[i++] = 1;
	indicesGem[i++] = 1; indicesGem[i++] = 16; indicesGem[i++] = 9;
	// Bottom
	indicesGem[i++] = 10; indicesGem[i++] = 9; indicesGem[i++] = 17;
	indicesGem[i++] = 11; indicesGem[i++] = 10; indicesGem[i++] = 17;
	indicesGem[i++] = 12; indicesGem[i++] = 11; indicesGem[i++] = 17;
	indicesGem[i++] = 13; indicesGem[i++] = 12; indicesGem[i++] = 17;
	indicesGem[i++] = 14; indicesGem[i++] = 13; indicesGem[i++] = 17;
	indicesGem[i++] = 15; indicesGem[i++] = 14; indicesGem[i++] = 17;
	indicesGem[i++] = 16; indicesGem[i++] = 15; indicesGem[i++] = 17;
	indicesGem[i++] = 9; indicesGem[i++] = 16; indicesGem[i++] = 17;
	// Set Vertex Vectors
	setVertexVectors(numVerticesGem, numIndicesGem, verticesGem, indicesGem, false, true);
	// Set Content
	g_gemMesh.setContentFromHeapMemory(numVerticesGem, numIndicesGem, verticesGem, indicesGem, false);
	if (!g_gemMesh.initBuffers(g_dev, g_devcon)) { return false; }
	return true;
}

bool generateLampMesh() {
	const unsigned int numVerticesLamp = 24;
	const unsigned int numIndicesLamp = 36;
	const double n2 = 0.125;
	// Vertices
	VERTEX* verticesLamp = new VERTEX[numVerticesLamp];
	// Bottom
	verticesLamp[0] = { (float)(-1 * n2), (float)(1 * n2), (float)(-1 * n2), 0, 0, 0, 0, 0, 0, 0, 0, 0, lampColor, 0, 0 };
	verticesLamp[1] = { (float)(1 * n2), (float)(1 * n2), (float)(-1 * n2), 0, 0, 0, 0, 0, 0, 0, 0, 0, lampColor, 0, 0 };
	verticesLamp[2] = { (float)(1 * n2), (float)(-1 * n2), (float)(-1 * n2), 0, 0, 0, 0, 0, 0, 0, 0, 0, lampColor, 0, 0 };
	verticesLamp[3] = { (float)(-1 * n2), (float)(-1 * n2), (float)(-1 * n2), 0, 0, 0, 0, 0, 0, 0, 0, 0, lampColor, 0, 0 };
	// Top
	verticesLamp[4] = { (float)(-1 * n2), (float)(1 * n2), (float)(1 * n2), 0, 0, 0, 0, 0, 0, 0, 0, 0, lampColor, 0, 0 };
	verticesLamp[5] = { (float)(1 * n2), (float)(1 * n2), (float)(1 * n2), 0, 0, 0, 0, 0, 0, 0, 0, 0, lampColor, 0, 0 };
	verticesLamp[6] = { (float)(1 * n2), (float)(-1 * n2), (float)(1 * n2), 0, 0, 0, 0, 0, 0, 0, 0, 0, lampColor, 0, 0 };
	verticesLamp[7] = { (float)(-1 * n2), (float)(-1 * n2), (float)(1 * n2), 0, 0, 0, 0, 0, 0, 0, 0, 0, lampColor, 0, 0 };
	// +Y
	verticesLamp[8] = { (float)(-1 * n2), (float)(1 * n2), (float)(1 * n2), 0, 0, 0, 0, 0, 0, 0, 0, 0, lampColor, 0, 0 };
	verticesLamp[9] = { (float)(1 * n2), (float)(1 * n2), (float)(1 * n2), 0, 0, 0, 0, 0, 0, 0, 0, 0, lampColor, 0, 0 };
	verticesLamp[10] = { (float)(1 * n2), (float)(1 * n2), (float)(-1 * n2), 0, 0, 0, 0, 0, 0, 0, 0, 0, lampColor, 0, 0 };
	verticesLamp[11] = { (float)(-1 * n2), (float)(1 * n2), (float)(-1 * n2), 0, 0, 0, 0, 0, 0, 0, 0, 0, lampColor, 0, 0};
	// +X
	verticesLamp[12] = { (float)(1 * n2), (float)(1 * n2), (float)(1 * n2), 0, 0, 0, 0, 0, 0, 0, 0, 0, lampColor, 0, 0};
	verticesLamp[13] = { (float)(1 * n2), (float)(-1 * n2), (float)(1 * n2), 0, 0, 0, 0, 0, 0, 0, 0, 0, lampColor, 0, 0};
	verticesLamp[14] = { (float)(1 * n2), (float)(-1 * n2), (float)(-1 * n2), 0, 0, 0, 0, 0, 0, 0, 0, 0, lampColor, 0, 0};
	verticesLamp[15] = { (float)(1 * n2), (float)(1 * n2), (float)(-1 * n2), 0, 0, 0, 0, 0, 0, 0, 0, 0, lampColor, 0, 0};
	// -Y
	verticesLamp[16] = { (float)(-1 * n2), (float)(-1 * n2), (float)(1 * n2), 0, 0, 0, 0, 0, 0, 0, 0, 0, lampColor, 0, 0};
	verticesLamp[17] = { (float)(1 * n2), (float)(-1 * n2), (float)(1 * n2), 0, 0, 0, 0, 0, 0, 0, 0, 0, lampColor, 0, 0};
	verticesLamp[18] = { (float)(1 * n2), (float)(-1 * n2), (float)(-1 * n2), 0, 0, 0, 0, 0, 0, 0, 0, 0, lampColor, 0, 0};
	verticesLamp[19] = { (float)(-1 * n2), (float)(-1 * n2), (float)(-1 * n2), 0, 0, 0, 0, 0, 0, 0, 0, 0, lampColor, 0, 0};
	// -X
	verticesLamp[20] = { (float)(-1 * n2), (float)(1 * n2), (float)(1 * n2), 0, 0, 0, 0, 0, 0, 0, 0, 0, lampColor, 0, 0};
	verticesLamp[21] = { (float)(-1 * n2), (float)(-1 * n2), (float)(1 * n2), 0, 0, 0, 0, 0, 0, 0, 0, 0, lampColor, 0, 0};
	verticesLamp[22] = { (float)(-1 * n2), (float)(-1 * n2), (float)(-1 * n2), 0, 0, 0, 0, 0, 0, 0, 0, 0, lampColor, 0, 0};
	verticesLamp[23] = { (float)(-1 * n2), (float)(1 * n2), (float)(-1 * n2), 0, 0, 0, 0, 0, 0, 0, 0, 0, lampColor, 0, 0};
	// Indices
	unsigned int i = 0;
	unsigned int* indicesLamp = new unsigned int[numIndicesLamp];
	// Bottom
	indicesLamp[i++] = 2; indicesLamp[i++] = 1; indicesLamp[i++] = 0;
	indicesLamp[i++] = 2; indicesLamp[i++] = 0; indicesLamp[i++] = 3;
	// Top
	indicesLamp[i++] = 6; indicesLamp[i++] = 4; indicesLamp[i++] = 5;
	indicesLamp[i++] = 6; indicesLamp[i++] = 7; indicesLamp[i++] = 4;
	// +Y
	indicesLamp[i++] = 10; indicesLamp[i++] = 9; indicesLamp[i++] = 8;
	indicesLamp[i++] = 10; indicesLamp[i++] = 8; indicesLamp[i++] = 11;
	// +X
	indicesLamp[i++] = 14; indicesLamp[i++] = 13; indicesLamp[i++] = 12;
	indicesLamp[i++] = 14; indicesLamp[i++] = 12; indicesLamp[i++] = 15;
	// -Y
	indicesLamp[i++] = 18; indicesLamp[i++] = 16; indicesLamp[i++] = 17;
	indicesLamp[i++] = 18; indicesLamp[i++] = 19; indicesLamp[i++] = 16;
	// -X
	indicesLamp[i++] = 22; indicesLamp[i++] = 20; indicesLamp[i++] = 21;
	indicesLamp[i++] = 22; indicesLamp[i++] = 23; indicesLamp[i++] = 20;
	// Set Vertex Vectors
	setVertexVectors(numVerticesLamp, numIndicesLamp, verticesLamp, indicesLamp, true, true);
	// Set Content
	g_lampMesh.setContentFromHeapMemory(numVerticesLamp, numIndicesLamp, verticesLamp, indicesLamp, false);
	if (!g_lampMesh.initBuffers(g_dev, g_devcon)) { return false; }
	return true;
}

bool generateRectPlatformMesh() {
	// Vertices
	VERTEX* verticesPlatform = new VERTEX[numVerticesPerPlatform];
	// Top
	verticesPlatform[0] = { -platform_dx2, +platform_dy2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, whiteColor, 0, 0 };
	verticesPlatform[1] = { +platform_dx2, +platform_dy2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, whiteColor, 1, 0 };
	verticesPlatform[2] = { +platform_dx2, -platform_dy2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, whiteColor, 1, 1 };
	verticesPlatform[3] = { -platform_dx2, -platform_dy2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, whiteColor, 0, 1 };
	// Y+
	verticesPlatform[4] = { -platform_dx2, +platform_dy2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, whiteColor, 0, 0 };
	verticesPlatform[5] = { +platform_dx2, +platform_dy2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, whiteColor, 0, 0 };
	verticesPlatform[6] = { -platform_dx2, +platform_dy2, +platformZOffset, 0, 0, 0, 0, 0, 0, 0, 0, 0, whiteColor, 0, 0 };
	verticesPlatform[7] = { +platform_dx2, +platform_dy2, +platformZOffset, 0, 0, 0, 0, 0, 0, 0, 0, 0, whiteColor, 0, 0 };
	// X+
	verticesPlatform[8] = { +platform_dx2, +platform_dy2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, whiteColor, 0, 0 };
	verticesPlatform[9] = { +platform_dx2, -platform_dy2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, whiteColor, 0, 0 };
	verticesPlatform[10] = { +platform_dx2, +platform_dy2, +platformZOffset, 0, 0, 0, 0, 0, 0, 0, 0, 0, whiteColor, 0, 0 };
	verticesPlatform[11] = { +platform_dx2, -platform_dy2, +platformZOffset, 0, 0, 0, 0, 0, 0, 0, 0, 0, whiteColor, 0, 0 };
	// Y+
	verticesPlatform[12] = { +platform_dx2, -platform_dy2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, whiteColor, 0, 0 };
	verticesPlatform[13] = { -platform_dx2, -platform_dy2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, whiteColor, 0, 0 };
	verticesPlatform[14] = { +platform_dx2, -platform_dy2, +platformZOffset, 0, 0, 0, 0, 0, 0, 0, 0, 0, whiteColor, 0, 0 };
	verticesPlatform[15] = { -platform_dx2, -platform_dy2, +platformZOffset, 0, 0, 0, 0, 0, 0, 0, 0, 0, whiteColor, 0, 0 };
	// X-
	verticesPlatform[16] = { -platform_dx2, -platform_dy2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, whiteColor, 0, 0 };
	verticesPlatform[17] = { -platform_dx2, +platform_dy2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, whiteColor, 0, 0 };
	verticesPlatform[18] = { -platform_dx2, -platform_dy2, +platformZOffset, 0, 0, 0, 0, 0, 0, 0, 0, 0, whiteColor, 0, 0 };
	verticesPlatform[19] = { -platform_dx2, +platform_dy2, +platformZOffset, 0, 0, 0, 0, 0, 0, 0, 0, 0, whiteColor, 0, 0 };
	// Bottom
	verticesPlatform[20] = { -platform_dx2, +platform_dy2, +platformZOffset, 0, 0, 0, 0, 0, 0, 0, 0, 0, whiteColor, 0, 0 };
	verticesPlatform[21] = { +platform_dx2, +platform_dy2, +platformZOffset, 0, 0, 0, 0, 0, 0, 0, 0, 0, whiteColor, 1, 0 };
	verticesPlatform[22] = { +platform_dx2, -platform_dy2, +platformZOffset, 0, 0, 0, 0, 0, 0, 0, 0, 0, whiteColor, 1, 1 };
	verticesPlatform[23] = { -platform_dx2, -platform_dy2, +platformZOffset, 0, 0, 0, 0, 0, 0, 0, 0, 0, whiteColor, 0, 1 };
	// Indices
	unsigned int i = 0;
	unsigned int* indicesPlatform = new unsigned int[numIndicesPerPlatform];
	indicesPlatform[i++] = 0; indicesPlatform[i++] = 1; indicesPlatform[i++] = 3;
	indicesPlatform[i++] = 1; indicesPlatform[i++] = 2; indicesPlatform[i++] = 3;
	indicesPlatform[i++] = 4; indicesPlatform[i++] = 6; indicesPlatform[i++] = 5;
	indicesPlatform[i++] = 5; indicesPlatform[i++] = 6; indicesPlatform[i++] = 7;
	indicesPlatform[i++] = 8; indicesPlatform[i++] = 10; indicesPlatform[i++] = 9;
	indicesPlatform[i++] = 9; indicesPlatform[i++] = 10; indicesPlatform[i++] = 11;
	indicesPlatform[i++] = 12; indicesPlatform[i++] = 14; indicesPlatform[i++] = 13;
	indicesPlatform[i++] = 13; indicesPlatform[i++] = 14; indicesPlatform[i++] = 15;
	indicesPlatform[i++] = 16; indicesPlatform[i++] = 18; indicesPlatform[i++] = 17;
	indicesPlatform[i++] = 17; indicesPlatform[i++] = 18; indicesPlatform[i++] = 19;
	indicesPlatform[i++] = 20; indicesPlatform[i++] = 23; indicesPlatform[i++] = 21;
	indicesPlatform[i++] = 21; indicesPlatform[i++] = 23; indicesPlatform[i++] = 22;
	// Set Vertex Vectors
	setVertexVectors(numVerticesPerPlatform, numIndicesPerPlatform, verticesPlatform, indicesPlatform, true, true);
	// Set Content
	g_rectPlatformMesh.setContentFromHeapMemory(numVerticesPerPlatform, numIndicesPerPlatform, verticesPlatform, indicesPlatform, true);
	if (!g_rectPlatformMesh.initBuffers(g_dev, g_devcon)) { return false; }
	return true;
}

bool generateCirclePlatformMesh() {
	uint32_t numIndices = 0;
	uint32_t numVertices = 0;
	uint32_t* indices = NULL;
	VERTEX* vertices = NULL;
	readBINMESH(ID_CIRCLEPLATFORM_MESH, numIndices, numVertices, indices, vertices);
	// Set Vertex Vectors
	setVertexVectors(numVertices, numIndices, vertices, indices, true, true);
	// Set Content
	g_circlePlatformMesh.setContentFromHeapMemory(numVertices, numIndices, vertices, indices, true);
	if (!g_circlePlatformMesh.initBuffers(g_dev, g_devcon)) { return false; }
	return true;
}

// === D3D11 ===

bool initSwapChainDeviceAndDeviceContext() {
	DXGI_SWAP_CHAIN_DESC scd = {};
	ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));
	scd.BufferDesc.Width = g_windowWidthInPixel;
	scd.BufferDesc.Height = g_windowHeightInPixel;
	scd.BufferCount = 1;
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scd.BufferDesc.RefreshRate.Numerator = g_fps;
	scd.BufferDesc.RefreshRate.Denominator = 1;
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scd.OutputWindow = g_hwnd;
	scd.SampleDesc.Count = numAntialiasingSamples;
	scd.SampleDesc.Quality = 0;
	scd.Windowed = FALSE;
	scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	if (FAILED(D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, NULL, NULL, NULL, D3D11_SDK_VERSION, &scd, &g_swapchain, &g_dev, NULL, &g_devcon))) { return false; }
	return true;
}

bool initBackBufferAndDepthStencilBuffer() {
	ID3D11Texture2D* pBackBuffer = NULL;
	if (FAILED(g_swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer))) { return false; }
	if (FAILED(g_dev->CreateRenderTargetView(pBackBuffer, NULL, &g_backbuffer))) { return false; }
	pBackBuffer->Release();
	D3D11_TEXTURE2D_DESC dtd = {};
	dtd.Width = g_windowWidthInPixel;
	dtd.Height = g_windowHeightInPixel;
	dtd.MipLevels = 1;
	dtd.ArraySize = 1;
	dtd.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dtd.SampleDesc.Count = numAntialiasingSamples;
	dtd.SampleDesc.Quality = 0;
	dtd.Usage = D3D11_USAGE_DEFAULT;
	dtd.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	dtd.CPUAccessFlags = 0;
	dtd.MiscFlags = 0;
	D3D11_DEPTH_STENCIL_DESC dsd = {};
	dsd.DepthEnable = true;
	dsd.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsd.DepthFunc = D3D11_COMPARISON_LESS;
	dsd.StencilEnable = true;
	dsd.StencilReadMask = 0xFF;
	dsd.StencilWriteMask = 0xFF;
	dsd.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsd.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	dsd.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsd.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	dsd.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsd.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	dsd.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsd.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	if (FAILED(g_dev->CreateDepthStencilState(&dsd, &g_pDSState))) { return false; }
	g_devcon->OMSetDepthStencilState(g_pDSState, 1);
	if (FAILED(g_dev->CreateTexture2D(&dtd, 0, &g_pDSBuffer))) { return false; }
	if (FAILED(g_dev->CreateDepthStencilView(g_pDSBuffer, 0, &g_pDSView))) { return false; }
	if (g_pDSBuffer == NULL) { return false; }
	if (g_pDSView == NULL) { return false; }
	setCameraRenderTarget();
	return true;
}

void initViewports() {
	// Camera
	ZeroMemory(&g_camera_viewport, sizeof(D3D11_VIEWPORT));
	g_camera_viewport.TopLeftX = 0.0f;
	g_camera_viewport.TopLeftY = 0.0f;
	g_camera_viewport.Width = (float)g_windowWidthInPixel;
	g_camera_viewport.Height = (float)g_windowHeightInPixel;
	g_camera_viewport.MinDepth = 0.0f;
	g_camera_viewport.MaxDepth = 1.0f;
	// Shadowmap
	ZeroMemory(&g_shadowmap_viewport, sizeof(D3D11_VIEWPORT));
	g_shadowmap_viewport.TopLeftX = 0.0f;
	g_shadowmap_viewport.TopLeftY = 0.0f;
	g_shadowmap_viewport.Width = (float)shadowMapResolution;
	g_shadowmap_viewport.Height = (float)shadowMapResolution;
	g_shadowmap_viewport.MinDepth = 0.0f;
	g_shadowmap_viewport.MaxDepth = 1.0f;
	// Set Camera Viewport
	setCameraViewport();
}

bool initShaders() {
	DWORD size = 0;
	void* data = NULL;
	// Create Vertex Shaders
	LoadFileInResource(ID_VS_PLAYER, SHADERFILE, size, data);
	if (FAILED(g_dev->CreateVertexShader(data, size, NULL, &g_pVS_PLAYER))) { return false; }
	LoadFileInResource(ID_VS_PLATFORM, SHADERFILE, size, data);
	if (FAILED(g_dev->CreateVertexShader(data, size, NULL, &g_pVS_PLATFORM))) { return false; }
	LoadFileInResource(ID_VS_STATIC, SHADERFILE, size, data);
	if (FAILED(g_dev->CreateVertexShader(data, size, NULL, &g_pVS_STATIC))) { return false; }
	LoadFileInResource(ID_VS_GEM, SHADERFILE, size, data);
	if (FAILED(g_dev->CreateVertexShader(data, size, NULL, &g_pVS_GEM))) { return false; }
	LoadFileInResource(ID_VS_CHECKPOINT, SHADERFILE, size, data);
	if (FAILED(g_dev->CreateVertexShader(data, size, NULL, &g_pVS_CHECKPOINT))) { return false; }
	LoadFileInResource(ID_VS_UPWIND, SHADERFILE, size, data);
	if (FAILED(g_dev->CreateVertexShader(data, size, NULL, &g_pVS_UPWIND))) { return false; }
	LoadFileInResource(ID_VS_LAMP, SHADERFILE, size, data);
	if (FAILED(g_dev->CreateVertexShader(data, size, NULL, &g_pVS_LAMP))) { return false; }
	LoadFileInResource(ID_VS_SKY, SHADERFILE, size, data);
	if (FAILED(g_dev->CreateVertexShader(data, size, NULL, &g_pVS_SKY))) { return false; }
	LoadFileInResource(ID_VS_TEXT, SHADERFILE, size, data);
	if (FAILED(g_dev->CreateVertexShader(data, size, NULL, &g_pVS_TEXT))) { return false; }
	LoadFileInResource(ID_VS_BOXLAYER, SHADERFILE, size, data);
	if (FAILED(g_dev->CreateVertexShader(data, size, NULL, &g_pVS_BOXLAYER))) { return false; }
	// Set Input Layout
	if (FAILED(g_dev->CreateInputLayout(ied, iedSize, data, size, &g_pLayout))) { return false; }
	g_devcon->IASetInputLayout(g_pLayout);
	// Create Pixel Shaders
	LoadFileInResource(ID_PS_PLAYER, SHADERFILE, size, data);
	if (FAILED(g_dev->CreatePixelShader(data, size, NULL, &g_pPS_PLAYER))) { return false; }
	LoadFileInResource(ID_PS_PLATFORM, SHADERFILE, size, data);
	if (FAILED(g_dev->CreatePixelShader(data, size, NULL, &g_pPS_PLATFORM))) { return false; }
	LoadFileInResource(ID_PS_STATIC, SHADERFILE, size, data);
	if (FAILED(g_dev->CreatePixelShader(data, size, NULL, &g_pPS_STATIC))) { return false; }
	LoadFileInResource(ID_PS_GEM, SHADERFILE, size, data);
	if (FAILED(g_dev->CreatePixelShader(data, size, NULL, &g_pPS_GEM))) { return false; }
	LoadFileInResource(ID_PS_CHECKPOINT, SHADERFILE, size, data);
	if (FAILED(g_dev->CreatePixelShader(data, size, NULL, &g_pPS_CHECKPOINT))) { return false; }
	LoadFileInResource(ID_PS_UPWIND, SHADERFILE, size, data);
	if (FAILED(g_dev->CreatePixelShader(data, size, NULL, &g_pPS_UPWIND))) { return false; }
	LoadFileInResource(ID_PS_LAMP, SHADERFILE, size, data);
	if (FAILED(g_dev->CreatePixelShader(data, size, NULL, &g_pPS_LAMP))) { return false; }
	LoadFileInResource(ID_PS_SKY, SHADERFILE, size, data);
	if (FAILED(g_dev->CreatePixelShader(data, size, NULL, &g_pPS_SKY))) { return false; }
	LoadFileInResource(ID_PS_TEXT, SHADERFILE, size, data);
	if (FAILED(g_dev->CreatePixelShader(data, size, NULL, &g_pPS_TEXT))) { return false; }
	LoadFileInResource(ID_PS_BOXLAYER, SHADERFILE, size, data);
	if (FAILED(g_dev->CreatePixelShader(data, size, NULL, &g_pPS_BOXLAYER))) { return false; }
	// Set Shaders
	g_devcon->VSSetShader(g_pVS_PLAYER, 0, 0);
	g_devcon->PSSetShader(g_pPS_PLAYER, 0, 0);
	return true;
}

bool initMeshes() {
	if (!generatePlayerMesh()) { return false; }
	if (!generateCheckpointMesh()) { return false; }
	if (!generateUpwindMesh()) { return false; }
	if (!generateSkyMesh()) { return false; }
	if (!generateBoxlayerMesh()) { return false; }
	if (!generateGemMesh()) { return false; }
	if (!generateLampMesh()) { return false; }
	if (!generateRectPlatformMesh()) { return false; }
	if (!generateCirclePlatformMesh()) { return false; }
	return true;
}

bool initRenderTexts() {
	for (int i = 0; i < num_startMenuScreenText_parts; i++) { if (!g_startMenuScreenText_array[i].mesh.initBuffers(g_dev, g_devcon)) { return false; } }
	for (int i = 0; i < num_manualScreenText_parts; i++) { if (!g_manualScreenText_array[i].mesh.initBuffers(g_dev, g_devcon)) { return false; } }
	for (int i = 0; i < num_creditsScreenText_parts; i++) { if (!g_creditsScreenText_array[i].mesh.initBuffers(g_dev, g_devcon)) { return false; } }
	for (int i = 0; i < num_pausedScreenText_parts; i++) { if (!g_pausedScreenText_array[i].mesh.initBuffers(g_dev, g_devcon)) { return false; } }
	for (int i = 0; i < num_gemsScreenText_parts; i++) { if (!g_gemsRenderText_array[i].mesh.initBuffers(g_dev, g_devcon)) { return false; } }
	for (int i = 0; i < num_checkpointsScreenText_parts; i++) { if (!g_checkpointsRenderText_array[i].mesh.initBuffers(g_dev, g_devcon)) { return false; } }
	return true;
}

bool initTextures() {
	DWORD size;
	void* data;
	// Glyph Atlas Texture
	size = 0;
	data = NULL;
	LoadFileInResource(ID_GLYPH_ATLAS_TEXTURE, GAME_TEXTURE, size, data);
	if (!getResourceViewFromPNG(data, size, &g_glyphAtlasTexture_SRV)) { return false; }
	if (g_glyphAtlasTexture_SRV == NULL) { return false; }
	g_devcon->PSSetShaderResources(0, 1, &g_glyphAtlasTexture_SRV);
	// Level Texture RGBA
	size = 0;
	data = NULL;
	LoadFileInResource(ID_LEVEL_RGBA_TEXTURE, GAME_TEXTURE, size, data);
	if (!getResourceViewFromPNG(data, size, &g_levelTexture_RGBA_SRV)) { return false; }
	g_devcon->PSSetShaderResources(1, 1, &g_levelTexture_RGBA_SRV);
	// Level Texture NORM
	size = 0;
	data = NULL;
	LoadFileInResource(ID_LEVEL_NORM_TEXTURE, GAME_TEXTURE, size, data);
	if (!getResourceViewFromPNG(data, size, &g_levelTexture_NORM_SRV)) { return false; }
	g_devcon->PSSetShaderResources(2, 1, &g_levelTexture_NORM_SRV);
	// Player Texture RGBA
	size = 0;
	data = NULL;
	LoadFileInResource(ID_PLAYER_RGBA_TEXTURE, GAME_TEXTURE, size, data);
	if (!getResourceViewFromPNG(data, size, &g_playerTexture_RGBA_SRV)) { return false; }
	g_devcon->PSSetShaderResources(3, 1, &g_playerTexture_RGBA_SRV);
	// Sky Texture RGBA
	size = 0;
	data = NULL;
	LoadFileInResource(ID_SKY_RGBA_TEXTURE, GAME_TEXTURE, size, data);
	if (!getResourceViewFromPNG(data, size, &g_skyTexture_RGBA_SRV)) { return false; }
	g_devcon->PSSetShaderResources(4, 1, &g_skyTexture_RGBA_SRV);
	return true;
}

bool initShadowMap() {
	D3D11_TEXTURE2D_DESC texDesc;
	texDesc.Width = shadowMapResolution;
	texDesc.Height = shadowMapResolution;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	dsvDesc.Flags = 0;
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Texture2D.MipSlice = 0;
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.MostDetailedMip = 0;
	g_Projection_shadowmap = XMMatrixPerspectiveFovRH(fov, 1.0, nearZ, farZ);
	if (FAILED(g_dev->CreateTexture2D(&texDesc, 0, &g_depthMap))) { return false; }
	if (FAILED(g_dev->CreateDepthStencilView(g_depthMap, &dsvDesc, &g_depthMapDSV))) { return false; }
	if (FAILED(g_dev->CreateShaderResourceView(g_depthMap, &srvDesc, &g_depthMapSRV))) { return false; }
	g_devcon->PSSetShaderResources(5, 1, &g_depthMapSRV);
	return true;
}

bool initSamplerState() {
	// Texture Sampling
	D3D11_SAMPLER_DESC sd_texture = {};
	ZeroMemory(&sd_texture, sizeof(D3D11_SAMPLER_DESC));
	sd_texture.Filter = D3D11_FILTER_ANISOTROPIC;
	sd_texture.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	sd_texture.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	sd_texture.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	sd_texture.MipLODBias = 0.0f;
	sd_texture.MaxAnisotropy = samplerMaxAnisotropy;
	sd_texture.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sd_texture.BorderColor[0] = 1.0f;
	sd_texture.BorderColor[1] = 1.0f;
	sd_texture.BorderColor[2] = 1.0f;
	sd_texture.BorderColor[3] = 1.0f;
	sd_texture.MinLOD = -FLT_MAX;
	sd_texture.MaxLOD = FLT_MAX;
	g_dev->CreateSamplerState(&sd_texture, &g_pSamplerState_texture);
	if (g_pSamplerState_texture == NULL) { return false; }
	g_devcon->PSSetSamplers(0, 1, &g_pSamplerState_texture);
	// Shadowmap Sampling
	D3D11_SAMPLER_DESC sd_shadowmap = {};
	ZeroMemory(&sd_shadowmap, sizeof(D3D11_SAMPLER_DESC));
	sd_shadowmap.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
	sd_shadowmap.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	sd_shadowmap.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	sd_shadowmap.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	sd_shadowmap.MipLODBias = 0.0f;
	sd_shadowmap.MaxAnisotropy = 1;
	sd_shadowmap.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
	sd_shadowmap.BorderColor[0] = 0.0f;
	sd_shadowmap.BorderColor[1] = 0.0f;
	sd_shadowmap.BorderColor[2] = 0.0f;
	sd_shadowmap.BorderColor[3] = 0.0f;
	sd_shadowmap.MinLOD = 0.0f;
	sd_shadowmap.MaxLOD = D3D11_FLOAT32_MAX;
	g_dev->CreateSamplerState(&sd_shadowmap, &g_pSamplerState_shadowmap);
	if (g_pSamplerState_shadowmap == NULL) { return false; }
	g_devcon->PSSetSamplers(1, 1, &g_pSamplerState_shadowmap);
	return true;
}

bool initRasterizerState() {
	D3D11_RASTERIZER_DESC rd = {};
	ZeroMemory(&rd, sizeof(D3D11_RASTERIZER_DESC));
	rd.FillMode = D3D11_FILL_SOLID;
	rd.CullMode = D3D11_CULL_BACK;
	rd.FrontCounterClockwise = FALSE;
	rd.DepthBias = 0;
	rd.DepthBiasClamp = 0.0f;
	rd.SlopeScaledDepthBias = 0.0f;
	rd.DepthClipEnable = TRUE;
	rd.ScissorEnable = FALSE;
	rd.MultisampleEnable = TRUE;
	rd.AntialiasedLineEnable = TRUE;
	g_dev->CreateRasterizerState(&rd, &g_pRasterizerState);
	if (g_pRasterizerState == NULL) { return false; }
	g_devcon->RSSetState(g_pRasterizerState);
	return true;
}

bool initBlendState() {
	// Blend State Default
	D3D11_BLEND_DESC bd_default = {};
	ZeroMemory(&bd_default, sizeof(D3D11_BLEND_DESC));
	bd_default.AlphaToCoverageEnable = FALSE;
	bd_default.IndependentBlendEnable = FALSE;
	bd_default.RenderTarget[0].BlendEnable = TRUE;
	bd_default.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	bd_default.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	bd_default.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	bd_default.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	bd_default.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	bd_default.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	bd_default.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	if (FAILED(g_dev->CreateBlendState(&bd_default, &g_pBlendState_default))) { return false; }
	// Blend State Alpha To Coverage
	D3D11_BLEND_DESC bd_alphaToCoverage = {};
	ZeroMemory(&bd_alphaToCoverage, sizeof(D3D11_BLEND_DESC));
	bd_alphaToCoverage.AlphaToCoverageEnable = TRUE;
	bd_alphaToCoverage.IndependentBlendEnable = FALSE;
	bd_alphaToCoverage.RenderTarget[0].BlendEnable = TRUE;
	bd_alphaToCoverage.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	bd_alphaToCoverage.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	bd_alphaToCoverage.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	bd_alphaToCoverage.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	bd_alphaToCoverage.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	bd_alphaToCoverage.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	bd_alphaToCoverage.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	if (FAILED(g_dev->CreateBlendState(&bd_alphaToCoverage, &g_pBlendState_alphaToCoverage))) { return false; }
	// Set Blend State
	g_devcon->OMSetBlendState(g_pBlendState_default, 0, 0xFFFFFFFF);
	return true;
}

bool initConstantBuffer() {
	// Camera
	D3D11_BUFFER_DESC cbd_c = {};
	ZeroMemory(&cbd_c, sizeof(cbd_c));
	cbd_c.Usage = D3D11_USAGE_DEFAULT;
	cbd_c.ByteWidth = sizeof(ConstantBufferCamera);
	cbd_c.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd_c.CPUAccessFlags = 0;
	cbd_c.MiscFlags = 0;
	if (FAILED(g_dev->CreateBuffer(&cbd_c, 0, &g_pCBuffer_camera))) { return false; }
	if (g_pCBuffer_camera == NULL) { return false; }
	// Per Frame
	D3D11_BUFFER_DESC cbd_pf = {};
	ZeroMemory(&cbd_pf, sizeof(cbd_pf));
	cbd_pf.Usage = D3D11_USAGE_DEFAULT;
	cbd_pf.ByteWidth = sizeof(ConstantBufferPerFrame);
	cbd_pf.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd_pf.CPUAccessFlags = 0;
	cbd_pf.MiscFlags = 0;
	if (FAILED(g_dev->CreateBuffer(&cbd_pf, 0, &g_pCBuffer_perFrame))) { return false; }
	if (g_pCBuffer_perFrame == NULL) { return false; }
	// Per Level
	D3D11_BUFFER_DESC cbd_pl = {};
	ZeroMemory(&cbd_pl, sizeof(cbd_pl));
	cbd_pl.Usage = D3D11_USAGE_DEFAULT;
	cbd_pl.ByteWidth = sizeof(ConstantBufferPerLevel);
	cbd_pl.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd_pl.CPUAccessFlags = 0;
	cbd_pl.MiscFlags = 0;
	if (FAILED(g_dev->CreateBuffer(&cbd_pl, 0, &g_pCBuffer_perLevel))) { return false; }
	if (g_pCBuffer_perLevel == NULL) { return false; }
	// Init Content
	updateConstantBufferContent(true);
	// Bind Constant Buffers
	g_devcon->VSSetConstantBuffers(0, 1, &g_pCBuffer_camera);
	g_devcon->VSSetConstantBuffers(1, 1, &g_pCBuffer_perFrame);
	g_devcon->VSSetConstantBuffers(2, 1, &g_pCBuffer_perLevel);
	g_devcon->PSSetConstantBuffers(0, 1, &g_pCBuffer_camera);
	g_devcon->PSSetConstantBuffers(1, 1, &g_pCBuffer_perFrame);
	g_devcon->PSSetConstantBuffers(2, 1, &g_pCBuffer_perLevel);
	return true;
}

// === System ===

bool initSystem(HINSTANCE hInstance, int nCmdShow) {
	if (!initWindow(hInstance, nCmdShow)) { return false; }
	if (!initSwapChainDeviceAndDeviceContext()) { return false; }
	if (!initBackBufferAndDepthStencilBuffer()) { return false; }
	initViewports();
	if (!initShaders()) { return false; }
	if (!initMeshes()) { return false; }
	if (!initRenderTexts()) { return false; }
	if (!initTextures()) { return false; }
	if (!initShadowMap()) { return false; }
	if (!initSamplerState()) { return false; }
	if (!initRasterizerState()) { return false; }
	if (!initBlendState()) { return false; }
	if (!initConstantBuffer()) { return false; }
	g_devcon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	return true;
}

void closeSystem() {
	// Release Textures
	if (g_glyphAtlasTexture_SRV != NULL) { g_glyphAtlasTexture_SRV->Release(); g_glyphAtlasTexture_SRV = NULL; }
	if (g_levelTexture_RGBA_SRV != NULL) { g_levelTexture_RGBA_SRV->Release(); g_levelTexture_RGBA_SRV = NULL; }
	if (g_levelTexture_NORM_SRV != NULL) { g_levelTexture_NORM_SRV->Release(); g_levelTexture_NORM_SRV = NULL; }
	if (g_playerTexture_RGBA_SRV != NULL) { g_playerTexture_RGBA_SRV->Release(); g_playerTexture_RGBA_SRV = NULL; }
	if (g_skyTexture_RGBA_SRV != NULL) { g_skyTexture_RGBA_SRV->Release(); g_skyTexture_RGBA_SRV = NULL; }
	// Vertex Shaders
	if (g_pVS_PLAYER != NULL) { g_pVS_PLAYER->Release(); g_pVS_PLAYER = NULL; }
	if (g_pVS_PLATFORM != NULL) { g_pVS_PLATFORM->Release(); g_pVS_PLATFORM = NULL; }
	if (g_pVS_STATIC != NULL) { g_pVS_STATIC->Release(); g_pVS_STATIC = NULL; }
	if (g_pVS_GEM != NULL) { g_pVS_GEM->Release(); g_pVS_GEM = NULL; }
	if (g_pVS_CHECKPOINT != NULL) { g_pVS_CHECKPOINT->Release(); g_pVS_CHECKPOINT = NULL; }
	if (g_pVS_UPWIND != NULL) { g_pVS_UPWIND->Release(); g_pVS_UPWIND = NULL; }
	if (g_pVS_LAMP != NULL) { g_pVS_LAMP->Release(); g_pVS_LAMP = NULL; }
	if (g_pVS_SKY != NULL) { g_pVS_SKY->Release(); g_pVS_SKY = NULL; }
	if (g_pVS_TEXT != NULL) { g_pVS_TEXT->Release(); g_pVS_TEXT = NULL; }
	if (g_pVS_BOXLAYER != NULL) { g_pVS_BOXLAYER->Release(); g_pVS_BOXLAYER = NULL; }
	// Pixel Shaders
	if (g_pPS_PLAYER != NULL) { g_pPS_PLAYER->Release(); g_pPS_PLAYER = NULL; }
	if (g_pPS_PLATFORM != NULL) { g_pPS_PLATFORM->Release(); g_pPS_PLATFORM = NULL; }
	if (g_pPS_STATIC != NULL) { g_pPS_STATIC->Release(); g_pPS_STATIC = NULL; }
	if (g_pPS_GEM != NULL) { g_pPS_GEM->Release(); g_pPS_GEM = NULL; }
	if (g_pPS_CHECKPOINT != NULL) { g_pPS_CHECKPOINT->Release(); g_pPS_CHECKPOINT = NULL; }
	if (g_pPS_UPWIND != NULL) { g_pPS_UPWIND->Release(); g_pPS_UPWIND = NULL; }
	if (g_pPS_LAMP != NULL) { g_pPS_LAMP->Release(); g_pPS_LAMP = NULL; }
	if (g_pPS_SKY != NULL) { g_pPS_SKY->Release(); g_pPS_SKY = NULL; }
	if (g_pPS_TEXT != NULL) { g_pPS_TEXT->Release(); g_pPS_TEXT = NULL; }
	if (g_pPS_BOXLAYER != NULL) { g_pPS_BOXLAYER->Release(); g_pPS_BOXLAYER = NULL; }
	// Constant Buffer
	if (g_pCBuffer_camera != NULL) { g_pCBuffer_camera->Release(); g_pCBuffer_camera = NULL; }
	if (g_pCBuffer_perFrame != NULL) { g_pCBuffer_perFrame->Release(); g_pCBuffer_perFrame = NULL; }
	if (g_pCBuffer_perLevel != NULL) { g_pCBuffer_perLevel->Release(); g_pCBuffer_perLevel = NULL; }
	// Release other D3D Resources
	if (g_pSamplerState_texture != NULL) { g_pSamplerState_texture->Release(); g_pSamplerState_texture = NULL; }
	if (g_pSamplerState_shadowmap != NULL) { g_pSamplerState_shadowmap->Release(); g_pSamplerState_shadowmap = NULL; }
	if (g_pRasterizerState != NULL) { g_pRasterizerState->Release(); g_pRasterizerState = NULL; }
	if (g_pBlendState_default != NULL) { g_pBlendState_default->Release(); g_pBlendState_default = NULL; }
	if (g_pBlendState_alphaToCoverage != NULL) { g_pBlendState_alphaToCoverage->Release(); g_pBlendState_alphaToCoverage = NULL; }
	if (g_pLayout != NULL) { g_pLayout->Release(); g_pLayout = NULL; }
	if (g_pDSBuffer != NULL) { g_pDSBuffer->Release(); g_pDSBuffer = NULL; }
	if (g_pDSState != NULL) { g_pDSState->Release(); g_pDSState = NULL; }
	if (g_pDSView != NULL) { g_pDSView->Release(); g_pDSView = NULL; }
	if (g_depthMap != NULL) { g_depthMap->Release(); g_depthMap = NULL; }
	if (g_depthMapDSV != NULL) { g_depthMapDSV->Release(); g_depthMapDSV = NULL; }
	if (g_depthMapSRV != NULL) { g_depthMapSRV->Release(); g_depthMapSRV = NULL; }
	if (g_swapchain != NULL) { g_swapchain->Release(); g_swapchain = NULL; }
	if (g_backbuffer != NULL) { g_backbuffer->Release(); g_backbuffer = NULL; }
	if (g_dev != NULL) { g_dev->Release(); g_dev = NULL; }
	if (g_devcon != NULL) { g_devcon->Release(); g_devcon = NULL; }
}

// === Game Variables ===

void resetMovements() {
	int curr_dx = 0;
	int curr_dy = 0;
	getMouseMovement(curr_dx, curr_dy);
	getMouseMovement(curr_dx, curr_dy);
	g_lastMouseMovements.clear();
	g_lastLeftStickMovements.clear();
	g_lastRightStickMovements.clear();
	for (int i = 0; i < movementBufferSize; i++) {
		g_lastMouseMovements.push_back(Point2int(0, 0));
		g_lastLeftStickMovements.push_back(Point2double(0, 0));
		g_lastRightStickMovements.push_back(Point2double(0, 0));
	}
}

void initGame(InitType initType) {
	if (initType == NEW_LEVEL) {
		// New Map
		g_map = Map();
	}
	if (initType == FIRST_INIT || initType == NEW_LEVEL || initType == RESPAWN) {
		// Player, Camera
		RespawnInfo& startRespawn = g_map.respawnInfos.at(g_map.respawnIndex);
		g_player.p = startRespawn.respawnPosition;
		g_player.angleOffset = startRespawn.respawnPlayerAngleOffset;
		g_camera.alpha = startRespawn.respawnCameraAlpha;
		g_camera.beta = startRespawn.respawnCameraBeta;
		g_camera.at = g_player.p;
		// Jumping
		g_lastReferencePoint = g_player.p;
		g_inAirFrames = 0;
		g_jumpState = ON_GROUND;
		// Reset Movements
		resetMovements();
		// Gems Render Text
		g_gemsRenderText_array[1].updateText(getCollectedGemsString(), whiteColor);
		g_gemsRenderText_array[1].mesh.updateVertexBuffer(g_devcon);
		// Checkpoints
		g_checkpointsRenderText_array[1].updateText(getActivatedCheckpointsString(), whiteColor);
		g_checkpointsRenderText_array[1].mesh.updateVertexBuffer(g_devcon);
	}
	if (initType == NEW_LEVEL || initType == RESPAWN) {
		// ConstantBuffer
		updateConstantBufferContent(true);
	}
}

// === Update Game Status ===

void updateGameStatus(GameStatus nextStatus) {
	// Mouse & Cursor
	if (g_gameStatus != RUNNING && nextStatus == RUNNING) { lockCursor(); }
	else if (g_gameStatus == RUNNING && nextStatus != RUNNING) { releaseCursor(); }
	// Reset Movements
	if (g_gameStatus != RUNNING && nextStatus == RUNNING) { resetMovements(); }
	// Init Game
	if (g_gameStatus == PAUSED && nextStatus == START_MENU) { initGame(NEW_LEVEL); }
	// Set isLevelStarted
	if (g_gameStatus == START_MENU && nextStatus == RUNNING) { g_isLevelStarted = true; }
	else if (g_gameStatus == PAUSED && nextStatus == START_MENU) { g_isLevelStarted = false; }
	// Update status
	g_gameStatus = nextStatus;
}

// === Cursor ===

void lockCursor() {
	// Get Window Rect
	RECT r;
	GetWindowRect(g_hwnd, &r);
	// Get Window Center
	int cx = r.left + (r.right - r.left) / 2;
	int cy = r.top + (r.bottom - r.top) / 2;
	// Shrink Rect
	r.left = cx - mouseLockedAreaSize2;
	r.right = cx + mouseLockedAreaSize2;
	r.top = cy - mouseLockedAreaSize2;
	r.bottom = cy + mouseLockedAreaSize2;
	// Clip Cursor
	ClipCursor(&r);
	// Hide Cursor
	ShowCursor(FALSE);
}

void releaseCursor() {
	// Unclip Cursor
	ClipCursor(NULL);
	// Show Cursor
	ShowCursor(TRUE);
}

// === Process Input ===

void get_movement_from_WASD_or_LeftStick(double& joyLeftDeltaScaledX, double& joyLeftDeltaScaledY, double& moveX, double& moveY) {
	// Keyboard
	if ((bool)GetAsyncKeyState(0x57)) { moveX -= 1.0; }
	if ((bool)GetAsyncKeyState(0x53)) { moveX += 1.0; }
	if ((bool)GetAsyncKeyState(0x41)) { moveY -= 1.0; }
	if ((bool)GetAsyncKeyState(0x44)) { moveY += 1.0; }
	if (abs(moveX) + abs(moveY) == 2.0) {
		moveX = moveX / sqrt2;
		moveY = moveY / sqrt2;
	}
	// Gamepad
	if (moveX == 0.0 && moveY == 0.0) {
		moveX = -joyLeftDeltaScaledY;
		moveY = joyLeftDeltaScaledX;
	}
}

bool is_Esc_or_B_pressed() {
	bool keyboard = (bool)GetAsyncKeyState(VK_ESCAPE);
	bool gamepad = g_isGamepadConnected && (g_gamepadState.Gamepad.wButtons & XINPUT_GAMEPAD_B);
	return (keyboard || gamepad);
}

bool is_Esc_or_Start_pressed() {
	bool keyboard = (bool)GetAsyncKeyState(VK_ESCAPE);
	bool gamepad = g_isGamepadConnected && (g_gamepadState.Gamepad.wButtons & XINPUT_GAMEPAD_START);
	return (keyboard || gamepad);
}

bool is_F1_or_Y_pressed(){
	bool keyboard = (bool)GetAsyncKeyState(VK_F1);
	bool gamepad = g_isGamepadConnected && (g_gamepadState.Gamepad.wButtons & XINPUT_GAMEPAD_Y);
	return (keyboard || gamepad);
}

bool is_F2_or_X_pressed(){
	bool keyboard = (bool)GetAsyncKeyState(VK_F2);
	bool gamepad = g_isGamepadConnected && (g_gamepadState.Gamepad.wButtons & XINPUT_GAMEPAD_X);
	return (keyboard || gamepad);
}

bool is_Space_or_A_pressed() {
	bool keyboard = (bool)GetAsyncKeyState(VK_SPACE);
	bool gamepad = g_isGamepadConnected && (g_gamepadState.Gamepad.wButtons & XINPUT_GAMEPAD_A);
	return (keyboard || gamepad);
}

bool is_MouseLeft_or_LB_pressed() {
	bool keyboard = (bool)GetAsyncKeyState(VK_LBUTTON);
	bool gamepad = g_isGamepadConnected && (g_gamepadState.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER);
	return (keyboard || gamepad);
}

bool is_MouseRight_or_RB_pressed() {
	bool keyboard = (bool)GetAsyncKeyState(VK_RBUTTON);
	bool gamepad = g_isGamepadConnected && (g_gamepadState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER);
	return (keyboard || gamepad);
}

bool is_Q_or_Y_pressed() {
	bool keyboard = (bool)GetAsyncKeyState(0x51);
	bool gamepad = g_isGamepadConnected && (g_gamepadState.Gamepad.wButtons & XINPUT_GAMEPAD_Y);
	return (keyboard || gamepad);
}

void getStickMovement(JoyStickType type, double& dx, double& dy) {
	double magnitude = 0.0;
	bool isUsed = false;
	dx = 0;
	dy = 0;
	if (g_isGamepadConnected) {
		// Get thumb vector
		double vecX = 0;
		double vecY = 0;
		if (type == JoyStickType::JOYSTICK_LEFT) {
			vecX = g_gamepadState.Gamepad.sThumbLX;
			vecY = g_gamepadState.Gamepad.sThumbLY;
		}
		else if (type == JoyStickType::JOYSTICK_RIGHT) {
			vecX = g_gamepadState.Gamepad.sThumbRX;
			vecY = g_gamepadState.Gamepad.sThumbRY;
		}
		double d = sqrt(vecX * vecX + vecY * vecY);
		// Get normalized magnitude vector
		if (d <= inputJoystickTh1) {
			magnitude = 0.0;
			isUsed = false;
		}
		else if (d > inputJoystickTh1 && d <= inputJoystickTh2) {
			magnitude = ( d - inputJoystickTh1 ) / inputJoystickThDist;
			isUsed = true;
		}
		else {
			magnitude = 1.0;
			isUsed = true;
		}
		if (isUsed) {
			dx = magnitude * (vecX / d);
			dy = magnitude * (vecY / d);
		}
	}
}

void getMouseMovement(int& dx, int& dy) {
	if (g_event_mouseMoveX != 0 || g_event_mouseMoveY != 0) {
		// Read out last movement
		dx = g_event_mouseMoveX;
		dy = g_event_mouseMoveY;
		// Reset movement
		g_event_mouseMoveX = 0;
		g_event_mouseMoveY = 0;
	}
	else {
		// No Movement
		dx = 0;
		dy = 0;
	}
}

void processInput() {
	// Gamepad State
	g_isGamepadConnected = (XInputGetState(0, &g_gamepadState) == ERROR_SUCCESS);
	// Keyboard
	bool EscapeCurr = false;
	if (g_gameStatus == RUNNING) { EscapeCurr = is_Esc_or_Start_pressed(); } else { EscapeCurr = is_Esc_or_B_pressed(); }
	bool SpaceCurr = is_Space_or_A_pressed();
	bool MouseLeftCurr = is_MouseLeft_or_LB_pressed();
	bool MouseRightCurr = is_MouseRight_or_RB_pressed();
	bool QCurr = is_Q_or_Y_pressed();
	bool F1Curr = is_F1_or_Y_pressed();
	bool F2Curr = is_F2_or_X_pressed();
	// --- START MENU ---
	if (g_gameStatus == START_MENU) {
		if (g_EscapePressed != EscapeCurr && EscapeCurr) { g_isRunning = false; }
		if (g_SpacePressed != SpaceCurr && SpaceCurr) { updateGameStatus(RUNNING); }
		if (g_F1Pressed != F1Curr && F1Curr) { updateGameStatus(MANUAL); }
		if (g_F2Pressed != F2Curr && F2Curr) { updateGameStatus(CREDITS); }
	}
	// --- MANUAL ---
	else if (g_gameStatus == MANUAL) {
		if (g_EscapePressed != EscapeCurr && EscapeCurr) { if (g_isLevelStarted) { updateGameStatus(PAUSED); } else { updateGameStatus(START_MENU); } }
	}
	// --- CREDITS ---
	else if (g_gameStatus == CREDITS) {
		if (g_EscapePressed != EscapeCurr && EscapeCurr) { if (g_isLevelStarted) { updateGameStatus(PAUSED); } else { updateGameStatus(START_MENU); } }
	}
	// --- PAUSED ---
	else if (g_gameStatus == PAUSED) {
		if (g_EscapePressed != EscapeCurr && EscapeCurr) { updateGameStatus(START_MENU); }
		if (g_SpacePressed != SpaceCurr && SpaceCurr) { updateGameStatus(RUNNING); }
		if (g_F1Pressed != F1Curr && F1Curr) { updateGameStatus(MANUAL); }
	}
	// --- RUNNING ---
	else if (g_gameStatus == RUNNING) {
		// Escape
		if (g_EscapePressed != EscapeCurr && EscapeCurr) { updateGameStatus(PAUSED); }
		// Q
		if (g_QPressed != QCurr && QCurr) { initGame(RESPAWN); }
		// Get movements
		int curr_mouse_dx, curr_mouse_dy;
		double curr_joy_left_dx, curr_joy_left_dy, curr_joy_right_dx, curr_joy_right_dy;
		getMouseMovement(curr_mouse_dx, curr_mouse_dy);
		getStickMovement(JOYSTICK_LEFT, curr_joy_left_dx, curr_joy_left_dy);
		getStickMovement(JOYSTICK_RIGHT, curr_joy_right_dx, curr_joy_right_dy);
		// Update movement buffer
		g_lastMouseMovements.push_back(Point2int(curr_mouse_dx, curr_mouse_dy));
		g_lastMouseMovements.erase(g_lastMouseMovements.begin());
		g_lastLeftStickMovements.push_back(Point2double(curr_joy_left_dx, curr_joy_left_dy));
		g_lastLeftStickMovements.erase(g_lastLeftStickMovements.begin());
		g_lastRightStickMovements.push_back(Point2double(curr_joy_right_dx, curr_joy_right_dy));
		g_lastRightStickMovements.erase(g_lastRightStickMovements.begin());
		// Get smoothed movement
		double mouseDeltaX = 0.0, mouseDeltaY = 0.0, joyLeftDeltaX = 0.0, joyLeftDeltaY = 0.0, joyRightDeltaX = 0.0, joyRightDeltaY = 0.0;
		for (Point2int& currMovement : g_lastMouseMovements) {
			mouseDeltaX += (double)currMovement.x;
			mouseDeltaY += (double)currMovement.y;
		}
		mouseDeltaX = mouseDeltaX / ((double)movementBufferSize);
		mouseDeltaY = mouseDeltaY / ((double)movementBufferSize);
		for (Point2double& currMovement : g_lastLeftStickMovements) {
			joyLeftDeltaX += currMovement.x;
			joyLeftDeltaY += currMovement.y;
		}
		joyLeftDeltaX = joyLeftDeltaX / ((double)movementBufferSize);
		joyLeftDeltaY = joyLeftDeltaY / ((double)movementBufferSize);
		for (Point2double& currMovement : g_lastRightStickMovements) {
			joyRightDeltaX += currMovement.x;
			joyRightDeltaY += currMovement.y;
		}
		joyRightDeltaX = joyRightDeltaX / ((double)movementBufferSize);
		joyRightDeltaY = joyRightDeltaY / ((double)movementBufferSize);
		// Scale movements
		double mouseDeltaScaledX = mouseScaleX * g_movement_scale_fps * mouseDeltaX;
		double mouseDeltaScaledY = mouseScaleY * g_movement_scale_fps * mouseDeltaY;
		double joyLeftDeltaScaledX = joyLeftScaleX * g_movement_scale_fps * joyLeftDeltaX;
		double joyLeftDeltaScaledY = joyLeftScaleY * g_movement_scale_fps * joyLeftDeltaY;
		double joyRightDeltaScaledX = joyRightScaleX * g_movement_scale_fps * joyRightDeltaX;
		double joyRightDeltaScaledY = joyRightScaleY * g_movement_scale_fps * joyRightDeltaY;
		// Update camera angles
		g_camera.alpha += (-mouseDeltaScaledY + joyRightDeltaScaledY);
		g_camera.beta += (-mouseDeltaScaledX - joyRightDeltaScaledX);
		if (g_camera.alpha < cameraMinAlpha) { g_camera.alpha = cameraMinAlpha; }
		else if (g_camera.alpha > cameraMaxAlpha) { g_camera.alpha = cameraMaxAlpha; }
		if (g_camera.beta < 0) { g_camera.beta = g_camera.beta + 360; }
		else if (g_camera.beta > 360) { g_camera.beta -= 360; }
		// Set Player Movement
		g_moveVec.x = 0;
		g_moveVec.y = 0;
		double moveX = 0.0;
		double moveY = 0.0;
		get_movement_from_WASD_or_LeftStick(joyLeftDeltaScaledX, joyLeftDeltaScaledY, moveX, moveY);
		if (moveX == 0.0 && moveY == 0.0) { g_isMoving = false; }
		else { g_isMoving = true; }
		double movementSpeed_XY = movementSpeeds_XY[0];
		if (g_jumpState == STOMP) { movementSpeed_XY = 0; }
		else if (MouseRightCurr) { movementSpeed_XY = movementSpeeds_XY[1]; }
		g_moveVec.x = movementSpeed_XY * g_movement_scale_fps * (moveX * cos(2 * DirectX::XM_PI * (g_camera.beta / 360)) - moveY * sin(2 * DirectX::XM_PI * (g_camera.beta / 360)));
		g_moveVec.y = movementSpeed_XY * g_movement_scale_fps * (moveX * sin(2 * DirectX::XM_PI * (g_camera.beta / 360)) + moveY * cos(2 * DirectX::XM_PI * (g_camera.beta / 360)));
		g_player.angleOffset = std::atan2(-moveY, -moveX);
		// Left Mouse
		if (g_MouseLeftPressed != MouseLeftCurr) {
			if (MouseLeftCurr) {
				if (g_jumpState == JUMP_NORMAL || g_jumpState == JUMP_SHORT || g_jumpState == JUMP_HIGH || g_jumpState == GLIDE || g_jumpState == FALL_NORMAL || g_jumpState == FALL_FINAL) {
					g_lastReferencePoint = g_player.p;
					g_inAirFrames = 0;
					g_jumpState = STOMP;
				}
			}
		}
		// Space
		if (g_SpacePressed != SpaceCurr) {
			if (SpaceCurr) {
				if (g_jumpState == ON_GROUND || g_jumpState == JUMP_NORMAL || g_jumpState == JUMP_SHORT || g_jumpState == JUMP_HIGH || g_jumpState == FALL_NORMAL || g_jumpState == GLIDE){
					// (1) On Ground
					if (g_jumpState == ON_GROUND) {
						g_lastReferencePoint = g_player.p;
						g_inAirFrames = 0;
						if (MouseLeftCurr) { g_jumpState = JUMP_HIGH; }
						else { g_jumpState = JUMP_NORMAL; }
					}
					// (2) In Jump or Normal Fall
					else if (g_jumpState == JUMP_NORMAL || g_jumpState == JUMP_SHORT || g_jumpState == JUMP_HIGH || g_jumpState == FALL_NORMAL) {
						g_lastReferencePoint = g_player.p;
						g_inAirFrames = 0;
						g_jumpState = GLIDE;
					}
					// (3) In Glide
					else if (g_jumpState == GLIDE) {
						g_lastReferencePoint = g_player.p;
						g_inAirFrames = 0;
						g_jumpState = FALL_FINAL;
					}
				}
			}
			// (4) Short Jump
			else if (!SpaceCurr && g_jumpState == JUMP_NORMAL && g_inAirFrames < (g_normal_jump.t_h - g_normal_jump.t_q)) {
				double t_short = g_inAirFrames;
				double t_h_short = t_short + g_normal_jump.t_q;
				double z_max_short = z_max_normal_jump * (t_h_short / g_normal_jump.t_h);
				g_short_jump.a = z_max_short / t_h_short;
				g_short_jump.b = -g_short_jump.a / (2 * g_normal_jump.t_q);
				g_short_jump.d = 2 * g_short_jump.b * g_normal_jump.t_q;
				g_short_jump.e = z_max_short - g_short_jump.d * t_h_short;
				g_short_jump.c = g_short_jump.a * (t_h_short - g_normal_jump.t_q) - g_short_jump.b * g_normal_jump.t_q * g_normal_jump.t_q;
				g_short_jump.t_h = t_h_short;
				g_short_jump.t_q = g_normal_jump.t_q;
				g_jumpState = JUMP_SHORT;
			}
		}
	}
	// Set Pressed Variables
	g_EscapePressed = EscapeCurr;
	g_SpacePressed = SpaceCurr;
	g_MouseLeftPressed = MouseLeftCurr;
	g_MouseRightPressed = MouseRightCurr;
	g_QPressed = QCurr;
	g_F1Pressed = F1Curr;
	g_F2Pressed = F2Curr;
}

// === Movement ===

double getInAirFunctionValue(double t, ZFunction f) {
	if (t <= (f.t_h - f.t_q)){ return f.a * t; }
	else if (t > (f.t_h - f.t_q) && t <= (f.t_h + f.t_q)) { return f.b * (t - f.t_h) * (t - f.t_h) + f.c; }
	else { return f.d * t + f.e; }
}

double getPlayerZMovement(double a_int) {
	if (g_jumpState != ON_GROUND) {
		if (g_jumpState == JUMP_NORMAL) { return getInAirFunctionValue( ((double)g_inAirFrames) + a_int, g_normal_jump); }
		else if (g_jumpState == JUMP_SHORT) { return getInAirFunctionValue( ((double)g_inAirFrames) + a_int, g_short_jump); }
		else if (g_jumpState == JUMP_HIGH) { return getInAirFunctionValue( ((double)g_inAirFrames) + a_int, g_high_jump); }
		else if (g_jumpState == GLIDE) { return getInAirFunctionValue( ((double)g_inAirFrames) + a_int + g_glide.t_h, g_glide) - g_glide.c; }
		else if (g_jumpState == FALL_NORMAL) { return getInAirFunctionValue( ((double)g_inAirFrames) + a_int + g_normal_jump.t_h, g_normal_jump) - g_normal_jump.c; }
		else if (g_jumpState == FALL_FINAL) { return getInAirFunctionValue( ((double)g_inAirFrames) + a_int + g_normal_jump.t_h, g_normal_jump) - g_normal_jump.c; }
		else if (g_jumpState == STOMP) { return getInAirFunctionValue(((double)g_inAirFrames) + a_int + g_stomp.t_h, g_stomp) - g_stomp.c; }
		else if (g_jumpState == UPWIND) { return g_upwind.a * (((double)g_inAirFrames) + a_int); }
		else if (g_jumpState == UPWIND_TOP) { return 0.0f; }
	}
	return 0;
}

void updatePlayerPositon() {
	// Start Point
	double player_start_x = g_player.p.x;
	double player_start_y = g_player.p.y;
	// Update Last Reference Point
	bool updateLastReferencePoint = false;
	// Set Jump State
	if (g_jumpState == ON_GROUND) { g_jumpState = FALL_NORMAL; }
	// Get Upwind state
	bool previous_isInUpwind = g_isInUpwind;
	bool previous_isInUpwindTop = g_isInUpwindTop;
	checkUpwind();
	bool current_isInUpwind = g_isInUpwind;
	bool current_isInUpwindTop = g_isInUpwindTop;
	// Upwind
	if (previous_isInUpwind != current_isInUpwind) {
		if (current_isInUpwind) {
			g_lastReferencePoint = g_player.p;
			g_inAirFrames = 0;
			g_jumpState = UPWIND;
		}
		else {
			g_lastReferencePoint = g_player.p;
			g_inAirFrames = 0;
			g_jumpState = FALL_NORMAL;
		}
	}
	// Upwind Top
	if (previous_isInUpwindTop != current_isInUpwindTop) {
		if (current_isInUpwindTop) {
			g_lastReferencePoint = g_player.p;
			g_inAirFrames = 0;
			g_jumpState = UPWIND_TOP;
		}
		else {
			g_lastReferencePoint = g_player.p;
			g_inAirFrames = 0;
			g_jumpState = FALL_NORMAL;
		}
	}
	// Move Vec Length
	double moveVec_XY_length = sqrt( g_moveVec.x * g_moveVec.x + g_moveVec.y * g_moveVec.y );
	double moveVec_Z_length = abs( getPlayerZMovement(1) - getPlayerZMovement(0) );
	double max_moveVec_length = max(moveVec_XY_length, moveVec_Z_length);
	// Vector subpart for current frame
	int num_subparts = max(((int)ceil(max_moveVec_length / subpartLength)), 1);
	bool check_subparts = true;
	for (int subpart_id = 1; check_subparts && ( subpart_id <= num_subparts ) ; subpart_id++) {
		// Set interpolation alpha
		double a_int = ( (double)subpart_id ) / ( (double)num_subparts );
		// Set Player Position
		g_player.p.x = player_start_x + a_int * g_moveVec.x;
		g_player.p.y = player_start_y + a_int * g_moveVec.y;
		g_player.p.z = g_lastReferencePoint.z + getPlayerZMovement(a_int);
		for (int dim = 0; dim <= 1; dim++) {
			// dim = 0: XY Shift
			// dim = 1: Z Shift
			bool perfomXY_Shift = (dim == 0);
			// Loop over all colisions
			int collisionIdx = 0;
			bool checkForCollision = true;
			while (checkForCollision && collisionIdx < maxCollisionChecks) {
				double maxShift_dx = 0;
				double maxShift_dy = 0;
				double maxShift_dz = 0;
				double maxShiftLength_XY = shiftEpsilon;
				double maxShiftLength_Z = shiftEpsilon;
				bool intersectionFound = false;
				// Iterate over all meshes
				for (int meshIndex = 0; meshIndex < 2; meshIndex++) {
					// Get current mesh
					// 0 - Rect Platform
					// 1 - Circle Platform
					Mesh* currMesh = NULL;
					if (meshIndex == 0) { currMesh = &g_rectPlatformMesh; }
					else if (meshIndex == 1) { currMesh = &g_circlePlatformMesh; }
					// Get current number of instances
					// 0 - Rect Platform: numPlatforms Instances
					// 1 - Circle Platform: 1 Instance
					int currNumInstances = 0;
					if (meshIndex == 0) { currNumInstances = g_map.numRectPlatforms; }
					else if (meshIndex == 1) { currNumInstances = 1; }
					// Check all instances
					for (int instanceID = 0; instanceID < currNumInstances; instanceID++) {
						// 0 - Platform: Get platform instance offset
						double instXOff = 0;
						double instYOff = 0;
						double instZOff = 0;
						if (meshIndex == 0) {
							RectPlatform& rp = g_map.allRectPlatforms.at(instanceID);
							instXOff = rp.xPos;
							instYOff = rp.yPos;
							instZOff = rp.z;
						}
						// Check if player is in collision check bounding box
						if (currMesh->use_collision &&
							g_player.p.x >= currMesh->coll_min_X + instXOff &&
							g_player.p.x <= currMesh->coll_max_X + instXOff &&
							g_player.p.y >= currMesh->coll_min_Y + instYOff &&
							g_player.p.y <= currMesh->coll_max_Y + instYOff &&
							g_player.p.z >= currMesh->coll_min_Z + instZOff &&
							g_player.p.z <= currMesh->coll_max_Z + instZOff){
							// Num Triangles
							int currNumTriangles = currMesh->numTriangles;
							// Check all triangles
							for (int triangleID = 0; triangleID < currNumTriangles; triangleID++) {
								// Get Triangle Indices
								unsigned int index0 = currMesh->indices[triangleID * 3 + 0];
								unsigned int index1 = currMesh->indices[triangleID * 3 + 1];
								unsigned int index2 = currMesh->indices[triangleID * 3 + 2];
								// Get triangle vertices
								VERTEX v0 = currMesh->vertices[index0];
								VERTEX v1 = currMesh->vertices[index1];
								VERTEX v2 = currMesh->vertices[index2];
								// Sort out far away triangles
								double trX = (v0.X + v1.X + v2.X) / 3;
								double trY = (v0.Y + v1.Y + v2.Y) / 3;
								double trZ = (v0.Z + v1.Z + v2.Z) / 3;
								double longEdge = max(max(getPointToPointDist(trX, trY, trZ, v0.X, v0.Y, v0.Z), getPointToPointDist(trX, trY, trZ, v1.X, v1.Y, v1.Z)), getPointToPointDist(trX, trY, trZ, v2.X, v2.Y, v2.Z));
								double distPlayerToTriangleMidPoint = getPointToPointDist(g_player.p.x, g_player.p.y, g_player.p.z, trX + instXOff, trY + instYOff, trZ + instZOff);
								if (distPlayerToTriangleMidPoint - collisionPrecheckRadius > longEdge) {
									continue;
								}
								// Query distance
								double closest_x = 0;
								double closest_y = 0;
								double closest_z = 0;
								double sqrDistance = 0;
								getDistancePointToTriangle(g_player.p.x, g_player.p.y, g_player.p.z, v0.X + instXOff, v0.Y + instYOff, v0.Z + instZOff, v1.X + instXOff, v1.Y + instYOff, v1.Z + instZOff, v2.X + instXOff, v2.Y + instYOff, v2.Z + instZOff, closest_x, closest_y, closest_z, sqrDistance);
								// dx, dy, dz
								double currDist_X = g_player.p.x - closest_x;
								double currDist_Y = g_player.p.y - closest_y;
								double currDist_Z = g_player.p.z - closest_z;
								// dxy_abs, dz_abs, dz_sign
								double dxy_abs = sqrt(currDist_X * currDist_X + currDist_Y * currDist_Y);
								double dz_abs = abs(currDist_Z);
								int dz_sign = 0;
								if (currDist_Z < 0) { dz_sign = -1; } // Top
								else if (currDist_Z > 0) { dz_sign = 1; } // Bottom
								// Point in Cylinder
								if (dxy_abs < playerRadius_XY && dz_abs < playerRadius_Z) {
									// (1) Center
									if (perfomXY_Shift && dz_abs < playerRadius_Z_Center) {
										intersectionFound = true;
										double shiftLength_XY = (playerRadius_XY - dxy_abs) + shiftEpsilon;
										if (maxShiftLength_XY < shiftLength_XY) {
											maxShiftLength_XY = shiftLength_XY;
											double dxn = currDist_X / dxy_abs;
											double dyn = currDist_Y / dxy_abs;
											maxShift_dx = dxn * shiftLength_XY;
											maxShift_dy = dyn * shiftLength_XY;
										}
									}
									// (2) Top, Bottom
									else if (!perfomXY_Shift && dz_abs < playerRadius_Z) {
										// player can walk
										intersectionFound = true;
										double shiftLength_Z = (playerRadius_Z - dz_abs) + shiftEpsilon;
										if (maxShiftLength_Z < shiftLength_Z) {
											maxShiftLength_Z = shiftLength_Z;
											double dzn = currDist_Z / dz_abs;
											maxShift_dz = dzn * shiftLength_Z;
										}
										if (dz_sign == 1) {
											// At Bottom
											g_jumpState = ON_GROUND;
											updateLastReferencePoint = true;
										}
										else if(dz_sign == -1 && g_jumpState != GLIDE) {
											// At Top
											g_jumpState = FALL_FINAL;
											updateLastReferencePoint = true;
										}
									}
								}
							}
						}
					}
				}
				if (intersectionFound) {
					// Intersection found
					// Shift Cylinder with the maximum shift
					g_player.p.x += maxShift_dx;
					g_player.p.y += maxShift_dy;
					g_player.p.z += maxShift_dz;
					// Update: Check Subparts
					check_subparts = false;
				}
				else {
					// No Intersection found
					// End loop
					checkForCollision = false;
				}
				// Increase collision Index
				collisionIdx++;
			}
		}
	}
	// End of Collision Correction
	if (updateLastReferencePoint) {
		// Update Last Point on Ground
		g_lastReferencePoint = g_player.p;
		g_inAirFrames = 0;
	}
	else {
		// Update in Air Frames
		g_inAirFrames++;
	}
}

double getplayerContourZ() {
	// Start Point
	double player_start_x = g_player.p.x;
	double player_start_y = g_player.p.y;
	double player_start_z = g_player.p.z;
	double player_curr_x = player_start_x;
	double player_curr_y = player_start_y;
	double player_curr_z = player_start_z;
	// Move Vec
	double moveVec_z = -player_start_z;
	double max_moveVec_length = abs(moveVec_z);
	// Vector subpart for current frame
	int num_subparts = max(((int)ceil(max_moveVec_length / subpartLength)), 1);
	bool check_subparts = true;
	for (int subpart_id = 1; check_subparts && (subpart_id <= num_subparts); subpart_id++) {
		// Set interpolation alpha
		double a_int = ((double)subpart_id) / ((double)num_subparts);
		// Set Player Position
		player_curr_z = player_start_z + a_int * moveVec_z;
		// Loop over all colisions
		int collisionIdx = 0;
		bool checkForCollision = true;
		while (checkForCollision && collisionIdx < maxCollisionChecks) {
			double maxShift_dz = 0;
			double maxShiftLength_Z = shiftEpsilon;
			bool intersectionFound = false;
			// Iterate over all meshes
			for (int meshIndex = 0; meshIndex < 2; meshIndex++) {
				// Get current mesh
				// 0 - Rect Platform
				// 1 - Circle Platform
				Mesh* currMesh = NULL;
				if (meshIndex == 0) { currMesh = &g_rectPlatformMesh; }
				else if (meshIndex == 1) { currMesh = &g_circlePlatformMesh; }
				// Get current number of instances
				// 0 - Rect Platform: numPlatforms Instances
				// 1 - Circle Platform: 1 Instance
				int currNumInstances = 0;
				if (meshIndex == 0) { currNumInstances = g_map.numRectPlatforms; }
				else if (meshIndex == 1) { currNumInstances = 1; }
				// Check all instances
				for (int instanceID = 0; instanceID < currNumInstances; instanceID++) {
					// 0 - Platform: Get platform instance offset
					double instXOff = 0;
					double instYOff = 0;
					double instZOff = 0;
					if (meshIndex == 0) {
						RectPlatform& rp = g_map.allRectPlatforms.at(instanceID);
						instXOff = rp.xPos;
						instYOff = rp.yPos;
						instZOff = rp.z;
					}
					// Check if player is in collision check bounding box
					if (currMesh->use_collision &&
						player_curr_x >= currMesh->coll_min_X + instXOff &&
						player_curr_x <= currMesh->coll_max_X + instXOff &&
						player_curr_y >= currMesh->coll_min_Y + instYOff &&
						player_curr_y <= currMesh->coll_max_Y + instYOff &&
						player_curr_z >= currMesh->coll_min_Z + instZOff &&
						player_curr_z <= currMesh->coll_max_Z + instZOff) {
						// Num Triangles
						int currNumTriangles = currMesh->numTriangles;
						// Check all triangles
						for (int triangleID = 0; triangleID < currNumTriangles; triangleID++) {
							// Get Triangle Indices
							unsigned int index0 = currMesh->indices[triangleID * 3 + 0];
							unsigned int index1 = currMesh->indices[triangleID * 3 + 1];
							unsigned int index2 = currMesh->indices[triangleID * 3 + 2];
							// Get Triangle vertices
							VERTEX v0 = currMesh->vertices[index0];
							VERTEX v1 = currMesh->vertices[index1];
							VERTEX v2 = currMesh->vertices[index2];
							// Query distance
							double closest_x = 0;
							double closest_y = 0;
							double closest_z = 0;
							double sqrDistance = 0;
							getDistancePointToTriangle(player_curr_x, player_curr_y, player_curr_z, v0.X + instXOff, v0.Y + instYOff, v0.Z + instZOff, v1.X + instXOff, v1.Y + instYOff, v1.Z + instZOff, v2.X + instXOff, v2.Y + instYOff, v2.Z + instZOff, closest_x, closest_y, closest_z, sqrDistance);
							// dx, dy, dz
							double currDist_X = player_curr_x - closest_x;
							double currDist_Y = player_curr_y - closest_y;
							double currDist_Z = player_curr_z - closest_z;
							// dxy_abs, dz_abs, dz_sign
							double dxy_abs = sqrt(currDist_X * currDist_X + currDist_Y * currDist_Y);
							double dz_abs = abs(currDist_Z);
							int dz_sign = 0;
							if (currDist_Z < 0) { dz_sign = -1; } // Top
							else if (currDist_Z > 0) { dz_sign = 1; } // Bottom
							// Point in Cylinder
							if (dxy_abs < playerRadius_XY && dz_abs < playerRadius_Z) {
								intersectionFound = true;
								double shiftLength_Z = (playerRadius_Z - dz_abs) + shiftEpsilon;
								if (maxShiftLength_Z < shiftLength_Z) {
									maxShiftLength_Z = shiftLength_Z;
									double dzn = currDist_Z / dz_abs;
									maxShift_dz = dzn * shiftLength_Z;
								}
							}
						}
					}
				}
			}
			if (intersectionFound) {
				// Intersection found
				// Shift Cylinder with the maximum shift
				player_curr_z += maxShift_dz;
				// Update: Check Subparts
				check_subparts = false;
			}
			else {
				// No Intersection found
				// End loop
				checkForCollision = false;
			}
			// Increase collision Index
			collisionIdx++;
		}
	}
	return (player_curr_z - playerRadius_Z);
}

// === Upwind ===

void checkUpwind() {
	g_isInUpwind = false;
	g_isInUpwindTop = false;
	// Check if player is in Upwind
	for (int i = 0; i < g_map.numUpwinds; i++) {
		Upwind& upwind = g_map.allUpwinds.at(i);
		if (upwind.isActive) {
			// Check Distance to Player
			double currDist_X = g_player.p.x - upwind.xPos;
			double currDist_Y = g_player.p.y - upwind.yPos;
			double currDist_Z = g_player.p.z - upwind.zPos;
			double currDist_XY_squared = currDist_X * currDist_X + currDist_Y * currDist_Y;
			if (currDist_XY_squared <= upwind.widthSquared) {
				if (currDist_Z >= 0){
					if (currDist_Z <= upwind.height) {
						// Activate Upwind
						g_isInUpwind = true;
					}
					else if (currDist_Z <= upwind.height + upwindTopRange) {
						// Activate Upwind Top
						g_isInUpwindTop = true;
					}
				}
			}
		}
	}
}

void updateUpwindState() {
	if (g_map.activatedCheckpoints == g_map.numCheckpoints && g_map.collectedGems == g_map.numGems) {
		for (int i = 0; i < g_map.numUpwinds; i++) {
			Upwind& upwind = g_map.allUpwinds.at(i);
			upwind.isActive = true;
		}
	}
}

// === Respawn ===

void updateRespawn() {
	if (g_player.p.z <= respawnThreshold_Z) {
		initGame(RESPAWN);
	}
}

// === Gems ===

void updateGems() {
	// Update Gem Positions
	for (int i = 0; i < g_map.numGems; i++) {
		// Get current gem
		Gem& gem = g_map.allGems.at(i);
		GemInfo& gemInfo = g_map.gemInfos.at(i);
		// Check if gem collected
		if (gem.isCollected == 0) {
			// Activate items in Range
			if (!gemInfo.isMoving) {
				// Check Distance to Player
				double currDist_X = gem.Position.x - g_player.p.x;
				double currDist_Y = gem.Position.y - g_player.p.y;
				double currDist_Z = gem.Position.z - g_player.p.z;
				double currDist_squared = currDist_X * currDist_X + currDist_Y * currDist_Y + currDist_Z * currDist_Z;
				if (currDist_squared <= itemMagnetThresholdSquared) {
					// Activate Movement
					gemInfo.isMoving = true;
				}
			}
			// Move active items to player
			if (gemInfo.isMoving) {
				double ratio = ((double)gemInfo.steps) / g_gemCollectTimeInFrames;
				ratio = ratio * ratio;
				double currX = (1.0 - ratio) * gemInfo.startPosition.x + ratio * g_player.p.x;
				double currY = (1.0 - ratio) * gemInfo.startPosition.y + ratio * g_player.p.y;
				double currZ = (1.0 - ratio) * gemInfo.startPosition.z + ratio * g_player.p.z;
				double currTheta = (1.0 - ratio) * gemThetaAngle;
				// Update Gem Positon, Theta
				gem.Position.x = (float)currX;
				gem.Position.y = (float)currY;
				gem.Position.z = (float)currZ;
				gem.thetaAngle = (float)currTheta;
				// Update steps
				if (gemInfo.steps < g_gemCollectTimeInFrames) { gemInfo.steps++; }
			}
		}
	}
	// Check for Gems to collect
	for (int i = 0; i < g_map.numGems; i++) {
		// Get current Gem
		Gem& gem = g_map.allGems.at(i);
		GemInfo& gemInfo = g_map.gemInfos.at(i);
		// Check if Gem collected
		if (gem.isCollected == 0) {
			// Get distance
			double currDist_X = gem.Position.x - g_player.p.x;
			double currDist_Y = gem.Position.y - g_player.p.y;
			double currDist_Z = gem.Position.z - g_player.p.z;
			double dxy_abs = sqrt(currDist_X * currDist_X + currDist_Y * currDist_Y);
			double dz_abs = abs(currDist_Z);
			// Check if point is in collection radius
			if (dxy_abs <= itemCollectionRadius_XY && dz_abs <= itemCollectionRadius_Z) {
				// Mark gem as collected
				gem.isCollected = 1;
				// Update Collected Gems Value
				g_map.collectedGems++;
				// Update gems render text
				g_gemsRenderText_array[1].updateText(getCollectedGemsString(), whiteColor);
				g_gemsRenderText_array[1].mesh.updateVertexBuffer(g_devcon);
			}
		}
	}
}

std::wstring getCollectedGemsString() {
	return to_wstring(g_map.collectedGems) + L"/" + to_wstring(g_map.numGems);
}

// === Checkpoints ===

void updateCheckpoints() {
	bool updateLightTypeIndex = false;
	for (int i = 0; i < g_map.numCheckpoints; i++) {
		Checkpoint& checkpoint = g_map.allCheckpoints.at(i);
		double currDist_X = checkpoint.Position.x - g_player.p.x;
		double currDist_Y = checkpoint.Position.y - g_player.p.y;
		double currDist_Z = checkpoint.Position.z - g_player.p.z;
		double dxy_abs = sqrt(currDist_X * currDist_X + currDist_Y * currDist_Y);
		double dz_abs = abs(currDist_Z);
		if (dxy_abs <= checkpointActivationRadius_XY && dz_abs <= checkpointActivationRadius_Z) {
			g_map.respawnIndex = i + 1;
			updateLightTypeIndex = true;
			if (checkpoint.isActive == 0) {
				checkpoint.isActive = 1;
				g_map.activatedCheckpoints++;
				g_checkpointsRenderText_array[1].updateText(getActivatedCheckpointsString(), whiteColor);
				g_checkpointsRenderText_array[1].mesh.updateVertexBuffer(g_devcon);
			}
		}
	}
	if (updateLightTypeIndex) {
		for (int i = 0; i < g_map.numCheckpoints; i++) {
			Checkpoint& checkpoint = g_map.allCheckpoints.at(i);
			if (checkpoint.isActive == 1) {
				if (g_map.respawnIndex == i + 1) { checkpoint.lightTypeIdx = checkpointOn_Active_LightIndex; }
				else { checkpoint.lightTypeIdx = checkpointOn_Inactive_LightIndex; }
			}
		}
	}
}

std::wstring getActivatedCheckpointsString() {
	return to_wstring(g_map.activatedCheckpoints) + L"/" + to_wstring(g_map.numCheckpoints);
}

// === Camera ===

void adjustCameraDistance(double at_posX, double at_posY, double at_posZ, double& eye_posX, double& eye_posY, double& eye_posZ) {
	double dirX = at_posX - eye_posX;
	double dirY = at_posY - eye_posY;
	double dirZ = at_posZ - eye_posZ;
	double n = sqrt(dirX * dirX + dirY * dirY + dirZ * dirZ);
	Point3 dir(dirX / n, dirY / n, dirZ / n);
	Point3 eye(eye_posX, eye_posY, eye_posZ);
	Mesh* currMesh = &g_circlePlatformMesh;
	int numTriangles = currMesh->numTriangles;
	double result_t = -1024;
	for (int triangleID = 0; triangleID < numTriangles; triangleID++) {
		// Get Triangle Indices
		unsigned int index0 = currMesh->indices[triangleID * 3 + 0];
		unsigned int index1 = currMesh->indices[triangleID * 3 + 1];
		unsigned int index2 = currMesh->indices[triangleID * 3 + 2];
		// Get Triangle vertices
		VERTEX& v0 = currMesh->vertices[index0];
		VERTEX& v1 = currMesh->vertices[index1];
		VERTEX& v2 = currMesh->vertices[index2];
		// Set Points
		Point3 p0(v0.X, v0.Y, v0.Z);
		Point3 p1(v1.X, v1.Y, v1.Z);
		Point3 p2(v2.X, v2.Y, v2.Z);
		// Check for intersection
		double t = 0;
		if (getRayTriangleIntersection(eye, dir, p0, p1, p2, t)) {
			if (t < cameraDistanceNormal && result_t < t) { result_t = t; }
		}
	}
	if (result_t >= -cameraAddShift) {
		eye_posX = eye_posX + dir.x * (result_t + cameraAddShift);
		eye_posY = eye_posY + dir.y * (result_t + cameraAddShift);
		eye_posZ = eye_posZ + dir.z * (result_t + cameraAddShift);
	}
}

void updateCamera() {
	// Set Camera At
	g_camera.at.x = g_player.p.x;
	g_camera.at.y = g_player.p.y;
	g_camera.at.z = g_player.p.z;
	// Set Camera Eye
	g_camera.eye.x = g_player.p.x + cameraDistanceNormal * sin(2 * DirectX::XM_PI * (g_camera.alpha / 360)) * cos(2 * DirectX::XM_PI * (g_camera.beta / 360));
	g_camera.eye.y = g_player.p.y + cameraDistanceNormal * sin(2 * DirectX::XM_PI * (g_camera.alpha / 360)) * sin(2 * DirectX::XM_PI * (g_camera.beta / 360));
	g_camera.eye.z = g_player.p.z + cameraDistanceNormal * cos(2 * DirectX::XM_PI * (g_camera.alpha / 360));
	adjustCameraDistance(g_camera.at.x, g_camera.at.y, g_camera.at.z, g_camera.eye.x, g_camera.eye.y, g_camera.eye.z);
	// Set Vectors
	XMVECTOR Eye = XMVectorSet((float)(g_camera.eye.x), (float)(g_camera.eye.y), (float)(g_camera.eye.z), 0.0f);
	XMVECTOR At = XMVectorSet((float)(g_camera.at.x), (float)(g_camera.at.y), (float)(g_camera.at.z), 0.0f);
	XMVECTOR Up = XMVectorSet( 0.0f , 0.0f , 1.0f , 0.0f );
	// Set Result
	g_Eye.x = (float)(g_camera.eye.x);
	g_Eye.y = (float)(g_camera.eye.y);
	g_Eye.z = (float)(g_camera.eye.z);
	g_Eye.w = 0.0f;
	g_View = XMMatrixLookAtRH(Eye, At, Up);
}

// === Constant Buffer ===

void updateConstantBufferContent(bool initContent) {
	// Game Status
	g_cb_perFrame.gameStatus = (int)g_gameStatus;
	// is Level Started
	g_cb_perFrame.isLevelStarted = (int)g_isLevelStarted;
	// is Player on Ground
	g_cb_perFrame.isPlayerNotOnGround = (g_jumpState != ON_GROUND);
	// Player Contour Z
	g_cb_perFrame.playerContourZ = (float)getplayerContourZ();
	// Set Player Variables
	g_cb_perFrame.playerX = (float)g_player.p.x;
	g_cb_perFrame.playerY = (float)g_player.p.y;
	g_cb_perFrame.playerZ = (float)(g_player.p.z - playerRadius_Z);
	if (initContent || g_isMoving) {
		g_cb_perFrame.playerAngle = (float)((2 * DirectX::XM_PI * g_camera.beta / 360) + g_player.angleOffset);
	}
	// RUNNING
	if (g_gameStatus == RUNNING) {
		// Update Camera
		updateCamera();
		// Update Shadow Map Camera
		updateShadowMapCamera();
		// Set Camera Matrices
		g_cb_camera.world = XMMatrixTranspose(g_World);
		g_cb_camera.view = XMMatrixTranspose(g_View);
		g_cb_camera.projection = XMMatrixTranspose(g_Projection);
		g_cb_camera.view_shadowmap = XMMatrixTranspose(g_View_shadowmap);
		g_cb_camera.projection_shadowmap = XMMatrixTranspose(g_Projection_shadowmap);
		g_cb_perFrame.eyePosition = g_Eye;
		// Get Global Gem Angle
		float currGemAngle = (float)((((double)g_map.globalGemAngle) / ((double)g_gemRotateTimeInFrames)) * 2 * DirectX::XM_PI);
		g_map.globalGemAngle++;
		if (g_map.globalGemAngle == g_gemRotateTimeInFrames) { g_map.globalGemAngle = 0; }
		// Set Gems Array
		for (int i = 0; i < g_map.numGems; i++) {
			Gem& currGem = g_map.allGems.at(i);
			GemInfo& gemInfo = g_map.gemInfos.at(i);
			currGem.phiAngle = currGemAngle + gemInfo.addAngle;
			g_cb_perFrame.gemsArray[i] = currGem;
		}
		// Set Checkpoints Array
		for (int i = 0; i < g_map.numCheckpoints; i++) {
			Checkpoint& currCheckpoint = g_map.allCheckpoints.at(i);
			g_cb_perFrame.checkpointArray[i] = currCheckpoint;
		}
		// Upwinds
		for (int i = 0; i < g_map.numUpwinds; i++) { g_cb_perFrame.upwindArray[i] = g_map.allUpwinds.at(i); }
	}
	// Update Constant Buffer - Camera
	g_devcon->UpdateSubresource(g_pCBuffer_camera, 0, 0, &g_cb_camera, 0, 0);
	// Update Constant Buffer - Per Frame
	g_devcon->UpdateSubresource(g_pCBuffer_perFrame, 0, 0, &g_cb_perFrame, 0, 0);
	// Init Level Variables
	if (initContent) {
		// Light Types
		g_cb_perLevel.numLightTypes = numLightTypes;
		for (int i = 0; i < numLightTypes; i++) { g_cb_perLevel.lightTypesArray[i] = allLightTypes[i]; }
		// Lamps
		g_cb_perLevel.numLamps = g_map.numLamps;
		for (int i = 0; i < g_map.numLamps; i++) { g_cb_perLevel.lampsArray[i] = g_map.allLamps.at(i); }
		// Gems
		g_cb_perLevel.numGems = g_map.numGems;
		// Checkpoints
		g_cb_perLevel.numCheckpoints = g_map.numCheckpoints;
		// Upwinds
		g_cb_perLevel.numUpwinds = g_map.numUpwinds;
		// Platforms
		g_cb_perLevel.numRectPlatforms = g_map.numRectPlatforms;
		for (int i = 0; i < g_map.numRectPlatforms; i++) { g_cb_perLevel.rectPlatformArray[i] = g_map.allRectPlatforms.at(i); }
		// Textboxes
		g_cb_perLevel.numTextboxes = num_textboxes;
		for (int i = 0; i < num_textboxes; i++) { g_cb_perLevel.textboxArray[i] = g_textbox_array[i]; }
		// Materials
		g_cb_perLevel.materialStatic = materialStatic;
		g_cb_perLevel.materialPlatform = materialPlatform;
		g_cb_perLevel.materialPlayer = materialPlayer;
		// Shadow Variables
		g_cb_perLevel.playerRadius = (float)playerRadius_XY;
		// Aspect Ratio
		g_cb_perLevel.aspectRatio = (float)((double)g_windowWidthInPixel / (double)g_windowHeightInPixel);
		// Padding
		g_cb_perLevel.padding_000 = 0;
		g_cb_perLevel.padding_001 = 0;
		g_cb_perLevel.padding_002 = 0;
		// Update Constant Buffer - Per Level
		g_devcon->UpdateSubresource(g_pCBuffer_perLevel, 0, 0, &g_cb_perLevel, 0, 0);
	}
}

// === Game Status Screens ===

void showStartMenuScreen() {
	updateConstantBufferContent(false);
	renderFrameStartMenu();
}

void showManualScreen() {
	updateConstantBufferContent(false);
	if (g_isLevelStarted) { renderFrameRunning(); }
	renderFrameManual();
	renderFrameBoxlayer();
}

void showCreditsScreen() {
	updateConstantBufferContent(false);
	renderFrameCredits();
	renderFrameBoxlayer();
}

void showPausedScreen() {
	updateConstantBufferContent(false);
	renderFrameRunning();
	renderFramePaused();
	renderFrameBoxlayer();
}

void showRunningScreen() {
	updatePlayerPositon();
	updateRespawn();
	updateGems();
	updateCheckpoints();
	updateUpwindState();
	updateConstantBufferContent(false);
	renderFrameRunning();
	renderFrameBoxlayer();
}

// === Shadow Map ===

void updateShadowMapCamera() {
	// Set Vectors
	XMVECTOR Eye = XMVectorSet((float)(0.0f), (float)(0.0f), (float)(10.0f), 0.0f);
	XMVECTOR At = XMVectorSet((float)(0.0f), (float)(0.0f), (float)(0.0f), 0.0f);
	XMVECTOR Up = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
	// Set Result
	g_View_shadowmap = XMMatrixLookAtRH(Eye, At, Up);
}

void computeShadowMap() {
	// Set Shadow Map Viewport
	setShadowMapViewport();
	// Set Shadow Map RenderTarget
	setShadowMapRenderTarget();
	// Set Camera Matrices
	setCameraMatricesForShadowMap();
	// Clear Shadow Map Frame
	clearShadowMapFrame();
	// Stride, Offset
	UINT stride = sizeof(VERTEX);
	UINT offset = 0;
	// Draw Rect Platforms
	g_devcon->VSSetShader(g_pVS_PLATFORM, 0, 0);
	g_devcon->PSSetShader(g_pPS_PLATFORM, 0, 0);
	g_devcon->IASetVertexBuffers(0, 1, &g_rectPlatformMesh.pVBuffer, &stride, &offset);
	g_devcon->IASetIndexBuffer(g_rectPlatformMesh.pIBuffer, DXGI_FORMAT_R32_UINT, 0);
	g_devcon->DrawIndexedInstanced(g_rectPlatformMesh.numIndices, g_map.numRectPlatforms, 0, 0, 0);
	// Draw Player
	g_devcon->VSSetShader(g_pVS_PLAYER, 0, 0);
	g_devcon->PSSetShader(g_pPS_PLAYER, 0, 0);
	g_devcon->IASetVertexBuffers(0, 1, &g_playerMesh.pVBuffer, &stride, &offset);
	g_devcon->IASetIndexBuffer(g_playerMesh.pIBuffer, DXGI_FORMAT_R32_UINT, 0);
	g_devcon->DrawIndexed(g_playerMesh.numIndices, 0, 0);
	// Set Camera Viewport
	setCameraViewport();
	// Set Camera RenderTarget
	setCameraRenderTarget();
	// Set Camera Matrices
	setCameraMatricesForPlayerCamera();
	// Set Updated Resource
	g_devcon->PSSetShaderResources(5, 1, &g_depthMapSRV);
}

// === Render Target ===

void setCameraRenderTarget() {
	g_devcon->OMSetRenderTargets(1, &g_backbuffer, g_pDSView);
}

void setShadowMapRenderTarget() {
	ID3D11RenderTargetView* renderTargets[1] = {0};
	g_devcon->OMSetRenderTargets(1, renderTargets, g_depthMapDSV);
}

// === Viewport ===

void setCameraViewport() {
	g_devcon->RSSetViewports(1, &g_camera_viewport);
}

void setShadowMapViewport() {
	g_devcon->RSSetViewports(1, &g_shadowmap_viewport);
}

// === Camera Matrices ===

void setCameraMatricesForShadowMap() {
	g_cb_camera.world = XMMatrixTranspose(g_World);
	g_cb_camera.view = XMMatrixTranspose(g_View_shadowmap);
	g_cb_camera.projection = XMMatrixTranspose(g_Projection_shadowmap);
	g_devcon->UpdateSubresource(g_pCBuffer_camera, 0, 0, &g_cb_camera, 0, 0);
}

void setCameraMatricesForPlayerCamera() {
	g_cb_camera.world = XMMatrixTranspose(g_World);
	g_cb_camera.view = XMMatrixTranspose(g_View);
	g_cb_camera.projection = XMMatrixTranspose(g_Projection);
	g_devcon->UpdateSubresource(g_pCBuffer_camera, 0, 0, &g_cb_camera, 0, 0);
}

// === Clear Frame ===

void clearCameraFrame() {
	g_devcon->ClearRenderTargetView(g_backbuffer, blackColor_float);
	g_devcon->ClearDepthStencilView(g_pDSView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void clearShadowMapFrame() {
	g_devcon->ClearDepthStencilView(g_depthMapDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

// === Present Frame ===

void presentFrame() {
	g_swapchain->Present(1, 0);
}

// === Render Frame ===

void renderFrameStartMenu() {
	// Stride, Offset
	UINT stride = sizeof(VERTEX);
	UINT offset = 0;
	// Draw Texts
	g_devcon->VSSetShader(g_pVS_TEXT, 0, 0);
	g_devcon->PSSetShader(g_pPS_TEXT, 0, 0);
	for (int i = 0; i <= 3; i++) {
		g_devcon->IASetVertexBuffers(0, 1, &g_startMenuScreenText_array[i].mesh.pVBuffer, &stride, &offset);
		g_devcon->IASetIndexBuffer(g_startMenuScreenText_array[i].mesh.pIBuffer, DXGI_FORMAT_R32_UINT, 0);
		g_devcon->DrawIndexed(g_startMenuScreenText_array[i].currNumIndices, 0, 0);
	}
	if (g_isGamepadConnected) {
		for (int i = 8; i <= 11; i++) {
			g_devcon->IASetVertexBuffers(0, 1, &g_startMenuScreenText_array[i].mesh.pVBuffer, &stride, &offset);
			g_devcon->IASetIndexBuffer(g_startMenuScreenText_array[i].mesh.pIBuffer, DXGI_FORMAT_R32_UINT, 0);
			g_devcon->DrawIndexed(g_startMenuScreenText_array[i].currNumIndices, 0, 0);
		}
	}
	else {
		for (int i = 4; i <= 7; i++) {
			g_devcon->IASetVertexBuffers(0, 1, &g_startMenuScreenText_array[i].mesh.pVBuffer, &stride, &offset);
			g_devcon->IASetIndexBuffer(g_startMenuScreenText_array[i].mesh.pIBuffer, DXGI_FORMAT_R32_UINT, 0);
			g_devcon->DrawIndexed(g_startMenuScreenText_array[i].currNumIndices, 0, 0);
		}
	}
}

void renderFrameManual() {
	// Stride, Offset
	UINT stride = sizeof(VERTEX);
	UINT offset = 0;
	if (g_isLevelStarted){
		// Blend State Alpha To Coverage
		g_devcon->OMSetBlendState(g_pBlendState_alphaToCoverage, 0, 0xFFFFFFFF);
	}
	// Draw Texts
	g_devcon->VSSetShader(g_pVS_TEXT, 0, 0);
	g_devcon->PSSetShader(g_pPS_TEXT, 0, 0);
	for (int i = 0; i < num_manualScreenText_parts; i++) {
		if (g_isGamepadConnected && i == 0) { continue; }
		else if (!g_isGamepadConnected && i == 1) { continue; }
		g_devcon->IASetVertexBuffers(0, 1, &g_manualScreenText_array[i].mesh.pVBuffer, &stride, &offset);
		g_devcon->IASetIndexBuffer(g_manualScreenText_array[i].mesh.pIBuffer, DXGI_FORMAT_R32_UINT, 0);
		g_devcon->DrawIndexed(g_manualScreenText_array[i].currNumIndices, 0, 0);
	}
	if (g_isLevelStarted) {
		// Blend State Default
		g_devcon->OMSetBlendState(g_pBlendState_default, 0, 0xFFFFFFFF);
	}
}

void renderFrameCredits() {
	// Stride, Offset
	UINT stride = sizeof(VERTEX);
	UINT offset = 0;
	// Draw Texts
	g_devcon->VSSetShader(g_pVS_TEXT, 0, 0);
	g_devcon->PSSetShader(g_pPS_TEXT, 0, 0);
	for (int i = 0; i <= 4; i++) {
		g_devcon->IASetVertexBuffers(0, 1, &g_creditsScreenText_array[i].mesh.pVBuffer, &stride, &offset);
		g_devcon->IASetIndexBuffer(g_creditsScreenText_array[i].mesh.pIBuffer, DXGI_FORMAT_R32_UINT, 0);
		g_devcon->DrawIndexed(g_creditsScreenText_array[i].currNumIndices, 0, 0);
	}
	if (g_isGamepadConnected) {
		g_devcon->IASetVertexBuffers(0, 1, &g_creditsScreenText_array[6].mesh.pVBuffer, &stride, &offset);
		g_devcon->IASetIndexBuffer(g_creditsScreenText_array[6].mesh.pIBuffer, DXGI_FORMAT_R32_UINT, 0);
		g_devcon->DrawIndexed(g_creditsScreenText_array[6].currNumIndices, 0, 0);
	}
	else {
		g_devcon->IASetVertexBuffers(0, 1, &g_creditsScreenText_array[5].mesh.pVBuffer, &stride, &offset);
		g_devcon->IASetIndexBuffer(g_creditsScreenText_array[5].mesh.pIBuffer, DXGI_FORMAT_R32_UINT, 0);
		g_devcon->DrawIndexed(g_creditsScreenText_array[5].currNumIndices, 0, 0);
	}
}

void renderFramePaused() {
	// Stride, Offset
	UINT stride = sizeof(VERTEX);
	UINT offset = 0;
	// Blend State Alpha To Coverage
	g_devcon->OMSetBlendState(g_pBlendState_alphaToCoverage, 0, 0xFFFFFFFF);
	// Draw Texts
	g_devcon->VSSetShader(g_pVS_TEXT, 0, 0);
	g_devcon->PSSetShader(g_pPS_TEXT, 0, 0);
	for (int i = 0; i <= 2; i++) {
		g_devcon->IASetVertexBuffers(0, 1, &g_pausedScreenText_array[i].mesh.pVBuffer, &stride, &offset);
		g_devcon->IASetIndexBuffer(g_pausedScreenText_array[i].mesh.pIBuffer, DXGI_FORMAT_R32_UINT, 0);
		g_devcon->DrawIndexed(g_pausedScreenText_array[i].currNumIndices, 0, 0);
	}
	if (g_isGamepadConnected) {
		for (int i = 6; i <= 8; i++) {
			g_devcon->IASetVertexBuffers(0, 1, &g_pausedScreenText_array[i].mesh.pVBuffer, &stride, &offset);
			g_devcon->IASetIndexBuffer(g_pausedScreenText_array[i].mesh.pIBuffer, DXGI_FORMAT_R32_UINT, 0);
			g_devcon->DrawIndexed(g_pausedScreenText_array[i].currNumIndices, 0, 0);
		}
	}
	else {
		for (int i = 3; i <= 5; i++) {
			g_devcon->IASetVertexBuffers(0, 1, &g_pausedScreenText_array[i].mesh.pVBuffer, &stride, &offset);
			g_devcon->IASetIndexBuffer(g_pausedScreenText_array[i].mesh.pIBuffer, DXGI_FORMAT_R32_UINT, 0);
			g_devcon->DrawIndexed(g_pausedScreenText_array[i].currNumIndices, 0, 0);
		}
	}
	// Blend State Default
	g_devcon->OMSetBlendState(g_pBlendState_default, 0, 0xFFFFFFFF);
}

void renderFrameRunning() {
	// Compute Shadow Map
	computeShadowMap();
	// Stride, Offset
	UINT stride = sizeof(VERTEX);
	UINT offset = 0;
	// Draw Sky
	g_devcon->VSSetShader(g_pVS_SKY, 0, 0);
	g_devcon->PSSetShader(g_pPS_SKY, 0, 0);
	g_devcon->IASetVertexBuffers(0, 1, &g_skyMesh.pVBuffer, &stride, &offset);
	g_devcon->IASetIndexBuffer(g_skyMesh.pIBuffer, DXGI_FORMAT_R32_UINT, 0);
	g_devcon->DrawIndexed(g_skyMesh.numIndices, 0, 0);
	// Draw Circle Platforms
	g_devcon->VSSetShader(g_pVS_STATIC, 0, 0);
	g_devcon->PSSetShader(g_pPS_STATIC, 0, 0);
	g_devcon->IASetVertexBuffers(0, 1, &g_circlePlatformMesh.pVBuffer, &stride, &offset);
	g_devcon->IASetIndexBuffer(g_circlePlatformMesh.pIBuffer, DXGI_FORMAT_R32_UINT, 0);
	g_devcon->DrawIndexed(g_circlePlatformMesh.numIndices, 0, 0);
	// Draw Rect Platforms
	g_devcon->VSSetShader(g_pVS_PLATFORM, 0, 0);
	g_devcon->PSSetShader(g_pPS_PLATFORM, 0, 0);
	g_devcon->IASetVertexBuffers(0, 1, &g_rectPlatformMesh.pVBuffer, &stride, &offset);
	g_devcon->IASetIndexBuffer(g_rectPlatformMesh.pIBuffer, DXGI_FORMAT_R32_UINT, 0);
	g_devcon->DrawIndexedInstanced(g_rectPlatformMesh.numIndices, g_map.numRectPlatforms, 0, 0, 0);
	// Draw Player
	g_devcon->VSSetShader(g_pVS_PLAYER, 0, 0);
	g_devcon->PSSetShader(g_pPS_PLAYER, 0, 0);
	g_devcon->IASetVertexBuffers(0, 1, &g_playerMesh.pVBuffer, &stride, &offset);
	g_devcon->IASetIndexBuffer(g_playerMesh.pIBuffer, DXGI_FORMAT_R32_UINT, 0);
	g_devcon->DrawIndexed(g_playerMesh.numIndices, 0, 0);
	// Draw Gems
	g_devcon->VSSetShader(g_pVS_GEM, 0, 0);
	g_devcon->PSSetShader(g_pPS_GEM, 0, 0);
	g_devcon->IASetVertexBuffers(0, 1, &g_gemMesh.pVBuffer, &stride, &offset);
	g_devcon->IASetIndexBuffer(g_gemMesh.pIBuffer, DXGI_FORMAT_R32_UINT, 0);
	g_devcon->DrawIndexedInstanced(g_gemMesh.numIndices, g_map.numGems, 0, 0, 0);
	// Draw Checkpoints
	g_devcon->VSSetShader(g_pVS_CHECKPOINT, 0, 0);
	g_devcon->PSSetShader(g_pPS_CHECKPOINT, 0, 0);
	g_devcon->IASetVertexBuffers(0, 1, &g_checkpointMesh.pVBuffer, &stride, &offset);
	g_devcon->IASetIndexBuffer(g_checkpointMesh.pIBuffer, DXGI_FORMAT_R32_UINT, 0);
	g_devcon->DrawIndexedInstanced(g_checkpointMesh.numIndices, g_map.numCheckpoints, 0, 0, 0);
	// Draw Upwinds
	g_devcon->VSSetShader(g_pVS_UPWIND, 0, 0);
	g_devcon->PSSetShader(g_pPS_UPWIND, 0, 0);
	g_devcon->IASetVertexBuffers(0, 1, &g_upwindMesh.pVBuffer, &stride, &offset); // TODO
	g_devcon->IASetIndexBuffer(g_upwindMesh.pIBuffer, DXGI_FORMAT_R32_UINT, 0); // TODO
	g_devcon->DrawIndexedInstanced(g_upwindMesh.numIndices, g_map.numCheckpoints, 0, 0, 0); // TODO
	// Draw Lamps
	g_devcon->VSSetShader(g_pVS_LAMP, 0, 0);
	g_devcon->PSSetShader(g_pPS_LAMP, 0, 0);
	g_devcon->IASetVertexBuffers(0, 1, &g_lampMesh.pVBuffer, &stride, &offset);
	g_devcon->IASetIndexBuffer(g_lampMesh.pIBuffer, DXGI_FORMAT_R32_UINT, 0);
	g_devcon->DrawIndexedInstanced(g_lampMesh.numIndices, g_map.numLamps, 0, 0, 0);
	// Blend State Alpha To Coverage
	g_devcon->OMSetBlendState(g_pBlendState_alphaToCoverage, 0, 0xFFFFFFFF);
	// Draw Texts
	g_devcon->VSSetShader(g_pVS_TEXT, 0, 0);
	g_devcon->PSSetShader(g_pPS_TEXT, 0, 0);
	for (int i = 0; i <= 1; i++) {
		g_devcon->IASetVertexBuffers(0, 1, &g_gemsRenderText_array[i].mesh.pVBuffer, &stride, &offset);
		g_devcon->IASetIndexBuffer(g_gemsRenderText_array[i].mesh.pIBuffer, DXGI_FORMAT_R32_UINT, 0);
		g_devcon->DrawIndexed(g_gemsRenderText_array[i].currNumIndices, 0, 0);
	}
	for (int i = 0; i <= 1; i++) {
		g_devcon->IASetVertexBuffers(0, 1, &g_checkpointsRenderText_array[i].mesh.pVBuffer, &stride, &offset);
		g_devcon->IASetIndexBuffer(g_checkpointsRenderText_array[i].mesh.pIBuffer, DXGI_FORMAT_R32_UINT, 0);
		g_devcon->DrawIndexed(g_checkpointsRenderText_array[i].currNumIndices, 0, 0);
	}
	// Blend State Default
	g_devcon->OMSetBlendState(g_pBlendState_default, 0, 0xFFFFFFFF);
}

void renderFrameBoxlayer() {
	// Stride, Offset
	UINT stride = sizeof(VERTEX);
	UINT offset = 0;
	// Draw Boxlayer
	g_devcon->VSSetShader(g_pVS_BOXLAYER, 0, 0);
	g_devcon->PSSetShader(g_pPS_BOXLAYER, 0, 0);
	g_devcon->IASetVertexBuffers(0, 1, &g_boxlayerMesh.pVBuffer, &stride, &offset);
	g_devcon->IASetIndexBuffer(g_boxlayerMesh.pIBuffer, DXGI_FORMAT_R32_UINT, 0);
	g_devcon->DrawIndexed(g_boxlayerMesh.numIndices, 0, 0);
}

// === Main ===

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	// Init
	if (!initSystem(hInstance, nCmdShow)) { return EXIT_FAILURE; }
	initGame(FIRST_INIT);
	// Main loop
	MSG Msg;
	while (true) {
		// Process Messages
		while (PeekMessage(&Msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&Msg);
			DispatchMessage(&Msg);
			if (Msg.message == WM_QUIT) { break; }
		}
		if (Msg.message == WM_QUIT) { break; }
		// Process Input
		processInput();
		// Check isRunning
		if (!g_isRunning) {
			DestroyWindow(g_hwnd);
			continue;
		}
		// Build Frame
		clearCameraFrame();
		if (g_gameStatus == START_MENU) { showStartMenuScreen(); }
		else if (g_gameStatus == MANUAL) { showManualScreen(); }
		else if (g_gameStatus == CREDITS) { showCreditsScreen(); }
		else if (g_gameStatus == PAUSED) { showPausedScreen(); }
		else if (g_gameStatus == RUNNING) { showRunningScreen(); }
		presentFrame();
	}
	// Close
	closeSystem();
	return (int)Msg.wParam;
}