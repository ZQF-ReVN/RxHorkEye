# ACV1FileHook
ACV1 Engine Dump Extract Read FileHook

# Usage
[GuideVideoHere](https://www.youtube.com/watch?v=t9PzEKXvcJs)  
You can call one of these in the [dllmain.cpp](https://github.com/Dir-A/ACV1FileHook/blob/main/ACV1FileHook/ACV1FileHook/dllmain.cpp) by removing the comment.  
```c
//Attention!! Only one of the three functions can be called at a time
SetFileDump();      //Extracting files from a running game  
SetFileExtract();   //Extracting files by filename  
SetFileHook();      //Read files without repack  
```


If you want to hook or dump script  
Call one of these functions.  
And you must to find these two functions address first.  
```c
/*
Game:		ハナヒメ＊アブソリュート！
LoadScript:	rva:0xCE9B0
DecScript:	rva:0x12AC00
	
Game:		我が姫君に栄冠を
LoadScript:	rva:0xD18B0
DecScript:	rva:0x1389E0

Game:		我が姫君に栄冠をFD
LoadScript:	rva:0xD30F0
DecScript:	rva:0x13A540
*/
//SetScriptDump(0xD30F0, 0x13A540);
SetScriptHook(0xD30F0, 0x13A540);
```

## About SetFileDump(); 
This function will dump file that are read during the game run.  
Files will be saved in `Dump` folder in game directory.  
Also the file name will be saved in `ACV1FileNameList.txt`.  
That text file can help you extract files from other tools or call the `SetFileExtract();` function.  

## About SetFileExtract();
This function extracts file by entering the filename.  
Files will be saved in `Extract` folder in game directory.  

## About SetFileHook();
This function allows the game engine to read file without repack(Except for script.dat file).  
Put the file to be read into `FileHook` folder.  
But remember the path is needed,You can copy `Dump` folder.  

## About SetScriptDump();
This function dump script files while the game is running.  
The game will actually load all the script files when it runs.  
So after game is opened and you can get all the script files in `Dump` folder.  
Script file name is composed of the first line of the file starting with * and the hash value  
`[first line]#[hash value]` like this `01A_0600_0#c7bad15bb65b0057`  

## About SetScriptHook();
Put dump script files into the `.\\FileHook\\Script`  
the script file name must be  
like this `[first line]#[hash value]`  
like this `01A_0600_0#c7bad15bb65b0057`  
