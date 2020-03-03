Echo deleting folders

@RD /S /Q "Binaries"
@RD /S /Q "Saved"
@RD /S /Q "Intermediate"
@RD /S /Q ".vs"
DEL "*.sln" /s /f /q

