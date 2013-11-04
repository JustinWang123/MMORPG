#include "Map.h"
#include <fstream>
#include <assert.h>
#include <math.h>
#include "GameBase.h"
#include "PacketProtocol.h"

// ------------------------------------------------------------------------------------------------
Map :: Map(vector3df setScreenPos, GameBase* setGameBase) {
    game = setGameBase;

	level = sceneManager->addMeshSceneNode(sceneManager->getMesh("Models/Level.3ds"), 0, COLLISION_BITMASK_LEVEL);
	//level->setScale(vector3df(0.1, 0.1, 0.1));
	level->setMaterialFlag(EMF_NORMALIZE_NORMALS, true);
	level->setMaterialFlag(EMF_LIGHTING, true);

	light = sceneManager->addLightSceneNode(0, vector3df(0,75,75));
	light->setID(COLLISION_BITMASK_NONE);

} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
Map :: ~Map() {

} // ----------------------------------------------------------------------------------------------




// ------------------------------------------------------------------------------------------------
void Map :: LoadMap(std::string fileName) {
    std::ifstream mapFile (fileName.c_str());
    assert(mapFile.is_open());
		// Do some loading here
    mapFile.close();
} // ----------------------------------------------------------------------------------------------







// ------------------------------------------------------------------------------------------------
bool Map :: IsPosSolid(vector3df atPos) const {
    return false;
} // ----------------------------------------------------------------------------------------------






// ----------------------------------------------------------------------------------------------
Uint32 Map :: WriteLevelDataToPacket(Uint8 data[]) {
    return 0;
} // --------------------------------------------------------------------------------------------




// ----------------------------------------------------------------------------------------------
void Map :: HandleLevelData(UDPpacket* packet) {

} // --------------------------------------------------------------------------------------------






void Map :: SaveMap() {
    std::ofstream myfile;
    myfile.open ("Data/newLevel.txt", std::ios::trunc);
		// Do some saving
    myfile.close();
}

