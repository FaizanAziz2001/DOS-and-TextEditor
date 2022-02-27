#pragma once
#include<iostream>
#include<string>
#include<list>
#include<time.h>
#include <iterator>
#include<iomanip>
#include"TextEditor.h"
#include<Windows.h>
#include<deque>
#include<fstream>
#include<queue>
#include<cstdlib>
using namespace std;


struct file
{
	string name, data, date, owner, location;
	string priority, extension;
	int size;
	int print_time, noc, lines;
	bool ReadOnly;
	TextEditor* T;
public:
	file(string n = " ")
	{
		name = n;
		priority = "low";
		extension = "txt";
		noc = 0;
		lines = 0;
		ReadOnly = false;
		T = nullptr;
	}

	file(file* file)
	{
		Deepcopy(file);
	}

	void Deepcopy(file* file)
	{
		name = file->name;
		data = file->data;
		priority = file->priority;
		print_time = file->print_time;
		size = file->size;
		location = file->location;
		data = file->date;
		owner = file->owner;
		extension = file->extension;
	}
};

struct folder
{
	string name, date, owner, location;
	folder* parent;
	list<folder*> dir;
	list<file*> files;
	int size;
public:
	folder(folder* p = nullptr, string n = " ")
	{
		parent = p;
		name = n;

	}

	folder* Search(string name)
	{
		for (folder* player : dir)
		{
			if (player->name == name)
				return player;
		}

		return nullptr;
	}

	file* Searchfile(string name)
	{
		for (file* player : files)
		{
			if (player->name == name)
				return player;
		}

		return nullptr;
	}
};

struct CustomComp
{
	bool operator()(const file* file1, const file* file2)
	{
		return file1->priority > file2->priority;
	}
};

class Dos
{
	folder* Vdrive = new folder(nullptr, "system");
	folder* CF = Vdrive;
	file* clipboard = nullptr;
	string username, prompt;
	deque<file*> q;
	priority_queue<file*, vector<file*>, CustomComp> pq;
	vector<file*> TE;

	string getTime()
	{
		char str[26];
		time_t result = time(NULL);
		ctime_s(str, sizeof str, &result);
		return str;
	}

	void ToUpper(string& name)
	{
		for (int i = 0; i < name.size(); i++)
		{
			if (islower(name[i]))
				name[i] = toupper(name[i]);
		}
	}

	void PrintDir_recursion(folder* n)
	{
		if (n == nullptr)
			return;
		PrintDir_recursion(n->parent);
		cout << n->name << "/";
	}

	void SavePathFile_recursion(file* n, folder* c)
	{
		if (c == nullptr)
			return;
		SavePathFile_recursion(n, c->parent);
		n->location = n->location + (c->name + "/");
	}

	void SavePathFolder_recursion(folder* n, folder* c)
	{
		if (c == nullptr)
			return;
		SavePathFolder_recursion(n, c->parent);
		n->location = n->location + (c->name + "/");
	}

	void Tree_Display_recursion(folder* r, int depth)
	{
		if (!r)
			return;
		for (folder* temp : r->dir)
		{
			cout << "|";
			for (int i = 0; i < depth; i++)
			{
				cout << "\t";
			}
			cout << "|--";
			cout << temp->name << endl;
			Tree_Display_recursion(temp, depth + 1);
		}

		for (file* temp : r->files)
		{
			cout << "|";
			for (int i = 0; i < depth; i++)
			{
				cout << "\t";
			}
			cout << "|--";
			cout << temp->name << endl;
		}
	}

	void Save_Tree_recursion(folder* r, ofstream& fout, int depth)
	{
		if (!r)
			return;
		for (folder* temp : r->dir)
		{
			for (int i = 0; i < depth; i++)
			{
				fout << "-" << " ";
			}
			fout << "<DIR> " << temp->name << endl;
			Save_Tree_recursion(temp, fout, depth + 1);
		}

		for (file* temp : r->files)
		{
			for (int i = 0; i < depth; i++)
			{
				fout << "-" << " ";
			}
			fout << "<File> " << temp->name << endl;
		}
	}

