# ACV1FileHook
ACV1 Engine Dump Extract Read FileHook

# Usage
This tool has three functions.  [GuideVideoHere](https://www.youtube.com/watch?v=t9PzEKXvcJs)
```c
//Attention!! Only one of these functions can be called at a time
SetFileDump();      //Extracting files from a running game  
SetFileExtract();   //Extracting files by filename  
SetFileHook();      //Read files without repack  
```
You can call one of these in the [dllmain.cpp](https://github.com/Dir-A/ACV1FileHook/blob/main/ACV1FileHook/ACV1FileHook/dllmain.cpp) by removing the comment.  

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
