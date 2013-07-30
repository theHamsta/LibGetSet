@rem Assumes that the AutoGui and GetSet's config utility are installed to ".\AutoGUI\"
@echo off
:configure
.\AutoGUI\AutoConf compute_fractal.ini --button Compute...
if not %errorlevel% == 0 goto cancel
@rem Notice that number of iterations is always 100
@rem Also note how strings should be escaped like this \"bla bla\"
.\AutoGUI\config run compute_fractal.ini "compute_fractal.exe \"GetSet[Image/File]\" GetSet[Image/Width] GetSet[Image/Height] GetSet[Fractal/Scale] 100 GetSet[Fractal/Center/Real] GetSet[Fractal/Center/Imaginary]"
if not %errorlevel% == 0 goto configure
pause
:cancel
