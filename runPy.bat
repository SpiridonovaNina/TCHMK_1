@ECHO off
set /P aFileName="Enter A file name: "

set /P operation="Enter operation [+ - * / %%]: "

set /P bFileName="Enter B file name: "

set /P resFileName="Enter result file name: "

@ECHO on
main.py "%aFileName%" "%operation%" "%bFileName%" "%resFileName%"
pause