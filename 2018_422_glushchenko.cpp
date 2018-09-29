/*
	Автор: Глущенко Майя, 422 группа, Задание 1
	2018 год

*/
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <set>
#include <vector>
#include <iomanip>
#include <cstdlib>

using namespace std;

// Класс хранящий и обрабатывающий аргументы командной строки
class Argument
{
	uint argc; 
	char** argv;

public:
	bool count; // флаг вывода общего числа состояний в ios
	string outfile; // имя файла для записи состояний
	int f_a, f_b, g_a, g_b; 

public:
	// Конструктор
	Argument(int a, char** v):
		argc(a),
		argv(v),
		count(false),
		outfile("states.txt"),
		f_a(0),
		f_b(0),
		g_a(0),
		g_b(0)
	{}

	// Парсинг аргументов
	void parse()
	{
		cerr << "Parsing arguments\n";
		if (this->argc < 5) // Обязательные аргументы - параметры функций
		{
			cerr << "--------------------------------------------------------\n";
			cerr << "Not enough arguments!\n";
			cerr << "Counting attainable program states.\n";
			cerr << "2018_422_glushchenko.c by Glushchenko Maya, September 2018\n";
			cerr << "--------------------------------------------------------\n";
			cerr << "Usage: ./your_prog f_a f_b g_a g_b [-file outfile] [-count]\n";
			exit(1);
		}
		else
		{
			try{
			for(uint i = 1; i < argc; i++)
			{	
				string s(argv[i]);
				if (s == "-file") // -file имя
				{
					this->outfile = string(argv[++i]);
					continue;
				}
				else if (s == "-count") // -count
				{
					this->count = true;
					continue;
				}
				else
				{
					this->f_a = stoi(argv[i++]);
					this->f_b = stoi(argv[i++]);
					this->g_a = stoi(argv[i++]);
					this->g_b = stoi(argv[i]);
				}
			}}
			catch(...){ /* Если произошла ошибка в парсинге */
				cerr << "--------------------------------------------------------\n";
				cerr << "Arguments error!\n";
				cerr << "Counting attainable program states.\n";
				cerr << "2018_422_glushchenko.c by Glushchenko Maya, September 2018\n";
				cerr << "--------------------------------------------------------\n";
				cerr << "Usage: ./your_prog f_a f_b g_a g_b [-file outfile] [-count]\n";
				exit(1);
			}
		}
		cerr << "Parsed!\nOutfile: " << outfile << "\nCount: " << (count == 0 ? "False" : "True")  << endl;
	}
};

// Ячейка таблицы: ключ - значение
struct Cell
{
	string key; // ключ
    string value; // значение
    
    Cell(string k = "", string v = ""): key(k), value(v)
    {}
};

// Класс состояния 
class State
{
	int f_a, f_b, g_a, g_b; // входные параметры функций
	map<string, string> state_map; // таблица состояния ключ-значение


public:	
	vector<Cell> stateF, stateG; // вектор достижимых состояний функции f() и g()
	set<State> states; // множество всех состояний
	map<string, int> count; // таблица состояний ключ-количество различных значений
	// Конструкторы
	State(int fa, int fb, int ga, int gb):
		f_a(fa),
		f_b(fb),
		g_a(ga),
		g_b(gb)
	{}
	State(map<string, string> smap):
		state_map(smap)
	{}
	State(const State& st):
		state_map(st.state_map)
	{} 
	// Перегрузка оператора [] и <
	const string& operator[](const string& f) const
	{
		return this->state_map.at(f);
	}
	string& operator[](const string& f)
	{
		return this->state_map.at(f);
	}
	bool operator<(const State& other) const
	{ // требуется для корректной работы set
		vector<bool> lc;
		lc.push_back((*this)["countF"] < other["countF"]);
		lc.push_back((*this)["countG"] < other["countG"]);
		lc.push_back((*this)["h"] < other["h"]);
		lc.push_back((*this)["f.x"] < other["f.x"]);
		lc.push_back((*this)["f.y"] < other["f.y"]);
		lc.push_back((*this)["g.x"] < other["g.x"]);
		lc.push_back((*this)["g.y"] < other["g.y"]);

		for (bool c : lc)
			if (c)
				return true;
		return false;
	}