	void Load_Tree_iteration(folder* r, ifstream& fin)
	{
		int depth = 0;
		int prev_depth = 0;
		string c;
		string name;
		bool check = false;
		while (fin.good())
		{
			fin >> c;

			if (fin.eof())
				break;
			while (c == "-")
			{
				fin >> c;
				depth++;
				check = true;

			}

			if (check)
			{
				if (depth > prev_depth)
					CD(name);
				else if (depth < prev_depth)
					CDparent();
			}
			else
			{
				while (CF != Vdrive)
				{

					CDparent();
				}
			}

			if (c == "<DIR>")
			{
				fin >> name;
				MKDIR(name);
				check = false;
			}
			else if (c == "<File>")
			{
				fin >> name;
				Load_Existing_File(name);
				check = false;
			}

			prev_depth = depth;
			depth = 0;
		}
	}
public:

	Dos()
	{
		prompt = ">";
	}

	bool MKDIR(string name)
	{
		folder* check = CF->Search(name);

		if (!check)
		{
			folder* temp = new folder(CF, name);
			temp->date = getTime();
			temp->date.pop_back();
			temp->owner = username;
			CF->dir.push_back(temp);
			SavePathFolder(temp, CF);
			return true;
		}
		return false;
	}

	bool CD(string name)
	{
		folder* temp = CF->Search(name);
		if (temp == nullptr)
			return false;
		else
			CF = temp;
		return true;
	}

	bool Rename(string name, string name2)
	{
		file* temp = CF->Searchfile(name);
		if (!temp)
			return false;

		temp->name = name2;
		return true;

	}

	bool Queue(string name)
	{
		file* temp = CF->Searchfile(name);
		if (temp)
			q.push_back(temp);
		else
			return false;
		return true;
	}

	bool Priority_Queue(string name)
	{
		string p;
		file* temp = CF->Searchfile(name);
		if (!temp)
			return false;
		cout << "Set Priority(H/l): ";
		cin >> p;
		ToUpper(p);

		if (p == "H")
			temp->priority = "high";
		else if (p == "L")
			temp->priority = "low";

		pq.push(temp);
		return true;
	}

	bool Find(string name)
	{
		file* temp = CF->Searchfile(name);
		if (!temp)
		{
			return false;
		}
		return true;
	}

	bool FindF(string name, string text)
	{

		file* temp = CF->Searchfile(name);
		if (!temp)
			return false;
		string path = "E:/C++/BSCS20035_DOS/BSCS20035_DOS/files/" + name;
		ifstream fin(path);
		string line;


		while (fin.good())
		{
			getline(fin, line);
			auto pos = line.find(text);
			if (pos != string::npos)
			{
				fin.close();
				return true;
			}
		}
		fin.close();
		return false;
	}

	bool edit(string name)
	{

		char choice;
		file* file = CF->Searchfile(name);
		if (!file)
			return false;

		if (file->ReadOnly)
		{
			file->T->Print();
			cout << endl << "File is ReadOnly";
			return true;
		}

		system("cls");
		file->T->Print();
		file->T->Run();
		file->T->Save_File(name);
		file->T->Save_attributes(file->noc, file->lines);

		cout << endl;
		cout << "Do you want to close file(Y/N): " << endl;
		cin >> choice;

		if (choice == 'Y' || choice == 'y')
		{
			for (auto i = TE.begin(); i != TE.end(); i++)
			{
				if (file->name == (*i)->name)
					TE.erase(i);
			}
		}
		else if (choice == 'N' || choice == 'n')
		{
			bool check = false;
			for (auto i = TE.begin(); i != TE.end(); i++)
			{
				if (file->name == (*i)->name)
				{
					check = true;
					break;
				}

			}
			if (!check)
				TE.push_back(file);
		}
		cout << endl;

		return true;
	}

	bool Attrib(string name)
	{
		file* temp = CF->Searchfile(name);
		if (temp)
		{
			cout << "size: " << temp->size << endl;
			cout << "owner: " << temp->owner << endl;
			cout << "priority: " << temp->priority << endl;
			cout << "extension: " << temp->extension << endl;
			cout << "location: " << temp->location << endl;
			cout << "print time: " << temp->print_time << " seconds" << endl;
			cout << "Number of characters: " << temp->noc - 1 << endl;
			cout << "Number of lines: " << temp->lines << endl;
			cout << "Average characters per lines: " << float(temp->noc / temp->lines) << endl;
			cout << "Read Only: " << (temp->ReadOnly == 1 ? "True" : "False") << endl;
			cout << endl << endl;
		}
		else
			return false;
		return true;

	}

	bool Attribr(string name)
	{
		file* temp = CF->Searchfile(name);
		if (!temp)
			return false;

		temp->ReadOnly = !(temp->ReadOnly);
		return true;
	}

