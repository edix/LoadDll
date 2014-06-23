
#ifdef _WIN64
extern "C" { DWORD64 _fastcall LDE(DWORD64 dwAddr, int nSize); }
#else
extern "C" { DWORD _stdcall LDE(DWORD dwAddr, int nSize); }
#endif




