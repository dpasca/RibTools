mkdir ..\Distrib
xcopy /E /Y DistribSrc\base ..\Distrib

mkdir ..\Distrib\docs
copy docs\*.pdf ..\Distrib\docs

REM == remove this for the time being
del ..\Distrib\rendermn.ini

xcopy /E /Y DistribSrc\pc ..\Distrib

