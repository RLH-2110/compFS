mkdir obj
wcl -DOS_DOS -c -0 -fo=obj\ src\*.c > complog.txt
wlib -n libcmpfs.lib +obj\fsdos.obj +obj\fsshared.obj +obj\setup.obj


wcl test\tests.c test\req\str.c libcmpfs.lib -0 -fe=test/test.exe > complog2.txt
cd test
test.exe > test.txt
type test.txt