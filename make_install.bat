xcopy /e /y Distrib RibTools\
"c:\Program Files\7-Zip\7z.exe" a -r -tzip -x!TestsOutput -x!*.autogen.* -x!Unused -x!*.pdb RibTools01.zip RibTools\*
rmdir /s /q RibTools
