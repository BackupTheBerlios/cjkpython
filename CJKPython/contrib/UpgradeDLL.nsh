; Macro - Upgrade DLL File
; Written by Joost Verburg
; ------------------------
;
; Parameters:
; LOCALFILE   - Location of the new DLL file (on the compiler system)
; DESTFILE    - Location of the DLL file that should be upgraded
;               (on the user's system)
; TEMPBASEDIR - Directory on the user's system to store a temporary file
;               when the system has to be rebooted.
;               For Win9x support, this should be on the same volume as the
;               DESTFILE!
;               The Windows temp directory could be located on any volume,
;               so you cannot use this directory.
;
; Note: If you want to support Win9x, you can only use short filenames
; (8.3).
;
; Example of usage:
; !insertmacro UpgradeDLL "dllname.dll" "$SYSDIR\dllname.dll" "$SYSDIR"
;
; !define UPGRADEDLL_NOREGISTER if you want to upgrade a DLL that cannot be
; registered
;
; Note that this macro sets overwrite to ON (the default) when it has been
; inserted.
; If you are using another setting, set it again after inserting the macro.


!macro UpgradeDLL LOCALFILE DESTFILE TEMPBASEDIR

  Push $R0
  Push $R1
  Push $R2
  Push $R3

  ;------------------------
  ;Check file and version

  IfFileExists "${DESTFILE}" "" "copy_${LOCALFILE}"

  ClearErrors
    GetDLLVersionLocal "${LOCALFILE}" $R0 $R1
    GetDLLVersion "${DESTFILE}" $R2 $R3
  IfErrors "upgrade_${LOCALFILE}"

  IntCmpU $R0 $R2 "" "done_${LOCALFILE}" "upgrade_${LOCALFILE}"
  IntCmpU $R1 $R3 \
    "done_${LOCALFILE}" "done_${LOCALFILE}" "upgrade_${LOCALFILE}"

  ;------------------------
  ;Let's upgrade the DLL!

  SetOverwrite try

  "upgrade_${LOCALFILE}:"
    !ifndef UPGRADEDLL_NOREGISTER
      ;Unregister the DLL
      UnRegDLL "${DESTFILE}"
    !endif

  ;------------------------
  ;Try to copy the DLL directly

  ClearErrors
    StrCpy $R0 "${DESTFILE}"
    Call ":file_${LOCALFILE}"
  IfErrors "" "noreboot_${LOCALFILE}"

  ;------------------------
  ;DLL is in use. Copy it to a temp file and Rename it on reboot.

  GetTempFileName $R0 "${TEMPBASEDIR}"
    Call ":file_${LOCALFILE}"
  Rename /REBOOTOK $R0 "${DESTFILE}"

  ;------------------------
  ;Register the DLL on reboot

  !ifndef UPGRADEDLL_NOREGISTER
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\RunOnce" \
      "Register ${DESTFILE}" '"$SYSDIR\rundll32.exe" \
      "${DESTFILE}",DllRegisterServer'
  !endif

  Goto "done_${LOCALFILE}"

  ;------------------------
  ;DLL does not exist - just extract

  "copy_${LOCALFILE}:"
    StrCpy $R0 "${DESTFILE}"
    Call ":file_${LOCALFILE}"

  ;------------------------
  ;Register the DLL

  "noreboot_${LOCALFILE}:"
    !ifndef UPGRADEDLL_NOREGISTER
      RegDLL "${DESTFILE}"
    !endif

  ;------------------------
  ;Done

  "done_${LOCALFILE}:"

  Pop $R3
  Pop $R2
  Pop $R1
  Pop $R0

  ;------------------------
  ;End

  Goto "end_${LOCALFILE}"

  ;------------------------
  ;Called to extract the DLL

  "file_${LOCALFILE}:"
    File /oname=$R0 "${LOCALFILE}"
    Return

  "end_${LOCALFILE}:"

 ;------------------------
 ;Set overwrite to default
 ;(was set to TRY above)

 SetOverwrite on

!macroend