	bool Copy(string name)
	{

		file* file = CF->Searchfile(name);
		if (!file)
			return false;
		return true;
	}

	bool MakeAcopy(string name, string path, file* f)
	{
		file* copy = new file(f);
		string temp_path;						//format system/dirname/
		folder* temp = Vdrive;

		if (path.back() != '/')
			return false;
		for (int i = 0; i < path.length(); i++)
		{
			if (path[i] != '/')
				temp_path.push_back(path[i]);
			else if (temp_path == "system")
			{
				temp_path.clear();
				continue;
			}
			else
			{
				temp = temp->Search(temp_path);
				if (temp == nullptr)
					return false;
				temp_path.clear();
			}

		}

		temp->files.push_back(copy);
		SavePathFile(copy, temp);
		return true;
	}

	bool Move(string name)
	{
		file* file = CF->Searchfile(name);
		if (!file)
			return false;
		return true;
	}

	bool Paste()
	{
		if (!clipboard)
			return false;

		file* temp = CF->Searchfile(clipboard->name);
		if (!temp)
		{
			if (clipboard)
			{
				CF->files.push_back(clipboard);
				return true;
			}
			return false;
		}
		else
		{
			string t;
			if (!clipboard)
			{
				return false;
			}

			cout << "Do you want to overwrite already existing file(Y/N)";
			cin >> t;

			if (t == "Y")
			{
				CF->files.push_back(clipboard);
			}
			return true;

		}
	}

	bool Del(string name)
	{
		file* temp = CF->Searchfile(name);
		if (temp)
		{
			CF->files.remove(temp);
			return true;
		}
		return false;

	}

	void CDparent()
	{
		if(CF->parent!=nullptr)
		CF = CF->parent;
	}

	void CDroot()
	{
		CF = Vdrive;
	}

	void Prompt(string p)
	{
		prompt = p;
	}

	void Convert(string ext, string convert)
	{
		for (file* temp : CF->files)
		{
			if (temp->extension == ext)
			{
				temp->extension = convert;
			}
		}
	}

	void FindStr(string text)
	{
		string path = "E:/C++/BSCS20035_DOS/BSCS20035_DOS/files/";
		string temp = path;
		ifstream fin(path);
		string line;
		vector<string> v;

		for (file* file : CF->files)
		{
			path = path + file->name;
			fin.open(path);
			while (fin.good())
			{
				getline(fin, line);
				auto p = line.find(text);
				if (p != string::npos)
				{
					v.push_back(file->name);
					line.erase();
					break;
				}
			}
			fin.close();
			path = temp;
		}

		cout << endl << "Filename" << endl << endl;
		for (int i = 0; i < v.size(); i++)
		{
			cout << v[i] << endl;
		}

	}

	void SavePathFile(file* n, folder* c)
	{
		n->location = "v:/";
		SavePathFile_recursion(n, c);
	}

	void SavePathFolder(folder* n, folder* c)
	{
		n->location = "v:/";
		SavePathFolder_recursion(n, c);
	}

	void RMDIR()
	{
		if (CF->parent == nullptr)
			return;
		folder* temp = CF->parent;
		temp->dir.remove(CF);
		delete CF;
		CF = temp;
	}

	void Format()
	{
		delete Vdrive;
		Vdrive = new folder(nullptr, "system");
		CF = Vdrive;
	}

	bool CreateFiles(string name)
	{
		file* f = CF->Searchfile(name);
		if (f)
			return false;
		char choice;
		string path = "E:/C++/BSCS20035_DOS/BSCS20035_DOS/files/" + name;
		cin.ignore();
		cout << "Enter data" << endl;
		system("Pause");
		system("cls");
		ofstream fout(path);
		file* temp = new file(name);
		temp->T = new TextEditor;
		temp->T->Open_File(name);									//Enter data in text editor
		temp->T->Run();
		temp->T->Save_File(name);
		temp->T->Save_attributes(temp->noc, temp->lines);
		temp->date = getTime();
		temp->owner = username;
		temp->date.pop_back();
		temp->size = temp->noc;
		temp->print_time = temp->noc;
		CF->files.push_back(temp);
		SavePathFile(temp, CF);

		cout << endl;

		cout << "Do you want to close file(Y/N): " << endl;
		cin >> choice;

		if (choice == 'n' || choice == 'N')
			TE.push_back(temp);
		cout << endl;

		return true;
	}

