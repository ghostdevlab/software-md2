//
// Created by Duch on 21.04.2026.
//

#include "Enemy.h"


EnemyPakDefinition enemyPakDefinition[] = {
        {
            "models/monsters/gunner/tris.md2",
            "models/monsters/gunner/skin.pcx",
            "models/monsters/gunner/pain.pcx",
            {
                    "sound/gunner/death1.wav",
                    "sound/gunner/Gunatck1.wav",
                    "sound/gunner/Gunatck2.wav",
                    "sound/gunner/Gunatck3.wav",
                    "sound/gunner/Gunidle1.wav",
                    "sound/gunner/gunpain1.wav",
                    "sound/gunner/Gunpain2.wav",
                    "sound/gunner/Gunsrch1.wav",
                    "sound/gunner/sight1.wav",
                    nullptr,
                    nullptr,
                    nullptr,
                    nullptr,
                    nullptr,
                    nullptr,
            },
        }
};

TEnemy* loadEnemy(EnemyPakDefinition* enemy) {
    return nullptr;
}