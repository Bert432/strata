;Installer for Strata
;Written by Albert Kottke

;--------------------------------
;Include Modern UI
!include "MUI.nsh"
;--------------------------------
;Revision of the respository
!system 'python getSvnVersion.py "!define REVISION" > %TEMP%\revision.nsh'
!include "$%TEMP%\revision.nsh"

;--------------------------------
; Path of the Strata executable
!Define STRATA_PATH 'build_${ARCH}/release'

;--------------------------------
; Path to libraries
!Define MINGW_PATH "C:\msys64\${ARCH}"
;!Define QT_PATH "C:\msys64\mingw64\bin"
;!Define MINGW_PATH "C:\Program Files\mingw-w64\i686-4.8.2-posix-dwarf-rt_v3-rev4\bin"

;--------------------------------
;Variables
Var MUI_TEMP
Var STARTMENU_FOLDER

;--------------------------------
;General

;Name and file
Name "Strata"
OutFile "Strata-${ARCH}-r${REVISION}.exe"

;Default installation folder
InstallDir ${INSTDIR}

;Get installation folder from registry if available
InstallDirRegKey HKCU "Software\Strata" ""

;Vista redirects $SMPROGRAMS to all users without this
RequestExecutionLevel admin

;Set the type of compress to LZMA
SetCompressor lzma

;--------------------------------
;Interface Settings

!define MUI_ABORTWARNING

;--------------------------------
;Pages
!insertmacro MUI_PAGE_LICENSE "COPYING"
!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_DIRECTORY

; Start menu folder page configuration
!define MUI_STARTMENUPAGE_REGISTRY_ROOT "HKCU" 
!define MUI_STARTMENUPAGE_REGISTRY_KEY "Software\Strata"
!define MUI_STARTMENUPAGE_REGISTRY_VALUENAME "Start Menu Folder"

!insertmacro MUI_PAGE_STARTMENU Strata $STARTMENU_FOLDER
!insertmacro MUI_PAGE_INSTFILES
; Finish page configuration
!define MUI_FINISHPAGE_RUN "$INSTDIR\strata.exe"
!define MUI_FINISHPAGE_SHOWREADME "$INSTDIR\readme.txt"
!define MUI_FINISHPAGE_SHOWREADME_NOTCHECKED
!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

;--------------------------------
;Languages

!insertmacro MUI_LANGUAGE "English"

;--------------------------------
; Association macros -- from: http://nsis.sourceforge.net/FileAssoc
!macro APP_ASSOCIATE EXT FILECLASS DESCRIPTION ICON COMMANDTEXT COMMAND
; Backup the previously associated file class
ReadRegStr $R0 HKCR ".${EXT}" ""
WriteRegStr HKCR ".${EXT}" "${FILECLASS}_backup" "$R0"

WriteRegStr HKCR ".${EXT}" "" "${FILECLASS}"

WriteRegStr HKCR "${FILECLASS}" "" `${DESCRIPTION}`
WriteRegStr HKCR "${FILECLASS}\DefaultIcon" "" `${ICON}`
WriteRegStr HKCR "${FILECLASS}\shell" "" "open"
WriteRegStr HKCR "${FILECLASS}\shell\open" "" `${COMMANDTEXT}`
WriteRegStr HKCR "${FILECLASS}\shell\open\command" "" `${COMMAND}`
!macroend

!macro APP_UNASSOCIATE EXT FILECLASS
; Backup the previously associated file class
ReadRegStr $R0 HKCR ".${EXT}" `${FILECLASS}_backup`
WriteRegStr HKCR ".${EXT}" "" "$R0"

DeleteRegKey HKCR `${FILECLASS}`
!macroend

;--------------------------------
;Installer Sections

Section "!Core Files" SecProgram
SectionIn RO
SetOutPath "$INSTDIR"

;Main Strata files
File "${STRATA_PATH}\strata.exe"
File "/oname=$INSTDIR\readme.txt" "README"

;Icons
File "/oname=$INSTDIR\strata.ico" "resources\images\application-icon.ico"
File "/oname=$INSTDIR\strata-data.ico" "resources\images\file-data.ico"

