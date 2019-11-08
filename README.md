Windows local service sample
==========================================================================================

Sample windows of windows service run under local service account and with manifest for log.

How to execute:
=================
Run under user:

        mkdir build_dir
        cd build_dir
        cmake build ..
        
Run under admin:

        register_log.cmd
        create_service.cmd
        net start service_win

