#pragma once
#include<iostream>
#include<queue>
#include<list>
#include<stack>
#include<conio.h>
#include<fstream>
#include<windows.h>
using namespace std;

static int console_width = 140;

static void gotoRowCol(int rpos, int cpos)
{
	COORD scrn;
	HANDLE hOuput = GetStdHandle(STD_OUTPUT_HANDLE);
	scrn.X = cpos;
	scrn.Y = rpos;
	SetConsoleCursorPosition(hOuput, scrn);
}

struct state
{
	list<list<char>> text;
	int row, col;
};

class TextEditor
{
	int CurrCol, CurrRow;
	list<list<char>> text;
	list<list<char>>::iterator ri;
	list<char>::iterator ci;
	deque<state> Undo;
	stack<state> Redo;

public:

	TextEditor()
	{
		CurrCol = CurrRow = 0;
		text.push_back(list<char>());
		ri = text.begin();
	}

	void Run()
	{
		char c;
		//c = _getch();
		//(*ri).push_back(c);
		///**ci = c;*/
		//cout << c;
		//ci = (*ri).begin();
		//CurrCol++;

		while (true)
		{
			gotoRowCol(CurrRow, CurrCol);
			c = _getch();

			if ((*ri).size() == 0 && text.size() == 1)
			{
				while (c == -32 || c == 8)
				{
					c = _getch();
					c = _getch();
				}
				InsertFront(c);
				continue;

			}
			else if (CurrCol == 0)
			{
				if (c == -32)
				{
					CurrCol++;
					goto A;
				}
				else if (c == 8 && CurrRow == 0)
					continue;
				else if (c == 8)
					goto A;
				else if (c == 26)
				{
					Undo_Editor();
					continue;
				}
				else if (c == 25)
				{
					Redo_Editor();
					continue;
				}
				InsertFront(c);
				continue;
			}

		A:
			if (c == -32) // arrow keys and delete key
			{
				c = _getch();
				if (c == 75) // left
				{
					if (CurrCol - 1 > 0)
					{
						CurrCol--;
						ci--;
					}
					else if (CurrCol - 1 == 0)
					{
						CurrCol--;
					}
				}
				else if (c == 77) // right
				{
					auto temp = ci;
					temp++;
					if (CurrCol <= console_width && temp != (*ri).end())
					{
						CurrCol++;
						ci++;
					}
				}
				else if (c == 72)//up
				{
					if (CurrRow != 0)
					{
						ri--;
						(ci) = (*ri).begin();
						if (CurrCol < (*ri).size())
						{
							for (int i = 0; i < CurrCol - 1; i++)
							{
								ci++;
							}
						}
						else
							CurrCol = 0;

						CurrRow--;
						gotoRowCol(CurrRow, CurrCol);
					}
				}
				else if (c == 80)			//down
				{
					auto temp = ri;
					temp++;
					if (temp != text.end())
					{
						ri++;
						ci = (*ri).begin();

						if (CurrCol < (*ri).size())
						{
							for (int i = 0; i < CurrCol - 1; i++)
							{
								ci++;
							}
						}
						else
							CurrCol = 0;
						CurrRow++;
					}
				}
				else if (c == 83)		//Delete
				{
					UpdateUndo();
					CurrCol--;
					Delete();
				}
			}
			else if (c == 13)			//Enter
			{
				UpdateUndo();
				Enter();
			}
			else if (c == 8)//backspace
			{
				UpdateUndo();
				backspace();
			}
			else if (c == 26)//undo
			{
				Undo_Editor();
			}
			else if (c == 25)		//redo
			{
				Redo_Editor();
			}
			else if (c == 27) //escape
			{
				cout << endl;
				break;
			}
			else
			{
				if (CurrCol > console_width)
				{
					Enter();
				}
				else
					Insert(c);
			}
		}

	}

	void Insert(char c)
	{
		auto temp = ci;					//text anywhere
		ci++;
		(*ri).insert(ci, c);
		ci = ++temp;

		CurrCol++;
		PrintLine();
	}

	void InsertFront(char c)
	{
		(*ri).push_front(c);					//text at front
		ci = (*ri).begin();
		CurrCol = 1;
		PrintLine();
	}

	void backspace()
	{
		if (CurrCol == 1)				//for col 1 position
		{
			ci++;
			(*ri).pop_front();
			CurrCol--;
			PrintLine();


		}
		else if (CurrCol != 0)
		{														//random position
			auto temp = ci;
			temp--;
			(*ri).erase(ci);
			ci = temp;
			CurrCol--;
			PrintLine();

		}
		else if (CurrCol == 0 && CurrRow != 0)
		{
			auto temp = ri;												//start position
			ri--;
			(*ri).splice((*ri).end(), (*temp));
			text.erase(temp);
			system("cls");
			Print();

			CurrCol = 0;
			CurrRow--;
			ci = (*ri).begin();
		}
	}