;Main libraries
File "${MINGW_PATH}\bin\Qt5Core.dll"
File "${MINGW_PATH}\bin\Qt5Gui.dll"
File "${MINGW_PATH}\bin\Qt5Network.dll"
File "${MINGW_PATH}\bin\Qt5OpenGL.dll"
File "${MINGW_PATH}\bin\Qt5PrintSupport.dll"
File "${MINGW_PATH}\bin\Qt5Script.dll"
File "${MINGW_PATH}\bin\Qt5Svg.dll"
File "${MINGW_PATH}\bin\Qt5Widgets.dll"
File "${MINGW_PATH}\bin\Qt5Xml.dll"
File "${MINGW_PATH}\bin\libbz2-1.dll"
File "${MINGW_PATH}\bin\libfftw3-3.dll"
File "${MINGW_PATH}\bin\libfreetype-6.dll"
File "${MINGW_PATH}\bin\libglib-2.0-0.dll"
File "${MINGW_PATH}\bin\libgsl-19.dll"
File "${MINGW_PATH}\bin\libgslcblas-0.dll"
File "${MINGW_PATH}\bin\libharfbuzz-0.dll"
File "${MINGW_PATH}\bin\libiconv-2.dll"
File "${MINGW_PATH}\bin\libicudt56.dll"
File "${MINGW_PATH}\bin\libicuin56.dll"
File "${MINGW_PATH}\bin\libicuuc56.dll"
File "${MINGW_PATH}\bin\libintl-8.dll"
File "${MINGW_PATH}\bin\libpcre16-0.dll"
File "${MINGW_PATH}\bin\libpng16-16.dll"
File "${MINGW_PATH}\bin\libstdc++-6.dll"
File "${MINGW_PATH}\bin\libwinpthread-1.dll"
File "${MINGW_PATH}\bin\qwt.dll"
File "${MINGW_PATH}\bin\zlib1.dll"
File "${MINGW_PATH}\bin\libgcc_s_*-1.dll"

;Plugins for SVG icons
SetOutPath "$INSTDIR\iconengines" 
File "${MINGW_PATH}\share\qt5\plugins\iconengines\qsvgicon.dll"
SetOutPath "$INSTDIR\imageformats" 
File "${MINGW_PATH}\share\qt5\plugins\imageformats\qsvg.dll"
SetOutPath "$INSTDIR\platforms" 
File "${MINGW_PATH}\share\qt5\plugins\platforms\qwindows.dll"

;Store installation folder
WriteRegStr HKCU "Software\Strata" "" $INSTDIR

;Create uninstaller
WriteUninstaller "$INSTDIR\Uninstall.exe"

!insertmacro MUI_STARTMENU_WRITE_BEGIN Strata
;Create shortcuts
CreateDirectory "$SMPROGRAMS\$STARTMENU_FOLDER"
CreateShortCut "$SMPROGRAMS\$STARTMENU_FOLDER\Strata.lnk" "$INSTDIR\Strata.exe"
CreateShortCut "$SMPROGRAMS\$STARTMENU_FOLDER\Uninstall.lnk" "$INSTDIR\Uninstall.exe"
!insertmacro MUI_STARTMENU_WRITE_END

SectionEnd

Section "Documentation" SecDocumentation
SetOutPath $INSTDIR

File "manual\manual.pdf"
CreateShortCut "$SMPROGRAMS\$STARTMENU_FOLDER\Manual.lnk" "$INSTDIR\manual.pdf"

SectionEnd

Section "Examples" SecExamples

SetOutPath "$INSTDIR\examples"

;Files to install
File "example\example-1-td.strata"
File "example\example-2-td.strata"
File "example\example-3-rvt.strata"
File "example\suite-10-1.csv"
File "example\response-spectrum.csv"
File /r "example\*.AT2"

CreateShortCut "$SMPROGRAMS\$STARTMENU_FOLDER\Examples.lnk" "$INSTDIR\examples\"

SectionEnd

Section "File Associations" SecAssociation
;File associations
!insertmacro APP_ASSOCIATE "strata" "strata.data" "Strata Data" "$INSTDIR\strata-data.ico" "Edit/Run with Strata" "$INSTDIR\strata.exe $\"%1$\""
!insertmacro APP_ASSOCIATE "stratahr" "stratahr.data" "Human Readable Strata Data" "$INSTDIR\strata-data.ico" "Edit/Run with Strata" "$INSTDIR\strata.exe $\"%1$\""
SectionEnd

Section /o "Source" SecSource

SetOutPath "$INSTDIR\src"

;Files to install
File "/oname=..\strata.pro" "strata.pro"
File "src\*.h"
File "src\*.cpp"

; 13:07 <Dirm> I normally just do something like exec("cmd /c start http://www.google.com")
; CreateShortCut "$SMPROGRAMS\$STARTMENU_FOLDER\Source Documentation.lnk" "cmd.exe" "/c weee"
SectionEnd

;--------------------------------
;Descriptions
;Language strings
LangString DESC_SecProgram ${LANG_ENGLISH} "Binaries required to run Strata."
LangString DESC_SecDocumentation ${LANG_ENGLISH} "User manual."
LangString DESC_SecExamples ${LANG_ENGLISH} "Example input files."
LangString DESC_SecAssociation ${LANG_ENGLISH} "Associate Strata data files with Strata."
LangString DESC_SecSource ${LANG_ENGLISH} "Source code."


