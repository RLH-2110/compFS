> [!WARNING]
> This readme is not complete
> [!WARNING]
> I still need to write some of the compiling scripts, since I moved this out from imgedit, and now need to compile this as a static lib.

This is a private FS library that should work on Linux, Windows NT, MS-DOS and Haiku

Here is what you need to do to compile the source for the different operating systems.
Linux and Haiku require Make and GCC, which are usually preinstalled.  
On Windows you have to install them yourself or compile my code manually with your own compiler.  
  
Some operating systems like DOS or early windows versions have scripts that requrie the <a href="https://github.com/open-watcom/open-watcom-v2">Open Watcom</a> compiler

Make for Windows: https://gnuwin32.sourceforge.net/packages/make.htm  
GCC for Windows: https://www.mingw-w64.org/  
  
In testing.csv you can see which operating systems where testsed, which OS versions where tested and when they where tested.  

<table>
    <thead>
        <tr>
            <th>Compiling</th>
            <th>Requirements</th>
            <th>Command</th>
            <th>Testing Command</th>
            <th>Versions Tested</th>
        </tr>
    </thead>
    <tbody>
        <tr>
            <td>Linux</td>
            <td>GCC, Make</td>
            <td>make</td>
            <td>make test</td>
        </tr>
        <tr>
            <td>Windows<br /></td>
            <td>
                GCC, Make<br /><b>OR</b><br />
                <a href="https://github.com/open-watcom/open-watcom-v2">Open Watcom</a><br />
            </td>
            <td>make<br /><b>OR</b><br />COMPNT</td>
            <td>make test<br /><b>OR</b><br />NTTEST</td>
        </tr>
        <tr>
            <td>DOS</td>
            <td><a href="https://github.com/open-watcom/open-watcom-v2">Open Watcom</a></td>
            <td>COMPDOS</td>
            <td>COMPTEST</td>
        </tr>
        <tr>
            <td>Haiku</td>
            <td>GCC, Make</td>
            <td>make</td>
            <td>make test</td>
        </tr>
    </tbody>
</table>

# Potential Build Errors

## COMPNT / NTTEST

### The name specified is not recognized
If you get this error:
```
The name specified is not recognized as an
internal or external command, operable program or batch file.  
```
  
It means wcl is not found in your current cmd.  
you can use an cmd from open watcom, that should have a path to wlc, you can find it on windows like this:  
`Start -> Programs -> Open Watcom 2.0 C - C++ -> Build Enviroment`



