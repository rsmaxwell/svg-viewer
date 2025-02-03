#ifndef AppVersion
#define AppVersion "1.1.3"
#endif

#ifndef config
#define config "Release"
#endif

#ifndef QtInstallPath
#define QtInstallPath "C:\\Qt"
#endif

#ifndef QtVersion
#define QtVersion "6.8.1"
#endif

#ifndef QtPlatform
#define QtPlatform "msvc2022_64"
#endif

#ifndef QtPlatformPath
#define QtPlatformPath "{#QtInstallPath}\{#QtVersion}\{#QtPlatform}"
#endif

#ifndef VisualStudioPath
#define VisualStudioPath "C:\\Program Files\\Microsoft Visual Studio\\2022\\Community"
#endif

#ifndef AppIdGUID
#define AppIdGUID "{{B018B71A-7A1D-4D0B-A5E9-D6EB519A4820}}"
#endif

#ifndef QtCoreFile
#if config == "Debug"
#define QtCoreFile "Qt6Cored.dll"
#define QtGuiFile "Qt6Guid.dll"
#define QtSvgFile "Qt6Svgd.dll"
#define QtWidgetsFile "Qt6Widgetsd.dll"
#define QtPlatformPluginFile "qwindowsd.dll"
#define QtCorePDB "Qt6Cored.pdb"
#define QtGuiPDB "Qt6Guid.pdb"
#define QtSvgPDB "Qt6Svgd.pdb"
#define QtWidgetsPDB "Qt6Widgetsd.pdb"
#define QtPlatformPluginPDB "qwindowsd.pdb"
#else
#define QtCoreFile "Qt6Core.dll"
#define QtGuiFile "Qt6Gui.dll"
#define QtSvgFile "Qt6Svg.dll"
#define QtWidgetsFile "Qt6Widgets.dll"
#define QtPlatformPluginFile "qwindows.dll"
#endif
#endif

[Setup]
AppId={#AppIdGUID}
AppName="ExplorerSvgSupport"
AppVersion={#AppVersion}
AppVerName="ExplorerSvgSupport 1.1.3 - {#config}"
AppPublisher="Richard Maxwell"
AppPublisherURL=https://rsmaxwell.co.uk/
AppSupportURL=https://github.com/rsmaxwell/ExplorerSvgSupport/issues
AppUpdatesURL=https://github.com/rsmaxwell/ExplorerSvgSupport/releases
DefaultDirName="{autopf}\ExplorerSvgSupport"
DefaultGroupName="ExplorerSvgSupport"
OutputBaseFilename="ExplorerSvgSupport-installer"
Compression=zip
SolidCompression=yes
ChangesAssociations=yes
ArchitecturesInstallIn64BitMode=x64compatible

[Languages]
Name: "en"; MessagesFile: "compiler:Default.isl"; LicenseFile: "..\LICENSE.md"

[Files]
Source: "..\build\{#config}\*"; DestDir: "{app}"; Flags: recursesubdirs
Source: "..\build\{#config}\ExplorerSvgSupport.dll"; DestDir: "{app}"; Flags: regserver

Source: "..\LICENSE.md"; DestDir: "{app}\license\";
Source: "{#QtInstallPath}\Licenses\LICENSE"; DestDir: "{app}\license\"; DestName: "Qt-LICENSE";

Source: "{#VisualStudioPath}\VC\Redist\MSVC\v143\vc_redist.x64.exe"; DestDir: "{tmp}"; Flags: dontcopy

Source: "{#QtPlatformPath}\bin\{#QtCoreFile}"; DestDir: "{app}"
Source: "{#QtPlatformPath}\bin\{#QtGuiFile}"; DestDir: "{app}"
Source: "{#QtPlatformPath}\bin\{#QtSvgFile}"; DestDir: "{app}"
Source: "{#QtPlatformPath}\bin\{#QtWidgetsFile}"; DestDir: "{app}"
Source: "{#QtPlatformPath}\plugins\platforms\{#QtPlatformPluginFile}"; DestDir: "{app}\platforms";

#if config == "Debug"
Source: "{#QtPlatformPath}\bin\{#QtCorePDB}"; DestDir: "{app}"
Source: "{#QtPlatformPath}\bin\{#QtGuiPDB}"; DestDir: "{app}"
Source: "{#QtPlatformPath}\bin\{#QtSvgPDB}"; DestDir: "{app}"
Source: "{#QtPlatformPath}\bin\{#QtWidgetsPDB}"; DestDir: "{app}"
Source: "{#QtPlatformPath}\plugins\platforms\{#QtPlatformPluginPDB}"; DestDir: "{app}\platforms";
#endif

[Code]

// Automatically uninstalls the previously installed version if any
Procedure CurStepChanged(CurStep: TSetupStep);
Var
  ResultCode: Integer;
  Uninstaller: String;
Begin
  If (CurStep = ssInstall) Then Begin
    If RegQueryStringValue(HKLM, 'Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\' + ExpandConstant('{#AppIdGUID}') + '_is1', 'UninstallString', Uninstaller) Then Begin
      Exec(RemoveQuotes(Uninstaller), ' /SILENT', '', SW_SHOWNORMAL, ewWaitUntilTerminated, ResultCode);
    End;
    ExtractTemporaryFile(ExpandConstant('vc_redist.x64.exe'));
    Exec(ExpandConstant('{tmp}\vc_redist.x64.exe'), '/install /quiet /norestart', '', SW_SHOWNORMAL, ewWaitUntilTerminated, ResultCode);
  End;
End;
