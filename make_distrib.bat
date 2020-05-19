mkdir _distrib
xcopy /E /Y DistribSrc\base _distrib

mkdir _distrib\docs
copy docs\*.pdf _distrib\docs

REM == remove this for the time being
del _distrib\rendermn.ini

xcopy /E /Y DistribSrc\pc _distrib

