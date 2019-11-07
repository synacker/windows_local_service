#include <windows.h>
#include <winsvc.h>
#include <tchar.h>
#include <evntprov.h>
#include <io.h>
#include <direct.h>
#include <shlobj.h>
#include <winmeta.h>

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

    REGHANDLE log_handle = NULL;
    if (EventRegister(&SERVICE_WIN_LOG_PROVIDER, NULL, NULL, &log_handle) == ERROR_SUCCESS) {
        updateStatus(SERVICE_RUNNING);
        while(!is_shutdown) {
            std::string log_message( "TEST SERVICE LOG" );
            EVENT_DATA_DESCRIPTOR data;
            EventDataDescCreate( &data, log_message.c_str(), static_cast<ULONG>(log_message.size() + 1) );

            if (EventWrite(log_handle, &SERVICE_LOG_FATAL, 1, &data) != ERROR_SUCCESS ||
                EventWrite(log_handle, &SERVICE_LOG_CRITICAL, 1, &data) != ERROR_SUCCESS ||
                EventWrite(log_handle, &SERVICE_LOG_WARNING, 1, &data) != ERROR_SUCCESS ||
                EventWrite(log_handle, &SERVICE_LOG_INFO, 1, &data) != ERROR_SUCCESS ||
                EventWrite(log_handle, &SERVICE_LOG_DEBUG, 1, &data) != ERROR_SUCCESS ||
                EventWrite(log_handle, &SERVICE_LOG_TRACE, 1, &data) != ERROR_SUCCESS ||
                EventWrite(log_handle, &SERVICE_AUDIT_SUCCESS, 1, &data) != ERROR_SUCCESS ||
                EventWrite(log_handle, &SERVICE_AUDIT_FAILURE, 1, &data) != ERROR_SUCCESS)
            {
                is_shutdown = true;
            } else {
                Sleep(timeout);
            }
            updateStatus(serviceStatus.dwCurrentState);
        }
    }
    updateStatus(SERVICE_STOPPED);
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

