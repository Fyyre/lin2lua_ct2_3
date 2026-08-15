void Tick(float DeltaTime);
void PostRender(struct UObject * pCanvas);
void PreRender(struct UObject * pCanvas);
int KeyEvent(DWORD Key, DWORD Action, float Value);
void Log(char * pFormat, ...);
void HexDump(char * pData, int Len);
void NullExport();
