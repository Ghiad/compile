#include "Lexer.h"

Lexer::Lexer() {
	result = true;
	f_p = 0;
	t_count = 0;
	loc.col = 0;
	loc.row = 1;
	pre_col = -1;
	type = Token_type::tok_def;
	empty = true; //还未获得字元
	initKeys();
}

int Lexer::get_row() {
	return loc.row;
}

int Lexer::get_col() {
	return loc.col;
}

int Lexer::get_c_col() {
	return c_col;
}

std::string Lexer::get_body() {
	return body;
}

Token_type Lexer::get_type() {
	return type;
}

bool Lexer::get_empty() {
	return empty;
}

void Lexer::close() {
	f_in.close();
}

void Lexer::file_initialize(std::string add) {
	memset(buffer, 0, TOTAL_SIZE);
	f_in.open(add.data());
	if (!f_in) {
		std::cout << "Failed to open input file." << std::endl;
		exit(1);
	}
	loadBuffer();
}


void Lexer::loadBuffer() {
	if (!loaded) { //未载入过
		f_in.read(&buffer[f_p], BUF_SIZE - 1);
		std::streamsize count = f_in.gcount();//可加载字符数
		if (count < BUF_SIZE - 1) {
			buffer[f_p + count] = EOF;
		}
		else if (count == BUF_SIZE - 1) {
			buffer[(f_p + BUF_SIZE - 1) % TOTAL_SIZE] = EOF;
		}
	}
}

void Lexer::initKeys() {
	keys["and"] = Token_type::tok_mulop;
	keys["array"] = Token_type::tok_array;
	keys["begin"] = Token_type::tok_begin;
	keys["boolean"] = Token_type::tok_boolean;
	keys["char"] = Token_type::tok_char;
	keys["case"] = Token_type::tok_case;
	keys["const"] = Token_type::tok_const;//识别出该符号，后续生成constAST，这之后识别的id是定义性
	keys["div"] = Token_type::tok_mulop;
	keys["do"] = Token_type::tok_keys;
	keys["downto"] = Token_type::tok_keys;
	keys["else"] = Token_type::tok_keys;

	keys["end"] = Token_type::tok_end;
	keys["for"] = Token_type::tok_for;
	keys["function"] = Token_type::tok_function;
	keys["if"] = Token_type::tok_if;
	keys["integer"] = Token_type::tok_integer;
	keys["mod"] = Token_type::tok_mulop;
	keys["not"] = Token_type::tok_not;
	keys["of"] = Token_type::tok_keys;
	keys["or"] = Token_type::tok_addop;
	keys["procedure"] = Token_type::tok_procedure;

	keys["program"] = Token_type::tok_program;//需要创建block
	keys["real"] = Token_type::tok_real;
	keys["record"] = Token_type::tok_record;
	keys["repeat"] = Token_type::tok_repeat;
	keys["then"] = Token_type::tok_keys;
	keys["to"] = Token_type::tok_keys;
	keys["type"] = Token_type::tok_type;
	keys["until"] = Token_type::tok_keys;
	keys["var"] = Token_type::tok_var;
	keys["while"] = Token_type::tok_while;

	keys["true"] = Token_type::tok_boolc;
	keys["false"] = Token_type::tok_boolc;
}


Token_type Lexer::iskey(std::string id) {
	auto iter = keys.find(id);
	if (iter == keys.end()) { //查找到keys末尾，即非关键字
		return Token_type::tok_identifier; //非关键字即标识符
	}
	return iter->second;      //返回对应关键字的Token_type
}

bool Lexer::letter(char s) {
	if ((s <= 'Z'&&s >= 'A') || (s >= 'a'&&s <= 'z'))
		return true;
	return false;
}

bool Lexer::digit(char s) {
	return s >= '0'&&s <= '9' ? true : false;
}

void Lexer::get_char(int type) {//type 为0则区分大小写，为1则不区分
	last_C = C;           //保存上个字符
	if (last_C == '\n') { //换行
		loc.row++;
		pre_col = c_col;
		c_col = 0;
	}
	else {
		c_col++;
	}
	C = buffer[f_p];
	if (type && (C <= 'Z'&&C >= 'A')) {
		C += 'a' - 'A';
	}
	f_p = (f_p + 1) % (TOTAL_SIZE);

	if (buffer[f_p] == EOF) { //向前指针指向EOF
		if ((f_p%BUF_SIZE) == BUF_SIZE - 1) { //向前指针指向缓冲半区的末尾
			f_p = (f_p + 1) % TOTAL_SIZE;
			loadBuffer();

			if (loaded) {
				loaded = false;
			}
		}
	}

}

void Lexer::getnextLine() {
	get_char(1);
	while (C != '\n')
		get_char(1);
}

void Lexer::get_nbc(int type) {
	while (C == ' ' || C == '\n' || C == '\t')
		get_char(type);
}

void Lexer::retract() {
	f_p = (f_p + TOTAL_SIZE - 1) % TOTAL_SIZE;
	if (buffer[f_p] == '\n') {
		loc.row--;
		c_col = pre_col;
	}
	else
		c_col--;
	if (buffer[f_p] == EOF && (f_p == BUF_SIZE - 1 || f_p == TOTAL_SIZE - 1)) {
		loaded = true; //避免覆盖掉刚刚加载的字符串
		retract();
	}
}

