////////////////////////////////////////////////////////////////////////////
// mzc2mini.h - MZC2 mini
// Copyright (C) 2012-2017 Katayama Hirofumi MZ.  All rights reserved.
////////////////////////////////////////////////////////////////////////////
#ifndef MZC2MINI
#define MZC2MINI

#include <windows.h>
#include <vector>
#include <cassert>
#include <string>
#include <tchar.h>

////////////////////////////////////////////////////////////////////////////
// tstring

#ifndef tstring
    #ifdef _UNICODE
        #define tstring std::wstring
    #else
        #define tstring std::string
    #endif
#endif

////////////////////////////////////////////////////////////////////////////
// MzcAssert, MzcVerify, MzcStaticAssert

#ifndef MzcAssert
    #define MzcAssert(exp) assert(exp)
#endif
#ifdef NDEBUG
    #define MzcVerify(exp) (exp)
#else
    #define MzcVerify(exp) assert(exp)
#endif

#define MzcStaticAssert(exp) C_ASSERT(exp)

////////////////////////////////////////////////////////////////////////////
// LOLONG, HILONG, MAKELONGLONG

#ifndef LOLONG
    #define LOLONG(dwl) ((DWORD)(dwl))
#endif
#ifndef HILONG
    #define HILONG(dwl) ((DWORD)(((dwl) >> 32) & 0xFFFFFFFF))
#endif
#ifndef MAKELONGLONG
    #define MAKELONGLONG(lo,hi) (((DWORDLONG)(hi) << 32) | (DWORD)(lo))
#endif

////////////////////////////////////////////////////////////////////////////
// Split and Join

VOID MzcSplit(std::vector<tstring>& v, const tstring& s, TCHAR separator);
VOID MzcSplit(std::vector<tstring>& v, const tstring& s, LPCTSTR separators);
VOID Join(tstring& s, const std::vector<tstring>& v, LPCTSTR separator);

////////////////////////////////////////////////////////////////////////////
// MZC2 MSecurityAttributes

class MSecurityAttributes : public SECURITY_ATTRIBUTES
{
public:
    MSecurityAttributes(BOOL bInherit = TRUE, LPVOID pSecurityDescriptor = NULL);
};

////////////////////////////////////////////////////////////////////////////
// MZC2 file functions

#define MzcBaseName GetFileTitle
VOID MzcDirName(LPCTSTR pszPath, LPTSTR pszDirName);
VOID MzcAddBackslash(LPTSTR pszPath);
VOID MzcRemoveBackslashA(LPTSTR pszPath);
LPTSTR MzcGetFileTitle(LPTSTR pszPath);
LPTSTR MzcGetDotExt(LPTSTR pszPath);
VOID MzcSetDotExt(LPTSTR pszPath, LPCTSTR pszDotExt);
BOOL MzcFileExists(LPCTSTR pszFileName);
BOOL MzcFileGetContents(LPCTSTR pszFileName, std::vector<BYTE>& v);
BOOL MzcFilePutContents(LPCTSTR pszFileName, LPCVOID pv, DWORD cb);
BOOL MzcFileSize(LPCTSTR pszFileName, LPDWORD pcbFile);
BOOL MzcIsDir(LPCTSTR pszFileName);
BOOL MzcIsFile(LPCTSTR pszFileName);
BOOL MzcGlob(std::vector<tstring>& vPaths, LPCTSTR pszWildcard);

////////////////////////////////////////////////////////////////////////////
// MZC2 MFile

class MFile
{
public:
    MFile();
    MFile(HANDLE hHandle);
    ~MFile();

    operator HANDLE() const;
    operator PHANDLE();
    PHANDLE operator&();
    bool operator!() const;
    bool operator==(HANDLE hHandle) const;
    bool operator!=(HANDLE hHandle) const;

    MFile& operator=(HANDLE hHandle);
    VOID Attach(HANDLE hHandle);
    HANDLE Detach();
    BOOL CloseHandle();

    BOOL DuplicateHandle(PHANDLE phHandle, BOOL bInherit);
    BOOL DuplicateHandle(
        PHANDLE phHandle, BOOL bInherit, DWORD dwDesiredAccess);
    DWORD WaitForSingleObject(DWORD dwTimeout = INFINITE);

    BOOL PeekNamedPipe(
        LPVOID pBuffer = NULL,
        DWORD cbBuffer = 0,
        LPDWORD pcbRead = NULL,
        LPDWORD pcbAvail = NULL,
        LPDWORD pBytesLeft = NULL);
    BOOL ReadFile(LPVOID pBuffer, DWORD cbToRead, LPDWORD pcbRead,
        LPOVERLAPPED pOverlapped = NULL);
    BOOL WriteFile(LPCVOID pBuffer, DWORD cbToWrite, LPDWORD pcbWritten,
        LPOVERLAPPED pOverlapped = NULL);
    BOOL WriteSzA(LPCSTR psz, LPDWORD pcbWritten,
        LPOVERLAPPED pOverlapped = NULL);
    BOOL WriteSzW(LPCWSTR psz, LPDWORD pcbWritten,
        LPOVERLAPPED pOverlapped = NULL);
    BOOL WriteSz(LPCTSTR psz, LPDWORD pcbWritten,
        LPOVERLAPPED pOverlapped = NULL);

    BOOL WriteBinary(LPCVOID pv, DWORD cb);
    BOOL WriteSzA(LPCSTR psz);
    BOOL WriteSzW(LPCWSTR psz);
    BOOL WriteSz(LPCTSTR psz);
    BOOL __cdecl WriteFormatA(LPCSTR pszFormat, ...);
    BOOL __cdecl WriteFormatW(LPCWSTR pszFormat, ...);
    BOOL __cdecl WriteFormat(LPCTSTR pszFormat, ...);

