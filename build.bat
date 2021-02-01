@echo off
cls
python "D:\boot\main.py" 5 x64
del *.obj&&del index.exe&&cl /EHsc *.cpp /link /OUT:index.exe&&del *.obj&&cls&&index.exe
if exist index.exe (
	rem del index.exe
)
