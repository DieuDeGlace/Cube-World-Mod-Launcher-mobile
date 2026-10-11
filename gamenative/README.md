# GameNative Steam-folder launcher

Optional native Win32 helper for the established phone configuration. Separate from the CubeModLoader implementation. The known .NET custom container maps S: to the real Steam Cube World folder and A: to the helper directory. The launcher checks its configured container and game path, forces ALSA, launches the existing dialog helper/Steam loader, then Geo from S:\Tools\Geo.

Normal startup does not install/copy/delete game or runtime files. Installation/update helpers are separate and backup/verify changes first. The configured container ID/paths are specific to the tested setup: edit them for another installation. Native .NET must already be installed in the same container. Steam tile launching uses its own container and does not automatically inherit .NET or this helper.

Compile in an x64 Visual Studio tools shell with clang-cl /O1 /GS- /c SteamFolderLauncher.cpp; link with lld-link /entry:mainCRTStartup /subsystem:windows /nodefaultlib SteamFolderLauncher.obj kernel32.lib advapi32.lib. Tested compiled helper restarts actual Steam files and starts original Geo. No proprietary game/Steam/.NET binaries are bundled.
