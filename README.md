# DOS-and-TextEditor
Mini Dos implementation in C++ with built in Text editor to create and edit files.
If files are not being created,make sure to add your own path using CTRL+F in ifstream and ofstream.
Use HELP command to get information of all functions.

**Implementation:**
- Using classes for folders containing a list of folder pointers (tree like structure)
- Using class for file where a folder object contains a list of files

**Functionality of Dos:**
- Create files and folders
- Find files and folders
- Copy and move files
- Open files in text editor
- display current directory
- display all directories
- Change extension of files
- Imitate printing behaviour by sending files in printer queue(time is not dynamic)
- Remove files and folders
- Erase all files and folders
- Display Dos in tree format
- Save and load program

**Functionality of TextEditor:**
Implemented uisng a 2d Linked list of character since it provides constant time removal and insertion given an iterator.
- Adding text
- Deleting and backspace
- Undo and Redo
- Save the state of Editor