	void Load_Existing_File(string name)
	{

		file* temp = new file(name);

		temp->date = getTime();
		temp->owner = username;
		temp->date.pop_back();
		temp->size = temp->noc;
		temp->print_time = temp->noc;
		temp->T = new TextEditor;
		temp->T->Open_File(temp->name);
		CF->files.push_back(temp);
		SavePathFile(temp, CF);
	}

	void PrintOnwership()
	{
		string temp;
		cout << "Enter username ";
		getline(cin, username);
		system("cls");
		cout << "Dos Shell,BSCS20035," << username << endl << endl;
	}

	void Print()
	{
		/*cout << "v:/" << CF->name << "/" << endl;*/
		for (folder* temp : CF->dir)
		{
			cout << temp->date << "\t\t" << "<DIR>" << "\t" << temp->name << endl;
		}

		for (file* temp : CF->files)
		{
			cout << temp->date << "\t\t" << "<File>" << "\t" << temp->name << endl;
		}

	}

	void PrintDir()
	{
		cout << "v:/";
		PrintDir_recursion(CF);
	}

	void PrintVer()
	{
		cout << "Version 1.02" << endl;
		cout << "Creater by ITU ENTERPRISE TM" << endl;
	}

	void Tree_Display()
	{
		cout << endl << Vdrive->name << endl;
		Tree_Display_recursion(Vdrive, 1);
	}

	void DisplayQueue()
	{
		int time = 5;
		cout << endl << setw(40) << left << "Filename" << setw(0) << left << "Time" << endl;

		if (q.empty())
			return;
		auto print_file = q.front();
		print_file->print_time -= time;
		if (print_file->print_time <= 0)
		{
			print_file->print_time = print_file->size;
			q.pop_front();
		}
		for (file* temp : q)
		{
			cout << setw(40) << left << temp->name << setw(0) << left << temp->print_time << " seconds" << endl;
		}
	}

	void DisplayProrityQueue()
	{
		int time = 5;

		cout << endl << setw(50) << left << "Filename" << setw(0) << left << "Time" << endl;

		if (pq.empty())
			return;
		auto print_file = pq.top();
		print_file->print_time -= time;
		if (print_file->print_time <= 0)
		{
			print_file->print_time = print_file->size;
			pq.pop();
		}

		auto temp = pq;
		while (temp.size() != 0)
		{
			cout << setw(50) << left << temp.top()->name << setw(0) << left << temp.top()->print_time << " seconds" << endl;
			temp.pop();
		}
	}

