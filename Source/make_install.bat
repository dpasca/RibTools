cd ..
xcopy /e /y Distrib RibTools\
"c:\Program Files\7-Zip\7z.exe" a -r -tzip -x!TestsOutput -x!TestsOutputAqsis -x!*.autogen.* -x!*.slx* -x!Unused -x!*.pdb RibTools[VERSION].zip RibTools\*
rmdir /s /q RibTools
