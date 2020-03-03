Echo deleting folders

@RD /S /Q "Binaries"
@RD /S /Q "Saved"
@RD /S /Q "Intermediate"
@RD /S /Q ".vs"
del ".sln" /s /f /q