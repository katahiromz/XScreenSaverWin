////////////////////////////////////////////////////////////////////////////
// mzc2mini.cpp - MZC2 mini
// Copyright (C) 2012-2013 Katayama Hirofumi MZ.  All rights reserved.
////////////////////////////////////////////////////////////////////////////

#define _CRT_SECURE_NO_WARNINGS
#include "mzc2mini.h"

////////////////////////////////////////////////////////////////////////////

VOID MzcSplit(std::vector<tstring>& v, const tstring& s, TCHAR separator)
{
    size_t i = 0, j = s.find_first_of(separator);

    v.clear();
    while(j != tstring::npos)
    {
        v.push_back(s.substr(i, j - i));
        i = j + 1;
        j = s.find_first_of(separator, i);
    }
    v.push_back(s.substr(i, -1));
}

VOID MzcSplit(std::vector<tstring>& v, const tstring& s, LPCTSTR separators)
{
    size_t i = s.find_first_not_of(separators), n = s.size();

    v.clear();
    while (0 <= i && i < n)
    {
        size_t stop = s.find_first_of(separators, i);
        if (stop == tstring::npos || stop > n) stop = n;
        v.push_back(s.substr(i, stop - i));
        i = s.find_first_not_of(separators, stop + 1);
    }
}

VOID Join(tstring& s, const std::vector<tstring>& v, LPCTSTR separator)
{
    size_t i, c;

    s.clear();
    c = v.size();
    if (c > 0)
    {
        s = v[0];
        for(i = 1; i < c; i++)
        {
            s += separator;
            s += v[i];
        }
    }
}

////////////////////////////////////////////////////////////////////////////
// MZC2 MSecurityAttributes

MSecurityAttributes::MSecurityAttributes(
    BOOL bInherit/* = TRUE*/, LPVOID pSecurityDescriptor/* = NULL*/)
{
    nLength = sizeof(SECURITY_ATTRIBUTES);
    lpSecurityDescriptor = pSecurityDescriptor;
    bInheritHandle = bInherit;
}

////////////////////////////////////////////////////////////////////////////
// MZC2 file functions

VOID MzcDirName(LPCTSTR pszPath, LPTSTR pszDirName)
{
    using namespace std;
    TCHAR szPath[MAX_PATH];
    lstrcpyn(szPath, pszPath, MAX_PATH);
    LPTSTR pch = MzcGetFileTitle(szPath);
    if (pch != NULL)
    {
        *(pch - 1) = _T('\0');
        lstrcpy(pszDirName, szPath);
    }
    else
    {
        pszDirName[0] = _T('.');
        pszDirName[1] = _T('\0');
    }
}

VOID MzcAddBackslash(LPTSTR pszPath)
{
    using namespace std;
    SIZE_T cchPath = lstrlen(pszPath);
    LPTSTR pchPrev = CharPrev(pszPath, pszPath + cchPath);
    if (*pchPrev != _T('\\') && *pchPrev != _T('/'))
    {
        pszPath[cchPath++] = _T('\\');
        pszPath[cchPath] = _T('\0');
    }
}

VOID MzcRemoveBackslashA(LPTSTR pszPath)
{
    using namespace std;
    SIZE_T cchPath = lstrlen(pszPath);
    LPTSTR pchPrev = CharPrev(pszPath, pszPath + cchPath);
    if (*pchPrev == _T('\\') || *pchPrev == _T('/'))
    {
        *pchPrev = _T('\0');
    }
}

LPTSTR MzcGetFileTitle(LPTSTR pszPath)
{
    using namespace std;
    LPTSTR pch = _tcsrchr(pszPath, _T('\\'));
    if (pch != NULL)
        return pch + 1;
    pch = _tcsrchr(pszPath, _T('/'));
    if (pch != NULL)
        return pch + 1;
    return pszPath;
}

LPTSTR MzcGetDotExt(LPTSTR pszPath)
{
    using namespace std;
    LPTSTR pch = MzcGetFileTitle(pszPath);
    if (pch != NULL) pch = _tcsrchr(pch, _T('.'));
    if (pch == NULL) pch = pszPath + lstrlen(pszPath);
    return pch;
}

VOID MzcSetDotExt(LPTSTR pszPath, LPCTSTR pszDotExt)
{
    using namespace std;
    lstrcpy(MzcGetDotExt(pszPath), pszDotExt);
}

BOOL MzcFileExists(LPCTSTR pszFileName)
{
    return GetFileAttributes(pszFileName) != 0xFFFFFFFF;
}

BOOL MzcFileGetContents(LPCTSTR pszFileName, std::vector<BYTE>& v)
{
    MFile file;
    DWORD cbFile, cbRead;

    v.clear();

    if (file.OpenFileForInput(pszFileName))
    {
        cbFile = file.GetFileSize();
        if (cbFile == 0xFFFFFFFF)
            return FALSE;
        v.resize(cbFile);
        if (file.ReadFile(&v[0], cbFile, &cbRead) && cbFile == cbRead)
            return TRUE;
    }

    return FALSE;
}

BOOL MzcFilePutContents(LPCTSTR pszFileName, LPCVOID pv, DWORD cb)
{
    MFile file;
    DWORD cbWritten;

    if (file.OpenFileForOutput(pszFileName))
    {
        if (file.WriteFile(pv, cb, &cbWritten) && cb == cbWritten)
            return TRUE;
    }
    return FALSE;
}

BOOL MzcFileSize(LPCTSTR pszFileName, LPDWORD pcbFile)
{
    MzcAssert(pcbFile != NULL);
    MFile file;
    if (file.OpenFileForInput(pszFileName))
    {
        *pcbFile = file.GetFileSize();
        if (*pcbFile != 0xFFFFFFFF)
            return TRUE;
    }
    return FALSE;
}

BOOL MzcIsDir(LPCTSTR pszFileName)
{
    DWORD attrs = GetFileAttributes(pszFileName);
    return attrs != 0xFFFFFFFF && (attrs & FILE_ATTRIBUTE_DIRECTORY);
}

BOOL MzcIsFile(LPCTSTR pszFileName)
{
    DWORD attrs = GetFileAttributes(pszFileName);
    return attrs != 0xFFFFFFFF && !(attrs & FILE_ATTRIBUTE_DIRECTORY);
}

BOOL MzcGlob(std::vector<tstring>& vPaths, LPCTSTR pszWildcard)
{
    TCHAR szFileName[MAX_PATH];
    MFindFile find;
    vPaths.clear();
    if (find.FindFirstFile(pszWildcard))
    {
        do
        {
            find.GetFileName(szFileName, MAX_PATH);
            vPaths.push_back(szFileName);
        } while (find.FindNextFile());
        return TRUE;
    }
    return FALSE;
}

////////////////////////////////////////////////////////////////////////////
// MZC2 MFile

MFile::MFile()
    : m_hHandle(NULL)
{
}

MFile::MFile(HANDLE hHandle)
    : m_hHandle(hHandle)
{
}

MFile::~MFile()
{
    if (m_hHandle != NULL && m_hHandle != INVALID_HANDLE_VALUE)
        MzcVerify(::CloseHandle(m_hHandle));
}

MFile::operator HANDLE() const
{
    return m_hHandle;
}

MFile::operator PHANDLE()
{
    return &m_hHandle;
}

PHANDLE MFile::operator&()
{
    return &m_hHandle;
}

MFile& MFile::operator=(HANDLE hHandle)
{
    if (m_hHandle != hHandle)
    {
        if (m_hHandle != NULL && m_hHandle != INVALID_HANDLE_VALUE)
            MzcVerify(::CloseHandle(m_hHandle));
        Attach(hHandle);
    }
    return *this;
}

bool MFile::operator!() const
{
    return m_hHandle == NULL || m_hHandle == INVALID_HANDLE_VALUE;
}

bool MFile::operator==(HANDLE hHandle) const
{
    return m_hHandle == hHandle;
}

bool MFile::operator!=(HANDLE hHandle) const
{
    return m_hHandle != hHandle;
}

