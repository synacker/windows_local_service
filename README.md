Windows local service sample
==========================================================================================

Sample windows of windows service run under [local service account](https://docs.microsoft.com/en-us/windows/win32/services/localservice-account) and with [manifest](https://docs.microsoft.com/en-us/windows/win32/etw/writing-manifest-based-events) for log.

How to execute:
=================
Run under user:

        mkdir build_dir
        cd build_dir
        cmake build ..
        
Run under admin:

        register_log.cmd
        register_service.cmd
        net start service_win

