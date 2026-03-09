mkdir obj
wcl386 -DOS_DOS -c -fo=obj\ src\*.c > complog.txt
wlib -n libcmpfs.lib +obj\fsdos.obj +obj\fsshared.obj +obj\setup.obj
