@echo off 
set RootDir=B:\Workspace\NetlibExample1\NetlibExample1\
set sources[0]=Crawler

for /F "tokens=2 delims==" %%s in ('set sources[') do moc.exe "%RootDir%%%s.h" -o "%(RootDir)%generated files\moc_%%%s.cpp"
