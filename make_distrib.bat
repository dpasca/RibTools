mkdir ..\Distrib
copy readme.html ..\Distrib\
xcopy /E /Y DistribSrc\base ..\Distrib

REM == remove this for the time being
del ..\Distrib\rendermn.ini

xcopy /E /Y DistribSrc\pc ..\Distrib
mkdir ..\Distrib\docs
mkdir ..\Distrib\docs\docs_content
xcopy /E /Y docs\*.txt ..\Distrib\docs\
xcopy /E /Y docs\*.html ..\Distrib\docs\
xcopy /E /Y docs\*.pdf ..\Distrib\docs\
xcopy /E /Y docs\docs_content ..\Distrib\docs\docs_content
