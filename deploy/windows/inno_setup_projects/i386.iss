; Script for create installer
[Setup]

; App name
#define   Name       "ProcDepMonitor"
; App version
#define   Version    "1.0.0"
; Author        
#define   Publisher  "3dproger"
; Author URL
#define   URL        "https://github.com/3dproger"
; Exe name
#define   ExeName    "ProcDepMonitor.exe"

; Generated from Tools -> Generate GUID
AppId={{69CE5F3C-A869-4FA6-967A-C18E0A01578F}

; Uncomment if need x64 only
; ArchitecturesInstallIn64BitMode=x64

; Compression parameters
Compression=lzma2
SolidCompression=yes

WizardStyle=modern

; Disable or not the page for selecting the folder where to install. [auto, yes, on]
; auto - do not show if already installed
; no - always show
; yes - never show
DisableDirPage=auto

DisableProgramGroupPage=no

AppName={#Name}
AppVersion={#Version}
AppPublisher={#Publisher}
AppPublisherURL={#URL}
AppSupportURL={#URL}
AppUpdatesURL={#URL}

DefaultGroupName={#Name}
DefaultDirName={pf}\{#Name}
UninstallDisplayIcon={app}\{#ExeName}

WizardImageFile="big_image.bmp"
WizardSmallImageFile="small_image.bmp"
SetupIconFile="main_icon.ico"
DisableWelcomePage=false

; Output filename
OutputBaseFileName={#Name}-{#Version}-win32
                                                
[Languages]
Name: en; MessagesFile: "compiler:Default.isl";
Name: ru; MessagesFile: "compiler:Languages\Russian.isl";

; Licenses
;Name: en; MessagesFile: "compiler:Default.isl"; LicenseFile: "License_ENG.txt"    
;Name: ru; MessagesFile: "compiler:Languages\Russian.isl"; LicenseFile: "License_RUS.txt"


[Messages]
ru.BeveledLabel=Russian
en.BeveledLabel=English

[Icons]
Name: "{group}\{#Name}"; Filename: "{app}\{#ExeName}"
Name: "{group}\{cm:UninstallProgram,{#Name}}"; Filename: "{uninstallexe}"
Name: "{commondesktop}\{#Name}"; Filename: "{app}\{#ExeName}"; Tasks: desktopicon

[Tasks]
; Icon at desktop
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}";

[Files]
;Input files
Source: "..\i386\appdir\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs

