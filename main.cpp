#include <windows.h>
#include <winsvc.h>
#include <tchar.h>
#include <evntprov.h>
#include <io.h>
#include <direct.h>
#include <shlobj.h>
#include <winmeta.h>

#include <locale>
#include <codecvt>
#include <string>
#include <atomic>
#include <mutex>


#include "manifest.h"

namespace  {
    char servName[] = "service_win";
    static const LPSTR serviceName = _T(servName);
    void WINAPI ServiceMain(DWORD argc, LPTSTR argv[]);

    VOID WINAPI ServiceCtrlHandler(DWORD);

    SERVICE_STATUS serviceStatus;
    SERVICE_STATUS_HANDLE hServiceStatus;

    void updateStatus(const DWORD status);
    const DWORD timeout = 1000;

    std::atomic<bool> is_shutdown(false);
    std::mutex mutex;
}

int _tmain( int, LPTSTR)
{
    SERVICE_TABLE_ENTRY ServiceTable[] = { {serviceName, ServiceMain}, {nullptr, nullptr} };
    return StartServiceCtrlDispatcher(ServiceTable);
}

namespace  {

void WINAPI ServiceMain(DWORD, LPTSTR[]) {
    serviceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    serviceStatus.dwCurrentState = SERVICE_START_PENDING;
    serviceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN | SERVICE_ACCEPT_PAUSE_CONTINUE;
    serviceStatus.dwWin32ExitCode = NO_ERROR;
    serviceStatus.dwServiceSpecificExitCode = 0;
    serviceStatus.dwCheckPoint = 0;
    serviceStatus.dwWaitHint = timeout * 2;

    hServiceStatus = RegisterServiceCtrlHandler(serviceName, ServiceCtrlHandler);

    EventRegisterservice_win_log();

    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    std::wstring log_data = converter.from_bytes("тест");

    const auto log_data_pointer = log_data.c_str();

    REGHANDLE log_handle = NULL;
    if (EventRegister(&SERVICE_WIN_LOG_PROVIDER, NULL, NULL, &log_handle) == ERROR_SUCCESS) {
        updateStatus(SERVICE_RUNNING);
        EventWriteSERVICE_ADMIN_START();
        while(!is_shutdown) {

            EventWriteSERVICE_LOG_FATAL(log_data_pointer);
            EventWriteSERVICE_LOG_CRITICAL(log_data_pointer);
            EventWriteSERVICE_LOG_WARNING(log_data_pointer);
            EventWriteSERVICE_LOG_INFO(log_data_pointer);
            EventWriteSERVICE_LOG_DEBUG(log_data_pointer);
            EventWriteSERVICE_AUDIT_SUCCESS(log_data_pointer);
            EventWriteSERVICE_AUDIT_FAILURE(log_data_pointer);

            Sleep(timeout);
            updateStatus(serviceStatus.dwCurrentState);
        }
        EventWriteSERVICE_ADMIN_STOP();
    }
    updateStatus(SERVICE_STOPPED);
    EventUnregisterservice_win_log();
}


VOID WINAPI ServiceCtrlHandler(DWORD dwControl)
{
    switch (dwControl)
    {
    case SERVICE_CONTROL_SHUTDOWN:
    case SERVICE_CONTROL_STOP:
        is_shutdown = true;
        updateStatus(SERVICE_STOP_PENDING);
        break;
    default:
        updateStatus(serviceStatus.dwCurrentState);
        break;
    }
}

void updateStatus(const DWORD status)
{
    std::lock_guard<std::mutex> lock(mutex);
    if (status == SERVICE_STOPPED) serviceStatus.dwCheckPoint = 0;
    else serviceStatus.dwCheckPoint++;
    if (serviceStatus.dwCurrentState != status) serviceStatus.dwCurrentState = status;
    SetServiceStatus(hServiceStatus, &serviceStatus);
}


}

