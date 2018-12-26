mkdir .\backup\current
mkdir .\backup\current\nas
mkdir .\backup\current\chr
mkdir .\backup\current\doc
rem mkdir .\backup\current\tlm

copy /b *.bat .\backup\current\*.bat
copy /b *.inc .\backup\current\*.inc
copy /b nas\*.exe .\backup\current\nas\*.exe
copy /b nas\*.nas .\backup\current\nas\*.nas
copy /b chr\*.chr .\backup\current\chr\*.chr
copy /b doc\*.txt .\backup\current\doc\*.txt
rem copy /b tlm\*.* .\backup\current\tlm\*.*
copy /b *.txt .\backup\current\*.txt