	// Инициализация таблицы состояний
	const map<string, string> init()
	{
		// Заполнение достижимых состояний для функции f()
		stateF.push_back(Cell("f.x", "9"));
		stateF.push_back(Cell("f.y", "3"));
		stateF.push_back(Cell("h", "3"));
		stateF.push_back(Cell("h", to_string(this->f_a + 3)));
		stateF.push_back(Cell());

		// Заполнение достижимых состояний для функции g()
		stateG.push_back(Cell("g.x", "2"));
		stateG.push_back(Cell("g.y", "7"));
		stateG.push_back(Cell("h", "5"));
		stateG.push_back(Cell("g.x", "4"));
		stateG.push_back(Cell("h", to_string(g_b)));
		stateG.push_back(Cell("g.y", "0"));
		stateG.push_back(Cell("g.x", "3"));
		stateG.push_back(Cell());

		// Начальные состояния
		const map<string, string> init_states = {
			{"countF", "0"},
			{"countG", "0"},
			{"h",   "#"},
			{"f.x", "#"},
			{"f.y", "#"},
			{"g.x", "#"},
			{"g.y", "#"}
		};
		// Начальные счеткчики различных состояний
		map<string, int> init_counts = {
			{"h",   1},
			{"f.x", 1},
			{"f.y", 1},
			{"g.x", 1},
			{"g.y", 1}
		};
		this->count = init_counts;

		return init_states;
	}
	bool find(Cell c)
	{
		set<State>::iterator it;
		for(it = this->states.begin(); it != this->states.end(); ++it)
			if ( (*it)[c.key] == c.value ) 
				return true;
		return false;
	}
	int calculate(map<string, string> init_states)
	{
		// Инициализируем начальное состояние
		State state(init_states);
		// Рекурсивный перебор состояний
		exec_next(state, 0, 0, "#");
		// Удаляем состояние g.x = 3 && g.y = 0
		Cell cx("g.x", "3");
		Cell cy("g.y", "0");
		set<State>::iterator it;
		for(it = this->states.begin(); it != this->states.end();)
			if ((*it)[cx.key] == cx.value && (*it)[cy.key] == cy.value)
				it = this->states.erase(it);
			else
				it++;
		return this->states.size();
	}

	/* Рекурсивный перебор всех возможных состояний */
	void exec_next(State state, uint countF, uint countG, string prev_h)
	{
		// Добавление состояния в набор
		state["countF"] = to_string(countF);
		state["countG"] = to_string(countG);
		this->states.insert(state);
		// Ячейка таблицы состояний
		Cell cell;
		State tmp(state);
		string h_cur = state["h"];
		/* Выполнение оператора функции f() */
		if (countF < this->stateF.size() - 1) {
			cell = this->stateF[countF];
			state[cell.key] = cell.value;
			if ( !this->find(cell) ) // если ячейка впервые - увеличиваем счетчик
				count[cell.key] += 1;
			exec_next(state, countF + 1, countG, h_cur);
		}
		
		/* Выполнение оператора функции g() */
		if (countG < this->stateG.size() - 1) {
			cell = this->stateG[countG];
			tmp[cell.key] = cell.value;
			if ( !this->find(cell) ) // если ячейка впервые - увеличиваем счетчик
				count[cell.key] += 1;
			exec_next(tmp, countF, countG + 1, h_cur);
		}    
	}

	// Запись состояний в файл
	void write_file(string file)
	{
		ofstream out(file);
		out << " countF | countG |  h | f.x | f.y | g.x | g.y" << endl;
		set<State>::iterator it;
		for(it = this->states.begin(); it != this->states.end(); ++it)
		{
			out << setw(5) << (*it)["countF"] << "   | ";
			out << setw(4) << (*it)["countG"] << "   | ";
			out << setw(2) << (*it)["h"] 	  << setw(2) << "|";
			out << setw(4) << (*it)["f.x"] 	  << setw(2) << "|";
			out << setw(4) << (*it)["f.y"] 	  << setw(2) << "|";
			out << setw(4) << (*it)["g.x"] 	  << setw(2) << "|";
			out << setw(4) << (*it)["g.y"] 	  << endl;
		}
	}

	// Вывод числа всех достижимых состояний
	void counting()
	{
		cout << "All states (count flag set): " << endl;
		cout << "f(): " << stateF.size() << endl;
		cout << "g(): " << stateG.size() << endl;
		cout << "h:   " << count.at("h") << endl;
		cout << "f.x: " << count.at("f.x") << endl;
		cout << "f.y: " << count.at("f.y") << endl;
		cout << "g.x: " << count.at("g.x") << endl;
		cout << "g.y: " << count.at("g.y") << endl;
		cout << "int: " << "2 ^ 128" << endl;
		cout << "Result = " << stateF.size() * stateG.size() * count.at("h") * count.at("f.x") *
							count.at("f.y") * count.at("g.x") * count.at("g.y") << " * 2 ^ 128" << endl;
	}
};


int main(int argc, char** argv)
{
	// Создаем объект аргументов
	Argument arg(argc, argv);
	// Парсим аргументы
	arg.parse();
	// Создаем объект состояний
	State states(arg.f_a, arg.f_b, arg.g_a, arg.g_b);
	// Инициализируем начальные состояния, вычисляем количество достижимых
	int attainable = states.calculate(states.init());
	// Записываем в файл таблицу состояний
	states.write_file(arg.outfile);

	cout << "----------------------------" << endl;
	cout << "Attainable states: " << attainable << endl;
	// Если установлен флаг -count - выводим всевозможные достижимые состояния
	if (arg.count)
		states.counting();

	return 0;
}