void Lexer::getTOKEN(int target) {
	if (target >= t_count) {
		not_num = 0;
		std::string s;
		bool undone = false;    //处理了一次注释则设置为真
		do {
			undone = false;
			get_char(1);
			get_nbc(1);
			loc.col = c_col;
			switch (C) {
			case '\0':
				//	cout << "err:char = '0'" << endl;
				break;
			case EOF:
				type = Token_type::tok_eof;
				body.clear();
				break;
			case 'a':case 'b':case 'c':case 'd':case 'e':case 'f':case 'g':case 'h':case 'i':
			case 'j':case 'k':case 'l':case 'm':case 'n':case 'o':case 'p':case 'q':case 'r':
			case 's':case 't':case 'u':case 'v':case 'w':case 'x':case 'y':case 'z':case 'A':
			case 'B':case 'C':case 'D':case 'E':case 'F':case 'G':case 'H':case 'I':case 'J':
			case 'K':case 'L':case 'M':case 'N':case 'O':case 'P':case 'Q':case 'R':case 'S':
			case 'T':case 'U':case 'V':case 'W':case 'X':case 'Y':case 'Z'://标识符
				s += C;
				get_char(1);
				while (letter(C) || digit(C)) {
					s += C;
					get_char(1);
				}
				retract();
				if (s.length() > idmax&&s.compare("procedure")) {
					std::cout << "error line" << std::setw(3) << loc.row << ":过长标识符： " << s << std::endl;
					body = s;
					type = iskey(s);
					result = false; //词法分析失败，遇到错误
				}
				else {
					body = s;
					type = iskey(s);
				}; break;
			case '1':case '2':case '3':case '4':case '5':case '6':case '7':case '8':case '9':
			case '0':
				s += C;
				get_char(1);
				while (digit(C)) {
					s += C;
					get_char(1);
				}
				type = tok_integer;
				if (C == '.') { //存在小数部分
					s += C;
					get_char(1);
					if (C == '.') { //对应 .. 符号，非数字组成部分
						retract();
						s.substr(0, s.length() - 1);
					}
					while (digit(C)) {
						s += C;
						type = tok_real;
						get_char(1);
					}
				}
				if (C == 'e') { //存在指数部分 待定为e
					s += C;
					get_char(1);
					if (C == '+' || C == '-') {
						s += C;
						get_char(1);
					}
					s += C;
					get_char(1);
					while (digit(C)) {
						s += C;
						get_char(1);
					}
				}
				if (letter(C)) {
					//	printf("error: Line: %d order:%d 标识符不能以数字开头\n", line_count + 1, w_count);
					//	fprintf(error_File, "error: Line: %d order:%d 标识符不能以数字开头\n", line_count + 1, w_count);
					while (letter(C) || digit(C)) {
						get_char(1);
					}
					not_num = 1;
				}
				retract();
				if (not_num == 0) {
					//	printf("<num %s>\n", token);
					body = s;
					//		num_count++;
				}
				//	w_count++;
				break;
			case '>':                //各类操作符
				get_char(1);
				if (C == '=') {
					body = ">=";
				}
				else {
					body = ">";
					retract();
				}
				type = Token_type::tok_relop;
				break;
			case '*':
				body = C;
				type = Token_type::tok_mulop;
				break;
			case '/':
				get_char(1);
				if (C != '/') {
					retract();
					body = '/';
					type = Token_type::tok_mulop;
					break;
				}
				else {
					getnextLine();
					undone = true;
					break;
				}
			case '+': case '-':
				body = C;
				type = Token_type::tok_addop;
				break;
			case '=':
				body = C;
				type = Token_type::tok_relop;
				break;
			case '<':
				get_char(1);
				if (C == '=') {
					body = "<=";
				}
				else if (C == '>') {
					body = "<>";
				}
				else {
					body = "<";
					retract();
				}
				type = Token_type::tok_relop;
				break;
			case ':':
				get_char(1);
				if (C == '=') {
					body = ":=";
					type = Token_type::tok_assignop;
				}
				else {
					body = ":";
					retract();
					type = Token_type::tok_delimiter;
				}
				break;
			case '{': //注释开始
				get_char(0);
				while (C != '}') //注释未结束 
					get_char(0);
				undone = true;
				break;
			case '[':case ']':case ',':
			case ';':case ')':
				body = C;
				type = Token_type::tok_delimiter;
				break;
			case '.':
				get_char(1);
				if (C == '.') {
					body = "..";
				}
				else {
					body = ".";
					retract();
				}
				type = tok_delimiter;
				break;
			case '(':
				get_char(1);
				if (C != '*') { //非注释
					retract();
					body = "(";
					type = Token_type::tok_delimiter;
				}
				else { //注释开始符号
					get_char(0);
					bool ending = false;
					while (!ending) {
						while (C != '*') {
							get_char(0);
						}
						get_char(0);
						if (C == ')')
							ending = true; //注释结束
					}
					undone = true;
				}
				break;
			case '\'':
				get_char(0);
				body = C;
				get_char(1);
				if (C == '\'') //对应'letter'
					type = tok_letter;
				else {
					//错误情况
					retract();
					retract();
					//errcode  不处理的效果为读取一个非法的 ' 期待后续可正常读取
				}
				break;
			default: //errcode
				std::cout << "error line" << std::setw(3) << loc.row << ":非法字符 " << C << std::endl;
				result = false;
				undone = true;
				break;
			}
		} while (undone);
		sToken tmp;
		tmp.body = body;
		tmp.type = type;
		tmp.loc = loc;
		t_flow.push_back(tmp);
		t_count++;
		empty = false;
	}
	else {
		body = t_flow[target].body;
		type = t_flow[target].type;
		loc = t_flow[target].loc;
	}
}


