!pragma warning error all

!addincludedir ./include
!addplugindir ./plugins

!include NSISList.nsh
!include TextFunc.nsh
!include MUI2.nsh
!include LogicLib.nsh
Unicode True

!define PRODUCT_NAME "ChituDental"
!define PRODUCT_PUBLISHER "-CBD Technology Co.,Ltd-"
!define OUTFILE "${PRODUCT_NAME}Installer.exe"
!define MUI_ICON ".\${PRODUCT_NAME}.ico"
!define MUI_UNICON "${NSISDIR}\Contrib\Graphics\Icons\modern-uninstall.ico"
!define BUILD_TYPE "Release"
!define PRODUCT_VERSION "v0.1.0"
!define PRODUCT_DIR_REGKEY "Software\${PRODUCT_NAME}"
!define PRODUCT_UNINST_KEY "Software\${PRODUCT_NAME}"
; !define LICENSE_ENGLISH "lisence_english.rtf"
; !define LICENSE_SIMPLE_CHINESE "lisence_simpleChinese.rtf"
; !define LICENSE_JAPANESE "license_japanese.rtf"

Name "${PRODUCT_NAME}"
BrandingText "${PRODUCT_PUBLISHER}"
OutFile "${OUTFILE}"
Unicode true
SetCompressor /SOLID lzma
ShowInstDetails show
ShowUnInstDetails show
RequestExecutionLevel admin

InstallDir "$PROGRAMFILES64\${PRODUCT_NAME}"
InstallDirRegKey HKCU "Software\${PRODUCT_NAME}" ""

!ifmacrondef _VerCheck2=>
!macro _VerCheck2_geq_imp l1 l2 r1 r2 _t _f
!insertmacro _LOGICLIB_TEMP
!define _VerCheck2_geq_imp _VerCheck2_geq_${__COUNTER__}
StrCpy $_LOGICLIB_TEMP 0
IntCmpU ${l1} ${r1} ${_VerCheck2_geq_imp}eq "" ${_VerCheck2_geq_imp}end
StrCpy $_LOGICLIB_TEMP 1
Goto ${_VerCheck2_geq_imp}end
${_VerCheck2_geq_imp}eq:
IntCmpU ${l2} ${r2} ${_VerCheck2_geq_imp}end "" ${_VerCheck2_geq_imp}end
StrCpy $_LOGICLIB_TEMP 1
${_VerCheck2_geq_imp}end:
!undef _VerCheck2_geq_imp
!insertmacro _= $_LOGICLIB_TEMP 0 `${_f}` `${_t}`
!macroend
!macro _VerCheck2=> _lhs _rhs _t _f
!insertmacro _VerCheck2_geq_imp ${_lhs} ${_rhs} `${_f}` `${_t}`
!macroend
!endif

!define MUI_LANGDLL_ALLLANGUAGES
!define MUI_LANGDLL_REGISTRY_ROOT "HKCU" 

!define MUI_COMPONENTSPAGE_SMALLDESC
!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE "lisence_english.rtf"
; !insertmacro MUI_PAGE_LICENSE $(MUILicense)
!insertmacro MUI_PAGE_DIRECTORY
Var StartMenuFolder
!insertmacro MUI_PAGE_STARTMENU "Application" $StartMenuFolder
!insertmacro MUI_PAGE_INSTFILES
; finish page readme or run insert here
!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

!insertmacro MUI_LANGUAGE "English"
!insertmacro MUI_LANGUAGE "SimpChinese" 
!insertmacro MUI_LANGUAGE "TradChinese"

!insertmacro LineRead

!define PackFolderName "${PRODUCT_NAME}"

LangString QuitProceStr ${LANG_SIMPCHINESE} "安装程序检测到程序正在运行，请退出程序后重试"
LangString QuitProceStr ${LANG_ENGLISH} "Setup detected that the program is running. Please exit the program and try again"
LangString QuitProceStr ${LANG_TRADCHINESE} "安裝程式檢測到程式正在運行，請退出程式後重試"

LangString UninstDone ${LANG_ENGLISH} "Thank you very much!$(^Name) was successfully removed from your computer."
LangString UninstDone ${LANG_SIMPCHINESE} "$(^Name) 已成功地从你的计算机移除."
LangString UninstDone ${LANG_TRADCHINESE} "$(^Name) 已成功地從你的計算機移除."

