@echo off

start ..\..\..\tools\textpad\TextPad.exe doc\notes.txt ram.inc macroses.inc bankF.inc bankE.inc bankD.inc bankC.inc bankB.inc bankA.inc bank9.inc bank7.inc bank6.inc bank5.inc bank4.inc bank3.inc bank2.inc bank1.inc bank0.inc

pause 0

:again

del !simprev.nes
rename !simcity.nes !simprev.nes

cd nas

echo assemble...

dasm bank0.nas -f3 -obank0.bin
dasm bank0.nas -f3 -l!simcity.lst -obank0.bin > !err.log

for %%f in (*.nas) do call :dodasm %%f > NUL

goto :build

:dodasm
dasm %1 -f3 -o%~n1.bin
goto :eof

:build
echo build...

rename hdr.bin !simcity.hdr > NUL
rename chr.bin !simcity.chr > NUL

copy /b bank0.bin+bank1.bin+bank2.bin+bank3.bin+bank4.bin+bank5.bin+bank6.bin+bank7.bin tmp0.bin > NUL
copy /b bank8.bin+bank9.bin+bankA.bin+bankB.bin+bankC.bin+bankD.bin+bankE.bin+bankF.bin tmp1.bin > NUL
copy /b tmp0.bin+tmp1.bin !simcity.prg > NUL

nesimage j !simcity > NUL

echo cleanup...

for %%f in (*.bin) do del %%f
rem del !err.log
del !simcity.prg
del !simcity.chr
del !simcity.hdr

copy /b !simcity.nes ..\!simcity.nes > NUL

del !simcity.nes

cd ..

echo ===OLD ROM===
g:\dos\m3checksum !simprev.nes
echo ===NEW ROM===
g:\dos\m3checksum !simcity.nes

echo done.
pause 0

goto :again

:eof
