@echo off
SET ScriptsDir=%~dp0
SET DepsDir=%ScriptsDir%..\Dependencies
SET SourceDir=%ScriptsDir%..\Source

echo soup build %DepsDir%\Opal\UnitTests\
call soup build %DepsDir%\Opal\UnitTests\
if %ERRORLEVEL% NEQ 0 exit /B %ERRORLEVEL%
echo soup run %DepsDir%\Opal\UnitTests\
call soup run %DepsDir%\Opal\UnitTests\
if %ERRORLEVEL% NEQ 0 exit /B %ERRORLEVEL%

echo soup build %SourceDir%\Build\Utilities.UnitTests\
call soup build %SourceDir%\Build\Utilities.UnitTests\
if %ERRORLEVEL% NEQ 0 exit /B %ERRORLEVEL%
echo soup run %SourceDir%\Build\Utilities.UnitTests\
call soup run %SourceDir%\Build\Utilities.UnitTests\
if %ERRORLEVEL% NEQ 0 exit /B %ERRORLEVEL%

echo soup build %SourceDir%\Extensions\Cpp\Compiler\Core.UnitTests\
call soup build %SourceDir%\Extensions\Cpp\Compiler\Core.UnitTests\
if %ERRORLEVEL% NEQ 0 exit /B %ERRORLEVEL%
echo soup run %SourceDir%\Extensions\Cpp\Compiler\Core.UnitTests\
call soup run %SourceDir%\Extensions\Cpp\Compiler\Core.UnitTests\
if %ERRORLEVEL% NEQ 0 exit /B %ERRORLEVEL%

echo soup build %SourceDir%\Extensions\Cpp\Compiler\Clang.UnitTests\
call soup build %SourceDir%\Extensions\Cpp\Compiler\Clang.UnitTests\
if %ERRORLEVEL% NEQ 0 exit /B %ERRORLEVEL%
echo soup run %SourceDir%\Extensions\Cpp\Compiler\Clang.UnitTests\
call soup run %SourceDir%\Extensions\Cpp\Compiler\Clang.UnitTests\
if %ERRORLEVEL% NEQ 0 exit /B %ERRORLEVEL%

echo soup build %SourceDir%\Extensions\Cpp\Compiler\MSVC.UnitTests\
call soup build %SourceDir%\Extensions\Cpp\Compiler\MSVC.UnitTests\
if %ERRORLEVEL% NEQ 0 exit /B %ERRORLEVEL%
echo soup run %SourceDir%\Extensions\Cpp\Compiler\MSVC.UnitTests\
call soup run %SourceDir%\Extensions\Cpp\Compiler\MSVC.UnitTests\
if %ERRORLEVEL% NEQ 0 exit /B %ERRORLEVEL%

REM echo soup build %SourceDir%\Extensions\Cpp\Extension.UnitTests\
REM call soup build %SourceDir%\Extensions\Cpp\Extension.UnitTests\
REM if %ERRORLEVEL% NEQ 0 exit /B %ERRORLEVEL%
REM echo soup run %SourceDir%\Extensions\Cpp\Extension.UnitTests\
REM call soup run %SourceDir%\Extensions\Cpp\Extension.UnitTests\
REM if %ERRORLEVEL% NEQ 0 exit /B %ERRORLEVEL%

echo soup build %SourceDir%\Extensions\CSharp\Compiler\Core.UnitTests\
call soup build %SourceDir%\Extensions\CSharp\Compiler\Core.UnitTests\
if %ERRORLEVEL% NEQ 0 exit /B %ERRORLEVEL%
echo soup run %SourceDir%\Extensions\CSharp\Compiler\Core.UnitTests\
call soup run %SourceDir%\Extensions\CSharp\Compiler\Core.UnitTests\
if %ERRORLEVEL% NEQ 0 exit /B %ERRORLEVEL%

REM echo soup build %SourceDir%\Extensions\CSharp\Extension.UnitTests\
REM call soup build %SourceDir%\Extensions\CSharp\Extension.UnitTests\
REM if %ERRORLEVEL% NEQ 0 exit /B %ERRORLEVEL%
REM echo soup run %SourceDir%\Extensions\CSharp\Extension.UnitTests\
REM call soup run %SourceDir%\Extensions\CSharp\Extension.UnitTests\
REM if %ERRORLEVEL% NEQ 0 exit /B %ERRORLEVEL%