LangString IfInstalled ${LANG_ENGLISH} "$(^Name) is already installed. Overwrite the install(Data may loss)?"
LangString IfInstalled ${LANG_SIMPCHINESE} "$(^Name) 已经安装. 是否覆盖安装?(覆盖安装可能丢失数据)"
LangString IfInstalled ${LANG_TRADCHINESE} "$(^Name) 已經安裝. 是否覆蓋安裝?(覆蓋安裝可能丟失數據)"

Function .onInit
  !insertmacro MUI_LANGDLL_DISPLAY
  
  nsProcess::_FindProcess "${PRODUCT_NAME}.exe"   
  Pop $R0
  IntCmp $R0 0 is0 less0 lager0
  is0:
      MessageBox MB_ICONSTOP $(QuitProceStr)
      Quit	
  less0:
  lager0:
FunctionEnd

Function un.onInit
  !insertmacro MUI_UNGETLANGUAGE
FunctionEnd

Function un.onUninstSuccess
  HideWindow  
  MessageBox MB_ICONINFORMATION|MB_OK $(UninstDone)
  Abort
FunctionEnd

Section CheckIfInstalled
  ; Check to see if already installed
  ReadRegStr $R0 HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}" "UninstallString"
  IfFileExists $R0 +1 NotInstalled
  MessageBox MB_YESNO $(IfInstalled) IDYES NotInstalled IDNO Quit

Quit:
  Quit

NotInstalled:

SectionEnd

Section "${PRODUCT_NAME}"
  SetOutPath "$INSTDIR"
  File /r "..\build\RedRabbitSymbols\win\bin\${BUILD_TYPE}\*"

  SetOutPath "$LOCALAPPDATA\${PRODUCT_NAME}"
  File /nonfatal /a /r "..\build\RedRabbitSymbols\win\bin\${BUILD_TYPE}\machinecfg"
  File /nonfatal /a /r "..\build\RedRabbitSymbols\win\bin\${BUILD_TYPE}\config.json"

  WriteUninstaller $INSTDIR\uninstaller.exe
SectionEnd

# check whether vc is installed, if not, installed it
# https://learn.microsoft.com/en-us/cpp/windows/redistributing-visual-cpp-files?view=msvc-170
Section
  ReadRegDWORD $0 HKLM "SOFTWARE\Microsoft\VisualStudio\14.0\VC\Runtimes\x64" "Installed"
  ReadRegDWORD $1 HKLM "SOFTWARE\Microsoft\VisualStudio\14.0\VC\Runtimes\x64" "Major"
  ReadRegDWORD $2 HKLM "SOFTWARE\Microsoft\VisualStudio\14.0\VC\Runtimes\x64" "Minor"
  ${If} $0 <> 0
      # DetailPrint "Found version $1.$2"
      ${If} "$1 $2" VerCheck2=> "14 36"
          DetailPrint "The installed version is usable"
      ${Else}
          DetailPrint "Must install redist"
          # MessageBox MB_OK "Must install redist1 $0"
          ExecWait '"$INSTDIR\vc_redist.x64.exe" /q /norestart'
      ${EndIf}
  ${Else}
      # DetailPrint "Must install redist"
      ExecWait '"$INSTDIR\vc_redist.x64.exe" /q /norestart'
  ${EndIf}
SectionEnd

Section Shortcut
  CreateDirectory "$SMPROGRAMS\${PRODUCT_NAME} ${PRODUCT_VERSION}"
  CreateShortCut "$SMPROGRAMS\${PRODUCT_NAME} ${PRODUCT_VERSION}\Uninstall.lnk" "$INSTDIR\uninstaller.exe"
  CreateShortCut "$INSTDIR\Uninstall.lnk" "$INSTDIR\uninstaller.exe"
  CreateShortCut "$SMPROGRAMS\${PRODUCT_NAME} ${PRODUCT_VERSION}\${PRODUCT_NAME}.lnk" "$INSTDIR\${PRODUCT_NAME}.exe" "" "$INSTDIR\${PRODUCT_NAME}.ico"
  CreateShortCut "$DESKTOP\${PRODUCT_NAME}.lnk" "$INSTDIR\${PRODUCT_NAME}.exe" "" "$INSTDIR\${PRODUCT_NAME}.ico"
