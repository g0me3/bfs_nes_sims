cd nas

echo assemble...

for f in *.nas; do ./dasm $f -f3 -o"${f%.nas}.bin"; done

echo build...

mv hdr.bin !simcity.hdr
mv chr.bin !simcity.chr

cat bank0.bin bank1.bin bank2.bin bank3.bin bank4.bin bank5.bin bank6.bin bank7.bin bank8.bin bank9.bin bankA.bin bankB.bin bankC.bin bankD.bin bankE.bin bankF.bin > !simcity.prg
cat !simcity.hdr !simcity.prg !simcity.chr > !simcity.nes

echo cleanup...

rm *.bin
rm !simcity.hdr !simcity.prg !simcity.chr

cp !simcity.nes ../!simcity.nes

echo done.