	void Delete()
	{
		if ((*ri).size() == 0)
			return;
		auto temp = ci;
		ci++;

		if (CurrCol == 0)
		{
			ci--;
			temp++;					//cursor at front
			(*ri).pop_front();
			PrintLine();
		}
		else if (ci != (*ri).end())
		{
			CurrCol++;					//cursor at random position
			(*ri).erase(ci);
			PrintLine();
		}
		else
		{
			CurrCol++;					//cursor at end
		}
		ci = temp;
	}

	void Enter()
	{
		auto start = ci, end = ci;						//new line
		start++;

		auto temp = ri;
		auto pre_row = ri;
		auto c = ci; c++;

		ri++;
		text.insert(ri, list<char>());
		ri = ++temp;

		for (; c != (*pre_row).end(); c++)
		{
			(*ri).push_back(*c);
			end++;
		}

		(*pre_row).erase(start, ++end);				//erase start and end range

		CurrRow++;
		CurrCol = 0;
		gotoRowCol(CurrRow, CurrCol);
		ci = (*ri).begin();


		system("cls");
		Print();

	}

	void ClearLine()
	{

		gotoRowCol(CurrRow, CurrCol);
		for (int i = CurrCol; i <= (*ri).size(); i++)
		{
			cout << " ";
		}

		if (CurrCol != 0)
			gotoRowCol(CurrRow, CurrCol - 1);
		else
			gotoRowCol(CurrRow, CurrCol);
	}

	void PrintLine()
	{
		ClearLine();
		auto c = (*ri).begin();

		for (int i = 0; i < CurrCol - 1; i++)
		{
			c++;
		}
		for (; c != (*ri).end(); c++)
		{
			cout << *c;
		}
		gotoRowCol(CurrRow, CurrCol);
	}

	void Print()
	{
		for (auto r = text.begin(); r != text.end(); r++)				//Print entire editor
		{
			for (auto c = (*r).begin(); c != (*r).end(); c++)
			{
				cout << *c;
			}
			cout << endl;
		}
	}

	state SaveState()
	{
		state* s = new state;
		s->text.push_back(list<char>());
		auto r_itr = s->text.begin();
		for (auto row = text.begin(); row != text.end(); row++)
		{
			if (row != text.begin())
			{

				s->text.push_back(list<char>());
				r_itr++;
			}
			for (auto col = (*row).begin(); col != (*row).end(); col++)
			{
				char ch = *col;
				(*r_itr).push_back(ch);
			}
		}
		s->col = CurrCol;
		s->row = CurrRow;
		return *s;
	}

	void LoadState(state s)
	{
		text = s.text;
		ri = text.begin();
		CurrCol = s.col;
		CurrRow = s.row;

		for (int r = 0; r < CurrRow; r++)
		{
			ri++;
		}

		ci = (*ri).begin();
		for (int c = 0; c < CurrCol - 1; c++)
		{
			ci++;
		}
	}

	void UpdateUndo()
	{
		int limit = 6;
		if (Undo.size() >= limit)
		{
			Undo.pop_front();
		}
		state s = SaveState();
		Undo.push_back(s);
	}

	void Undo_Editor()
	{
		if (!Undo.empty())
		{
			Redo.push(SaveState());
			LoadState(Undo.back());


			Undo.pop_back();
			system("cls");
			Print();
		}
	}

	void Redo_Editor()
	{
		if (!Redo.empty())
		{
			Undo.push_back(Redo.top());
			LoadState(Redo.top());
			Redo.pop();
			system("cls");
			Print();
		}
	}

	void Open_File(string name)
	{
		string path = "E:/C++/BSCS20035_DOS/BSCS20035_DOS/files/" + name;
		ifstream fin(path);
		char c;

		while (fin.good())
		{

			fin.get(c);
			if (fin.eof())
				break;
			if (c == '\n')
			{
				text.push_back(list<char>());
				ri++;
			}
			else
			{
				(*ri).push_back(c);
			}
		}

		ri = text.begin();
		ci = (*ri).begin();
		CurrCol = 0;
		CurrRow = 0;
		//gotoRowCol(CurrRow, CurrCol);
	}

	void Save_File(string name)
	{
		string path = "E:/C++/BSCS20035_DOS/BSCS20035_DOS/files/" + name;
		ofstream fout(path);

		for (auto r = text.begin(); r != text.end(); r++)
		{
			for (auto c = (*r).begin(); c != (*r).end(); c++)
			{
				fout << *c;
			}
			fout << endl;
		}
	}

	void Save_attributes(int& noc, int& lines)
	{
		lines = text.size();
		for (auto r = text.begin(); r != text.end(); r++)
		{
			noc = noc + (*r).size();
		}
	}
};