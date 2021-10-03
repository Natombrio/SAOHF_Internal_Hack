// dllmain.cpp : Defines the entry point for the DLL application.

#include "pch.h"
#include <Windows.h>
#include <iostream>
#include <TlHelp32.h>
#include "mem.h"
#include "proc.h"
#include <vector>
#include "Entities.h"
#include "dllmain.h"
#include <cmath>



DWORD WINAPI HackThread(HMODULE hModule) {
    //Create console
    AllocConsole();
    FILE* f;
    freopen_s(&f, "CONOUT$", "w", stdout);


    bool bHealth = false, bSpeedUncap = false, bDamage = false, bTeleportBall = false;
    int16_t oldStr = 0, oldAcc = 0;

    uintptr_t moduleBase = reinterpret_cast<uintptr_t>(GetModuleHandle(L"SAOHF.exe"));
    EntityList* entList = reinterpret_cast<EntityList*>(mem::FindDMAAddy(moduleBase + 0x00C99B70, {0x0}));
    PlayerEntity* playerBase = static_cast<PlayerEntity*>(entList->ents[2]);

    int* numOfEntities = reinterpret_cast<int*>(mem::FindDMAAddy(moduleBase + 0x00C99B70, { 0x7F8 }));


    PrintInstructions(bHealth, bSpeedUncap, bDamage, bTeleportBall);

    while (true) {
        //Detach
        if (GetAsyncKeyState(VK_END) & 1) {
            break;
        }


        //Speed Uncap
        if (GetAsyncKeyState(VK_LSHIFT) & 1) {
            bSpeedUncap = !bSpeedUncap;
            PrintInstructions(bHealth, bSpeedUncap, bDamage, bTeleportBall);

            if (bSpeedUncap) {
                //Change the jb to jmp
                mem::Patch((BYTE*)(moduleBase + 0x1000 + 0x291F06), (BYTE*)"\xEB", 1);
            }

            else {
                //jb as usual
                mem::Patch((BYTE*)(moduleBase + 0x1000 + 0x291F06), (BYTE*)"\x72", 1);
            }
        }
        
        //1: Infinite Health
        if (GetAsyncKeyState(VK_NUMPAD1) & 1){
            bHealth = !bHealth;
            PrintInstructions(bHealth, bSpeedUncap, bDamage, bTeleportBall);
        }

        if (playerBase) {
            if (bHealth) {
                playerBase->maxHealth = 1337360;
                playerBase->health = playerBase->maxHealth;
            }


            if (bSpeedUncap && playerBase->speed > 1) {
                playerBase->speed = 1;
            }
        }

        
        //2: Teleport Ball
        if (GetAsyncKeyState(VK_NUMPAD2) & 1) {
            bTeleportBall = !bTeleportBall;
            PrintInstructions(bHealth, bSpeedUncap, bDamage, bTeleportBall);
        }

        if (playerBase) {
            if (bTeleportBall) {
                for (int i = 7; i < (*numOfEntities + 7); i++) {
                    uintptr_t entType = entList->ents[i]->vTable;

                    //If enemy
                    if (entType == MonsterEntity::vTableValue) {
                        entList->ents[i]->coordinates = { playerBase->coordinates.X + 2, playerBase->coordinates.Y, playerBase->coordinates.Z + 2};
                    }
                    else if (isUnknownEnt(entType)) {
                        entList->ents[i]->coordinates = { playerBase->coordinates.X + 3, playerBase->coordinates.Y, playerBase->coordinates.Z };
                    }
                }
            }
        }

        //3: Damage Hack
        if (GetAsyncKeyState(VK_NUMPAD3) & 1)
        {
            if (playerBase) {
                bDamage = !bDamage;
                PrintInstructions(bHealth, bSpeedUncap, bDamage, bTeleportBall);

                if (bDamage)
                {
                    oldStr = playerBase->strength;
                    oldAcc = playerBase->accuracy;
                    playerBase->strength = 16000;
                    playerBase->accuracy = 16000;
                } 
                else {
                    playerBase->strength = oldStr;
                    playerBase->accuracy = oldAcc;
                }
            }
        }
        
        //4: Find Closest Monster
        if (GetAsyncKeyState(VK_NUMPAD4) & 1) {
            if (playerBase) {
                PrintInstructions(bHealth, bSpeedUncap, bDamage, bTeleportBall);
                ActorEntity* closestMonster = FindClosestMonster(numOfEntities, playerBase, entList);
                float angle = CalculateAngleToEntityInRad(playerBase->coordinates, closestMonster->coordinates);
                playerBase->direction = angle;
                PrintInstructions(bHealth, bSpeedUncap, bDamage, bTeleportBall);
                std::cout << "Closest Monster is a " << closestMonster->name <<". It is " << closestMonster->distance << " away and it has " << closestMonster->health << " health!" << std::endl;
                
            }
        }
        Sleep(10);
    }


    //cleanup & eject
    fclose(f);
    FreeConsole();
    FreeLibraryAndExitThread(hModule, 0);

    return 0;
}



ActorEntity* FindClosestMonster(int *numOfEntities, PlayerEntity *playerBase, EntityList *entList) {
    struct ActorDistance { int actorIndex; float distance; };
    ActorDistance closestMonster = { 4, 10000 };

    for (int i = 7; i < (*numOfEntities + 7); i++) {
        uintptr_t entType = entList->ents[i]->vTable;

        if (entType == MonsterEntity::vTableValue) {
            float distance = CalculateDistance(playerBase->coordinates, entList->ents[i]->coordinates);
            if (distance < closestMonster.distance) {
                closestMonster = { i, distance };
            }
            entList->ents[closestMonster.actorIndex]->distance = closestMonster.distance;
        }
    }

    return entList->ents[closestMonster.actorIndex];
}


float CalculateDistance(Vector3 ent1, Vector3 ent2) {
    float distance = sqrt(pow((ent1.X - ent2.X),2) + pow((ent1.Y - ent2.Y), 2) + pow((ent1.Z - ent2.Z), 2));
    return distance;
}


float CalculateAngleToEntityInRad(Vector3 ent1, Vector3 ent2) {
    float angle = atan2f((ent1.X - ent2.X), (ent1.Z - ent2.Z)) + 3.1415926536;
    return angle;
}



void PrintInstructions(bool bHealth, bool bSpeedUncap, bool bDamage, bool bTeleportBall) {
    system("cls");
    std::cout << "Let's get to it!" << std::endl;
    std::cout << "************************************" << std::endl;
    std::cout << "Numpad 1: Infinite Health\t" << bHealth << "\t*" << std::endl;
    std::cout << "Numpad 2: Teleport Ball\t\t" << bTeleportBall << "\t*" << std::endl;
    std::cout << "Numpad 3: Damage Hack\t\t" << bDamage << "\t*" << std::endl;
    std::cout << "Numpad 4: Find Closest Monster\t" << "\t*" << std::endl;
    std::cout << "LShift: Speed Uncap\t\t" << bSpeedUncap << "\t*" << std::endl;
    std::cout << "END to detach\t\t\t" << "\t*" << std::endl;
    std::cout << "************************************" << std::endl;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    {
        CloseHandle(CreateThread(nullptr, 0, ((LPTHREAD_START_ROUTINE)HackThread), hModule, 0, nullptr));
    }
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