	void Help()
	{
		cout << "DOS Functions:" << endl << endl;
		cout << setw(10) << left << "Attrib:" << setw(90) << left << " Display the atrributes of file who's name is provided" << "Syntax: attrib filename" << endl
			<< setw(10) << left << "Cd:" << setw(90) << left << " Changes the directory to other directories who's name is provided" << "Syntax: Cd directoryname" << endl
			<< setw(10) << left << "Cd. :" << setw(90) << left << " Prints the currently working directory" << "Syntax: Cd." << endl
			<< setw(10) << left << "CD.. :" << setw(90) << left << " Changes the directory to parent directory" << "Syntax: CD.." << endl
			<< setw(10) << left << "CD/ :" << setw(90) << left << " Returns to the root directory" << "Syntax: CD/" << endl
			<< setw(10) << left << "Convert:" << setw(90) << left << " Change extensions of files" << "Syntax: Convert prev_ext  new_ext" << endl
			<< setw(10) << left << "Copy:" << setw(90) << left << " Copy a file from current directory to given destination" << "Syntax: Copy filename path/" << endl
			<< setw(10) << left << "Move:" << setw(90) << left << " Move a file from current directory to given directory" << "Syntax: Move filename path/" << endl
			<< setw(10) << left << "Create:" << setw(90) << left << " Create a new file in current directory" << "Syntax: Create filename" << endl
			<< setw(10) << left << "Del:" << setw(90) << left << " Delete a file from current directory " << "Syntax: Del filename" << endl
			<< setw(10) << left << "Dir:" << setw(90) << left << " Display contents of current directory" << "Syntax: Dir" << endl
			<< setw(10) << left << "Edit:" << setw(90) << left << " Edit contents of a file from current directory" << "Syntax: Edit filename" << endl
			<< setw(10) << left << "Exit:" << setw(90) << left << " Exit the program" << "Syntax: Exit" << endl
			<< setw(10) << left << "Find:" << setw(90) << left << " Find a file with given name in current directory" << "Syntax: Find filename" << endl
			<< setw(10) << left << "Findf:" << setw(90) << left << " Find a string in given file" << "Syntax: Findf filename string" << endl
			<< setw(10) << left << "Findstr:" << setw(90) << left << " Display a list of files with given string in current directory" << "Syntax: Find string" << endl
			<< setw(10) << left << "Format:" << setw(90) << left << " Erase the drive" << "Syntax: Format" << endl
			<< setw(10) << left << "Mkdir:" << setw(90) << left << " Create a new Directory in current directory" << "Syntax: mkdir directoryname" << endl
			<< setw(10) << left << "Print:" << setw(90) << left << " Add a file into print queue" << "Syntax: print filename" << endl
			<< setw(10) << left << "PPrint:" << setw(90) << left << " Add a file into priority print queue" << "Syntax: pprint filename" << endl
			<< setw(10) << left << "Prompt:" << setw(90) << left << " Change prompt" << "Syntax: Prompt symbol" << endl
			<< setw(10) << left << "Pqueue:" << setw(90) << left << " Display priority print queue" << "Syntax: pqueue" << endl
			<< setw(10) << left << "Queue:" << setw(90) << left << " Display print queue" << "Syntax: queue" << endl
			<< setw(10) << left << "PWD:" << setw(90) << left << " print working directory " << "Syntax: pwd" << endl
			<< setw(10) << left << "Rename:" << setw(90) << left << " Rename a file " << "Syntax: rename old_filename new_filename" << endl
			<< setw(10) << left << "Rmdir:" << setw(90) << left << " Remove currect directory will all its content" << "Syntax: rmdir " << endl
			<< setw(10) << left << "Tree:" << setw(90) << left << " Display dos in tree format" << "Syntax: Tree" << endl
			<< setw(10) << left << "Save:" << setw(90) << left << " Save the current program" << "Syntax: Save" << endl
			<< setw(10) << left << "Load:" << setw(90) << left << " Load the program" << "Syntax: Load" << endl
			<< setw(10) << left << "Ver:" << setw(90) << left << " Display current version of Dos" << "Syntax: Ver" << endl << endl;
	}

	void Editor_interface()
	{

		char input;
		int choice;
		cout << "Filenames" << endl;
		for (int i = 0; i < TE.size(); i++)
		{
			cout << i + 1 << " " << TE[i]->name << endl;
		}

		if (TE.empty())
			return;
		cout << endl;
		do
		{
			cout << "Enter file number: ";
			cin >> choice;
			cout << endl;

			if (choice > TE.size())
				continue;
			else if (choice == 0)
				return;
			else
				break;
		} while (true);

		if (!TE.empty())
		{
			auto temp = TE[choice - 1];
			system("cls");
			temp->T->Print();
			temp->T->Run();
			temp->T->Save_File(temp->name);
			temp->T->Save_attributes(temp->noc, temp->lines);

			cout << "Do you want to close file(Y/N): " << endl;
			cin >> input;

			if (input == 'Y' || input == 'y')
			{
				if (TE.size() == 1)
					TE.pop_back();
				for (auto i = TE.begin(); i != TE.end(); i++)
				{
					if (temp->name == (*i)->name)
						TE.erase(i);
				}
			}
		}


	}

	void Save_Tree()
	{
		string name = "Save.txt";
		string path = "E:/C++/BSCS20035_DOS/BSCS20035_DOS/files/" + name;
		ofstream fout(path);
		Save_Tree_recursion(Vdrive, fout, 0);

		/*queue<folder*> save;
		save.push(Vdrive);
		save.push(nullptr);
		while (!save.empty())
		{
			auto temp = save.front();
			save.pop();
			if (temp)
			{
				fout << '[' << endl;
				for (folder* f : temp->dir)
				{
					save.push(f);
					fout << f->name << endl;
				}
				fout << ']' << endl;
			}
			else
			{
				if (save.size() > 0)
				{
					save.push(nullptr);
					fout << 'x' << endl;
				}
			}
		}*/

		cout << endl;
	}

	void Load_Tree()
	{
		string name = "Save.txt";
		string path = "E:/C++/BSCS20035_DOS/BSCS20035_DOS/files/" + name;
		ifstream fin(path);
		Format();
		Load_Tree_iteration(Vdrive, fin);
		CF = Vdrive;
	}

