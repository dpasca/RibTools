mkdir _tmpdocs

call docs\src\make_doc . readme .
call docs\src\make_doc docs\src User_Manual docs
call docs\src\make_doc docs\src Dev_Manual docs

del /Q _tmpdocs\*
rmdir /Q _tmpdocs