    BOOL OpenFileForInput(
        LPCTSTR pszFileName, DWORD dwFILE_SHARE_ = FILE_SHARE_READ);
    BOOL OpenFileForOutput(
        LPCTSTR pszFileName, DWORD dwFILE_SHARE_ = FILE_SHARE_READ);
    BOOL OpenFileForRandom(
        LPCTSTR pszFileName, DWORD dwFILE_SHARE_ = FILE_SHARE_READ);
    BOOL OpenFileForAppend(
        LPCTSTR pszFileName, DWORD dwFILE_SHARE_ = FILE_SHARE_READ);

    BOOL CreateFile(LPCTSTR pszFileName, DWORD dwDesiredAccess,
        DWORD dwShareMode, LPSECURITY_ATTRIBUTES pSA,
        DWORD dwCreationDistribution,
        DWORD dwFlagsAndAttributes = FILE_ATTRIBUTE_NORMAL,
        HANDLE hTemplateFile = NULL);
    DWORD GetFileSize(LPDWORD pdwHighPart = NULL) const;
    BOOL SetEndOfFile();
    DWORD SetFilePointer(
        LONG nDeltaLow, PLONG pnDeltaHigh = NULL, DWORD dwOrigin = FILE_BEGIN);
    VOID SeekToBegin();
    DWORD SeekToEnd();
    BOOL FlushFileBuffers();
    BOOL GetFileTime(
        LPFILETIME pftCreate = NULL,
        LPFILETIME pftLastAccess = NULL,
        LPFILETIME pftLastWrite = NULL) const;

public:
    HANDLE m_hHandle;
};

////////////////////////////////////////////////////////////////////////////
// MZC2 MFindFile

class MFindFile
{
public:
    MFindFile();
    MFindFile(LPCTSTR pszWildcard);
    ~MFindFile();

    bool operator!() const;
    BOOL FindFirstFile(LPCTSTR pszWildcard = NULL);
    BOOL FindNextFile();
    ULONGLONG GetFileSize() const;
    BOOL GetFileName(LPTSTR pszFileName, INT cchLength) const;
    BOOL GetFilePath(LPTSTR pszFilePath, INT cchLength) const;
    BOOL GetRoot(LPTSTR pszRoot, INT cchLength) const;
    LPCTSTR GetFileName() const;
    tstring GetFilePath() const;
    LPCTSTR GetRoot() const;
    BOOL GetLastWriteTime(FILETIME* pTimeStamp) const;
    BOOL GetLastAccessTime(FILETIME* pTimeStamp) const;
    BOOL GetCreationTime(FILETIME* pTimeStamp) const;
    BOOL MatchesMask(DWORD dwMask) const;
    BOOL IsDots() const;    // "." or ".."
    BOOL IsReadOnly() const;
    BOOL IsDirectory() const;
    BOOL IsCompressed() const;
    BOOL IsSystem() const;
    BOOL IsHidden() const;
    BOOL IsTemporary() const;
    BOOL IsNormal() const;
    BOOL IsArchived() const;
    VOID Close();

protected:
    HANDLE m_hFind;
    TCHAR m_szRoot[MAX_PATH];
#ifndef NDEBUG
    BOOL m_bFound;
#endif
    WIN32_FIND_DATA m_find;
};

////////////////////////////////////////////////////////////////////////////
// MZC2 MProcessMaker

class MProcessMaker
{
public:
    MProcessMaker();
    ~MProcessMaker();

    bool operator!() const;
    HANDLE GetHandle() const;
    DWORD GetExitCode() const;

    VOID SetShowWindow(INT nCmdShow = SW_HIDE);
    VOID SetCreationFlags(DWORD dwFlags = CREATE_NEW_CONSOLE);
    VOID SetCurrentDirectory(LPCTSTR pszCurDir);

    VOID SetDesktop(LPTSTR lpDesktop);
    VOID SetTitle(LPTSTR lpTitle);
    VOID SetPosition(DWORD dwX, DWORD dwY);
    VOID SetSize(DWORD dwXSize, DWORD dwYSize);
    VOID SetCountChars(DWORD dwXCountChars, DWORD dwYCountChars);
    VOID SetFillAttirbutes(DWORD dwFillAttribute);

    VOID SetStdInput(HANDLE hStdIn);
    VOID SetStdOutput(HANDLE hStdOut);
    VOID SetStdError(HANDLE hStdErr);
    BOOL PrepareForRedirect(
        PHANDLE phInputWrite, PHANDLE phOutputRead,
        PHANDLE phErrorRead);

    BOOL CreateProcess(
        LPCTSTR pszAppName, LPCTSTR pszCommandLine = NULL,
        LPCTSTR pszzEnvironment = NULL, BOOL bInherit = TRUE,
        LPSECURITY_ATTRIBUTES lpProcessAttributes = NULL,
        LPSECURITY_ATTRIBUTES lpThreadAttributes = NULL);
    BOOL CreateProcessAsUser(
        HANDLE hToken, LPCTSTR pszAppName, LPCTSTR pszCommandLine = NULL,
        LPCTSTR pszzEnvironment = NULL, BOOL bInherit = TRUE,
        LPSECURITY_ATTRIBUTES lpProcessAttributes = NULL,
        LPSECURITY_ATTRIBUTES lpThreadAttributes = NULL);
    DWORD WaitForExit(DWORD dwTimeout = INFINITE);
    BOOL TerminateProcess(UINT uExitCode);
    BOOL IsRunning() const;
    VOID Close();

public:
    PROCESS_INFORMATION m_pi;
    STARTUPINFO m_si;

protected:
    DWORD   m_dwCreationFlags;
    LPCTSTR m_pszCurDir;
};

////////////////////////////////////////////////////////////////////////////

#endif  // ndef MZC2MINI
