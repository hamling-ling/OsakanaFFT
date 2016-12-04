set BATDIR=%~DP0
set DSTROOT=%BATDIR%\..\..\release_src
set OSAKANAFFT_DSTDIR=%DSTROOT%\OsakanaFFT
set PITCHDETECTOR_DSTDIR=%DSTROOT%\PitchDetector

set TARGET=%1

rd /S /Q %DSTROOT%
mkdir %OSAKANAFFT_DSTDIR%\include
robocopy %BATDIR%\..\..\src\OsakanaFFT\include %OSAKANAFFT_DSTDIR%\include
robocopy %BATDIR%\..\..\src\OsakanaFFT\src %OSAKANAFFT_DSTDIR%\src

robocopy %BATDIR%\..\..\src\OsakanaPitchDetection\include %PITCHDETECTOR_DSTDIR%\include
robocopy %BATDIR%\..\..\src\OsakanaPitchDetection\src %PITCHDETECTOR_DSTDIR%\src

if %TARGET%==KURUMI (
	del %OSAKANAFFT_DSTDIR%\include\OsakanaFft.h
	del %OSAKANAFFT_DSTDIR%\src\OsakanaFft.cpp
	del %OSAKANAFFT_DSTDIR%\src\OsakanaFftUtil.h
	del %PITCHDETECTOR_DSTDIR%\include\LooseMelodyDetector.h
	del %PITCHDETECTOR_DSTDIR%\include\OsakanaPitchDetection.h
	del %PITCHDETECTOR_DSTDIR%\src\ContinuityDetector.h
	del %PITCHDETECTOR_DSTDIR%\src\ContinuityDetector.cpp
	del %PITCHDETECTOR_DSTDIR%\src\LooseMelodyDetector.cpp
	del %PITCHDETECTOR_DSTDIR%\src\NoteTable8.h
	del %PITCHDETECTOR_DSTDIR%\src\NoteTable8_N512.h
	del %PITCHDETECTOR_DSTDIR%\src\NoteTable8IndexRange.h
	del %PITCHDETECTOR_DSTDIR%\src\NoteTable8IndexRange_N512.h
	del %PITCHDETECTOR_DSTDIR%\src\OsakanaPitchDetection.cpp
	del %PITCHDETECTOR_DSTDIR%\src\PeakDetectMachine.cpp
)