	void Run()
	{
		PrintOnwership();
		string input;
		string command;
		while (true)
		{
			PrintDir();
			cout << prompt << " ";
			cin >> input;
			ToUpper(input);
			if (input == "CD..")
			{
				CDparent();
			}
			else if (input == "CD/")
			{
				CDroot();
			}
			else if (input == "CD")
			{

				cin >> command;
				if (!CD(command))
				{
					cout << "Invalid folder name" << endl;
				}
			}
			else if (input == "MKDIR")
			{
				cin >> command;
				if (!MKDIR(command))
					cout << "Folder already exists" << endl;
			}
			else if (input == "CREATE")
			{
				cin >> command;
				if (CreateFiles(command))
					cout << "File created" << endl;
				else
					cout << "File already exists" << endl;
			}
			else if (input == "COPY")
			{
				string path;
				cin >> command;
				cin >> path;
				if (!Copy(command))
				{
					cout << "Invalid file" << endl;
				}
				file* file = CF->Searchfile(command);

				if (!MakeAcopy(command, path, file))
				{
					cout << "Invalid path" << endl;
				}
				else
					cout << "Copy successful" << endl;

			}
			else if (input == "ATTRIB")
			{
				cin >> command;
				if (!Attrib(command))
				{
					cout << "Invalid File" << endl;
				}
			}
			else if (input == "ATTRIBR")
			{
				cin >> command;
				if (!Attribr(command))
				{
					cout << "Invalid File" << endl;
				}
			}
			else if (input == "DEL")
			{
				cin >> command;
				if (!Del(command))
				{
					cout << "Invalid File" << endl;
				}
			}
			else if (input == "MOVE")
			{
				string path;
				cin >> command;
				cin >> path;
				if (!Move(command))
				{
					cout << "Invalid file" << endl;
				}
				file* file = CF->Searchfile(command);

				if (!MakeAcopy(command, path, file))
				{
					cout << "Invalid path" << endl;
				}
				else
					cout << "Move successful" << endl;

				CF->files.remove(file);
			}
			else if (input == "FIND")
			{
				cin >> command;
				if (!Find(command))
					cout << "No such File exists" << endl;
				else
					cout << "File exists" << endl;
			}
			else if (input == "FINDF")
			{
				string text;
				cin >> command;
				getline(cin, text);
				if (!FindF(command, text))
					cout << "Not found" << endl;
				else
					cout << "Found" << endl;
			}
			else if (input == "FINDSTR")
			{
				string text;
				cin.ignore();
				getline(cin, text);
				FindStr(text);
			}
			else if (input == "RMDIR")
			{
				RMDIR();
			}
			else if (input == "FORMAT")
			{
				Format();
				system("cls");
				PrintOnwership();
			}
			else if (input == "PROMPT")
			{
				cin >> command;
				Prompt(command);
			}
			else if (input == "QUEUE")
			{
				DisplayQueue();
			}
			else if (input == "PQUEUE")
			{
				DisplayProrityQueue();
			}
			else if (input == "PPRINT")
			{
				cin >> command;
				if (!Priority_Queue(command))
					cout << "Invalid filename" << endl;
			}
			else if (input == "PRINT")
			{
				cin >> command;
				if (!Queue(command))
					cout << "Invalid filename" << endl;
			}
			else if (input == "PWD")
			{
				cout << endl << "\t"; PrintDir(); cout << endl;
			}
			else if (input == "EDIT")
			{
				string name;
				cin >> name;
				edit(name);
			}
			else if (input == "CONVERT")
			{
				string command2;
				cin >> command;
				cin >> command2;
				Convert(command, command2);
			}
			else if (input == "RENAME")
			{
				string command2;
				cin >> command;
				cin >> command2;
				if (!Rename(command, command2))
					cout << "Invalid filename" << endl;
			}
			else if (input == "DIR")
			{
				Print();
			}
			else if (input == "VER")
			{
				PrintVer();
			}
			else if (input == "CLS")
			{
				system("cls");
				PrintOnwership();
			}
			else if (input == "TREE")
			{
				Tree_Display();
			}
			else if (input == "HELP")
			{
				Help();
			}
			else if (input == "SAVE")
			{
				Save_Tree();
			}
			else if (input == "LOAD")
			{
				Load_Tree();
			}
			else if (input == "LIST")
			{
				Editor_interface();
			}
			else if (input == "EXIT")
			{
				break;
			}
			else
				cout << "Invalid command" << endl;
			cout << endl;


		}
	}

};