;Assign language strings to sections
!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
!insertmacro MUI_DESCRIPTION_TEXT ${SecProgram} $(DESC_SecProgram)
!insertmacro MUI_DESCRIPTION_TEXT ${SecDocumentation} $(DESC_SecDocumentation)
!insertmacro MUI_DESCRIPTION_TEXT ${SecExamples} $(DESC_SecExamples)
!insertmacro MUI_DESCRIPTION_TEXT ${SecAssociation} $(DESC_SecAssociation)
!insertmacro MUI_DESCRIPTION_TEXT ${SecSource} $(DESC_SecSource)
!insertmacro MUI_FUNCTION_DESCRIPTION_END

;--------------------------------
;Uninstaller Section

Section "Uninstall"
;Remove the files
Delete "$INSTDIR\strata.exe"
Delete "$INSTDIR\readme.txt"
Delete "$INSTDIR\manual.pdf"
Delete "$INSTDIR\Qt5Core.dll"
Delete "$INSTDIR\Qt5Gui.dll"
Delete "$INSTDIR\Qt5Network.dll"
Delete "$INSTDIR\Qt5OpenGL.dll"
Delete "$INSTDIR\Qt5PrintSupport.dll"
Delete "$INSTDIR\Qt5Script.dll"
Delete "$INSTDIR\Qt5Svg.dll"
Delete "$INSTDIR\Qt5Widgets.dll"
Delete "$INSTDIR\Qt5Xml.dll"
Delete "$INSTDIR\libbz2-1.dll"
Delete "$INSTDIR\libfftw3-3.dll"
Delete "$INSTDIR\libfreetype-6.dll"
Delete "$INSTDIR\libglib-2.0-0.dll"
Delete "$INSTDIR\libgsl-19.dll"
Delete "$INSTDIR\libgslcblas-0.dll"
Delete "$INSTDIR\libharfbuzz-0.dll"
Delete "$INSTDIR\libiconv-2.dll"
Delete "$INSTDIR\libicudt56.dll"
Delete "$INSTDIR\libicuin56.dll"
Delete "$INSTDIR\libicuuc56.dll"
Delete "$INSTDIR\libintl-8.dll"
Delete "$INSTDIR\libpcre16-0.dll"
Delete "$INSTDIR\libpng16-16.dll"
Delete "$INSTDIR\libstdc++-6.dll"
Delete "$INSTDIR\libwinpthread-1.dll"
Delete "$INSTDIR\qwt.dll"
Delete "$INSTDIR\zlib1.dll"
Delete "$INSTDIR\libgcc_s_*-1.dll"
Delete "$INSTDIR\iconengines\qsvgicon.dll"
RMDir  "$INSTDIR\iconengines"
Delete "$INSTDIR\imageformats\qsvg.dll"
RMDir  "$INSTDIR\imageformats"
Delete "$INSTDIR\platforms\qwindows.dll"
RMDir  "$INSTDIR\platforms" 

Delete "$INSTDIR\examples\example-1-td.strata"
Delete "$INSTDIR\examples\example-2-td.strata"
Delete "$INSTDIR\examples\example-3-rvt.strata"
Delete "$INSTDIR\examples\suite-10-1.csv"
Delete "$INSTDIR\examples\response-spectrum.csv"
RMDIR /r "$INSTDIR\examples\motions"
RMDir  "$INSTDIR\examples"
Delete "$INSTDIR\strata.pro"
Delete "$INSTDIR\strata.ico"
Delete "$INSTDIR\strata-data.ico"
Delete "$INSTDIR\Uninstall.exe"

;Remove all source files
RMDir /r "$INSTDIR\src"

;Remove the install directory
RMDir $INSTDIR

;Remove the association with Strata
!insertmacro APP_UNASSOCIATE "strata" "strata.datafile"

;Remove the Start menu items
!insertmacro MUI_STARTMENU_GETFOLDER Strata $MUI_TEMP

;Delete the shortcuts
Delete "$SMPROGRAMS\$MUI_TEMP\Strata.lnk"
Delete "$SMPROGRAMS\$MUI_TEMP\Uninstall.lnk"
Delete "$SMPROGRAMS\$MUI_TEMP\Manual.lnk"
Delete "$SMPROGRAMS\$MUI_TEMP\Examples.lnk"

;Delete empty start menu parent diretories
StrCpy $MUI_TEMP "$SMPROGRAMS\$MUI_TEMP"

startMenuDeleteLoop:
ClearErrors
RMDir $MUI_TEMP
GetFullPathName $MUI_TEMP "$MUI_TEMP\.."

IfErrors startMenuDeleteLoopDone

StrCmp $MUI_TEMP $SMPROGRAMS startMenuDeleteLoopDone startMenuDeleteLoop
startMenuDeleteLoopDone:

DeleteRegKey /ifempty HKCU "Software\Strata"
SectionEnd
