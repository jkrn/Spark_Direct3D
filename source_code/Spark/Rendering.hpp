#pragma once

struct RectPlatform {
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
	RectPlatform(float xPos, float yPos, float z) :
		xPos(xPos),
		yPos(yPos),
		dx2(platform_dx2),
		dy2(platform_dy2),
		z(z),
		padding_000(0),
		padding_001(0),
		padding_002(0)
	{}
	RectPlatform() { ZeroMemory(this, sizeof(this)); }
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
	uint32_t isActive; // 4 byte
	uint32_t padding_000; // 4 byte
	// ------------------------
	Upwind(float xPos, float yPos, float zPos, float width, float height, bool isActive) :
		xPos(xPos),
		yPos(yPos),
		zPos(zPos),
		width(width),
		widthSquared(width * width),
		height(height),
		isActive(isActive)
	{}
	Upwind() { ZeroMemory(this, sizeof(this)); }
};

struct Gem {
	// ------------------------
	DirectX::XMFLOAT3 Position; // 12 byte
	float spinDirection; // 4 byte
	// ------------------------
	float phiAngle; // 4 byte
	float thetaAngle; // 4 byte
	uint32_t lightTypeIdx; // 4 byte
	uint32_t isCollected; // 4 byte
	// ------------------------
	Gem(float x, float y, float z, uint32_t lightTypeIdxValue)
	{
		Position.x = x;
		Position.y = y;
		Position.z = z + GEM_Z_OFFSET;
		spinDirection = 0.0f;
		phiAngle = 0.0f;
		thetaAngle = gemThetaAngle;
		lightTypeIdx = lightTypeIdxValue;
		isCollected = 0;
	}
	Gem() { ZeroMemory(this, sizeof(this)); }
};

struct Checkpoint {
	// ------------------------
	DirectX::XMFLOAT3 Position; // 12 byte
	uint32_t lightTypeIdx; // 4 byte
	// ------------------------
	uint32_t isActive; // 4 byte
	uint32_t padding_000; // 4 byte
	uint32_t padding_001; // 4 byte
	uint32_t padding_002; // 4 byte
	// ------------------------
	Checkpoint(float x, float y, float z, uint32_t lightTypeIdxValue)
	{
		Position.x = x;
		Position.y = y;
		Position.z = z;
		lightTypeIdx = lightTypeIdxValue;
		isActive = 0;
		padding_000 = 0;
		padding_001 = 0;
		padding_002 = 0;
	}
	Checkpoint() { ZeroMemory(this, sizeof(this)); }
};

struct Lamp {
	// ------------------------
	DirectX::XMFLOAT3 Position; // 12 byte
	uint32_t lightTypeIdx; // 4 byte
	// ------------------------
	Lamp( float positionX, float positionY, float positionZ,
		uint32_t lightTypeIdxValue)
	{
		Position.x = positionX;
		Position.y = positionY;
		Position.z = positionZ;
		lightTypeIdx = lightTypeIdxValue;
	}
	Lamp() { ZeroMemory(this, sizeof(this)); }
};

struct Textbox {
	// ------------------------
	float xPos; // 4 byte
	float yPos; // 4 byte
	float dx2; // 4 byte
	float dy2; // 4 byte
	// ------------------------
	Textbox() : xPos(0), yPos(0), dx2(0), dy2(0) {}
	void init(double p_normX, double p_normY, double p_x1, double p_y1, double p_w, double p_h){
		double cx = p_x1 + (p_w / 2);
		double cy = p_y1 + (p_h / 2);
		xPos = (float)( (cx / p_normX) * 2.0 - 1.0);
		yPos = (float)(-(cy / p_normY) * 2.0 + 1.0);
		dx2 = (float)(p_w / p_normX);
		dy2 = (float)(p_h / p_normY);
	}
};

struct ConstantBufferCamera
{
	DirectX::XMMATRIX world; // 4 * 16 byte
	DirectX::XMMATRIX view; // 4 * 16 byte
	DirectX::XMMATRIX projection; // 4 * 16 byte
	DirectX::XMMATRIX view_shadowmap; // 4 * 16 byte
	DirectX::XMMATRIX projection_shadowmap; // 4 * 16 byte
};

struct ConstantBufferPerFrame
{
	// ------------------------
	Gem gemsArray[NUM_MAX_GEMS]; // 2 * NUM_MAX_GEMS * 16 byte
	Checkpoint checkpointArray[NUM_MAX_CHECKPOINTS]; // 2 * NUM_MAX_CHECKPOINTS * 16 byte
	Upwind upwindArray[NUM_MAX_UPWINDS]; // 2 * NUM_MAX_UPWINDS * 16 byte
	// ------------------------
	DirectX::XMFLOAT4 eyePosition; // 16 byte
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
};

