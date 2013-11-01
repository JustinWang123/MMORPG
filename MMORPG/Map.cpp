#include "Map.h"
#include <fstream>
#include <assert.h>
#include <math.h>
#include "GameBase.h"
#include "PacketProtocol.h"

// ------------------------------------------------------------------------------------------------
Map :: Map(Vector2df setScreenPos, GameBase* setGameBase) {
    tileSurfaces[TILE_TYPE_SPACE] = LoadSurface("Surfaces/GrassTile.png");
    tileSurfaces[TILE_TYPE_WALL] = LoadSurface("Surfaces/WallTile.png");
    tileSurfaces[TILE_TYPE_UP_WELL] = LoadSurface("Surfaces/UpGravityWell.png");
    tileSurfaces[TILE_TYPE_DOWN_WELL] = LoadSurface("Surfaces/DownGravityWell.png");
    tileSurfaces[TILE_TYPE_LEFT_WELL] = LoadSurface("Surfaces/LeftGravityWell.png");
    tileSurfaces[TILE_TYPE_RIGHT_WELL] = LoadSurface("Surfaces/RightGravityWell.png");
    tileSurfaces[TILE_TYPE_BULLET_PICKUP] = LoadSurface("Surfaces/BulletPickUpTile.png");
    tileSurfaces[TILE_TYPE_MINE_PICKUP] = LoadSurface("Surfaces/MinePickUpTile.png");
    tileSurfaces[TILE_TYPE_MISSILE_PICKUP] = LoadSurface("Surfaces/MissilePickUpTile.png");
    tileSurfaces[TILE_TYPE_REPAIR_PICKUP] = LoadSurface("Surfaces/RepairPickUpTile.png");
    tileSurfaces[TILE_TYPE_SPAWN] = LoadSurface("Surfaces/SpawnTile.png");

    screenPos = setScreenPos;
    game = setGameBase;
    currentSpawnPoint = 0;

    for(Tile y = 0; y < MAP_NUM_TILES_Y; y++) {
        for(Tile x = 0; x < MAP_NUM_TILES_X; x++) {
            tileTypes[x][y] = TILE_TYPE_SPACE;
        }
    }

	level = sceneManager->addCubeSceneNode(100.0f, 0, -1, vector3df(50,-50, -25));
	level->setMaterialFlag(EMF_LIGHTING, true);
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
Map :: ~Map() {
    for(int i = 0; i < NUM_OF_TILE_TYPES; i++) {
        if(tileSurfaces[i] != 0) {
            FreeSurface(tileSurfaces[i]);
        }
    }
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void Map :: LoadMap(std::string fileName) {
    std::ifstream mapFile (fileName.c_str());
    assert(mapFile.is_open());

    mapFile.close();
} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void Map :: Draw(Vector2df camPos) const {

} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
bool Map :: IsPosSolid(Vector2df atPos) const {
    // Out of bounds positions are always considered solid:
    if(atPos.x < MAP_MIN_POS_X  || atPos.x >= MAP_MAX_POS_X || atPos.y < MAP_MIN_POS_Y || atPos.y >= MAP_MAX_POS_Y) {
        return true;
    }
    else {
        int tileX = (atPos.x - screenPos.x) / MAP_TILE_SIZE;
        int tileY = (atPos.y - screenPos.y) / MAP_TILE_SIZE;

        return IsTileSolid(tileX, tileY);
    }
} // ----------------------------------------------------------------------------------------------




// ----------------------------------------------------------------------------------------------
bool Map :: IsTileSolid(Tile atTileX, Tile atTileY) const {
    // Out of bounds tiles are always considered solid:
    if(atTileX < 0 || atTileX >= MAP_NUM_TILES_X || atTileY < 0 || atTileY >= MAP_NUM_TILES_Y) {
        return true;
    }
    else {
        return tileTypes[atTileX][atTileY] == TILE_TYPE_WALL;
    }
} // --------------------------------------------------------------------------------------------



// ----------------------------------------------------------------------------------------------
bool Map :: IsPosGravityWell(Vector2df atPos) const {
    // Out of bounds positions are always considered not gravity wells:
    if(atPos.x < MAP_MIN_POS_X  || atPos.x >= MAP_MAX_POS_X || atPos.y < MAP_MIN_POS_Y || atPos.y >= MAP_MAX_POS_Y) {
        return false;
    }
    else {
        int tileX = (atPos.x - screenPos.x) / MAP_TILE_SIZE;
        int tileY = (atPos.y - screenPos.y) / MAP_TILE_SIZE;

        return IsTileGravityWell(tileX, tileY);
    }
} // ----------------------------------------------------------------------------------------------

// ----------------------------------------------------------------------------------------------
bool Map :: IsTileGravityWell(Tile atTileX, Tile atTileY) const {
    // Out of bounds tiles are always considered not to be gravity wells:
    if(atTileX < 0 || atTileX >= MAP_NUM_TILES_X || atTileY < 0 || atTileY >= MAP_NUM_TILES_Y) {
        return false;
    }
    else {
        return tileTypes[atTileX][atTileY] == TILE_TYPE_DOWN_WELL
               || tileTypes[atTileX][atTileY] == TILE_TYPE_UP_WELL
               || tileTypes[atTileX][atTileY] == TILE_TYPE_RIGHT_WELL
               || tileTypes[atTileX][atTileY] == TILE_TYPE_LEFT_WELL;
    }
} // --------------------------------------------------------------------------------------------




// ----------------------------------------------------------------------------------------------
Vector2df Map :: GetPosGravity(Vector2df atPos) const {
    // Out of bounds positions are always considered to have 0 gravity:
    if(atPos.x < MAP_MIN_POS_X  || atPos.x >= MAP_MAX_POS_X || atPos.y < MAP_MIN_POS_Y || atPos.y >= MAP_MAX_POS_Y) {
        return Vector2df();
    }
    else {
        // This is probly fucky:
        int tileX = (atPos.x - screenPos.x) / MAP_TILE_SIZE;
        int tileY = (atPos.y - screenPos.y) / MAP_TILE_SIZE;

        return GetTileGravity(tileX, tileY);
    }
} // --------------------------------------------------------------------------------------------




// ----------------------------------------------------------------------------------------------
Vector2df Map :: GetTileGravity(Tile atTileX, Tile atTileY) const {
    // Out of bounds tiles are always considered to have gravity of 0:
    if(atTileX < 0 || atTileX >= MAP_NUM_TILES_X || atTileY < 0 || atTileY >= MAP_NUM_TILES_Y) {
        return 0.0f;
    }
    else {
        return tileGravity[atTileX][atTileY];
    }
} // --------------------------------------------------------------------------------------------





// ----------------------------------------------------------------------------------------------
Vector2df Map :: GetRandomSpawnPointPos() const {
    if(currentSpawnPoint != 0) {
        int spawnPointID = rand() % currentSpawnPoint;
        return spawnPoints[spawnPointID];
    }
    else {
        return Vector2df(0,0);
    }
} // --------------------------------------------------------------------------------------------




// ----------------------------------------------------------------------------------------------
void Map :: CreateSpawnPoint(Vector2df atPos) {
    assert(currentSpawnPoint < MAP_MAX_NUM_OF_SPAWN_POINTS && "Map::CreateSpawnPoint currentSpawnPoint < MAP_MAX_NUM_OF_SPAWN_POINTS");

    if(currentSpawnPoint < MAP_MAX_NUM_OF_SPAWN_POINTS) {
        spawnPoints[currentSpawnPoint] = atPos;
        currentSpawnPoint++;
    }
} // --------------------------------------------------------------------------------------------




// ----------------------------------------------------------------------------------------------
Uint32 Map :: WriteLevelDataToPacket(Uint8 data[]) {
    Uint32 dataWritePos = PACKET_LEVEL_DATA_CONTENTS;

    for(Uint32 y = 0; y < MAP_NUM_TILES_Y; y++) {
        for(Uint32 x = 0; x < MAP_NUM_TILES_X; x++) {
            Uint8 temp = (Uint8)tileTypes[x][y];

            memcpy(&data[dataWritePos], &temp, 1 );
            dataWritePos += 1;
        }
    }

    return dataWritePos;
} // --------------------------------------------------------------------------------------------




// ----------------------------------------------------------------------------------------------
void Map :: HandleLevelData(UDPpacket* packet) {
    Uint32 dataReadPos = PACKET_LEVEL_DATA_CONTENTS;

    for(Uint32 y = 0; y < MAP_NUM_TILES_Y; y++) {
        for(Uint32 x = 0; x < MAP_NUM_TILES_X; x++) {
            Uint8 temp;
            memcpy(&temp, &packet->data[dataReadPos], 1 );
            tileTypes[x][y] = (TileType)temp;
            dataReadPos += 1;
        }
    }
} // --------------------------------------------------------------------------------------------






void Map :: SetTile(Uint32 tileX, Uint32 tileY, TileType type) {
    tileTypes[tileX][tileY] = type;
}





SDL_Surface*  Map :: GetTileSurface(TileType type) {
    return tileSurfaces[(int)type];
}




void Map :: SaveMap() {
    std::ofstream myfile;
    myfile.open ("Data/newLevel.txt", std::ios::trunc);

    for(Uint32 y = 0; y < MAP_NUM_TILES_Y; y++)
        for(Uint32 x = 0; x < MAP_NUM_TILES_X; x++) {
            myfile << (int)tileTypes[x][y];
            myfile << " ";
        }

    myfile.close();
}

