mkdir ..\Distrib
xcopy /E /Y DistribSrc\base ..\Distrib

REM == remove this for the time being
del ..\Distrib\rendermn.ini

xcopy /E /Y DistribSrc\pc ..\Distrib

