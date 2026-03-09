mkdir obj
wcl -DOS_DOS -c -0 -fo=obj\ src\*.c > complog.txt
wlib -n libcmpfs.lib +obj\fsdos.obj +obj\fsshared.obj +obj\setup.obj
