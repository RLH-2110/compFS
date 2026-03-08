import os

todoPaths = [os.path.abspath("..")];
wrongFiles = 0;
hits = 0;
currentPath = ""
root = todoPaths[0] + "/"
relativePath = ""

print("\n");

while (len(todoPaths) > 0):
       
    if os.path.isdir(todoPaths[0]):
        currentPath = todoPaths[0] + "/";
        
    name =  os.path.split(todoPaths[0])[1].split(".")
    
    #print("current file: {0}\n".format(os.path.split(todoPaths[0])[1]));
    if (currentPath != root):
        relativePath = todoPaths[0].split(root)[1];
    else:
        relativePath = ".."
    
    if len(name[0]) > 8 or len(name[0]) == 0:
        print("{0} is not a valid filename in DOS!".format(os.path.split(todoPaths[0])[1]));
        hits = hits + 1
    else:
        if len(name) == 2:
            if len(name[1]) > 3:
                print("{0} is not a valid filename in DOS!".format(os.path.split(todoPaths[0])[1]));
                hits = hits + 1
    
    firstFilenameChar = os.path.split(todoPaths[0])[1][0]
    if os.path.isdir(todoPaths[0]) and firstFilenameChar != ".":
        #print ("{0} is a directory and it does not start with .!".format(todoPaths[0]));
        
        newDirs = os.listdir(currentPath);
        
        i = 0;
        while (i < len(newDirs)):
            newDirs[i] = currentPath + newDirs[i];
            i = i + 1
            
        # print(newDirs)
        todoPaths.extend(newDirs); # append the result of the file
    
    todoPaths.pop(0);

if (hits == 0):
    print("All files are compatible with MS-DOS!");
else:
    print("\n{0} files are not compatible with MS-DOS".format(hits));