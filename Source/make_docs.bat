mkdir ..\tmpdocs

call docs\src\make_doc . readme .
call docs\src\make_doc docs\src User_Manual docs

del /Q ..\tmpdocs\*
rmdir /Q ..\tmpdocs

