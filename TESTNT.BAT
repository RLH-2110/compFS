mkdir obj
wcl386 -DOS_DOS -c -fo=obj\ src\*.c > complog.txt
wlib -n libcmpfs.lib +obj\fsdos.obj +obj\fsshared.obj +obj\setup.obj


wcl386 test\tests.c test\req\str.c libcmpfs.lib -fe=test/test.exe > complog2.txt
cd test
test.exe > test.txt
type test.txt