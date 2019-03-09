@echo off

:again

cd nas

echo assemble...

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
del !simcity.prg
del !simcity.hdr
del !simcity.chr

copy /b !simcity.nes ../!simcity.nes > NUL
cd ..

echo done.
