mkdir build
mkdir build\Distrib
copy readme.html build\Distrib\
xcopy /E /Y DistribSrc\base build\Distrib

REM == remove this for the time being
del build\Distrib\rendermn.ini

xcopy /E /Y DistribSrc\pc build\Distrib
mkdir build\Distrib\docs
mkdir build\Distrib\docs\docs_content
xcopy /E /Y docs\*.txt build\Distrib\docs\
xcopy /E /Y docs\*.html build\Distrib\docs\
xcopy /E /Y docs\*.pdf build\Distrib\docs\
xcopy /E /Y docs\docs_content build\Distrib\docs\docs_content