struct ConstantBufferPerLevel
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
	uint32_t padding_000; // 4 byte
	uint32_t padding_001; // 4 byte
	uint32_t padding_002; // 4 byte
	// ------------------------
};

class Mesh {
public:
	unsigned int numVertices = 0;
	unsigned int numIndices = 0;
	unsigned int numTriangles = 0;
	bool use_collision = false;
	double coll_min_X = 0.0;
	double coll_max_X = 0.0;
	double coll_min_Y = 0.0;
	double coll_max_Y = 0.0;
	double coll_min_Z = 0.0;
	double coll_max_Z = 0.0;
	VERTEX* vertices = NULL;
	unsigned int* indices = NULL;
	ID3D11Buffer* pVBuffer = NULL;
	ID3D11Buffer* pIBuffer = NULL;
	Mesh() {}
	~Mesh() {
		if (vertices != NULL) { delete[] vertices; vertices = NULL; }
		if (indices != NULL) { delete[] indices; indices = NULL; }
		if (pVBuffer != NULL) { pVBuffer->Release(); pVBuffer = NULL; }
		if (pIBuffer != NULL) { pIBuffer->Release(); pIBuffer = NULL; }
	}
	void updateVertices(VERTEX* vertexValues) {
		for (unsigned int i = 0; i < numVertices; i++) { vertices[i] = vertexValues[i]; }
	}
	void updateIndices(unsigned int* indexValues) {
		for (unsigned int i = 0; i < numIndices; i++) { indices[i] = indexValues[i]; }
	}
	void setContentFromHeapMemory(unsigned int numVertices, unsigned int numIndices, VERTEX* vertexValues, unsigned int* indexValues, bool enableCollision) {
		this->numVertices = numVertices;
		this->numIndices = numIndices;
		this->numTriangles = numIndices / 3;
		vertices = vertexValues;
		indices = indexValues;
		if (enableCollision) { setCollisionBoundaries(); }
	}
	bool updateVertexBuffer(ID3D11DeviceContext* devcon){
		D3D11_MAPPED_SUBRESOURCE ms;
		ZeroMemory(&ms, sizeof(D3D11_MAPPED_SUBRESOURCE));
		if (FAILED(devcon->Map(pVBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &ms))) { return false; }
		memcpy(ms.pData, vertices, sizeof(VERTEX) * numVertices);
		devcon->Unmap(pVBuffer, 0);
		return true;
	}
	bool initVertexBuffer(ID3D11Device* dev, ID3D11DeviceContext* devcon) {
		D3D11_BUFFER_DESC vbd = {};
		ZeroMemory(&vbd, sizeof(vbd));
		vbd.Usage = D3D11_USAGE_DYNAMIC;
		vbd.ByteWidth = sizeof(VERTEX) * numVertices;
		vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		vbd.MiscFlags = 0;
		vbd.StructureByteStride = 0;
		if (FAILED(dev->CreateBuffer(&vbd, NULL, &pVBuffer))) { return false; }
		if (pVBuffer == NULL) { return false; }
		if (!updateVertexBuffer(devcon)) { return false; }
		return true;
	}
	bool updateIndexBuffer(ID3D11DeviceContext* devcon) {
		D3D11_MAPPED_SUBRESOURCE ms;
		ZeroMemory(&ms, sizeof(D3D11_MAPPED_SUBRESOURCE));
		if (FAILED(devcon->Map(pIBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &ms))) { return false; }
		memcpy(ms.pData, indices, sizeof(unsigned int) * numIndices);
		devcon->Unmap(pIBuffer, 0);
		return true;
	}
	bool initIndexBuffer(ID3D11Device* dev, ID3D11DeviceContext* devcon) {
		D3D11_BUFFER_DESC ibd = {};
		ZeroMemory(&ibd, sizeof(ibd));
		ibd.Usage = D3D11_USAGE_DYNAMIC;
		ibd.ByteWidth = sizeof(unsigned int) * numIndices;
		ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		ibd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		ibd.MiscFlags = 0;
		ibd.StructureByteStride = 0;
		if (FAILED(dev->CreateBuffer(&ibd, NULL, &pIBuffer))) { return false; }
		if (pIBuffer == NULL) { return false; }
		if (!updateIndexBuffer(devcon)) { return false; }
		return true;
	}
	bool initBuffers(ID3D11Device* dev, ID3D11DeviceContext* devcon) {
		if (!initVertexBuffer(dev, devcon)) { return false; }
		if (!initIndexBuffer(dev, devcon)) { return false; }
		return true;
	}
	void setCollisionBoundaries() {
		coll_min_X = vertices[0].X;
		coll_max_X = vertices[0].X;
		coll_min_Y = vertices[0].Y;
		coll_max_Y = vertices[0].Y;
		coll_min_Z = vertices[0].Z;
		coll_max_Z = vertices[0].Z;
		for (unsigned int i = 1; i < numVertices; i++) {
			coll_min_X = min(coll_min_X, vertices[i].X);
			coll_min_Y = min(coll_min_Y, vertices[i].Y);
			coll_min_Z = min(coll_min_Z, vertices[i].Z);
			coll_max_X = max(coll_max_X, vertices[i].X);
			coll_max_Y = max(coll_max_Y, vertices[i].Y);
			coll_max_Z = max(coll_max_Z, vertices[i].Z);
		}
		coll_min_X -= collisionCheckBufferX;
		coll_max_X += collisionCheckBufferX;
		coll_min_Y -= collisionCheckBufferY;
		coll_max_Y += collisionCheckBufferY;
		coll_min_Z -= collisionCheckBufferZ;
		coll_max_Z += collisionCheckBufferZ;
		use_collision = true;
	}
};

