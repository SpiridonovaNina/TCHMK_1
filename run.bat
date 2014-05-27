@ECHO off
set /P aFileName="Enter A file name: "

set /P operation="Enter operation [+ - * / %%]: "

set /P bFileName="Enter B file name: "

set /P resFileName="Enter result file name: "

@ECHO on
TCHMK_1 "%aFileName%" "%operation%" "%bFileName%" "%resFileName%"
pause