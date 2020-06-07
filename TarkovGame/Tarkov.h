#include "../TarkovGame/Includes.h"

struct Offsets
{
    uint64_t GameObjectManager = 0x151A218;
}Offsets;



uint64_t FindLocalGameWorld(WinProcess* Tarkov, uint64_t GameObjectManager);
std::string GetGameObjectName(WinProcess* Tarkov, uint64_t GameObject);
int32_t GetPlayerCount(WinProcess* Tarkov, uint64_t LocalGameWorldAddr);

void runImpl(WinProcess* Tarkov, uint64_t ModuleAddr)
{
    WinProcess* tProcess = Tarkov;
    uint64_t mAddress = ModuleAddr;

    printf("\nModule Base Address: ");
    printf("0x%" PRIx64 "\n", mAddress);

    // Find The GameObjectManager's Base Address
    uint64_t GOM = tProcess->Read<uint64_t>(mAddress + Offsets.GameObjectManager);

    printf("Game Object Manager: ");
    printf("0x%" PRIx64 "\n", GOM);

    // Find Local Game World's Base Entity Address
    uint64_t LGM = FindLocalGameWorld(tProcess, GOM);

    printf("Local Game World: ");
    printf("0x%" PRIx64 "\n", LGM);

    if (LGM == 0)
    {
        printf("Local Game World not found, are you in a game?\n\n");
        printf("\033c");
        sleep(3);
        runImpl(tProcess, mAddress);
    }

    // Main Info Loop
    while (true)
    {
        printf("Game Object Manager: ");
        printf("0x%" PRIx64 "\n", GOM);

        printf("Local Game World: ");
        printf("0x%" PRIx64 "\n", LGM);

        // Get Player Count
        int32_t playerCount = GetPlayerCount(tProcess, LGM);
        printf("Players Alive: ");
        printf("%" PRId32, playerCount);
        printf("\n");
        sleep(2);
        printf("\033c");

        // Restart if no players are alive
        if (playerCount == 0)
        {
            runImpl(tProcess, mAddress);
            break;
        }
    }
}

uint64_t FindLocalGameWorld(WinProcess* Tarkov, uint64_t GameObjectManager)
{
    WinProcess* tProcess = Tarkov;
    uint64_t GOM = GameObjectManager;

    uint64_t ActiveObject = tProcess->Read<uint64_t>(GOM + 0x18);

    for (int object = 0; object < 50000; object++)
    {
        uint64_t GameObject = tProcess->Read<uint64_t>(ActiveObject + 0x10);
        std::string GameObjectName = GetGameObjectName(tProcess, GameObject);
        if (GameObjectName == "GameWorld")
        {
            uint64_t ObjectClass = tProcess->Read<uint64_t>(GameObject + 0x30);
            uint64_t Entity = tProcess->Read<uint64_t>(ObjectClass + 0x18);
            uint64_t BaseEntity = tProcess->Read<uint64_t>(Entity + 0x28);
            return BaseEntity;
            break;
        }
        ActiveObject = tProcess->Read<uint64_t>(ActiveObject + 0x8);
    } 

    return 0;
}

std::string GetGameObjectName(WinProcess* Tarkov, uint64_t GameObject)
{
    WinProcess* tProcess = Tarkov;
    uint64_t GameObjectAddr = GameObject;

    std::string result;
    uint64_t GameObjectNameAddr = tProcess->Read<uint64_t>(GameObjectAddr + 0x60);

    for (int i = 0; i <= 100; i++)
    {
        char CurrentChar = tProcess->Read<char>(GameObjectNameAddr + i);

        if (CurrentChar == '\0')
            break;

        result.push_back(CurrentChar);
    }
    return result;
}

int32_t GetPlayerCount(WinProcess* Tarkov, uint64_t LocalGameWorldAddr)
{
    WinProcess* tProcess = Tarkov;
    uint64_t LGM = LocalGameWorldAddr;

    uint64_t m_pPlayerList = tProcess->Read<uint64_t>(LGM + 0x78);

    return tProcess->Read<int32_t>(m_pPlayerList + 0x18);
}