class RenderText {
public:
	Mesh mesh;
	double normX = 0;
	double normY = 0;
	int posX = 0;
	int posY = 0;
	int maxLength = 0;
	int currLength = 0;
	int currNumIndices = 0;
	RenderText() {}
	void init(double normX, double normY, int posX, int posY, int maxLength, std::wstring text, COLOR color) {
		init(normX, normY, posX, posY, maxLength);
		updateText(text, color);
	}
	void init(double normX, double normY, int posX, int posY, std::wstring text, COLOR color) {
		init(normX, normY, posX, posY, (int)text.length(), text, color);
	}
	void init(double p_normX, double p_normY, int p_posX, int p_posY, int p_maxLength) {
		// Set Variables
		normX = p_normX;
		normY = p_normY;
		posX = p_posX;
		posY = p_posY;
		maxLength = p_maxLength;
		currLength = 0;
		currNumIndices = 0;
		// Init data storage
		unsigned int numVert = 4 * maxLength;
		unsigned int numInd = 6 * maxLength;
		VERTEX* vertArray = new VERTEX[numVert];
		ZeroMemory(vertArray, sizeof(VERTEX) * (numVert));
		unsigned int* indArray = new unsigned int[numInd];
		// Set every char
		for (int i = 0; i < maxLength; i++) {
			// Index data
			indArray[0 + i * 6] = 0 + i * 4;
			indArray[1 + i * 6] = 1 + i * 4;
			indArray[2 + i * 6] = 3 + i * 4;
			indArray[3 + i * 6] = 1 + i * 4;
			indArray[4 + i * 6] = 2 + i * 4;
			indArray[5 + i * 6] = 3 + i * 4;
		}
		// Set Content
		mesh.setContentFromHeapMemory(numVert, numInd, vertArray, indArray, false);
	}
	void updateText(std::wstring text, COLOR color) {
		// Get Length
		currLength = (int)text.length();
		currNumIndices = currLength * 6;
		// Init data storage
		VERTEX vertArray[4 * numMaxCharsPerText] = {};
		unsigned int indArray[6 * numMaxCharsPerText] = {};
		// Set every char
		int currPositionX = 0;
		for (int i = 0; i < currLength; i++) {
			// Set coordinates
			double minX = ((posX + currPositionX) * 2) / (normX);
			double minY = ((posY) * 2) / (normY);
			uint16_t currIndex = ( ((uint16_t)text.at(i)) - startChar);
			int currOffset = atlasOffsetX[currIndex];
			int widthX = atlasAdvance[currIndex];
			int heightY = atlasHeight;
			double u1 = (currOffset) / ((double)atlasWidth);
			double u2 = (currOffset + widthX) / ((double)atlasWidth);
			double maxX = minX + ((widthX * 2) / (normX));
			double maxY = minY + ((heightY * 2) / (normY));
			// Vertex data
			vertArray[0 + i * 4] = { (float)(minX - 1), (float)(1 - minY), 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 0, color, (float)u1, 0 };
			vertArray[1 + i * 4] = { (float)(maxX - 1), (float)(1 - minY), 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 0, color, (float)u2, 0 };
			vertArray[2 + i * 4] = { (float)(maxX - 1), (float)(1 - maxY), 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 0, color, (float)u2, 1 };
			vertArray[3 + i * 4] = { (float)(minX - 1), (float)(1 - maxY), 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 0, color, (float)u1, 1 };
			// Index data
			indArray[0 + i * 6] = 0 + i * 4;
			indArray[1 + i * 6] = 1 + i * 4;
			indArray[2 + i * 6] = 3 + i * 4;
			indArray[3 + i * 6] = 1 + i * 4;
			indArray[4 + i * 6] = 2 + i * 4;
			indArray[5 + i * 6] = 3 + i * 4;
			// Update
			currPositionX += widthX;
		}
		// Update Content
		mesh.updateVertices(vertArray);
	}
	~RenderText() {}
};