BOOL MFile::OpenFileForInput(
    LPCTSTR pszFileName, DWORD dwFILE_SHARE_/* = FILE_SHARE_READ*/)
{
    return MFile::CreateFile(pszFileName, GENERIC_READ,
        dwFILE_SHARE_, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
}

BOOL MFile::OpenFileForOutput(
    LPCTSTR pszFileName, DWORD dwFILE_SHARE_/* = FILE_SHARE_READ*/)
{
    return MFile::CreateFile(pszFileName, GENERIC_WRITE,
        dwFILE_SHARE_, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
}

BOOL MFile::OpenFileForRandom(
    LPCTSTR pszFileName, DWORD dwFILE_SHARE_/* = FILE_SHARE_READ*/)
{
    return MFile::CreateFile(pszFileName,
        GENERIC_READ | GENERIC_WRITE, dwFILE_SHARE_, NULL, OPEN_ALWAYS,
        FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS, NULL);
}

BOOL MFile::DuplicateHandle(PHANDLE phHandle, BOOL bInherit)
{
    MzcAssert(m_hHandle != NULL && m_hHandle != INVALID_HANDLE_VALUE);
    HANDLE hProcess = ::GetCurrentProcess();
    return ::DuplicateHandle(hProcess, m_hHandle, hProcess, phHandle, 0,
        bInherit, DUPLICATE_SAME_ACCESS);
}

BOOL MFile::DuplicateHandle(
    PHANDLE phHandle, BOOL bInherit, DWORD dwDesiredAccess)
{
    MzcAssert(m_hHandle != NULL && m_hHandle != INVALID_HANDLE_VALUE);
    HANDLE hProcess = ::GetCurrentProcess();
    return ::DuplicateHandle(hProcess, m_hHandle, hProcess, phHandle,
        dwDesiredAccess, bInherit, 0);
}

DWORD MFile::WaitForSingleObject(
    DWORD dwTimeout/* = INFINITE*/)
{
    MzcAssert(m_hHandle != NULL && m_hHandle != INVALID_HANDLE_VALUE);
    return ::WaitForSingleObject(m_hHandle, dwTimeout);
}

VOID MFile::Attach(HANDLE hHandle)
{
    MzcAssert(m_hHandle == NULL || m_hHandle == INVALID_HANDLE_VALUE);
    m_hHandle = hHandle;
}

HANDLE MFile::Detach()
{
    HANDLE hHandle = m_hHandle;
    m_hHandle = NULL;
    return hHandle;
}

BOOL MFile::CloseHandle()
{
    MzcAssert(m_hHandle != NULL && m_hHandle != INVALID_HANDLE_VALUE);
    BOOL b = ::CloseHandle(m_hHandle);
    MzcAssert(b);
    m_hHandle = INVALID_HANDLE_VALUE;
    return b;
}

BOOL MFile::PeekNamedPipe(
    LPVOID pBuffer/* = NULL*/,
    DWORD cbBuffer/* = 0*/,
    LPDWORD pcbRead/* = NULL*/,
    LPDWORD pcbAvail/* = NULL*/,
    LPDWORD pBytesLeft/* = NULL*/)
{
    MzcAssert(m_hHandle != NULL && m_hHandle != INVALID_HANDLE_VALUE);
    return ::PeekNamedPipe(m_hHandle, pBuffer, cbBuffer,
        pcbRead, pcbAvail, pBytesLeft);
}

BOOL MFile::ReadFile(LPVOID pBuffer, DWORD cbToRead,
    LPDWORD pcbRead, LPOVERLAPPED pOverlapped/* = NULL*/)
{
    MzcAssert(m_hHandle != NULL && m_hHandle != INVALID_HANDLE_VALUE);
    return ::ReadFile(m_hHandle, pBuffer, cbToRead, pcbRead, pOverlapped);
}

BOOL MFile::WriteFile(LPCVOID pBuffer, DWORD cbToWrite,
    LPDWORD pcbWritten, LPOVERLAPPED pOverlapped/* = NULL*/)
{
    MzcAssert(m_hHandle != NULL && m_hHandle != INVALID_HANDLE_VALUE);
    return ::WriteFile(
        m_hHandle, pBuffer, cbToWrite, pcbWritten, pOverlapped);
}

BOOL MFile::WriteSzA(LPCSTR psz,
    LPDWORD pcbWritten, LPOVERLAPPED pOverlapped/* = NULL*/)
{
    return WriteFile(psz, (DWORD)strlen(psz), pcbWritten, pOverlapped);
}

BOOL MFile::WriteSzW(LPCWSTR psz,
    LPDWORD pcbWritten, LPOVERLAPPED pOverlapped/* = NULL*/)
{
    return WriteFile(psz, (DWORD)(wcslen(psz) * sizeof(WCHAR)), pcbWritten,
        pOverlapped);
}

BOOL MFile::WriteSz(LPCTSTR psz,
    LPDWORD pcbWritten, LPOVERLAPPED pOverlapped/* = NULL*/)
{
    return WriteFile(psz, (DWORD)(_tcslen(psz) * sizeof(TCHAR)), pcbWritten, pOverlapped);
}

BOOL MFile::CreateFile(LPCTSTR pszFileName,
    DWORD dwDesiredAccess, DWORD dwShareMode,
    LPSECURITY_ATTRIBUTES pSA, DWORD dwCreationDistribution,
    DWORD dwFlagsAndAttributes/* = FILE_ATTRIBUTE_NORMAL*/,
    HANDLE hTemplateFile/* = NULL*/)
{
    MzcAssert(m_hHandle == NULL || m_hHandle == INVALID_HANDLE_VALUE);
    m_hHandle = ::CreateFile(pszFileName, dwDesiredAccess, dwShareMode,
        pSA, dwCreationDistribution, dwFlagsAndAttributes, hTemplateFile);
    return (m_hHandle != INVALID_HANDLE_VALUE);
}

DWORD MFile::SetFilePointer(
    LONG nDeltaLow,
    PLONG pnDeltaHigh/* = NULL*/,
    DWORD dwOrigin/* = FILE_BEGIN*/)
{
    MzcAssert(m_hHandle != NULL && m_hHandle != INVALID_HANDLE_VALUE);
    return ::SetFilePointer(m_hHandle, nDeltaLow, pnDeltaHigh, dwOrigin);
}

DWORD MFile::SeekToEnd()
{
    MzcAssert(m_hHandle != NULL && m_hHandle != INVALID_HANDLE_VALUE);
    return SetFilePointer(0, NULL, FILE_END);
}

VOID MFile::SeekToBegin()
{
    MzcAssert(m_hHandle != NULL && m_hHandle != INVALID_HANDLE_VALUE);
    SetFilePointer(0, NULL, FILE_BEGIN);
}

DWORD MFile::GetFileSize(
    LPDWORD pdwHighPart/* = NULL*/) const
{
    MzcAssert(m_hHandle != NULL && m_hHandle != INVALID_HANDLE_VALUE);
    return ::GetFileSize(m_hHandle, pdwHighPart);
}

BOOL MFile::SetEndOfFile()
{
    MzcAssert(m_hHandle != NULL && m_hHandle != INVALID_HANDLE_VALUE);
    return ::SetEndOfFile(m_hHandle);
}

BOOL MFile::FlushFileBuffers()
{
    MzcAssert(m_hHandle != NULL && m_hHandle != INVALID_HANDLE_VALUE);
    return ::FlushFileBuffers(m_hHandle);
}

BOOL MFile::WriteSzA(LPCSTR psz)
{
    INT cb = (INT) strlen(psz);
    return WriteBinary(psz, (DWORD) cb);
}

BOOL MFile::WriteSzW(LPCWSTR psz)
{
    INT cb = (INT) (wcslen(psz) * sizeof(WCHAR));
    return WriteBinary(psz, (DWORD) cb);
}

BOOL MFile::WriteSz(LPCTSTR psz)
{
    INT cb = (INT) (_tcslen(psz) * sizeof(TCHAR));
    return WriteBinary(psz, (DWORD) cb);
}

BOOL __cdecl MFile::WriteFormatA(LPCSTR pszFormat, ...)
{
    va_list argList;
    CHAR sz[1024];
    va_start(argList, pszFormat);
    std::vsprintf(sz, pszFormat, argList);
    BOOL b = WriteSzA(sz);
    va_end(argList);
    return b;
}

BOOL __cdecl MFile::WriteFormatW(LPCWSTR pszFormat, ...)
{
    using namespace std;
    va_list argList;
    WCHAR sz[1024];
    va_start(argList, pszFormat);
    vswprintf(sz, 1024, pszFormat, argList);
    BOOL b = WriteSzW(sz);
    va_end(argList);
    return b;
}

BOOL __cdecl MFile::WriteFormat(LPCTSTR pszFormat, ...)
{
    using namespace std;
    va_list argList;
    TCHAR sz[1024];
    va_start(argList, pszFormat);
    _vstprintf(sz, pszFormat, argList);
    BOOL b = WriteSz(sz);
    va_end(argList);
    return b;
}

BOOL MFile::GetFileTime(
    LPFILETIME pftCreate/* = NULL*/,
    LPFILETIME pftLastAccess/* = NULL*/,
    LPFILETIME pftLastWrite/* = NULL*/) const
{
    MzcAssert(m_hHandle != NULL && m_hHandle != INVALID_HANDLE_VALUE);
    return ::GetFileTime(m_hHandle, pftCreate, pftLastAccess, pftLastWrite);
}

BOOL MFile::OpenFileForAppend(
    LPCTSTR pszFileName, DWORD dwFILE_SHARE_/* = FILE_SHARE_READ*/)
{
    BOOL bExisted = (::GetFileAttributes(pszFileName) != 0xFFFFFFFF);
    if (!MFile::CreateFile(pszFileName, GENERIC_READ | GENERIC_WRITE,
        dwFILE_SHARE_, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL))
        return FALSE;
    if (SetFilePointer(0, NULL, FILE_END) == 0xFFFFFFFF)
    {
        MzcAssert(FALSE);
        CloseHandle();
        if (!bExisted)
            ::DeleteFile(pszFileName);
        return FALSE;
    }
    return TRUE;
}

BOOL MFile::WriteBinary(LPCVOID pv, DWORD cb)
{
    CONST BYTE *pb = (CONST BYTE *) pv;
    DWORD cbWritten;
    while (cb != 0)
    {
        if (WriteFile(pb, cb, &cbWritten))
        {
            cb -= cbWritten;
            pb += cbWritten;
        }
        else
            break;
    }
    return (cb == 0) && FlushFileBuffers();
}

////////////////////////////////////////////////////////////////////////////
// MZC2 MFindFile

MFindFile::MFindFile()
    : m_hFind(INVALID_HANDLE_VALUE)
{
    m_szRoot[0] = _T('\0');
#ifdef _DEBUG
    m_bFound = FALSE;
#endif
}

MFindFile::MFindFile(LPCTSTR pszWildcard)
    : m_hFind(INVALID_HANDLE_VALUE)
{
    m_szRoot[0] = _T('\0');
#ifdef _DEBUG
    m_bFound = FALSE;
#endif
    FindFirstFile(pszWildcard);
}

MFindFile::~MFindFile()
{
    if (m_hFind != INVALID_HANDLE_VALUE) Close();
}

bool MFindFile::operator!() const
{
    return m_hFind == INVALID_HANDLE_VALUE;
}

BOOL MFindFile::FindNextFile()
{
    MzcAssert(m_hFind != INVALID_HANDLE_VALUE);
    MzcAssert(m_bFound);
    BOOL b = ::FindNextFile(m_hFind, &m_find);
#ifdef _DEBUG
    m_bFound = b;
#endif
    return b;
}

VOID MFindFile::Close()
{
    MzcVerify(::FindClose(m_hFind));
    m_hFind = INVALID_HANDLE_VALUE;
}

ULONGLONG MFindFile::GetFileSize() const
{
    MzcAssert(m_hFind != INVALID_HANDLE_VALUE);
    MzcAssert(m_bFound);
    ULARGE_INTEGER nFileSize;
    nFileSize.LowPart = m_find.nFileSizeLow;
    nFileSize.HighPart = m_find.nFileSizeHigh;
    return nFileSize.QuadPart;
}

BOOL MFindFile::GetFileName(LPTSTR pszFileName, INT cchLength) const
{
    MzcAssert(m_hFind != INVALID_HANDLE_VALUE);
    MzcAssert(m_bFound);

    if ((INT) _tcslen(m_find.cFileName) >= cchLength)
        return FALSE;

    ::lstrcpyn(pszFileName, m_find.cFileName, cchLength);
    return TRUE;
}

BOOL MFindFile::GetFilePath(LPTSTR pszFilePath, INT cchLength) const
{
    MzcAssert(m_hFind != INVALID_HANDLE_VALUE);
    MzcAssert(m_bFound);

    if ((INT) (_tcslen(m_szRoot) + 1 + _tcslen(m_find.cFileName)) >= cchLength)
        return FALSE;

    ::lstrcpyn(pszFilePath, m_szRoot, cchLength);
    MzcAddBackslash(pszFilePath);
    _tcscat(pszFilePath, m_find.cFileName);

    return TRUE;
}

BOOL MFindFile::GetRoot(LPTSTR pszRoot, INT cchLength) const
{
    MzcAssert(m_bFound);
    if ((INT) _tcslen(m_szRoot) >= cchLength)
        return FALSE;

    ::lstrcpyn(pszRoot, m_szRoot, cchLength);
    return TRUE;
}

BOOL MFindFile::GetLastWriteTime(FILETIME* pTimeStamp) const
{
    MzcAssert(m_hFind != INVALID_HANDLE_VALUE);
    MzcAssert(m_bFound);
    MzcAssert(pTimeStamp != NULL);
    *pTimeStamp = m_find.ftLastWriteTime;
    return TRUE;
}

BOOL MFindFile::GetLastAccessTime(FILETIME* pTimeStamp) const
{
    MzcAssert(m_hFind != INVALID_HANDLE_VALUE);
    MzcAssert(m_bFound);
    MzcAssert(pTimeStamp != NULL);
    *pTimeStamp = m_find.ftLastAccessTime;
    return TRUE;
}

BOOL MFindFile::GetCreationTime(FILETIME* pTimeStamp) const
{
    MzcAssert(m_hFind != INVALID_HANDLE_VALUE);
    MzcAssert(m_bFound);
    MzcAssert(pTimeStamp != NULL);
    *pTimeStamp = m_find.ftCreationTime;
    return TRUE;
}

BOOL MFindFile::MatchesMask(DWORD dwMask) const
{
    MzcAssert(m_hFind != INVALID_HANDLE_VALUE);
    MzcAssert(m_bFound);
    return ((m_find.dwFileAttributes & dwMask) != 0);
}

BOOL MFindFile::IsDots() const
{
    MzcAssert(m_hFind != INVALID_HANDLE_VALUE);
    MzcAssert(m_bFound);

    if (m_find.cFileName[0] == _T('.') &&
        (m_find.cFileName[1] == _T('\0') ||
            (m_find.cFileName[1] == _T('.') &&
                m_find.cFileName[2] == _T('\0'))))
        return TRUE;
    return FALSE;
}

BOOL MFindFile::IsReadOnly() const
{
    return MatchesMask(FILE_ATTRIBUTE_READONLY);
}

BOOL MFindFile::IsDirectory() const
{
    return MatchesMask(FILE_ATTRIBUTE_DIRECTORY);
}

BOOL MFindFile::IsCompressed() const
{
    return MatchesMask(FILE_ATTRIBUTE_COMPRESSED);
}

BOOL MFindFile::IsSystem() const
{
    return MatchesMask(FILE_ATTRIBUTE_SYSTEM);
}

BOOL MFindFile::IsHidden() const
{
    return MatchesMask(FILE_ATTRIBUTE_HIDDEN);
}

BOOL MFindFile::IsTemporary() const
{
    return MatchesMask(FILE_ATTRIBUTE_TEMPORARY);
}

BOOL MFindFile::IsNormal() const
{
    return MatchesMask(FILE_ATTRIBUTE_NORMAL);
}

BOOL MFindFile::IsArchived() const
{
    return MatchesMask(FILE_ATTRIBUTE_ARCHIVE);
}

BOOL MFindFile::FindFirstFile(LPCTSTR pszWildcard/* = NULL*/)
{
    if (m_hFind != INVALID_HANDLE_VALUE) Close();

    if (pszWildcard == NULL)
        pszWildcard = _T("*.*");
    MzcAssert(_tcslen(pszWildcard) < MAX_PATH);

    m_hFind = ::FindFirstFile(pszWildcard, &m_find);
    if(m_hFind == INVALID_HANDLE_VALUE)
        return FALSE;

    DWORD dw = ::GetFullPathName(pszWildcard, MAX_PATH, m_szRoot, NULL);
    MzcAssert(dw != 0);
    if (dw == 0)
    {
        Close();
        ::SetLastError(ERROR_INVALID_NAME);
        return FALSE;
    }

    LPTSTR pchBack = _tcsrchr(m_szRoot, _T('\\'));
    LPTSTR pchFore = _tcsrchr(m_szRoot, _T('/'));
    if (pchBack != NULL || pchFore != NULL)
    {
        if (pchBack == NULL) pchBack = m_szRoot;
        if (pchFore == NULL) pchFore = m_szRoot;
        if (pchFore <= pchBack) *pchBack = _T('\0');
        else *pchFore = _T('\0');
    }

#ifdef _DEBUG
    m_bFound = TRUE;
#endif

    return TRUE;
}

LPCTSTR MFindFile::GetFileName() const
{
    MzcAssert(m_bFound);
    return m_find.cFileName;
}

tstring MFindFile::GetFilePath() const
{
    TCHAR szPath[MAX_PATH];
    lstrcpy(szPath, m_szRoot);
    MzcAddBackslash(szPath);
    lstrcat(szPath, GetFileName());
    return szPath;
}

LPCTSTR MFindFile::GetRoot() const
{
    return m_szRoot;
}

////////////////////////////////////////////////////////////////////////////
// MZC2 MProcessMaker

MProcessMaker::~MProcessMaker()
{
    Close();
}

BOOL MProcessMaker::TerminateProcess(UINT uExitCode)
{
    return ::TerminateProcess(m_pi.hProcess, uExitCode);
}

VOID MProcessMaker::SetStdInput(HANDLE hStdIn)
{
    if (hStdIn != NULL)
    {
        m_si.hStdInput = hStdIn;
        m_si.dwFlags |= STARTF_USESTDHANDLES;
    }
}

VOID MProcessMaker::SetStdOutput(HANDLE hStdOut)
{
    if (hStdOut != NULL)
    {
        m_si.hStdOutput = hStdOut;
        m_si.dwFlags |= STARTF_USESTDHANDLES;
    }
}

VOID MProcessMaker::SetStdError(HANDLE hStdErr)
{
    if (hStdErr != NULL)
    {
        m_si.hStdError = hStdErr;
        m_si.dwFlags |= STARTF_USESTDHANDLES;
    }
}

VOID MProcessMaker::SetShowWindow(INT nCmdShow/* = SW_HIDE*/)
{
    m_si.wShowWindow = (WORD) nCmdShow;
    m_si.dwFlags |= STARTF_USESHOWWINDOW;
}

VOID MProcessMaker::SetCreationFlags(
    DWORD dwFlags/* = CREATE_NEW_CONSOLE*/)
{
    m_dwCreationFlags = dwFlags;
}

VOID MProcessMaker::SetCurrentDirectory(LPCTSTR pszCurDir)
{
    m_pszCurDir = pszCurDir;
}

VOID MProcessMaker::SetDesktop(LPTSTR lpDesktop)
{
    m_si.lpDesktop = lpDesktop;
}

VOID MProcessMaker::SetTitle(LPTSTR lpTitle)
{
    m_si.lpTitle = lpTitle;
}

VOID MProcessMaker::SetPosition(DWORD dwX, DWORD dwY)
{
    m_si.dwX = dwX;
    m_si.dwY = dwY;
    m_si.dwFlags |= STARTF_USEPOSITION;
}

VOID MProcessMaker::SetSize(DWORD dwXSize, DWORD dwYSize)
{
    m_si.dwXSize = dwXSize;
    m_si.dwYSize = dwYSize;
    m_si.dwFlags |= STARTF_USESIZE;
}

VOID MProcessMaker::SetCountChars(
    DWORD dwXCountChars, DWORD dwYCountChars)
{
    m_si.dwXCountChars = dwXCountChars;
    m_si.dwYCountChars = dwYCountChars;
    m_si.dwFlags |= STARTF_USECOUNTCHARS;
}

VOID MProcessMaker::SetFillAttirbutes(DWORD dwFillAttribute)
{
    m_si.dwFillAttribute = dwFillAttribute;
    m_si.dwFlags |= STARTF_USEFILLATTRIBUTE;
}

HANDLE MProcessMaker::GetHandle() const
{
    return m_pi.hProcess;
}

DWORD MProcessMaker::GetExitCode() const
{
    MzcAssert(m_pi.hProcess != NULL);
    DWORD dwExitCode;
    ::GetExitCodeProcess(m_pi.hProcess, &dwExitCode);
    return dwExitCode;
}

DWORD MProcessMaker::WaitForExit(DWORD dwTimeout/* = INFINITE*/)
{
    MzcAssert(m_pi.hProcess != NULL);
    return ::WaitForSingleObject(m_pi.hProcess, dwTimeout);
}

BOOL MProcessMaker::IsRunning() const
{
    return (m_pi.hProcess != NULL &&
        ::WaitForSingleObject(m_pi.hProcess, 0) == WAIT_TIMEOUT);
}

bool MProcessMaker::operator!() const
{
    return !IsRunning();
}

MProcessMaker::MProcessMaker()
{
    ZeroMemory(&m_si, sizeof(m_si));
    m_si.cb = sizeof(STARTUPINFO);
    ZeroMemory(&m_pi, sizeof(m_pi));
    m_dwCreationFlags = 0;
    m_pszCurDir = NULL;
    m_si.hStdInput = ::GetStdHandle(STD_INPUT_HANDLE);
    m_si.hStdOutput = ::GetStdHandle(STD_OUTPUT_HANDLE);
    m_si.hStdError = ::GetStdHandle(STD_ERROR_HANDLE);
}

BOOL MProcessMaker::CreateProcess(
    LPCTSTR pszAppName, LPCTSTR pszCommandLine/* = NULL*/,
    LPCTSTR pszzEnvironment/* = NULL*/, BOOL bInherit/* = TRUE*/,
    LPSECURITY_ATTRIBUTES lpProcessAttributes/* = NULL*/,
    LPSECURITY_ATTRIBUTES lpThreadAttributes/* = NULL*/)
{
    BOOL b;
    if (pszCommandLine == NULL)
    {
#ifdef _UNICODE
        b = ::CreateProcess(pszAppName, NULL,
            lpProcessAttributes, lpThreadAttributes,
            bInherit, m_dwCreationFlags | CREATE_UNICODE_ENVIRONMENT,
            (LPVOID) pszzEnvironment, m_pszCurDir, &m_si, &m_pi);
#else
        b = ::CreateProcess(pszAppName, NULL,
            lpProcessAttributes, lpThreadAttributes,
            bInherit, m_dwCreationFlags, (LPVOID) pszzEnvironment,
            m_pszCurDir, &m_si, &m_pi);
#endif
    }
    else
    {
        LPTSTR pszCmdLine = _tcsdup(pszCommandLine);
#ifdef _UNICODE
        b = ::CreateProcess(pszAppName, pszCmdLine, 
            lpProcessAttributes, lpThreadAttributes,
            bInherit, m_dwCreationFlags | CREATE_UNICODE_ENVIRONMENT,
            (LPVOID) pszzEnvironment, m_pszCurDir, &m_si, &m_pi);
#else
        b = ::CreateProcess(pszAppName, pszCmdLine, 
            lpProcessAttributes, lpThreadAttributes,
            bInherit, m_dwCreationFlags, (LPVOID) pszzEnvironment,
            m_pszCurDir, &m_si, &m_pi);
#endif
        free(pszCmdLine);
    }
    return b;
}

BOOL MProcessMaker::CreateProcessAsUser(
    HANDLE hToken, LPCTSTR pszAppName, LPCTSTR pszCommandLine/* = NULL*/,
    LPCTSTR pszzEnvironment/* = NULL*/, BOOL bInherit/* = TRUE*/,
    LPSECURITY_ATTRIBUTES lpProcessAttributes/* = NULL*/,
    LPSECURITY_ATTRIBUTES lpThreadAttributes/* = NULL*/)
{
    BOOL b;
    if (pszCommandLine == NULL)
    {
#ifdef _UNICODE
        b = ::CreateProcessAsUser(hToken, pszAppName, NULL, 
            lpProcessAttributes, lpThreadAttributes,
            bInherit, m_dwCreationFlags | CREATE_UNICODE_ENVIRONMENT,
            (LPVOID) pszzEnvironment, m_pszCurDir, &m_si, &m_pi);
#else
        b = ::CreateProcessAsUser(hToken, pszAppName, NULL, 
            lpProcessAttributes, lpThreadAttributes,
            bInherit, m_dwCreationFlags, (LPVOID) pszzEnvironment,
            m_pszCurDir, &m_si, &m_pi);
#endif
    }
    else
    {
        LPTSTR pszCmdLine = _tcsdup(pszCommandLine);
#ifdef _UNICODE
        b = ::CreateProcessAsUser(hToken, pszAppName, pszCmdLine,
            lpProcessAttributes, lpThreadAttributes,
            bInherit, m_dwCreationFlags | CREATE_UNICODE_ENVIRONMENT,
            (LPVOID) pszzEnvironment, m_pszCurDir, &m_si, &m_pi);
#else
        b = ::CreateProcessAsUser(hToken, pszAppName, pszCmdLine,
            lpProcessAttributes, lpThreadAttributes,
            bInherit, m_dwCreationFlags, (LPVOID) pszzEnvironment,
            m_pszCurDir, &m_si, &m_pi);
#endif
        free(pszCmdLine);
    }
    MzcAssert(b);
    return b;
}

VOID MProcessMaker::Close()
{
    if (m_pi.hProcess != NULL)
    {
        ::CloseHandle(m_pi.hProcess);
        m_pi.hProcess = NULL;
    }
    if (m_pi.hThread != NULL)
    {
        ::CloseHandle(m_pi.hThread);
        m_pi.hThread = NULL;
    }
    HANDLE hStdInput = ::GetStdHandle(STD_INPUT_HANDLE);
    if (m_si.hStdInput != hStdInput)
    {
        ::CloseHandle(m_si.hStdInput);
        m_si.hStdInput = hStdInput;
    }
    HANDLE hStdOutput = ::GetStdHandle(STD_OUTPUT_HANDLE);
    if (m_si.hStdOutput != hStdOutput)
    {
        ::CloseHandle(m_si.hStdOutput);
        m_si.hStdOutput = hStdOutput;
    }
    HANDLE hStdError = ::GetStdHandle(STD_ERROR_HANDLE);
    if (m_si.hStdError != hStdError)
    {
        ::CloseHandle(m_si.hStdError);
        m_si.hStdError = hStdError;
    }
}

BOOL MProcessMaker::PrepareForRedirect(
    PHANDLE phInputWrite, PHANDLE phOutputRead,
    PHANDLE phErrorRead)
{
    MSecurityAttributes sa;

    MFile hInputRead, hInputWriteTmp;
    MFile hOutputReadTmp, hOutputWrite;
    MFile hErrorReadTmp, hErrorWrite;

    if (phInputWrite != NULL)
    {
        if (::CreatePipe(&hInputRead, &hInputWriteTmp, &sa, 0))
        {
            if (!hInputWriteTmp.DuplicateHandle(phInputWrite, FALSE))
                return FALSE;
            hInputWriteTmp.CloseHandle();
        }
        else
            return FALSE;
    }

    if (phOutputRead != NULL)
    {
        if (::CreatePipe(&hOutputReadTmp, &hOutputWrite, &sa, 0))
        {
            if (!hOutputReadTmp.DuplicateHandle(phOutputRead, FALSE))
                return FALSE;
            hOutputReadTmp.CloseHandle();
        }
        else
            return FALSE;
    }

    if (phOutputRead != NULL && phOutputRead == phErrorRead)
    {
        if (!hOutputWrite.DuplicateHandle(&hErrorWrite, TRUE))
            return FALSE;
    }
    else if (phErrorRead != NULL)
    {
        if (::CreatePipe(&hErrorReadTmp, &hErrorWrite, &sa, 0))
        {
            if (!hErrorReadTmp.DuplicateHandle(phErrorRead, FALSE))
                return FALSE;
            hErrorReadTmp.CloseHandle();
        }
        else
            return FALSE;
    }

    if (phInputWrite != NULL)
        SetStdInput(hInputRead.Detach());
    if (phOutputRead != NULL)
        SetStdOutput(hOutputWrite.Detach());
    if (phErrorRead != NULL)
        SetStdError(hErrorWrite.Detach());

    return TRUE;
}

////////////////////////////////////////////////////////////////////////////
