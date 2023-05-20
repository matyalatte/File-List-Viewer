# Getting Started

## 1. File Format

Input file should be a list of paths.  
And each line should have a file path like this.  

```
noneed/foo/bar.png
noneed/foo/hello.txt
wantit/test/this.txt
wantit/that.jpg
what
```

## 2. Open Path List

Run exe and drop a file onto the window.  
It will read the path list.  

![OpenFile](https://user-images.githubusercontent.com/69258547/231189381-420ebdf2-8463-4ff9-bae8-4bed6f3eb550.png)

> You can also drop a file onto the exe to open the window.

## 3. Check Files

Check file paths you want to export.  
You can double-click files to check them.  
You can also right-click a folder and select `Check All` to check all files in the folder.  

![GUI](https://user-images.githubusercontent.com/69258547/231185813-1d31405b-cefb-4f52-9255-8ce24dbeb9c5.png)

> If a check mark is on a folder, it means all files in the folder are checked.  

## 4. Dump Paths

Save the selected paths. (`Menu->Save`)  
You can get a new txt file like this.  

```
wantit/test/this.txt
wantit/that.jpg
```

## Create Directories

You can drop a file onto `_mkdir.bat` to make directories.  
It'll make all directories that are written in the file.  
Its root directory will be the same dir as the input file.  

## Sort Path List

You can drop a file onto `_sort.bat` to sort paths.  
The sorted list (`*.new`) will be generated in the same directory as the input file.  

## Convert Paths to Lower Case Strings

You can drop a file onto `_lower.bat` to convert paths to lower case strings.  
The converted list (`*.new`) will be generated in the same directory as the input file.  
