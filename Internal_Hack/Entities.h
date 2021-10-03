#include <iostream>
#define STR_MERGE_IMPL(a, b) a##b
#define STR_MERGE(a, b) STR_MERGE_IMPL(a, b)
#define MAKE_PAD(size) STR_MERGE(_pad, __COUNTER__)[size]
#define DEFINE_MEMBER_N(type, name, offset) struct {unsigned char MAKE_PAD(offset); type name;}

struct Vector3 { float X, Y, Z; };

class ActorEntity
{
public:

    union {
        //              Type     Name    Offset
        uintptr_t vTable;
        DEFINE_MEMBER_N(Vector3, coordinates, 0x00E0);
        DEFINE_MEMBER_N(float, direction, 0x018C);
        DEFINE_MEMBER_N(float, speed, 0x0190);
        DEFINE_MEMBER_N(float, acceleration, 0x0198);
        DEFINE_MEMBER_N(uint32_t, health, 0x0264);
        DEFINE_MEMBER_N(uint32_t, maxHealth, 0x0268);
        DEFINE_MEMBER_N(uint32_t, xp, 0x027C);
        DEFINE_MEMBER_N(int16_t, weaponType, 0x0290);
        DEFINE_MEMBER_N(int16_t, weaponDMG, 0x0294);
        DEFINE_MEMBER_N(int16_t, weaponAttck, 0x0296);
        DEFINE_MEMBER_N(int16_t, weaponDefense, 0x0298);
        DEFINE_MEMBER_N(int16_t, weapon_ATKDelay, 0x02A4);
        DEFINE_MEMBER_N(int16_t, resistance, 0x02AA);
        DEFINE_MEMBER_N(int16_t, armorDexterity, 0x02AC);
        DEFINE_MEMBER_N(int16_t, resistance2, 0x02AE);
        DEFINE_MEMBER_N(int16_t, resistance3, 0x02B0);
        DEFINE_MEMBER_N(int16_t, extraSP, 0x02C4);
        DEFINE_MEMBER_N(int16_t, strength, 0x02C8);
        DEFINE_MEMBER_N(int16_t, vitality, 0x02CA);
        DEFINE_MEMBER_N(int16_t, agility, 0x02CC);
        DEFINE_MEMBER_N(int16_t, dexterity, 0x02CE);
        DEFINE_MEMBER_N(int16_t, accuracy, 0x02D4);
        DEFINE_MEMBER_N(int16_t, evasion, 0x02D6);
        DEFINE_MEMBER_N(char *, name, 0x028B0);
        DEFINE_MEMBER_N(char *, name2, 0x028B8);
        DEFINE_MEMBER_N(char *, name3, 0x028C0);
    };

    float distance;


    friend std::ostream& operator<<(std::ostream& os, const Vector3& coords);

}; //Size: 0x6AD0

class MonsterEntity : public ActorEntity
{
public:
    const static uintptr_t vTableValue = 0x140923D68;

}; //Size: 0x6AD0

class FriendlyNpcEntity : public ActorEntity
{
public:
    const static uintptr_t vTableValue = 0x140928C90;

}; //Size: 0x6AD0

class PartymemEntity : public ActorEntity
{
public:
    const static uintptr_t vTableValue = 0x140929D10;

}; //Size: 0x6AD0

class PlayerEntity : public ActorEntity
{
public:
    const static uintptr_t vTableValue = 0x14092ABF0;

}; //Size: 0x6AD0


struct EntityList
{
//Player entity persists between scenes at ents[2]
//The current area's entity list starts at ents[7]
public:
    ActorEntity * ents[100];

};

static bool isUnknownEnt(uintptr_t entType) {
    return entType != PlayerEntity::vTableValue && entType != FriendlyNpcEntity::vTableValue && entType != PartymemEntity::vTableValue && entType != MonsterEntity::vTableValue;
}

std::ostream& operator<<(std::ostream& os, const Vector3& coords)
{
    os << coords.X << ", " << coords.Y << ", " << coords.Z;
    return os;
}