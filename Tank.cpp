/*
#include "Tank.h"
#include "data/DataCenter.h"
#include "data/GIFCenter.h"
#include "algif5/algif.h"
#include "shapes/Rectangle.h"
#include <stdio.h>

namespace TankSetting
{
    static constexpr char gif_root_path[50] = "./assets/gif/Tank";
    static constexpr char gif_postfix[][10] = {
        "alive",
        "dead"
    };

}

void Tank::init() {
    for (size_t type = 0; type < static_cast<size_t>(TankState::DEAD); ++type)
    {
        char buffer[50];
        sprintf(
            buffer, "%s/tank_%s.gif",
            TankSetting::gif_root_path,
            TankSetting::gif_postfix[static_cast<int>(type)]);
        gifPath[static_cast<TankState>(type)] = std::string{buffer};
    }
}
*/