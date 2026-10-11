#include <windows.h>

namespace {
constexpr wchar_t containerFile[] = L"\\\\?\\unix\\data\\user\\0\\app.gamenative\\files\\imagefs\\home\\xuser\\.container";
constexpr char requiredContainer[] = "CUSTOM_GAME_2076787665";
constexpr wchar_t steamDirectory[] = L"C:\\Program Files (x86)\\Steam";
constexpr wchar_t loaderIni[] = L"C:\\Program Files (x86)\\Steam\\ColdClientLoader.ini";

bool isExpectedContainer() {
    static char configuration[16384];
    HANDLE file = CreateFileW(containerFile, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE,
                              nullptr, OPEN_EXISTING, 0, nullptr);
    if (file == INVALID_HANDLE_VALUE)
        return false;
    DWORD bytesRead = 0;
    const BOOL read = ReadFile(file, configuration, sizeof(configuration) - 1, &bytesRead, nullptr);
    CloseHandle(file);
    if (!read)
        return false;
    for (const char *start = configuration; *start; ++start) {
        const char *cursor = start;
        const char *expected = requiredContainer;
        while (*expected && *cursor == *expected) {
            ++cursor;
            ++expected;
        }
        if (!*expected)
            return true;
    }
    return false;
}

HANDLE launch(wchar_t *command, const wchar_t *directory) {
    STARTUPINFOW startup{};
    PROCESS_INFORMATION process{};
    startup.cb = sizeof(startup);
    if (!CreateProcessW(nullptr, command, nullptr, nullptr, FALSE, 0, nullptr, directory,
                        &startup, &process))
        return nullptr;
    CloseHandle(process.hThread);
    return process.hProcess;
}

void configureAudio() {
    HKEY key;
    if (RegCreateKeyExW(HKEY_CURRENT_USER, L"Software\\Wine\\Drivers", 0, nullptr, 0,
                        KEY_SET_VALUE, nullptr, &key, nullptr) == ERROR_SUCCESS) {
        constexpr wchar_t driver[] = L"alsa";
        RegSetValueExW(key, L"Audio", 0, REG_SZ, reinterpret_cast<const BYTE *>(driver),
                       sizeof(driver));
        RegCloseKey(key);
    }
    SetEnvironmentVariableW(L"WINEDLLOVERRIDES",
                            L"mscoree=n;winepulse.drv=d;winealsa.drv=b");
}
} // namespace

void WINAPI mainCRTStartup() {
    if (!isExpectedContainer())
        ExitProcess(90);
    if (GetFileAttributesW(L"S:\\cubeworld.exe") == INVALID_FILE_ATTRIBUTES)
        ExitProcess(91);
    configureAudio();
    if (!WritePrivateProfileStringW(L"SteamClient", L"Exe", L"S:\\cubeworld.exe", loaderIni) ||
        !WritePrivateProfileStringW(L"SteamClient", L"ExeRunDir", L"S:\\", loaderIni))
        ExitProcess(92);

    // Installation is separate; normal startup only launches existing files.
    wchar_t dialogs[] = L"C:\\CubeTest\\Tools\\Dialogs.exe";
    HANDLE dialogProcess = launch(dialogs, L"C:\\CubeTest\\Tools");
    if (dialogProcess)
        CloseHandle(dialogProcess);

    wchar_t game[] = L"\"C:\\Program Files (x86)\\Steam\\steamclient_loader_x64.exe\"";
    HANDLE gameProcess = launch(game, steamDirectory);
    if (!gameProcess)
        ExitProcess(GetLastError());
    Sleep(12000);
    static wchar_t geo[] = L"S:\\Tools\\Geo\\GeoCubeWorldHackTool.exe";
    HANDLE geoProcess = launch(geo, L"S:\\Tools\\Geo");
    if (geoProcess)
        CloseHandle(geoProcess);
    WaitForSingleObject(gameProcess, INFINITE);
    CloseHandle(gameProcess);
    ExitProcess(0);
}
