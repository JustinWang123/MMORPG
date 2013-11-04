#ifndef MAP_H
#define MAP_H

#include "SDLWrapper.h"
#include "Irr_Wrapper.hpp"
class GameBase;

const float 				MAP_MIN_POS_X = 0.0f;
const float 				MAP_MIN_POS_Y = 0.0f;
const float 				MAP_MAX_POS_X = 1000.f;
const float 				MAP_MAX_POS_Y = 1000.f;

class Map {
public:
    Map(vector3df setScreenPos, GameBase* setGameBase);
    ~Map();

    void					LoadMap(std::string fileName);
    void					SaveMap();

    Uint32 					WriteLevelDataToPacket(Uint8 data[]);
    void					HandleLevelData(UDPpacket* packet);

    // Accessors:
    bool					IsPosSolid(vector3df atPos) const;

protected:
    GameBase* 				game;
	ISceneNode*				level;
	ILightSceneNode*		light;
};

#endif
