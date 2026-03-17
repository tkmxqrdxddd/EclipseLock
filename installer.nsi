; EclipseLock Windows Installer Script
; Build with: makensis installer.nsi
; Requires: NSIS 3.0+

;--------------------------------
;Configuration

!define APP_NAME "EclipseLock"
!define APP_VERSION "1.1.0"
!define APP_PUBLISHER "Jan Fidra"
!define APP_URL "https://github.com/tkmxqrdxddd/eclipselock"
!define APP_EXE "eclipselock.exe"

;--------------------------------
;Include Modern UI

!include "MUI2.nsh"
!include "FileFunc.nsh"

;--------------------------------
;General Settings

Name "${APP_NAME} ${APP_VERSION}"
OutFile "eclipselock_${APP_VERSION}_windows_installer.exe"
DefaultDir "$PROGRAMFILES64\${APP_NAME}"
InstallDir "$PROGRAMFILES64\${APP_NAME}"
ShowInstDetails show
ShowUnInstDetails show
RequestExecutionLevel admin
BrandingText "${APP_NAME} - Secure File Encryption"

;--------------------------------
;UI Settings

!define MUI_ABORTWARNING
!define MUI_ICON "assets\icon.ico"
!define MUI_UNICON "assets\icon.ico"
!define MUI_WELCOMEFINISHPAGE_BITMAP "assets\installer_banner.bmp"

;--------------------------------
;Pages

!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE "LICENSE"
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES
!insertmacro MUI_UNPAGE_FINISH

;--------------------------------
;Languages

!insertmacro MUI_LANGUAGE "English"

;--------------------------------
;Installer Sections

Section "EclipseLock (Required)" SecRequired
    SetOutPath "$INSTDIR"
    
    ; Copy main executable
    File "eclipselock.exe"
    
    ; Copy license
    File "LICENSE"
    
    ; Copy README
    File "README.md"
    
    ; Create start menu shortcut
    CreateDirectory "$SMPROGRAMS\${APP_NAME}"
    CreateShortcut "$SMPROGRAMS\${APP_NAME}\${APP_NAME}.lnk" "$INSTDIR\${APP_EXE}"
    CreateShortcut "$SMPROGRAMS\${APP_NAME}\Uninstall ${APP_NAME}.lnk" "$INSTDIR\uninst.exe"
    
    ; Create desktop shortcut
    CreateShortcut "$DESKTOP\${APP_NAME}.lnk" "$INSTDIR\${APP_EXE}"
    
    ; Write uninstaller
    WriteUninstaller "$INSTDIR\uninst.exe"
    
    ; Write registry information
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APP_NAME}" "DisplayName" "${APP_NAME} ${APP_VERSION}"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APP_NAME}" "UninstallString" "$INSTDIR\uninst.exe"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APP_NAME}" "DisplayIcon" "$INSTDIR\${APP_EXE}"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APP_NAME}" "Publisher" "${APP_PUBLISHER}"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APP_NAME}" "URLInfoAbout" "${APP_URL}"
    WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APP_NAME}" "NoModify" 1
    WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APP_NAME}" "NoRepair" 1
    
    ; Get installed size
    ${GetSize} "$INSTDIR" "/S=0K" $0 $1 $2
    IntFmt $0 "0x%08X" $0
    WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APP_NAME}" "EstimatedSize" "$0"
    
SectionEnd

Section "Add to PATH" SecPath
    ; Add installation directory to PATH
    nsExec::ExecToLog 'setx PATH "%PATH%;$INSTDIR"'
SectionEnd

;--------------------------------
;Uninstaller Section

Section "Uninstall"
    ; Remove registry keys
    DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APP_NAME}"
    
    ; Remove files
    Delete "$INSTDIR\${APP_EXE}"
    Delete "$INSTDIR\LICENSE"
    Delete "$INSTDIR\README.md"
    Delete "$INSTDIR\uninst.exe"
    
    ; Remove shortcuts
    Delete "$SMPROGRAMS\${APP_NAME}\${APP_NAME}.lnk"
    Delete "$SMPROGRAMS\${APP_NAME}\Uninstall ${APP_NAME}.lnk"
    RMDir "$SMPROGRAMS\${APP_NAME}"
    
    Delete "$DESKTOP\${APP_NAME}.lnk"
    
    ; Remove installation directory
    RMDir "$INSTDIR"
    
SectionEnd

;--------------------------------
;Callbacks

Function .onInit
    ; Check if already installed
    ReadRegStr $0 HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APP_NAME}" "UninstallString"
    StrCmp $0 "" done
    
    MessageBox MB_YESNO|MB_ICONEXCLAMATION \
        "${APP_NAME} is already installed.$\r$\n$\r$\nClick Yes to uninstall the previous version first." \
        IDYES uninst
    Abort
    
    uninst:
        ; Run uninstaller
        ExecWait '$0 _?=$INSTDIR'
    
    done:
FunctionEnd

Function un.onUninstallSuccess
    MessageBox MB_OK "EclipseLock has been successfully uninstalled."
FunctionEnd
