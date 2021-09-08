// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include <Windows.h>
#include <iostream>
#include <TlHelp32.h>
#include "mem.h"
#include "proc.h"
#include <vector>
#include "Entities.h"


int DefineEntity(ActorEntity* ent)
{
    if (ent)
    {
        switch (ent->vTable)
        {
        case PlayerEntity::vTableValue:
            return 1;
        case MonsterEntity::vTableValue:
            return 2;
        case FriendlyNpcEntity::vTableValue:
            return 3;
        case PartymemEntity::vTableValue:
            return 4;
        default:
            //Unknown Entity Type
            return 5;
        }
    }
    return -1;
}



DWORD WINAPI HackThread(HMODULE hModule)
{
    //Create console
    AllocConsole();
    FILE* f;
    freopen_s(&f, "CONOUT$", "w", stdout);

    std::cout << "Let's get to it!" << std::endl;
    std::cout << "Numpad 1: Health toggle" << std::endl;
    std::cout << "Numpad 2: Teleport Ball" << std::endl;
    std::cout << "Numpad 3: Damage" << std::endl;
    std::cout << "Numpad 4: TEST hotkey" << std::endl;
    std::cout << "LShift: Speed Uncap" << std::endl;
    std::cout << "END to detach" << std::endl;

    //
    bool bHealth = false, bSpeedUncap = false, bDamage = false, bTest = false;
    int16_t oldStr = 0, oldAcc = 0;

    uintptr_t moduleBase = reinterpret_cast<uintptr_t>(GetModuleHandle(L"SAOHF.exe"));
    EntityList* entList = reinterpret_cast<EntityList*>(mem::FindDMAAddy(moduleBase + 0x00C99B70, {0x0}));
    PlayerEntity* playerBase = static_cast<PlayerEntity*>(entList->ents[2]);

    int* numOfEntities = reinterpret_cast<int*>(mem::FindDMAAddy(moduleBase + 0x00C99B70, { 0x7F8 }));

    while (true)
    {
        //Detach
        if (GetAsyncKeyState(VK_END) & 1)
        {
            break;
        }


        //Speed Uncap
        if (GetAsyncKeyState(VK_LSHIFT) & 1)
        {
            bSpeedUncap = !bSpeedUncap;

            if (bSpeedUncap)
            {
                //Change the jb to jmp
                mem::Patch((BYTE*)(moduleBase + 0x1000 + 0x291F06), (BYTE*)"\xEB", 1);
            }

            else
            {
                //jb as usual
                mem::Patch((BYTE*)(moduleBase + 0x1000 + 0x291F06), (BYTE*)"\x72", 1);
            }
        }
        
        //Health toggle
        if (GetAsyncKeyState(VK_NUMPAD1) & 1)
        {
            bHealth = !bHealth;
        }
        
        //2: Teleport ball
        if (GetAsyncKeyState(VK_NUMPAD2) & 1)
        {
            for (int i = 7; i < (*numOfEntities + 7); i++)
            {
                int entType = DefineEntity(entList->ents[i]);
                if (entType==2) //If enemy
                {
                    entList->ents[i]->coordinates = { playerBase->coordinates.X + 1, playerBase->coordinates.Y, playerBase->coordinates.Z };
                }
            }
        }

        //Damage toggle
        if (GetAsyncKeyState(VK_NUMPAD3) & 1)
        {
            bDamage = !bDamage;

            if (bDamage)
            {
                oldStr = playerBase->strength;
                oldAcc = playerBase->accuracy;
                playerBase->strength = 16000;
                playerBase->accuracy = 16000;
            }
            else
            {
                playerBase->strength = oldStr;
                playerBase->accuracy = oldAcc;
            }

        }
        



        if (playerBase)
        {
            if (bHealth)
            {
                playerBase->maxHealth = 1337360;
                playerBase->health = playerBase->maxHealth;
            }


            if (bSpeedUncap && playerBase->speed > 1)
            {
                playerBase->speed = 1;
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


BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
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
