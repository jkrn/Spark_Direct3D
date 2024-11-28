#pragma once

class Map {
public:
	// Lamps
	int numLamps = 0;
	std::vector<Lamp> allLamps;
	// Checkpoints
	int numCheckpoints = 0;
	int activatedCheckpoints = 0;
	std::vector<Checkpoint> allCheckpoints;
	// Rect Platforms
	int numRectPlatforms = 0;
	std::vector<RectPlatform> allRectPlatforms;
	// Upwinds
	int numUpwinds = 0;
	std::vector<Upwind> allUpwinds;
	// Gems
	int numGems = 0;
	int collectedGems = 0;
	int globalGemAngle = 0;
	std::vector<Gem> allGems;
	std::vector<GemInfo> gemInfos;
	// Respawn
	std::vector<RespawnInfo> respawnInfos;
	int respawnIndex = 0;
	Map() {
		setContent();
		setNumVariables();
		initGemInfos();
		initRespawnInfos();
	}
	void setContent(){
		// Start Position
		respawnInfos.push_back(RespawnInfo(0, 45, 0, Point3(0, 0, playerRadius_Z)));
		// Rect Platforms
		allRectPlatforms.push_back(RectPlatform(0.0f, 1.5f, (float)(playerRadius_Z * 1)));
		allRectPlatforms.push_back(RectPlatform(1.5f, 1.5f, (float)(playerRadius_Z * 3)));
		allRectPlatforms.push_back(RectPlatform(1.5f, 0.0f, (float)(playerRadius_Z * 5)));
		allRectPlatforms.push_back(RectPlatform(1.5f, -1.5f, (float)(playerRadius_Z * 7)));
		allRectPlatforms.push_back(RectPlatform(0.0f, -1.5f, (float)(playerRadius_Z * 9)));
		allRectPlatforms.push_back(RectPlatform(-1.5f, -1.5f, (float)(playerRadius_Z * 11)));
		allRectPlatforms.push_back(RectPlatform(-1.5f, 0.0f, (float)(playerRadius_Z * 13)));
		allRectPlatforms.push_back(RectPlatform(-1.5f, 1.5f, (float)(playerRadius_Z * 15)));
		allRectPlatforms.push_back(RectPlatform(-1.5f, 3.0f, (float)(playerRadius_Z * 17)));
		allRectPlatforms.push_back(RectPlatform(0.0f, 3.0f, (float)(playerRadius_Z * 19)));
		allRectPlatforms.push_back(RectPlatform(1.5f, 3.0f, (float)(playerRadius_Z * 21)));
		allRectPlatforms.push_back(RectPlatform(3.0f, 3.0f, (float)(playerRadius_Z * 23)));
		allRectPlatforms.push_back(RectPlatform(3.0f, 1.5f, (float)(playerRadius_Z * 25)));
		allRectPlatforms.push_back(RectPlatform(3.0f, 0.0f, (float)(playerRadius_Z * 27)));
		allRectPlatforms.push_back(RectPlatform(3.0f, -1.5f, (float)(playerRadius_Z * 29)));
		// Upwinds
		allUpwinds.push_back(Upwind(-4.0f, 0.0f, 0.0f, 1.0f, 4.0f, false));
		// Gems
		allGems.push_back(Gem(0.0f, 1.5f, (float)(playerRadius_Z * 1), gem_LightIndex));
		allGems.push_back(Gem(1.5f, 1.5f, (float)(playerRadius_Z * 3), gem_LightIndex));
		allGems.push_back(Gem(1.5f, 0.0f, (float)(playerRadius_Z * 5), gem_LightIndex));
		allGems.push_back(Gem(1.5f, -1.5f, (float)(playerRadius_Z * 7), gem_LightIndex));
		allGems.push_back(Gem(0.0f, -1.5f, (float)(playerRadius_Z * 9), gem_LightIndex));
		allGems.push_back(Gem(-1.5f, -1.5f, (float)(playerRadius_Z * 11), gem_LightIndex));
		allGems.push_back(Gem(-1.5f, 0.0f, (float)(playerRadius_Z * 13), gem_LightIndex));
		allGems.push_back(Gem(-1.5f, 1.5f, (float)(playerRadius_Z * 15), gem_LightIndex));
		allGems.push_back(Gem(0.0f, 3.0f, (float)(playerRadius_Z * 19), gem_LightIndex));
		allGems.push_back(Gem(1.5f, 3.0f, (float)(playerRadius_Z * 21), gem_LightIndex));
		allGems.push_back(Gem(3.0f, 1.5f, (float)(playerRadius_Z * 25), gem_LightIndex));
		allGems.push_back(Gem(3.0f, 0.0f, (float)(playerRadius_Z * 27), gem_LightIndex));
		// Checkpoints
		allCheckpoints.push_back(Checkpoint(-1.5f, 3.0f, (float)(playerRadius_Z * 17), checkpointOff_LightIndex));
		allCheckpoints.push_back(Checkpoint(3.0f, 3.0f, (float)(playerRadius_Z * 23), checkpointOff_LightIndex));
		allCheckpoints.push_back(Checkpoint(3.0f, -1.5f, (float)(playerRadius_Z * 29), checkpointOff_LightIndex));
		// Lamps
		allLamps.push_back(Lamp(0.0f, 0.0f, 10.0f, lamp_LightIndex));
	}
	void setNumVariables() {
		numLamps = (int)allLamps.size();
		numGems = (int)allGems.size();
		numCheckpoints = (int)allCheckpoints.size();
		numRectPlatforms = (int)allRectPlatforms.size();
		numUpwinds = (int)allUpwinds.size();
	}
	void initGemInfos() {
		for (int i = 0; i < numGems; i++) {
			Gem& currGem = allGems.at(i);
			float dir = 1.0f;
			if (i % 2 == 1) { dir = -1.0f; }
			currGem.spinDirection = dir;
			gemInfos.push_back(GemInfo((float)(DirectX::XM_2PI * 0.125 * i), Point3(currGem.Position.x, currGem.Position.y, currGem.Position.z)));
		}
	}
	void initRespawnInfos() {
		for (int i = 0; i < numCheckpoints; i++) {
			Checkpoint& currCheckpoint = allCheckpoints.at(i);
			respawnInfos.push_back(RespawnInfo(0, 45, 0, Point3(currCheckpoint.Position.x, currCheckpoint.Position.y, currCheckpoint.Position.z + playerRadius_Z)));
		}
	}
};