SectionEnd

Section WriteReigstry
  ; LCID string ->  LCID decimal
  ${Map.Create} LocaleMap
  ${Map.Set} LocaleMap 2052 0
  ${Map.Set} LocaleMap 1028 1
  ${Map.Set} LocaleMap 1033 2
  ${Map.Set} LocaleMap 1031 3
  ${Map.Set} LocaleMap 1034 4
  ${Map.Set} LocaleMap 2077 5
  ${Map.Set} LocaleMap 1036 6
  ${Map.Set} LocaleMap 1038 7
  ${Map.Set} LocaleMap 1040 8
  ${Map.Set} LocaleMap 1041 9
  ${Map.Set} LocaleMap 1042 10
  ${Map.Set} LocaleMap 1043 11
  ${Map.Set} LocaleMap 1045 12
  ${Map.Set} LocaleMap 1046 13
  ${Map.Set} LocaleMap 1049 14
  ${Map.Set} LocaleMap 1055 15
  ${Map.Set} LocaleMap 1025 16
  ${Map.Set} LocaleMap 1032 17
  ${Map.Set} LocaleMap 1029 18

  ${Map.Get} $2 LocaleMap $LANGUAGE
  ${If} $2 != "__NULL"
    WriteRegStr HKCU "Software\${PRODUCT_NAME}" "Installer Language" $2
  ${EndIf}

  WriteRegStr HKCU "${PRODUCT_DIR_REGKEY}" "" $INSTDIR
  WriteRegStr HKLM "${PRODUCT_UNINST_KEY}" "DisplayName" "${PRODUCT_NAME} ${PRODUCT_VERSION}"
  WriteRegStr HKLM "${PRODUCT_UNINST_KEY}" "UninstallString" "$INSTDIR\uninstaller.exe"
  WriteRegStr HKLM "${PRODUCT_UNINST_KEY}" "DisplayIcon" "$INSTDIR\${PRODUCT_NAME}.ico"
  WriteRegStr HKLM "${PRODUCT_UNINST_KEY}" "DisplayVersion" "${PRODUCT_VERSION}"  
  WriteRegStr HKLM "${PRODUCT_UNINST_KEY}" "Publisher" "${PRODUCT_PUBLISHER}"
  WriteRegStr HKLM "Software\${PRODUCT_NAME}" "" "$INSTDIR"
	
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}" "DisplayName"  "${PRODUCT_NAME}"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}" "UninstallString" '$INSTDIR\uninstaller.exe'
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}" "NoModify" 1
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}" "NoRepair" 1
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}" "DisplayIcon" '$INSTDIR\${PRODUCT_NAME}.ico'
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}" "InstallLocation" '$INSTDIR'
SectionEnd

Section un.Uninstall
  Delete "$SMPROGRAMS\${PRODUCT_NAME} ${PRODUCT_VERSION}\*.*"
  Delete "$SMPROGRAMS\${PRODUCT_NAME} ${PRODUCT_VERSION}\Uninstall.lnk"
  Delete "$SMPROGRAMS\${PRODUCT_NAME} ${PRODUCT_VERSION}\${PRODUCT_NAME}.lnk"
  RMDir "$SMPROGRAMS\${PRODUCT_NAME} ${PRODUCT_VERSION}"

  Delete "$DESKTOP\${PRODUCT_NAME}.lnk" 
  Delete "$INSTDIR\uninstall.exe"

  RMDir /r "$INSTDIR\*"
  RMDir "$INSTDIR" 

  DeleteRegKey HKCU "Software\${PRODUCT_NAME}"

  DeleteRegKey /ifempty HKCU "Software\${PRODUCT_NAME}"  
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
  DeleteRegKey HKLM "SOFTWARE\${PRODUCT_NAME}"
  DeleteRegKey HKLM "${PRODUCT_UNINST_KEY}"
  DeleteRegKey HKLM "${PRODUCT_DIR_REGKEY}"  
  DeleteRegKey HKCU "${PRODUCT_DIR_REGKEY}"  
SectionEnd