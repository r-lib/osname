
#include <R.h>
#include <Rinternals.h>
#include <R_ext/Rdynload.h>

#ifdef WIN32

/* src/library/utils/src/windows/util.c */

#include <windows.h>

typedef void (WINAPI *PGNSI)(LPSYSTEM_INFO);

SEXP winver() {
  char ver[256];
  OSVERSIONINFOEX osvi;

  osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
  if(!GetVersionEx((OSVERSIONINFO *)&osvi)) {
    return mkString("Windows (unknown version");
  }

  /* see http://msdn2.microsoft.com/en-us/library/ms724429.aspx
     for ways to get more info.
     Pre-NT versions are all 4.x, so no need to separate test.
     See also http://msdn.microsoft.com/en-us/library/ms724832.aspx
     https://msdn.microsoft.com/en-us/library/windows/desktop/ms724833%28v=vs.85%29.aspx
     for version number naming.
  */
  if(osvi.dwMajorVersion >= 5) {
    char *desc = "", *type="";
    SYSTEM_INFO si;
    // future-proof
    snprintf(ver, 256, "%d.%d",
	     (int) osvi.dwMajorVersion, (int) osvi.dwMinorVersion);
    if(osvi.dwMajorVersion == 10) {
      if(osvi.wProductType == VER_NT_WORKSTATION) {
	desc = "10";
      } else {
	desc = "Server";
      }

    } else if(osvi.dwMajorVersion == 6) {
      // see See https://msdn.microsoft.com/en-us/library/windows/desktop/ms724451%28v=vs.85%29.aspx for the >= here.
      if(osvi.wProductType == VER_NT_WORKSTATION) {
	if(osvi.dwMinorVersion == 0) desc = "Vista";
	else if(osvi.dwMinorVersion == 1) desc = "7";
	else if(osvi.dwMinorVersion == 2) desc = ">= 8";
	else if(osvi.dwMinorVersion == 3) desc = "8.1";
	else desc = "> 8.1";
      } else {
	if(osvi.dwMinorVersion == 0) desc = "Server 2008";
	else if(osvi.dwMinorVersion == 1) desc = "Server 2008 R2";
	else if(osvi.dwMinorVersion == 2) desc = "Server >= 2012";
	else if(osvi.dwMinorVersion == 3) desc = "Server 2012 R2";
	else desc = "Server > 2012";
      }

    } else if(osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0) {
      desc = "2000";

    } else if(osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 1) {
      desc = "XP";

    } else if(osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 2) {
      if(osvi.wProductType == VER_NT_WORKSTATION) {
	desc = "XP Professional";
      } else {
	desc = "Server 2003";
      }
    }

    /* GetNativeSystemInfo is XP or later */
    GetNativeSystemInfo(&si);
    if(si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64)
      type = " x64";

    if(osvi.wServicePackMajor > 0) {
      snprintf(ver, 256,
	       "Windows %s%s SP %d",
	       desc, type,
	       LOWORD(osvi.dwBuildNumber),
	       (int) osvi.wServicePackMajor);
    } else {
      snprintf(ver, 256,
	       "Windows %s%s",
	       desc, type,
	       LOWORD(osvi.dwBuildNumber));
    }
  } else { /* should not get here */
    snprintf(ver, 256, "Windows %d.%d %s",
	     (int) osvi.dwMajorVersion, (int) osvi.dwMinorVersion,
	     LOWORD(osvi.dwBuildNumber), osvi.szCSDVersion);
  }

  return mkString(ver);
}

#else

SEXP winver() {
  error("This function only works on Windows systems");
}

#endif

static const R_CallMethodDef callMethods[]  = {
  { "winver", (DL_FUNC) &winver, 0 },
  { NULL, NULL, 0 }
};

void R_init_osname(DllInfo *dll) {
  R_registerRoutines(dll, NULL, callMethods, NULL, NULL);
  R_useDynamicSymbols(dll, FALSE);
  R_forceSymbols(dll, TRUE);
}
