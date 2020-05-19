@echo off

..\..\docutils\tools\rst2latex.py %1\%2.txt ..\tmpdocs\%2.tex --latex-preamble="\usepackage{geometry} \geometry{hmargin={2.5cm,2.5cm},height=10in} \usepackage{parskip}" --font-encoding=""

pdflatex -aux-directory="..\tmpdocs" -include-directory="docs\src" -output-directory="%3" ..\tmpdocs\%2.tex

