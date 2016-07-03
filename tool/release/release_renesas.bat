set BATDIR=%~DP0
set DSTROOT=%BATDIR%\..\..\release_src
set OSAKANAFFT_DSTDIR=%DSTROOT%\OsakanaFFT

rd /S /Q %DSTROOT%
mkdir %OSAKANAFFT_DSTDIR%\include
robocopy %BATDIR%\..\..\src\OsakanaFFT\include %OSAKANAFFT_DSTDIR%\include
robocopy %BATDIR%\..\..\src\OsakanaFFT\src %OSAKANAFFT_DSTDIR%\src
