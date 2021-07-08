#include "Parser.h"

bool Parser::FACTOR() {
	int count_now = cur_count;//记录当前非终结符接收开始位置
	Token_type left = factor;
	Token_type var = variable;
	Token_type newType;
	std::string tmp_name = "";
	ExprAST* v;
	UnaryExprAST* tmp_v;
	std::vector<ExprAST*> args;

	UnaryOpKind op = UnaryOpKind::negKind;
	int v_int;
	double v_d;
	bool v_b;
	char** endPtr = NULL;
	if (lexer.get_type() == tok_identifier) { //factor->variable检查
		if (VARIABLE()) { //VARIABLE()将AST入栈
			return true;
		}
	}
	cur_count = count_now; //factor->variable未接收，复位
	lexer.getTOKEN(cur_count - 1);//重新获取对应的TOKEN

	if (lexer.get_type() == tok_identifier && n_stack.top() == NULL)
		n_stack.pop();

	switch (lexer.get_type()) {
	case Token_type::tok_boolc:
		if (lexer.get_body()[0] == 't')
			v_b = true;
		else
			v_b = false;
		v = new BoolExprAST(v_b, lexer.get_row());
		v->expr_type = ExprType::BoolExpr;
		n_stack.push(v);
		return true;
		break;
	case Token_type::tok_identifier:
		tmp_name = lexer.get_body();
		lexer.getTOKEN(cur_count++);
		newType = lexer.get_type();

		if (newType == tok_delimiter && lexer.get_body()[0] == '(') {

			lexer.getTOKEN(cur_count++);
			if (lexer.get_body()[0] == ')') { //对应无参数函数调用
				v = new CallFunctionExprAST(tmp_name, args, false, lexer.get_row());
				v->expr_type = ExprType::CallFuncExpr;
				n_stack.push(v);
				return true;
			}
			else if (EXPRESSION_LIST(&args)) {//对应 factor ->id(exp)
				lexer.getTOKEN(cur_count++);
				if (lexer.get_body()[0] == ')') {
					v = new CallFunctionExprAST(tmp_name, args, false, lexer.get_row());
					v->expr_type = ExprType::CallFuncExpr;
					n_stack.push(v);
					return true;
				}
			}
		}
		else {
			cur_count--;
			v = new VariableExprAST(tmp_name, lexer.get_row());
			v->expr_type = ExprType::Variable;
			n_stack.push(v);
			return true;
		};//接收factor->id
		break;
	case Token_type::tok_integer:
		v_int = stoi(lexer.get_body());
		v = new NumberExprAST(v_int, lexer.get_row());
		v->expr_type = ExprType::NumberExpr;
		n_stack.push(v);
		return true;
	case Token_type::tok_real:
		v_int = lexer.get_body().length();
		v_d = strtod(lexer.get_body().data(), endPtr);
		v = new RealExprAST(v_d, lexer.get_row());
		v->expr_type = ExprType::RealExpr;
		n_stack.push(v);
		return true;
	case Token_type::tok_not:
		lexer.getTOKEN(cur_count++);
		if (FACTOR()) {
			ExprAST * tmp = (ExprAST*)n_stack.top();
			n_stack.pop();
			v = new UnaryExprAST(UnaryOpKind::notKind, tmp, lexer.get_row());
			tmp_v = (UnaryExprAST*)v;
			//	tmp_v->type = tmp->expr_type;
			//  其他属性赋值

			v->expr_type = ExprType::UnaryExpr;
			n_stack.push(v);
			return true;
		}
		break;
	case Token_type::tok_letter:
		v = new CharExprAST(lexer.get_body()[0], lexer.get_row());
		v->expr_type = ExprType::CharExpr;
		n_stack.push(v);
		return true;
	case Token_type::tok_delimiter:
		if (lexer.get_body()[0] == '(') {
			lexer.getTOKEN(cur_count++);
			if (EXPRESSION()) {
				lexer.getTOKEN(cur_count++);
				if (lexer.get_body()[0] == ')')
					return true;
			}
		}
		break;
	case Token_type::tok_addop:
		if (lexer.get_body()[0] == '-') {
			lexer.getTOKEN(cur_count++);
			if (FACTOR()) {
				op = UnaryOpKind::negKind;
				v = (ExprAST*)n_stack.top();
				n_stack.pop();
				tmp_v = new UnaryExprAST(op, v, lexer.get_row());
				tmp_v->expr_type = ExprType::UnaryExpr;
				n_stack.push(tmp_v);
				return true;
			}
		}
		else if (lexer.get_body()[0] == '+') {
			lexer.getTOKEN(cur_count++);
			return FACTOR();
		}
		break;
	default:
		break;
	}
	std::cout << "error line" << std::setw(3) << lexer.get_row() << ":FACTOR accept failed. ";
	std::cout << "body: " << lexer.get_body() << std::endl;
	return false;
}

bool Parser::TERM1() {
	if (lexer.get_type() == tok_mulop) {
		BinaryOpKind op;
		switch (lexer.get_body()[0]) {
		case '*':op = BinaryOpKind::mulKind;     break;
		case '/':op = BinaryOpKind::divRealKind; break;
		case 'a':op = BinaryOpKind::andKind;     break;
		case 'd':op = BinaryOpKind::divKind;     break;
		case 'm':op = BinaryOpKind::modKind;     break;
		default:
			break;
		}
		lexer.getTOKEN(cur_count++);
		ExprAST*L = (ExprAST*)n_stack.top(); //获取之前存入的L端Expr
		n_stack.pop();
		if (FACTOR()) {
			ExprAST *R = (ExprAST*)n_stack.top();
			n_stack.pop();
			BinaryExprAST* AST_TERM1 = new BinaryExprAST(op, L, R, lexer.get_row());
			AST_TERM1->expr_type = ExprType::BinaryExpr;
			n_stack.push(AST_TERM1);
			lexer.getTOKEN(cur_count++);
			if (TERM1()) {
				return true;
			}
		}
	}
	else {
		Token_type tmp = lexer.get_type();
		std::string tmps = lexer.get_body();
		if (tmps[0] == ')' || tmps[0] == '\'' || tmps[0] == ';' || tmps[0] == ',' ||
			tmps[0] == ']' || tmp == tok_addop || !tmps.compare("do") ||
			!tmps.compare("else") || tmp == tok_end ||
			!tmps.compare("of") || tmp == tok_relop ||
			!tmps.compare("then") || !tmps.compare("to") ||
			!tmps.compare("until")) { //对应 term1->空
			cur_count--;
			return true;
		}
	}
	return false;
}

bool Parser::TERM() {
	if (FACTOR()) {
		lexer.getTOKEN(cur_count++);
		if (TERM1()) {
			/*BinaryExprAST* tmp = (BinaryExprAST*)n_stack.top();
			n_stack.pop();
			if (tmp != NULL) { //不是TERM->FACTOR
				ExprAST * left = (ExprAST*)n_stack.top();
				n_stack.pop();
				tmp->LExpr = left;
				n_stack.push(tmp);
				return true;
			} */
			//于下层函数构造AST并存入stack
			return true;
		}
	}
	return false;
}

bool Parser::SEXP1() {
	Token_type tmp = lexer.get_type();
	std::string tmps = lexer.get_body();
	BinaryOpKind op;
	if (lexer.get_type() == tok_addop) {
		switch (lexer.get_body()[0]) {
		case '+':op = BinaryOpKind::plusKind;  break;
		case '-':op = BinaryOpKind::minusKind; break;
		case 'o':op = BinaryOpKind::orKind;    break;
		default:
			break;
		}
		lexer.getTOKEN(cur_count++);
		if (TERM()) {
			ExprAST* R1 = (ExprAST*)n_stack.top();
			n_stack.pop();
			ExprAST* L1 = (ExprAST*)n_stack.top();
			n_stack.pop();
			BinaryExprAST* tmp1 = new BinaryExprAST(op, L1, R1, lexer.get_row());
			n_stack.push(tmp1);
			tmp1->expr_type = ExprType::BinaryExpr;
			lexer.getTOKEN(cur_count++);
			if (SEXP1()) {
				/*	BinaryExprAST* tmp = (BinaryExprAST*)n_stack.top();
					n_stack.pop();
					if (tmp != NULL) {
						tmp->LExpr = R1;
						R1 = tmp;
					}
					BinaryExprAST* SEXP1AST = new BinaryExprAST(op, NULL, R1, lexer.get_row());
					SEXP1AST->expr_type = ExprType::BinaryExpr;
					n_stack.push(SEXP1AST); */
				return true;
			}
		}
		return false;
	}

	else if (tmps[0] == ')' || tmps[0] == '\'' || tmps[0] == ',' ||
		tmps[0] == ';' || tmps[0] == ']' || !tmps.compare("do")
		|| !tmps.compare("else") || !tmps.compare("end")
		|| !tmps.compare("of") || !tmps.compare("then")
		|| !tmps.compare("to") || !tmps.compare("until")
		|| tmp == tok_relop) { //对应 sexp'->空
		cur_count--;
		//n_stack.push(NULL);
		return true;
	}
	else return false;
}

bool Parser::SIMPLE_EXPRESSION() {
	if (lexer.get_body()[0] == '-') {
		UnaryOpKind op = UnaryOpKind::negKind;
		lexer.getTOKEN(cur_count++);
		if (TERM()) {
			ExprAST * term = (ExprAST*)n_stack.top();
			UnaryExprAST *n_term = new UnaryExprAST(op, term, term->lineNum);
			n_term->expr_type = ExprType::UnaryExpr;
			n_stack.push(n_term);
			lexer.getTOKEN(cur_count++);
			if (SEXP1()) {
				/*BinaryExprAST* ast2 = (BinaryExprAST*)n_stack.top();
				n_stack.pop();
				if (ast2 != NULL) { //不等则说明不是SEXP1->空，否则说明是
					ExprAST * ast1 = (ExprAST*)n_stack.top();
					n_stack.pop();
					ast2->LExpr = ast1;
					n_stack.push(ast2);
				} */
				return true;
			}
		}
	}
	if (lexer.get_body()[0] == '+')
		lexer.getTOKEN(cur_count++);//对应 SEXP->+TERM
	if (TERM()) {
		lexer.getTOKEN(cur_count++);
		if (SEXP1()) {
			/*BinaryExprAST* ast2 = (BinaryExprAST*)n_stack.top();
			n_stack.pop();
			if (ast2 != NULL) { //不等则说明不是SEXP1->空，否则说明是
				ExprAST * ast1 = (ExprAST*)n_stack.top();
				n_stack.pop();
				ast2->LExpr = ast1;
				n_stack.push(ast2);
			}*/
			return true;
		}
	}
	return false;
}

Parser::Parser(std::string input) {
	lexer.file_initialize(input);
	cur_count = 1;
	start();
	int x = 0;
	c_value = new constValue(x);
}

bool Parser::EXPRESSION() {
	if (SIMPLE_EXPRESSION()) {
		lexer.getTOKEN(cur_count++);
		if (lexer.get_type() == tok_relop) {
			BinaryOpKind op;
			switch (lexer.get_body()[0]) {
			case '=':op = BinaryOpKind::eqKind; break;
			case '>':
				if (lexer.get_body().length() >= 2)
					op = BinaryOpKind::geKind;
				else
					op = BinaryOpKind::gtKind;
				break;
			case '<':
				if (lexer.get_body().length() >= 2) {
					switch (lexer.get_body()[1]) {
					case '>':op = BinaryOpKind::ueqKind; break;
					case '=':op = BinaryOpKind::leKind;  break;
					default:
						break;
					}
				}
				else
					op = BinaryOpKind::ltKind;
			default:
				break;
			}
			lexer.getTOKEN(cur_count++);
			if (SIMPLE_EXPRESSION()) {
				ExprAST* R = (ExprAST*)n_stack.top();
				n_stack.pop();
				ExprAST* L = (ExprAST*)n_stack.top();
				n_stack.pop();
				BinaryExprAST* AST = new BinaryExprAST(op, L, R, lexer.get_row());
				AST->expr_type = ExprType::BinaryExpr;
				n_stack.push(AST);
				return true;
			}
			n_stack.push(NULL);
			return false;
		}
		else {
			//对应exp->simple_exp
			cur_count--;
			return true;
		}
	}

	n_stack.push(NULL);
	return false;
}

bool Parser::EXP1(std::vector<ExprAST*>* e_v) {
	if (lexer.get_body()[0] == ')' ||
		lexer.get_body()[0] == ']') { //对应exp1->空
		cur_count--;
		//	n_stack.push(NULL);
		return true;
	}
	if (lexer.get_body()[0] == ',') {
		lexer.getTOKEN(cur_count++);
		if (EXPRESSION()) {
			ExprAST * AST = (ExprAST*)n_stack.top();
			n_stack.pop();
			e_v->push_back(AST);
			lexer.getTOKEN(cur_count++);
			return EXP1(e_v);
		}
	}
	std::cout << "error line" << std::setw(3) << lexer.get_row() << ":EXP1 错误" << std::endl;
	return false;
}

bool Parser::EXPRESSION_LIST(std::vector<ExprAST*>* p) {
	if (EXPRESSION()) {
		ExprAST * AST = (ExprAST*)n_stack.top();
		n_stack.pop();
		p->push_back(AST);
		lexer.getTOKEN(cur_count++);
		return EXP1(p);
	}
	return false;
}

bool Parser::PROCEDURE_CALL(bool* flag) {
	bool result = true;
	*flag = true;
	int curNow = cur_count - 1;
	if (lexer.get_type() == tok_identifier) {
		std::string callee = lexer.get_body();
		lexer.getTOKEN(cur_count++);
		if (lexer.get_body().compare("(") == 0) {
			lexer.getTOKEN(cur_count++);
			std::vector<ExprAST*> index;
			if (lexer.get_body()[0] == ')') {
				CallProcedureExprAST* callP = new CallProcedureExprAST(callee, index, lexer.get_row());
				callP->expr_type = ExprType::CallProcExpr;
				n_stack.push(callP);
				return true;
			}
			else if (EXPRESSION_LIST(&index)) {
				lexer.getTOKEN(cur_count++);
				if (lexer.get_body().compare(")") == 0) {
					CallProcedureExprAST* callP = new CallProcedureExprAST(callee, index, lexer.get_row());
					callP->expr_type = ExprType::CallProcExpr;
					n_stack.push(callP);
					return true;
				}
				else {
					n_stack.push(NULL);
					return false;
				}
			}
			else {
				n_stack.push(NULL);
				return false;
			}

		}
		else if (lexer.get_body().compare(";") == 0
			|| lexer.get_type() == tok_end
			|| lexer.get_body().compare("until") == 0
			|| lexer.get_body().compare("else") == 0) {
			cur_count--;
			std::vector<ExprAST*> index;//绌哄€?
			CallProcedureExprAST* callP = new CallProcedureExprAST(callee, index, lexer.get_row());
			callP->expr_type = ExprType::CallProcExpr;
			n_stack.push(callP);
			return true;
		}
		else {
			cur_count = curNow;
			//lexer.getTOKEN(cur_count++);
			*flag = false;
			return false;
		}
	}
	return false;


}

bool Parser::UPDOWN(bool* rd) {
	if (lexer.get_type() == tok_keys) {
		std::string to = "to";
		std::string downto = "downto";
		std::string body = lexer.get_body();
		if (!body.compare(to)) {
			*rd = true;
			return true;
		}
		if (!body.compare(to)) {
			*rd = false;
			return true;
		}
	}
	return false;
}

bool Parser::ID_VARPART() {
	ExprAST* name = (ExprAST*)n_stack.top();
	n_stack.pop();
	if (lexer.get_body()[0] == '[') {
		std::vector<ExprAST*> index;
		lexer.getTOKEN(cur_count++);
		if (EXPRESSION_LIST(&index)) {
			ArrayVariableExprAST* tmp = new ArrayVariableExprAST(name, index, lexer.get_row());
			tmp->expr_type = ExprType::ArrayVariable;
			n_stack.push(tmp);
			lexer.getTOKEN(cur_count++);
			if (lexer.get_body()[0] == ']') {
				return true;
			}
		}
	}
	else if (lexer.get_body()[0] == '.') {
		lexer.getTOKEN(cur_count++);
		if (lexer.get_type() == tok_identifier) {
			RecordVariableExprAST* tmp = new RecordVariableExprAST(name, lexer.get_body(), lexer.get_row());
			tmp->expr_type = ExprType::RecordVariable;
			n_stack.push(tmp);
			return true;
		}
	}
	n_stack.push(name);
	return false;
}


bool Parser::ID_VARPARTS() {
	ExprAST* name = (ExprAST*)n_stack.top();
	n_stack.pop();
	std::string s = lexer.get_body();
	Token_type type = lexer.get_type();
	if (s[0] == '.' || s[0] == '[') { //对应结构体
		n_stack.push(name);
		if (ID_VARPART()) {
			lexer.getTOKEN(cur_count++);
			return ID_VARPARTS();
		}
	}
	if (s[0] == ')' || s[0] == ',' || s[0] == ';' ||
		s[0] == ']' || type == tok_addop || type == tok_assignop ||
		!s.compare("do") || !s.compare("else") || type == tok_end ||
		type == tok_mulop || !s.compare("of") || type == tok_relop ||
		!s.compare("then") || !s.compare("to") || !s.compare("until")) {
		//对应 ID_VARPARTS->空
		n_stack.push(name);
		cur_count--;
		return true;
	}

	return false;
}

bool Parser::VARIABLE() {
	if (lexer.get_type() == tok_identifier) { //获取终结符 id
		VariableExprAST * name = new VariableExprAST(lexer.get_body(), lexer.get_row());
		name->expr_type = ExprType::Variable;
		n_stack.push(name);
		lexer.getTOKEN(cur_count++);
		if (ID_VARPARTS()) { //在后续函数中直接将AST生成并入栈，若成功接收则直接返回真
			return true;
		}
	}
	n_stack.push(NULL);
	return false;
}

bool Parser::PROGRAMSTRUCE() {
	bool result = true;
	if (!PROGRAM_HEAD()) {
		//program_head接收失败，跳转到分号
		result = false;
		while (lexer.get_body()[0] != ';'&&lexer.get_type() != tok_eof) {
			lexer.getTOKEN(cur_count++);
		}
	}
	lexer.getTOKEN(cur_count++);
	if (PROGRAM_BODY()) {
		lexer.getTOKEN(cur_count++);
		if (lexer.get_body()[0] == '.') {
			std::vector<VariableDeclAST*>tmp;
			BlocksType tmp_type = BlocksType::procedure;
			program = new FunctionAST(name, tmp, NULL, cAst_vector, nameAst_vector, varD_vector, fun_vector, boDy, tmp_type);
			return result&&lexer.result;
		}
		std::cout << "程序尾部缺句号" << std::endl;
	}
	return false;
}

bool Parser::PROGRAM_HEAD() {
	if (lexer.get_type() == tok_program) {
		lexer.getTOKEN(cur_count++);
		if (lexer.get_type() == tok_identifier) {
			name = lexer.get_body();
			lexer.getTOKEN(cur_count++);
			if (lexer.get_type() == tok_delimiter
				&& lexer.get_body()[0] == '(') {//对应program_head->program id(idlist)
				lexer.getTOKEN(cur_count++);
				if (IDLIST(&arg)) {
					lexer.getTOKEN(cur_count++);
					if (lexer.get_type() == tok_delimiter
						&& lexer.get_body()[0] == ')') {
						lexer.getTOKEN(cur_count++);
						if (lexer.get_body()[0] == ';')
							return true;
					}
				}
				std::cout << "error line" << std::setw(3) << lexer.get_row() << ":程序声明非法" << std::endl;
				return false;
			}
			else { //以program_head->program id 接收
				if (lexer.get_body()[0] == ';') {
					return true;
				}
				cur_count--;
				std::cout << "error line" << std::setw(3) << lexer.get_row() << ":程序声明非法" << std::endl;
				return false;
			}
		}
		else {
			std::cout << "error line" << std::setw(3) << lexer.get_row() << ":程序名非法" << std::endl;
			return false;
		}
	}
	else {
		std::cout << "error line" << std::setw(3) << lexer.get_row() << ":程序应以 关键字 program 为开启" << std::endl;
	}
	return false;
}

bool Parser::PROGRAM_BODY() {
	bool result = true;
	if (!CONST_DECLARATIONS(&cAst_vector, 1)) {
		result = false;
		std::cout << "error line" << std::setw(3) << lexer.get_row() << ":CONST DECLARATIONS错误" << std::endl;
		while (lexer.get_type() != tok_type
			&& lexer.get_type() != tok_begin
			&& lexer.get_type() != tok_var
			&& lexer.get_type() != tok_procedure
			&& lexer.get_type() != tok_function
			&& lexer.get_type() != tok_eof
			) {
			lexer.getTOKEN(cur_count++);
		}
		cur_count--; //恢复到下一部分可接收部分
	}
	lexer.getTOKEN(cur_count++);
	if (!TYPE_DECLARATIONS(&nameAst_vector)) {
		//type 声明接收失败
		result = false;
		std::cout << "error line" << std::setw(3) << lexer.get_row() << ":TYPE DECLARATIONS错误" << std::endl;
		while (lexer.get_type() != tok_begin
			&& lexer.get_type() != tok_var
			&& lexer.get_type() != tok_procedure
			&& lexer.get_type() != tok_function
			&& lexer.get_type() != tok_eof) {
			lexer.getTOKEN(cur_count++);
		}
		cur_count--;
	}
	lexer.getTOKEN(cur_count++);
	if (!VAR_DECLARATIONS(&varD_vector)) {
		//var 声明接收失败
		result = false;
		std::cout << "error line" << std::setw(3) << lexer.get_row() << ":VAR DECLARATIONS错误" << std::endl;
		while (lexer.get_type() != tok_begin
			&& lexer.get_type() != tok_procedure
			&& lexer.get_type() != tok_function
			&& lexer.get_type() != tok_eof) {
			lexer.getTOKEN(cur_count++);
		}
		cur_count--;
	}
	else {
		std::vector<VariableDeclAST*>::iterator it;
		for (it = varD_vector.begin();
			it != varD_vector.end(); it++) {
			(*it)->isGlobal = 1;
		}
	}
	lexer.getTOKEN(cur_count++);
	if (!SUBPROGRAM_DECLARATIONS(&fun_vector)) {
		//函数声明接收失败
		result = false;
		std::cout << "error line" << std::setw(3) << lexer.get_row() << ":SUBPROGRAM DECLARATIONS错误" << std::endl;
		//子函数中完成了 字元流 到达 begin 的任务 (因为正常情况也下subprogram_declaration也许完成该工作
		//故这里由该函数完成该工作)
	}
	lexer.getTOKEN(cur_count++);
	if (!COMPOUND_STATEMENT()) {
		std::cout << "error line" << std::setw(3) << lexer.get_row() << ":COMPOUND STATEMENT错误" << std::endl;
		//执行语句部分接收失败
		result = false;
		n_stack.push(NULL);
		//程序结尾，后续无产生式 无需调整入口
	}
	BeginEndExprAST* tmp = (BeginEndExprAST*)n_stack.top();
	if (tmp != NULL)
		boDy = tmp->body;
	return result;
}

bool Parser::CONST1(std::vector<ConstAST*>* p) {
	int emp_count = cur_count - 1;
	if (lexer.get_type() == tok_delimiter
		&& lexer.get_body()[0] == ';') {
		lexer.getTOKEN(cur_count++);
		if (lexer.get_type() == tok_identifier) {
			std::string name = lexer.get_body();
			lexer.getTOKEN(cur_count++);
			if (lexer.get_body()[0] == '=') {
				lexer.getTOKEN(cur_count++);
				if (CONST_VALUE()) {
					lexer.getTOKEN(cur_count++);
					ConstAST* con = new ConstAST(name, (BasicTypeAST*)ast_type, *c_value);
					con->value = *c_value;
					con->value.s = (*c_value).s;
					con->lineNum = lexer.get_row();
					(*p).push_back(con);
					if (CONST1(p)) {
						return true;
					}
				}
			}
			//	std::cout << "error line" << std::setw(3) << lexer.get_row() << ":CONST DECLARATION 非法 " << std::endl;
			return false;
		}
	}

	cur_count = emp_count;
	lexer.getTOKEN(cur_count);
	if (lexer.get_body()[0] == ';') {
		return true;
	}
	return false;
}

bool Parser::CONST_VALUE() {
	std::string tmps;
	Token_type tmp;
	switch (lexer.get_type()) {
	case tok_boolc:
		if (lexer.get_body()[0] == 't')
			c_value->b = true;
		else
			c_value->b = false;
		ast_type = boolTypeAST;
		c_value->s = "+";
		return true;
	case tok_identifier:
		c_value->s = "+" + lexer.get_body();
		ast_type = idTypeAST;
		return true;
	case tok_number:
	case tok_integer:
		c_value->i = atoi(lexer.get_body().data());//将字符串转换为整型号
		ast_type = intTypeAST;
		c_value->s = "+";
		return true;
	case tok_real:
		tmps = lexer.get_body();
		char *tmp_ptr;
		c_value->d = strtod(tmps.data(), &tmp_ptr);//转换成d型
		c_value->s = "+";
		ast_type = realTypeAST;
		return true;
	case tok_letter:
		c_value->c = lexer.get_body()[0];
		ast_type = charTypeAST;
		c_value->s = "+";
		return true;
	case tok_addop:
		if (lexer.get_body()[0] == 'o')
			break;//对应or
		c_value->s = lexer.get_body();
		lexer.getTOKEN(cur_count++);
		tmp = lexer.get_type();
		if (tmp == tok_identifier) {
			c_value->s = c_value->s + lexer.get_body();
			ast_type = idTypeAST;
			return true;
		}
		else if (tmp == tok_integer || tmp == tok_real) {
			switch (tmp) {
			case tok_integer:
				c_value->i = atoi(lexer.get_body().data());//将字符串转换为整型号
				ast_type = intTypeAST;
				return true;
			case tok_real:
				tmps = lexer.get_body();
				char *tmp_ptr;
				c_value->d = strtod(tmps.data(), &tmp_ptr);//转换成d型
				ast_type = realTypeAST;
			}
		}
		cur_count--;//接收失败
		break;
	case tok_not:
		lexer.getTOKEN(cur_count++);
		if (lexer.get_type() == tok_boolc) {
			if (lexer.get_body()[0] == 't') //由于是not 所以值与 token相反
				c_value->b = false;
			else
				c_value->b = true;
			ast_type = boolTypeAST;
			return true;
		}
		else {
			std::cout << "error line" << std::setw(3) << lexer.get_row() << ":not 应作用于bool值" << std::endl;
		}
		break;
	}
	return false;
}

bool Parser::CONST_DECLARATION(std::vector<ConstAST*>* p, int type) {
	if (lexer.get_type() == tok_identifier) {
		std::string name = lexer.get_body();
		lexer.getTOKEN(cur_count++);
		if (lexer.get_body()[0] == '=') {
			lexer.getTOKEN(cur_count++);
			if (CONST_VALUE()) {
				lexer.getTOKEN(cur_count++);
				ConstAST* con = new ConstAST(name, (BasicTypeAST*)ast_type, *c_value);
				con->value = *c_value;
				con->value.s = (*c_value).s;
				con->lineNum = lexer.get_row();

				(*p).push_back(con);
				if (CONST1(p)) {
					std::vector<ConstAST*>::iterator it;
					for (it = p->begin(); it != p->end(); it++) {
						(*it)->isGlobal = type;
					}
					return true;
				}
			}
		}
	}
	std::cout << "error line" << std::setw(3) << lexer.get_row() << ":CONST DECLARATION ILLEGAL" << std::endl;
	return false;
}

bool Parser::CONST_DECLARATIONS(std::vector<ConstAST*>* p, int type) {
	int emp_count = cur_count - 1;//记录下若产生空，cur_count的值
	if (lexer.get_type() == tok_const) {
		lexer.getTOKEN(cur_count++);
		if (CONST_DECLARATION(p, type)) {
			lexer.getTOKEN(cur_count++);
			if (lexer.get_type() == tok_delimiter
				&& lexer.get_body()[0] == ';')
				return true;
		}
	}
	Token_type tmp = lexer.get_type();
	if (tmp == tok_array || tmp == tok_begin ||
		tmp == tok_boolean || tmp == tok_char ||
		tmp == tok_function || tmp == tok_integer ||
		tmp == tok_procedure || tmp == tok_real ||
		tmp == tok_var) {//对应C_DS->空
		cur_count = emp_count;
		return true;
	}
	return false;
}

bool Parser::BASIC_TYPE() {
	switch (lexer.get_type()) {
	case tok_integer:
	case tok_number:
		ast_type = intTypeAST;
		return true;
	case tok_real:
		ast_type = realTypeAST;
		return true;
	case tok_boolean:
		ast_type = boolTypeAST;
		return true;
	case tok_char:
		ast_type = charTypeAST;
		return true;
	default:
		break;
	}
	return false;
}

bool Parser::PERIOD(std::vector<ConstAST*> *con1, std::vector<ConstAST*> *con2) {
	int emp_count = cur_count - 1;
	Token_type tmp = lexer.get_type();
	if (tmp == tok_integer || tmp == tok_identifier
		|| (tmp == tok_addop && lexer.get_body()[0] != 'o') || tmp == tok_letter) {
		if (CONST_VALUE()) {
			ConstAST* c1 = new ConstAST("c1", (BasicTypeAST*)ast_type, *c_value);
			c1->value = *c_value;
			c1->value.s = (*c_value).s;
			c1->lineNum = lexer.get_row();
			lexer.getTOKEN(cur_count++);
			std::string tmps = lexer.get_body();
			if (tmps.length() == 2 && tmps[1] == '.') {
				lexer.getTOKEN(cur_count++);
				if (CONST_VALUE()) {
					ConstAST* c2 = new ConstAST("c2", (BasicTypeAST*)ast_type, *c_value);
					c2->value = *c_value;
					c2->value.s = (*c_value).s;
					c2->lineNum = lexer.get_row();
					(*con1).push_back(c1);
					(*con2).push_back(c2);
					lexer.getTOKEN(cur_count++);
					if (PERIOD1(con1, con2)) {
						//
						return true;
					}
				}
			}
		}

	}
	cur_count = emp_count;
	return false;
}

bool Parser::PERIOD1(std::vector<ConstAST*> *con1, std::vector<ConstAST*> *con2) {
	int emp_count = cur_count - 1;
	if (lexer.get_body()[0] == ',') {
		lexer.getTOKEN(cur_count++);
		if (CONST_VALUE()) {
			ConstAST* c1 = new ConstAST("c1", (BasicTypeAST*)ast_type, *c_value);
			c1->value = *c_value;
			c1->value.s = (*c_value).s;
			c1->lineNum = lexer.get_row();
			lexer.getTOKEN(cur_count++);
			std::string tmps = lexer.get_body();
			if (tmps.length() == 2 && tmps[1] == '.') {
				lexer.getTOKEN(cur_count++);
				if (CONST_VALUE()) {
					ConstAST* c2 = new ConstAST("c2", (BasicTypeAST*)ast_type, *c_value);
					c2->value = *c_value;
					c2->value.s = (*c_value).s;
					c2->lineNum = lexer.get_row();
					(*con1).push_back(c1);
					(*con2).push_back(c2);
					lexer.getTOKEN(cur_count++);
					if (PERIOD1(con1, con2)) {
						//
						return true;
					}
				}
			}
		}

	}
	if (lexer.get_body()[0] == ']') { //对应 period1->空
		cur_count = emp_count;
		return true;
	}
	return false;
}

bool Parser::RECORD_BODY(std::vector<VariableDeclAST*>* v_list) {
	int emp_count = cur_count - 1;
	if (lexer.get_type() == tok_end) {
		cur_count = emp_count;
		return true;
	}
	if (lexer.get_type() == tok_identifier) {
		if (VAR_DECLARATION(v_list)) {
			lexer.getTOKEN(cur_count++);
			if (lexer.get_body()[0] == ';')
				//构造AST
				return true;
		}

	}
	cur_count = emp_count;
	return true;
}

bool Parser::TYPE() {
	std::vector<VariableDeclAST*> tmp;
	switch (lexer.get_type()) {
	case tok_array:
		lexer.getTOKEN(cur_count++);
		if (lexer.get_body()[0] == '[') {
			lexer.getTOKEN(cur_count++);
			std::vector<ConstAST*> con1, con2;
			if (PERIOD(&con1, &con2)) {
				lexer.getTOKEN(cur_count++);
				if (lexer.get_body()[0] == ']') {
					lexer.getTOKEN(cur_count++);
					if (!lexer.get_body().compare("of")) {
						lexer.getTOKEN(cur_count++);
						if (TYPE()) {
							ast_type = new ArrayTypeAST(ast_type, con1, con2);
							ast_type->lineNum = lexer.get_row();
							return true;
						}
					}
				}
			}
		}
		break;
	case tok_record:
		lexer.getTOKEN(cur_count++);
		if (RECORD_BODY(&tmp)) {
			lexer.getTOKEN(cur_count++);
			ast_type = new RecordTypeAST(tmp);
			ast_type->lineNum = lexer.get_row();
			return lexer.get_type() == tok_end;
		}
		break;
	default:
		return BASIC_TYPE();
		break;
	}
	return false;
}

bool Parser::TYPE_DECLARATION1(std::vector<ChangeNameTypeAST*>* p) {
	int emp_count = cur_count - 1;
	if (lexer.get_body()[0] == ';') {
		lexer.getTOKEN(cur_count++);
		if (lexer.get_type() == tok_identifier) {
			std::string name = lexer.get_body();
			lexer.getTOKEN(cur_count++);
			if (lexer.get_body()[0] == '=') {
				lexer.getTOKEN(cur_count++);
				if (TYPE()) {
					ChangeNameTypeAST* tmp_ast =
						new ChangeNameTypeAST(name, ast_type);
					tmp_ast->lineNum = lexer.get_row();
					(*p).push_back(tmp_ast);
					lexer.getTOKEN(cur_count++);
					if (TYPE_DECLARATION1(p)) {
						return true;
					}
				}
			}
			std::cout << "error line" << std::setw(3) << lexer.get_row() << ":TYPE DECLARATION 非法" << std::endl;
			return false;
		}

		//对应 产生空
		cur_count = emp_count;
		return true;
	}
	return false;
}


bool Parser::TYPE_DECLARATION(std::vector<ChangeNameTypeAST*>*p) {
	if (lexer.get_type() == tok_identifier) {
		std::string name = lexer.get_body();
		lexer.getTOKEN(cur_count++);
		if (lexer.get_body()[0] == '=') {
			lexer.getTOKEN(cur_count++);
			if (TYPE()) {
				ChangeNameTypeAST* tmp_ast =
					new ChangeNameTypeAST(name, ast_type);
				tmp_ast->lineNum = lexer.get_row();
				(*p).push_back(tmp_ast);
				lexer.getTOKEN(cur_count++);
				if (TYPE_DECLARATION1(p)) {
					//AST
					return true;
				}
			}
		}
	}
	std::cout << "error line" << std::setw(3) << lexer.get_row() << ":TYPE DECLARATION ILLEGAL" << std::endl;
	return false;
}


bool Parser::TYPE_DECLARATIONS(std::vector<ChangeNameTypeAST*>*p) {
	int tmp_count = cur_count - 1;
	Token_type tmp_type = lexer.get_type();
	if (tmp_type == tok_type) {
		lexer.getTOKEN(cur_count++);
		if (TYPE_DECLARATION(p)) {
			lexer.getTOKEN(cur_count++);
			if (lexer.get_body()[0] == ';') {
				//AST
				return true;
			}
		}
	}
	if (tmp_type == tok_var || tmp_type == tok_function
		|| tmp_type == tok_begin || tmp_type == tok_procedure
		) {//对应TYPE_DECLARATIONS->空
		cur_count--;
		return true;
	}
	cur_count = tmp_count;
	return false;
}

bool Parser::VAR1(std::vector<VariableDeclAST*>* v_list) {
	int emp_count = cur_count - 1;
	if (lexer.get_body()[0] == ';') {
		lexer.getTOKEN(cur_count++);
		std::vector<std::string> tmp;
		if (lexer.get_type() == tok_identifier) {
			if (IDLIST(&tmp)) {
				lexer.getTOKEN(cur_count++);
				if (lexer.get_body()[0] == ':') {
					lexer.getTOKEN(cur_count++);
					if (TYPE()) {
						VariableDeclAST* tmp_vast = new VariableDeclAST((BasicTypeAST*)ast_type, tmp, 0, 1);
						tmp_vast->lineNum = lexer.get_row();
						(*v_list).push_back(tmp_vast);
						lexer.getTOKEN(cur_count++);
						if (VAR1(v_list))
							return true;
					}
				}
			}
			std::cout << "error line" << std::setw(3) << lexer.get_row() << ":CONST DECLARATIONS错误" << std::endl;
			return false;
		}
	}
	lexer.getTOKEN(emp_count);
	if (lexer.get_type() == tok_end
		|| lexer.get_body()[0] == ';') { //var1->空
		cur_count = emp_count;
		n_stack.push(NULL);
		return true;
	}
	cur_count = emp_count;
	return false;
}

bool Parser::VAR_DECLARATION(std::vector<VariableDeclAST*>* v_list) {
	std::vector<std::string> tmp;
	if (IDLIST(&tmp)) {
		lexer.getTOKEN(cur_count++);
		if (lexer.get_body()[0] == ':') {
			lexer.getTOKEN(cur_count++);
			if (TYPE()) {
				VariableDeclAST* tmp_vast = new VariableDeclAST((BasicTypeAST*)ast_type, tmp, 0, 1);
				tmp_vast->lineNum = lexer.get_row();
				tmp_vast->isGlobal = true;
				(*v_list).push_back(tmp_vast);
				lexer.getTOKEN(cur_count++);
				return VAR1(v_list);
			}
		}
	}
	std::cout << "error line" << std::setw(3) << lexer.get_row() << ":VARIABLE DECLARATION ILLEGAL" << std::endl;
	return false;
}

bool Parser::VAR_DECLARATIONS(std::vector<VariableDeclAST*>* p) {
	switch (lexer.get_type()) {
	case tok_procedure:case tok_function:
	case  tok_begin://对应VAR_DECLARATIONS->空
		cur_count--;
		return true;
	case tok_var:
		lexer.getTOKEN(cur_count++);
		if (VAR_DECLARATION(p)) {
			lexer.getTOKEN(cur_count++);
			return lexer.get_body()[0] == ';';
		}
	}
	return false;
}

bool Parser::PARAMETER1(std::vector<VariableDeclAST*>* p) {
	if (lexer.get_body()[0] == ')') {
		cur_count--;
		return true;
	}
	if (lexer.get_body()[0] == ';') {
		lexer.getTOKEN(cur_count++);
		if (PARAMETER(p)) {
			lexer.getTOKEN(cur_count++);
			return PARAMETER1(p);
		}
	}
	return false;
}

bool Parser::VALUE_PARAMETER(std::vector<VariableDeclAST*>* p, int type) {
	std::vector<std::string> tmps;
	if (IDLIST(&tmps)) {
		lexer.getTOKEN(cur_count++);
		if (lexer.get_body()[0] == ':') {
			lexer.getTOKEN(cur_count++);
			if (BASIC_TYPE()) {
				VariableDeclAST* tmp;
				std::vector<std::string>::iterator it;
				for (it = tmps.begin(); it != tmps.end(); it++) {
					std::vector<std::string> vec_tmp;
					vec_tmp.push_back(*it);
					tmp = new VariableDeclAST(ast_type, vec_tmp, type, 0);
					tmp->lineNum = lexer.get_row();
					tmp->isRef = type;
					tmp->isGlobal = false;
					(*p).push_back(tmp);
				}
				return true;
			}
		}
	}
	return false;
}

bool Parser::PARAMETER(std::vector<VariableDeclAST*>* p) {
	switch (lexer.get_type()) {
	case tok_identifier:
		if (VALUE_PARAMETER(p, 0)) {
			return true;
		}
		break;
	case tok_var:
		lexer.getTOKEN(cur_count++);
		if (VALUE_PARAMETER(p, 1)) {
			return true;
		}
		break;
	default:
		break;
	}
	return false;
}

bool Parser::PARAMETER_LIST(std::vector<VariableDeclAST*>* p) {
	if (PARAMETER(p)) {
		lexer.getTOKEN(cur_count++);
		if (PARAMETER1(p)) {
			return true;
		}
	}
	return false;
}


bool Parser::FORMAL_PARAMETER(std::vector<VariableDeclAST*>* p) {
	switch (lexer.get_body()[0]) {
	case '(':
		lexer.getTOKEN(cur_count++);
		if (PARAMETER_LIST(p)) {
			lexer.getTOKEN(cur_count++);
			return lexer.get_body()[0] == ')';
		}
		break;
	case ':':case ';':
		cur_count--;
		return true;
	default:
		break;
	}
	return false;
}

bool Parser::SUBPROGRAM_BODY(std::vector<ConstAST*>*c_p, std::vector<ChangeNameTypeAST*>* s_p,
	std::vector<VariableDeclAST*>* v_p, std::vector<FunctionAST*>*  f_p) {
	bool result = true;
	if (!CONST_DECLARATIONS(c_p, 0)) {
		result = false;
		std::cout << "error line" << std::setw(3) << lexer.get_row() << ":CONST DECLARATIONS错误" << std::endl;
		while (lexer.get_type() != tok_type
			&& lexer.get_type() != tok_begin
			&& lexer.get_type() != tok_var
			&& lexer.get_type() != tok_procedure
			&& lexer.get_type() != tok_function
			&& lexer.get_type() != tok_eof
			) {
			lexer.getTOKEN(cur_count++);
		}
		cur_count--; //恢复到下一部分可接收部分
	}
	lexer.getTOKEN(cur_count++);
	if (!TYPE_DECLARATIONS(s_p)) {
		//type 声明接收失败
		result = false;
		std::cout << "error line" << std::setw(3) << lexer.get_row() << ":TYPE DECLARATIONS错误" << std::endl;
		while (lexer.get_type() != tok_begin
			&& lexer.get_type() != tok_var
			&& lexer.get_type() != tok_procedure
			&& lexer.get_type() != tok_function
			&& lexer.get_type() != tok_eof) {
			lexer.getTOKEN(cur_count++);
		}
		cur_count--;
	}
	lexer.getTOKEN(cur_count++);
	if (!VAR_DECLARATIONS(v_p)) {
		//var 声明接收失败
		result = false;
		std::cout << "error line" << std::setw(3) << lexer.get_row() << ":VAR DECLARATIONS错误" << std::endl;
		while (lexer.get_type() != tok_begin
			&& lexer.get_type() != tok_procedure
			&& lexer.get_type() != tok_function
			&& lexer.get_type() != tok_eof) {
			lexer.getTOKEN(cur_count++);
		}
		cur_count--;
	}
	else {
		std::vector<VariableDeclAST*>::iterator it;
		for (it = v_p->begin();
			it != v_p->end(); it++) {
			(*it)->isGlobal = 1;
		}
	}
	lexer.getTOKEN(cur_count++);
	if (!SUBPROGRAM_DECLARATIONS(f_p)) {
		//函数声明接收失败
		result = false;
		std::cout << "error line" << std::setw(3) << lexer.get_row() << ":SUBPROGRAM DECLARATIONS错误" << std::endl;
		while (lexer.get_type() != tok_begin
			&& lexer.get_type() != tok_eof) {
			lexer.getTOKEN(cur_count++);
		}
		cur_count--;
	}
	lexer.getTOKEN(cur_count++);
	if (!COMPOUND_STATEMENT()) {
		//执行语句部分接收失败
		result = false;
		//	n_stack.push(NULL);
			//子程序结尾，后续应为 
	}
	if (result) { //目前为止接收成功，检测end 后是否有分号
		lexer.getTOKEN(cur_count++);
		result &= lexer.get_body()[0] == ';';
	}
	return result;



	//接受subprogram_body 并存入
/*	if (CONST_DECLARATIONS(c_p,0)) {
		lexer.getTOKEN(cur_count++);
		if (TYPE_DECLARATIONS(s_p)) {
			lexer.getTOKEN(cur_count++);
			if (VAR_DECLARATIONS(v_p)) {
				lexer.getTOKEN(cur_count++);
				if (SUBPROGRAM_DECLARATIONS(f_p)) {
					lexer.getTOKEN(cur_count++);
					if (COMPOUND_STATEMENT()) {
						lexer.getTOKEN(cur_count++);
						return lexer.get_body()[0] == ';';
					}
				}
			}
		}
	}
	return false; 错误停止版*/
}

bool Parser::SUBPROGRAM_HEAD(std::vector<VariableDeclAST*>* p, std::string* name, int* f_type) {
	switch (lexer.get_type()) {
	case tok_function:
		*f_type = 1; //代表function
		lexer.getTOKEN(cur_count++);
		if (lexer.get_type() == tok_identifier) {
			*name = lexer.get_body();
			lexer.getTOKEN(cur_count++);
			if (FORMAL_PARAMETER(p)) {
				lexer.getTOKEN(cur_count++);
				if (lexer.get_body()[0] == ':') {
					lexer.getTOKEN(cur_count++);
					if (BASIC_TYPE()) {
						lexer.getTOKEN(cur_count++);
						return lexer.get_body()[0] == ';';
					}
				}
			}
		}
		break;
	case tok_procedure:
		*f_type = 2; //代表procedure
		lexer.getTOKEN(cur_count++);
		if (lexer.get_type() == tok_identifier) {
			*name = lexer.get_body();
			lexer.getTOKEN(cur_count++);
			if (FORMAL_PARAMETER(p)) {
				lexer.getTOKEN(cur_count++);
				return lexer.get_body()[0] == ';';
			}
		}
		break;
	case tok_begin:
		//对应无subprogram情况
		return false;
	default:
		std::cout << "error line" << std::setw(3) << lexer.get_row() << "：关键词 procedure 或 function 应在子程序开头" << std::endl;
		break;
	}
	return false;
}

bool Parser::SUBPROGRAM(std::vector<FunctionAST*>* TOP_f_p) {
	std::vector<VariableDeclAST*> p;
	std::vector<ConstAST*> c_p;
	std::vector<ChangeNameTypeAST*> name_p;
	std::vector<VariableDeclAST*> v_p;
	std::vector<FunctionAST*> f_p;
	std::string name;
	int f_type;
	bool result = true;
	if (!SUBPROGRAM_HEAD(&p, &name, &f_type)) {
		//子程序 头部接收失败
		std::cout << "error line" << std::setw(3) << lexer.get_row() << ":子程序头部错误" << std::endl;
		result = false;
		while (lexer.get_body()[0] != ';'&&lexer.get_type() != tok_begin
			&& lexer.get_type() != tok_eof) {
			lexer.getTOKEN(cur_count++);
		}
		if (lexer.get_type() == tok_begin || lexer.get_type() != tok_eof)
			cur_count--;
	}
	lexer.getTOKEN(cur_count++);
	BlocksType type = BlocksType::function; //默认情况， 避免头部接收失败时未初始化
	switch (f_type) {
	case 1:type = BlocksType::function;
		break;
	case 2:type = BlocksType::procedure;
		break;
	default:
		break;
	}
	if (!SUBPROGRAM_BODY(&c_p, &name_p, &v_p, &f_p)) {
		//子程序执行语句部分接收失败
		result = false;
		n_stack.push(NULL);
		while (lexer.get_type() != tok_begin
			&& lexer.get_type() != tok_function
			&& lexer.get_type() != tok_procedure) {
			lexer.getTOKEN(cur_count++);
		}
		cur_count--;
	}
	std::vector<VariableDeclAST*>::iterator it;
	for (it = v_p.begin();
		it != v_p.end(); it++) {
		(*it)->isGlobal = 0;
	}
	BeginEndExprAST* tmp = (BeginEndExprAST*)n_stack.top();
	n_stack.pop();
	FunctionAST*f;
	if (tmp == NULL) {
		f = NULL;
	}
	else {
		f = new FunctionAST(name, p, (BasicTypeAST*)ast_type,
			c_p, name_p, v_p, f_p, tmp->body, type);
		f->lineNum = lexer.get_row();
	}
	TOP_f_p->push_back(f);
	//产生FUNCTIONAST并放入TOP_f_p中
	return result;
}

bool Parser::SUBPROGRAM_DECLARATIONS(std::vector<FunctionAST*>* f_p) {
	if (lexer.get_type() == tok_begin) {
		//对应 subprogram_declarations->空
		cur_count--;
		return true;
	}
	bool result = true;
	while (lexer.get_type() != tok_begin && lexer.get_type() != tok_eof)
		/*lexer.get_type() == tok_function
		|| lexer.get_type() == tok_procedure*/ {
		result &= SUBPROGRAM(f_p);
		lexer.getTOKEN(cur_count++);
	}
	if (lexer.get_type() == tok_begin)
		cur_count--;
	return result;
}




bool Parser::IDLIST(std::vector<std::string>* v_s) {
	if (lexer.get_type() == tok_identifier) {
		(*v_s).push_back(lexer.get_body());
		lexer.getTOKEN(cur_count++);
		return IDLIST1(v_s);
	}
	return false;
}

bool Parser::IDLIST1(std::vector<std::string>* v_s) {
	int emp_count = cur_count - 1;//记录下若产生空，cur_count的值
	if (lexer.get_type() == tok_delimiter
		&& lexer.get_body()[0] == ',') {
		lexer.getTOKEN(cur_count++);
		if (lexer.get_type() == tok_identifier) {
			(*v_s).push_back(lexer.get_body());
			lexer.getTOKEN(cur_count++);
			if (IDLIST1(v_s))
				return true;
		}
	}
	if (lexer.get_body()[0] == ':' || lexer.get_body()[0] == ')') { //对应 idlist'->空
		cur_count = emp_count;
		return true;
	}
	return false;
}



void Parser::start() {
	lexer.getTOKEN(0);
}





//张姐函数

bool Parser::COMPOUND_STATEMENT() {
	if (lexer.get_type() == tok_begin) {
		lexer.getTOKEN(cur_count++);
		std::vector<ExprAST*> exBody;
		if (STATEMENT_LIST(&exBody)) {
			lexer.getTOKEN(cur_count++);
			if (lexer.get_type() == tok_end) {
				BeginEndExprAST* beExpr = new BeginEndExprAST(exBody, lexer.get_row());
				n_stack.push(beExpr);
				beExpr->expr_type = ExprType::BeExpr;
				int i = 0;
				for (int j = 0; j < beExpr->body.size(); j++) {

				}
				return true;
			}
		}
		else {
			n_stack.push(NULL);

		}
	}
}

bool Parser::STATEMENT_LIST(std::vector<ExprAST*>* exBody) {
	bool result = true;
	if (!STATEMENT()) {
		result = false;
		int flag = 0;
		if (lexer.get_type() == tok_begin
			|| lexer.get_type() == tok_case
			|| lexer.get_type() == tok_if
			|| lexer.get_type() == tok_for
			|| lexer.get_type() == tok_begin
			|| lexer.get_type() == tok_identifier
			|| lexer.get_type() == tok_repeat
			|| lexer.get_type() == tok_while) {
			cur_count--;
		}
		else {
			while (!(lexer.get_type() == tok_end
				|| lexer.get_body().compare("else") == 0
				|| lexer.get_body().compare("until") == 0
				|| lexer.get_body().compare(";") == 0)) {
				flag = 1;
				lexer.getTOKEN(cur_count++);
			}
			if (flag == 1) {
				cur_count--;
			}
		}
	}
	exBody->push_back((ExprAST*)n_stack.top());
	n_stack.pop();
	lexer.getTOKEN(cur_count++);
	if (!STATEMENT1(exBody)) {
		result = false;
	}
	return result;

}


bool Parser::STATEMENT() {
	bool result = true;
	switch (lexer.get_type()) {
	case tok_delimiter: {
		if (lexer.get_body().compare(";") == 0) {
			cur_count--;
			n_stack.push(NULL);
			return result;
		}
		break;
	}
	case tok_end: {
		cur_count--;
		n_stack.push(NULL);
		return result;
		break;
	}
	case tok_keys: {
		if (lexer.get_body().compare("until") == 0) {
			cur_count--;
			n_stack.push(NULL);
			return result;
		}
		if (lexer.get_body().compare("else") == 0) {
			cur_count--;
			n_stack.push(NULL);
			return result;
		}
		n_stack.push(NULL);
		break;
	}
	case tok_begin: {
		if (COMPOUND_STATEMENT()) {
			return true;
		}
		break;
	}
	case tok_case: {
		lexer.getTOKEN(cur_count++);
		if (!EXPRESSION()) {
			result = false;
			std::cout << "error line" << std::setw(3) << lexer.get_row() << ":case语句中表达式错误" << std::endl;
			while (lexer.get_body().compare("of") != 0) {
				lexer.getTOKEN(cur_count++);
			}
			cur_count--;
		}
		ExprAST* cond = (ExprAST*)n_stack.top();
		n_stack.pop();
		lexer.getTOKEN(cur_count++);

		if (lexer.get_body().compare("of") == 0) {
			std::map<ExprAST*, ExprAST*> bo;
			lexer.getTOKEN(cur_count++);
			if (!CASE_BODY(&bo)) {
				result = false;
				std::cout << "error line" << std::setw(3) << lexer.get_row() << ":case语句中CASE_BODY错误" << std::endl;
				while (lexer.get_type() != tok_end) {
					lexer.getTOKEN(cur_count++);
				}
				cur_count--;
			}
			lexer.getTOKEN(cur_count++);
			if (lexer.get_type() == tok_end) {
				if (result == true) {
					CaseofEndExprAST* cof = new CaseofEndExprAST(cond, bo, lexer.get_row());
					n_stack.push(cof);
					cof->expr_type = ExprType::CaseExpr;
				}
				else {
					n_stack.push(NULL);
				}
				return result;
			}
			else {
				std::cout << "error line" << std::setw(3) << lexer.get_row() << ":case语句中缺少end" << std::endl;
				n_stack.push(NULL);
				return false;
			}
		}
		else {
			std::cout << "error line" << std::setw(3) << lexer.get_row() << ":case语句中缺少of" << std::endl;
			n_stack.push(NULL);
			return false;
		}
		break;
	}
	case tok_for: {
		lexer.getTOKEN(cur_count++);
		if (lexer.get_type() != tok_identifier) {
			result = false;
			std::cout << "error line" << std::setw(3) << lexer.get_row() << ":for语句循环条件中缺少标识符 " << std::endl;
			while (lexer.get_type() != tok_assignop) {
				lexer.getTOKEN(cur_count++);
			}
			cur_count--;
		}
		std::string varName;
		if (result == true) {
			varName = lexer.get_body();
		}
		lexer.getTOKEN(cur_count++);
		if (lexer.get_type() == tok_assignop) {
			lexer.getTOKEN(cur_count++);
			if (!EXPRESSION()) {
				result = false;
				std::cout << "error line" << std::setw(3) << lexer.get_row() << ":for语句循环条件中缺少表达式" << std::endl;
				while (lexer.get_body().compare("to") != 0 && lexer.get_body().compare("downto") != 0) {
					lexer.getTOKEN(cur_count++);
				}
				cur_count--;
			}
			ExprAST* start = (ExprAST*)n_stack.top();
			n_stack.pop();
			lexer.getTOKEN(cur_count++);
			bool toOrdown;

			if (UPDOWN(&toOrdown)) {
				bool increaseDirection;
				if (toOrdown == true) {
					increaseDirection = true;
				}
				else {
					increaseDirection = false;
				}
				lexer.getTOKEN(cur_count++);
				if (!EXPRESSION()) {
					result = false;
					std::cout << "error line" << std::setw(3) << lexer.get_row() << ":for语句循环条件中缺少表达式" << std::endl;
					while (lexer.get_body().compare("do") != 0) {
						lexer.getTOKEN(cur_count++);
					}
					cur_count--;
				}
				ExprAST* end = (ExprAST*)n_stack.top();
				n_stack.pop();
				lexer.getTOKEN(cur_count++);

				if (lexer.get_body().compare("do") == 0) {
					lexer.getTOKEN(cur_count++);
					if (!STATEMENT()) {
						result = false;

					}
					ExprAST* body = (ExprAST*)n_stack.top();
					n_stack.pop();
					if (result == true) {
						ForExprAST* fExpr = new ForExprAST(varName, start, end, body, increaseDirection, lexer.get_row());
						n_stack.push(fExpr);
						fExpr->expr_type = ExprType::ForExpr;
					}
					else {
						n_stack.push(NULL);
					}
					return result;
				}
				else {
					std::cout << "error line" << std::setw(3) << lexer.get_row() << ":for语句中缺少do" << std::endl;
					n_stack.push(NULL);
					return false;
				}
			}
			else {
				std::cout << "error line" << std::setw(3) << lexer.get_row() << ":for语句中缺少to/downto" << std::endl;
				n_stack.push(NULL);
				return false;
			}
		}
		else {
			std::cout << "error line" << std::setw(3) << lexer.get_row() << ":for语句循环条件中缺少赋值符号:=" << std::endl;
			n_stack.push(NULL);
			return false;
		}
		break;
	}
	case tok_identifier: {
		bool flag;
		bool isS = PROCEDURE_CALL(&flag);
		if (flag == true) {
			if (!isS) {
				result = false;
				std::cout << "error line" << std::setw(3) << lexer.get_row() << ":函数调用错误" << std::endl;

			}
			return result;
		}
		else {
			lexer.getTOKEN(cur_count++);
			if (!VARIABLE()) {
				result = false;
				std::cout << "error line" << std::setw(3) << lexer.get_row() << ":赋值语句中变量错误" << std::endl;
				while (lexer.get_type() != tok_assignop) {
					lexer.getTOKEN(cur_count++);
				}
				cur_count--;
			}
			ExprAST* left = (ExprAST*)n_stack.top();
			n_stack.pop();
			lexer.getTOKEN(cur_count++);
			if (lexer.get_type() == tok_assignop) {
				lexer.getTOKEN(cur_count++);
				if (!EXPRESSION()) {
					result = false;
					std::cout << "error line" << std::setw(3) << lexer.get_row() << ":赋值语句右侧表达式错误" << std::endl;
				}
				ExprAST* right = (ExprAST*)n_stack.top();
				n_stack.pop();
				if (result == true) {
					BinaryOpKind binaryOp = BinaryOpKind::assignmentKind;
					BinaryExprAST* assign = new BinaryExprAST(binaryOp, left, right, lexer.get_row());
					n_stack.push(assign);
					assign->expr_type = ExprType::BinaryExpr;
				}
				else {
					n_stack.push(NULL);
				}
				return result;
			}
			else {
				std::cout << "error line" << std::setw(3) << lexer.get_row() << ":赋值语句中缺少赋值符号:=" << std::endl;
				n_stack.push(NULL);
				return false;
			}
		}
		break;
	}
	case tok_if: {
		lexer.getTOKEN(cur_count++);
		if (!EXPRESSION()) {
			result = false;
			std::cout << "error line" << std::setw(3) << lexer.get_row() << ":if语句中判断条件错误" << std::endl;
			while (lexer.get_body().compare("then") != 0) {
				lexer.getTOKEN(cur_count++);
			}
			cur_count--;
		}
		ExprAST* ifCond = (ExprAST*)n_stack.top();
		n_stack.pop();
		lexer.getTOKEN(cur_count++);
		if (lexer.get_body().compare("then") == 0) {
			lexer.getTOKEN(cur_count++);
			if (!STATEMENT()) {
				result = false;
				std::cout << "error line" << std::setw(3) << lexer.get_row() << ":if语句中if体错误" << std::endl;
				while (!(lexer.get_type() == tok_end
					|| lexer.get_body().compare("else") == 0
					|| lexer.get_body().compare("until") == 0
					|| lexer.get_body().compare(";") == 0)) {
					lexer.getTOKEN(cur_count++);
				}
				cur_count--;
			}
			ExprAST* thenComponent = (ExprAST*)n_stack.top();
			n_stack.pop();
			lexer.getTOKEN(cur_count++);
			if (!ELSE_PART()) {
				result = false;
				std::cout << "error line" << std::setw(3) << lexer.get_row() << ":if语句中else体错误" << std::endl;
			}
			ExprAST* elseComponent = (ExprAST*)n_stack.top();
			n_stack.pop();
			if (result == true) {
				IfExprAST* ifExpr = new IfExprAST(ifCond, thenComponent, elseComponent, lexer.get_row());
				n_stack.push(ifExpr);
				ifExpr->expr_type = ExprType::IfExpr;
			}
			else {
				n_stack.push(NULL);
			}
			return result;
		}
		else {
			std::cout << "error line" << std::setw(3) << lexer.get_row() << ":if语句中缺少then" << std::endl;
			n_stack.push(NULL);
			return false;
		}
		break;
	}
	case tok_repeat: {
		lexer.getTOKEN(cur_count++);
		std::vector<ExprAST*> body;
		if (!STATEMENT_LIST(&body)) {
			result = false;
			std::cout << "error line" << std::setw(3) << lexer.get_row() << ":repeat语句中STATEMENT_LIST错误" << std::endl;
			while (lexer.get_body().compare("until") != 0) {
				lexer.getTOKEN(cur_count++);
			}
			cur_count--;
		}
		lexer.getTOKEN(cur_count++);
		if (lexer.get_body().compare("until") == 0) {
			lexer.getTOKEN(cur_count++);
			if (!EXPRESSION()) {
				result = false;
				std::cout << "error line" << std::setw(3) << lexer.get_row() << ":repeat语句中判断条件错误" << std::endl;
			}
			ExprAST* untilCond = (ExprAST*)n_stack.top();
			n_stack.pop();
			if (result == true) {
				RepeatExprAST* re = new RepeatExprAST(untilCond, body, lexer.get_row());
				n_stack.push(re);
				re->expr_type = ExprType::RepeatExpr;
			}
			else {
				n_stack.push(NULL);
			}
			return result;
		}
		else {
			std::cout << "error line" << std::setw(3) << lexer.get_row() << ":repeat语句中缺少until" << std::endl;
			n_stack.push(NULL);
			return false;
		}
		break;
	}
	case tok_while: {
		lexer.getTOKEN(cur_count++);
		if (!EXPRESSION()) {
			result = false;
			std::cout << "error line" << std::setw(3) << lexer.get_row() << ":while语句中循环条件错误" << std::endl;
			while (lexer.get_body().compare("do") != 0) {
				lexer.getTOKEN(cur_count++);
			}
			cur_count--;
		}
		ExprAST* whileCond = (ExprAST*)n_stack.top();
		n_stack.pop();
		lexer.getTOKEN(cur_count++);
		if (lexer.get_body().compare("do") == 0) {
			lexer.getTOKEN(cur_count++);
			if (!STATEMENT()) {
				result = false;
				std::cout << "error line" << std::setw(3) << lexer.get_row() << ":while语句中循环体错误" << std::endl;
			}
			ExprAST* body = (ExprAST*)n_stack.top();
			n_stack.pop();
			if (result == true) {
				WhileExprAST* w = new WhileExprAST(whileCond, body, lexer.get_row());
				n_stack.push(w);
				w->expr_type = ExprType::WhileExpr;
			}
			else {
				n_stack.push(NULL);
			}
			return result;
		}
		else {
			std::cout << "error line" << std::setw(3) << lexer.get_row() << ":while语句中缺少do" << std::endl;
			n_stack.push(NULL);
			return false;
		}
		break;
	}
	default:
		std::cout << "error line" << std::setw(3) << lexer.get_row() << ":STATEMENT类型未知" << std::endl;
		n_stack.push(NULL);
		return false;
		break;
	}
	return false;
}



bool Parser::STATEMENT1(std::vector<ExprAST*>* exBody) {
	bool result = true;
	if (lexer.get_type() == tok_end) {
		cur_count--;
		return true;
	}
	else if (lexer.get_body().compare("until") == 0) {
		cur_count--;
		return true;
	}
	else {
		if (lexer.get_body().compare(";") != 0) {
			std::cout << "error line" << std::setw(3) << lexer.get_row() << ":该句子前缺少分号;" << std::endl;
			result = false;
			int flag = 0;
			while (!(lexer.get_type() == tok_begin
				|| lexer.get_type() == tok_case
				|| lexer.get_type() == tok_if
				|| lexer.get_type() == tok_for
				|| lexer.get_type() == tok_begin
				|| lexer.get_type() == tok_identifier
				|| lexer.get_type() == tok_repeat
				|| lexer.get_type() == tok_while)) {
				lexer.getTOKEN(cur_count++);
				flag = 1;
			}
			if (flag == 0) {
				cur_count--;
			}
		}
		lexer.getTOKEN(cur_count++);
		if (!STATEMENT()) {
			result = false;
			int flag = 0;
			if (lexer.get_type() == tok_begin
				|| lexer.get_type() == tok_case
				|| lexer.get_type() == tok_if
				|| lexer.get_type() == tok_for
				|| lexer.get_type() == tok_begin
				|| lexer.get_type() == tok_identifier
				|| lexer.get_type() == tok_repeat
				|| lexer.get_type() == tok_while) {
				cur_count--;
			}
			else {
				while (!(lexer.get_type() == tok_end
					|| lexer.get_body().compare("else") == 0
					|| lexer.get_body().compare("until") == 0
					|| lexer.get_body().compare(";") == 0)) {
					flag = 1;
					lexer.getTOKEN(cur_count++);
				}
				if (flag == 1) {
					cur_count--;
				}
			}
		}
		exBody->push_back((ExprAST*)n_stack.top());
		n_stack.pop();
		lexer.getTOKEN(cur_count++);
		if (!STATEMENT1(exBody)) {
			result = false;
		}
		return result;
	}
}

bool Parser::ELSE_PART() {
	bool result = true;
	switch (lexer.get_type()) {
	case tok_delimiter:
		if (lexer.get_body().compare(";") == 0) {
			cur_count--;
			n_stack.push(NULL);
			return true;
		}
		break;
	case tok_end:
		cur_count--;
		n_stack.push(NULL);
		return true;
		break;
	case tok_keys:
		if (lexer.get_body().compare("until") == 0) {
			cur_count--;
			n_stack.push(NULL);
			return true;
		}
		if (lexer.get_body().compare("else") == 0) {
			lexer.getTOKEN(cur_count++);
			if (!STATEMENT()) {
				result = false;
			}
			return result;
		}
		break;
	default:
		n_stack.push(NULL);
		return false;
		break;
	}
	return false;
}

bool Parser::CASE_BODY(std::map<ExprAST*, ExprAST*>* body) {
	bool result = true;
	switch (lexer.get_type()) {
	case tok_integer: case tok_identifier: case tok_addop: case tok_letter: case tok_boolc: case tok_not:
		if (lexer.get_type() == tok_addop && lexer.get_body()[0] == 'o') {
			break;
		}
		if (BRANCH_LIST(body)) {
			return true;
		}
		break;
	case tok_delimiter:
		if (lexer.get_body().compare(";") == 0) {
			if (BRANCH_LIST(body)) {
				return true;
			}
		}
		break;
	case tok_end:
		cur_count--;
		return true;
		break;
	default:
		//cout << "error line" << setw(3) << lexer.get_row() << ":BRANCH_LIST閿欒" << endl;
		return false;
		break;
	}
	return false;
}

bool Parser::BRANCH_LIST(std::map<ExprAST*, ExprAST*>* body) {
	bool result = true;
	switch (lexer.get_type()) {
	case tok_identifier: case tok_integer: case tok_addop:case tok_letter:
		if (lexer.get_type() == tok_addop && lexer.get_body()[0] == 'o') {
			break;
		}
		if (BRANCH(body)) {
			lexer.getTOKEN(cur_count++);
			if (BRANCH1(body)) {
				return true;
			}
		}
		break;
	default:
		return false;
		break;
	}
	return false;
}

bool Parser::BRANCH1(std::map<ExprAST*, ExprAST*>* body) {
	bool result = true;
	switch (lexer.get_type()) {
	case tok_delimiter:
		if (lexer.get_body().compare(";") == 0) {
			lexer.getTOKEN(cur_count++);
			if (BRANCH(body)) {
				lexer.getTOKEN(cur_count++);
				if (BRANCH1(body)) {
					return true;
				}
			}
		}
		break;
	case tok_end:
		cur_count--;
		return true;
		break;
	default:
		return false;
		break;
	}
	return false;
}

bool Parser::BRANCH(std::map<ExprAST*, ExprAST*>* body) {
	bool result = true;
	switch (lexer.get_type()) {
	case tok_integer: case tok_identifier: case tok_addop: case tok_letter:
	{
		if (lexer.get_type() == tok_addop && lexer.get_body()[0] == 'o') {
			break;
		}
		std::vector<ExprAST*> constBody;
		if (CONST_LIST(&constBody)) {
			lexer.getTOKEN(cur_count++);
			if (lexer.get_body().compare(":") == 0) {
				lexer.getTOKEN(cur_count++);
				if (STATEMENT()) {
					for (int i = 0; i < constBody.size(); i++) {
						body->insert(std::map<ExprAST*, ExprAST*>::value_type(constBody[i], (ExprAST*)n_stack.top()));
					}
					n_stack.pop();
					return true;
				}
			}
		}
		break;
	}
	case tok_delimiter:
		if (lexer.get_body().compare(";") == 0) {
			cur_count--;
			return true;
		}
		break;
	case tok_end:
		cur_count--;
		return true;
		break;
	default:
		return false;
		break;
	}
	return false;
}

bool Parser::CONST_LIST(std::vector<ExprAST*>* body) {
	bool result = true;
	switch (lexer.get_type()) {
	case tok_addop: case tok_integer: case tok_identifier: case tok_letter: case tok_not:
		if (lexer.get_type() == tok_addop && lexer.get_body().compare("o") == 0) {
			break;
		}
		if (CONST_VALUE()) {
			ExprAST* exp = NULL;
			if (ast_type->GetType() == TypeName::Integer) {
				if (c_value->s[0] == '-') {
					NumberExprAST* num = new NumberExprAST(c_value->i, lexer.get_row());
					exp = new UnaryExprAST(UnaryOpKind::negKind, num, lexer.get_row());
					exp->expr_type = ExprType::UnaryExpr;
				}
				else {
					exp = new NumberExprAST(c_value->i, lexer.get_row());
					exp->expr_type = ExprType::NumberExpr;
				}
			}
			else if (ast_type->GetType() == TypeName::Real) {
				if (c_value->s[0] == '-') {
					RealExprAST* real = new RealExprAST(c_value->d, lexer.get_row());
					exp = new UnaryExprAST(UnaryOpKind::negKind, real, lexer.get_row());
					exp->expr_type = ExprType::UnaryExpr;
				}
				else {
					exp = new RealExprAST(c_value->d, lexer.get_row());
					exp->expr_type = ExprType::RealExpr;
				}
			}
			else if (ast_type->GetType() == TypeName::Char) {
				exp = new CharExprAST(c_value->c, lexer.get_row());
				exp->expr_type = ExprType::CharExpr;
			}
			else if (ast_type->GetType() == TypeName::Id) {//id
				if (c_value->s[0] == '-') {
					std::string st = c_value->s.substr(1);
					VariableExprAST* id = new VariableExprAST(st, lexer.get_row());
					id->expr_type = ExprType::Variable;
					exp = new UnaryExprAST(UnaryOpKind::negKind, id, lexer.get_row());
					exp->expr_type = ExprType::UnaryExpr;
				}
				else {
					std::string st = c_value->s.substr(1);
					exp = new VariableExprAST(st, lexer.get_row());
					exp->expr_type = ExprType::Variable;
				}
			}
			else if (ast_type->GetType() == TypeName::Bool) {//bool
				exp = new BoolExprAST(c_value->b, lexer.get_row());
				exp->expr_type = ExprType::BoolExpr;
			}
			body->push_back(exp);
			lexer.getTOKEN(cur_count++);
			if (CONST_LIST1(body)) {
				return true;
			}
			else {
				return false;
			}
		}
		else {
			return false;
		}
		break;
	default:
		return false;
		break;
	}
	return false;
}

bool Parser::CONST_LIST1(std::vector<ExprAST*>* body) {
	bool result = true;
	if (lexer.get_body().compare(",") == 0) {
		lexer.getTOKEN(cur_count++);
		if (CONST_VALUE()) {
			body->push_back((ExprAST*)n_stack.top());
			n_stack.pop();
			lexer.getTOKEN(cur_count++);
			if (CONST_LIST1(body)) {
				return true;
			}
			else {
				return false;
			}
		}
		else {
			return false;
		}
	}
	else if (lexer.get_body().compare(":") == 0) {
		cur_count--;
		return true;
	}
	else {
		return false;
	}
}



std::string Parser::TYPE_S(TypeName n) {
	switch (n) {
	case TypeName::Array:
		return "Array";
		break;
	case TypeName::Bool:
		return "Boolean";
		break;
	case TypeName::changeType:
		return "ChangeType";
		break;
	case TypeName::Char:
		return "Char";
		break;
	case TypeName::Id:
		return "Identifier";
		break;
	case TypeName::Integer:
		return "Interger";
		break;
	case TypeName::Real:
		return "Real";
	case TypeName::Record:
		return "Record";
		break;
	default:
		return "error";
		break;
	}
}

std::string Parser::VALUE_S(constValue* v, TypeName n) {
	std::string result = v->s;
	switch (n) {
	case TypeName::Integer:
		result += std::to_string(v->i);
		return result;
	case TypeName::Real:
		result += std::to_string(v->d);
		return result;
	case TypeName::Char:
		result = "\'";
		result += v->c;
		result += '\'';
		return result;
	case TypeName::Bool:
		if (v->b)
			result = "true";
		else
			result = "false";
	case TypeName::Id:
		return result;
	}
	return "error";
}

void Parser::outc(ConstAST* p, int count) {
	outtab(count);
	std::cout << "	name:" << std::left << std::setw(9) << p->variableName.data() << " type:" << std::left << std::setw(11) << TYPE_S(p->type->GetType()).data();
	std::cout << "value:" << VALUE_S(&(p->value), p->type->GetType()) << std::endl;
}

void Parser::outtab(int i) {
	while (i--) {
		std::cout << "	";
	}
}

void Parser::out_arr(TypeAST*p, int count) {
	ArrayTypeAST* a = (ArrayTypeAST*)p;
	outtab(count);
	std::cout << "	Index constAST:" << std::endl;
	std::vector<ConstAST*>::iterator it1, it2;
	for (it1 = a->startIndexs.begin(), it2 = a->endIndexs.begin();
		it1 != a->startIndexs.end(); it1++, it2++) {
		outc(*it1, count + 1);
		outc(*it2, count + 1);
	}
	outtab(count);
	std::cout << "	array_type:" << std::left << std::setw(11) << TYPE_S(a->type->GetType()).data() << std::endl;
	if (a->type->GetType() == TypeName::Array) {
		out_arr(a->type, count + 1);
	}
	if (a->type->GetType() == TypeName::Record) {
		out_rec(a->type, count + 1);
	}
}

void Parser::out_rec(TypeAST*p, int count) {
	RecordTypeAST* r = (RecordTypeAST*)p;
	outtab(count);
	std::cout << "record body(" << r->declList.size() << "):" << std::endl;
	std::vector<VariableDeclAST*>::iterator it;
	for (it = r->declList.begin(); it != r->declList.end(); it++) {
		outv(*it, count + 1);
	}
}

void Parser::outt(ChangeNameTypeAST*p, int count) {
	outtab(count);
	std::cout << "	new name:" << p->newName << "	";
	std::cout << "	old type:" << std::left << std::setw(11) << TYPE_S(p->originalType->GetType()).data() << std::endl;
	if (p->originalType->GetType() == TypeName::Array) {
		out_arr(p->originalType, count + 1);
	}
	if (p->originalType->GetType() == TypeName::Record) {
		out_rec(p->originalType, count + 1);
	}
	std::cout << std::endl;

}



void Parser::outv(VariableDeclAST* p, int count) {
	outtab(count);
	std::cout << "	VAR_name:";
	std::vector<std::string>::iterator it;
	for (it = p->variableName.begin(); it != p->variableName.end(); it++) {
		std::cout << (*it).data() << " ";
	} std::cout << std::endl;
	outtab(count);
	std::cout << "	type:" << std::left << std::setw(11) << TYPE_S(p->type->GetType()).data() << std::endl;
	if (p->type->GetType() == TypeName::Array) {
		out_arr(p->type, count + 1);
	}
	if (p->type->GetType() == TypeName::Record) {
		out_rec(p->type, count + 1);
	}
	std::cout << std::endl;
}

void Parser::outf(FunctionAST* p, int count) {
	outtab(count);
	switch (p->type) {
	case BlocksType::function:
		std::cout << "	function";
		break;
	case BlocksType::procedure:
		std::cout << "	procedure";
		break;
	default:
		std::cout << "	";
		break;
	}
	std::cout << " name:" << p->name.data() << "	arg:" << std::endl;
	std::vector<VariableDeclAST*>::iterator itv;
	for (itv = p->headerDecl.begin(); itv != p->headerDecl.end(); itv++) {
		outtab(count + 1);
		switch ((*itv)->isRef) {
		case 1:
			std::cout << "	引用参数:" << std::endl;
			outv(*itv, count + 3);
			break;
		case 0:
			std::cout << "	传值参数:" << std::endl;
			outv(*itv, count + 3);
			break;
		default:
			std::cout << "ERROR:!!!!!!!!!!!!!!!" << std::endl;
			break;
		}
	}
	std::cout << std::endl;
	std::vector<ConstAST*>::iterator it1;
	outtab(count);
	std::cout << "	SUBPROGRAM_CONST声明：" << std::endl;
	for (it1 = p->consts.begin(); it1 != p->consts.end(); it1++)
		outc(*it1, count + 1);
	std::vector<ChangeNameTypeAST*>::iterator it2;
	outtab(count);
	std::cout << "	SUBPROGRAM_TYPE 声明：" << std::endl;
	for (it2 = p->selfdefineType.begin(); it2 != p->selfdefineType.end(); it2++)
		outt(*it2, count + 1);
	std::vector<VariableDeclAST*>::iterator it3;
	outtab(count);
	std::cout << "	SUBPROGRAM_VAR  声明：" << std::endl;
	for (it3 = p->bodyDecl.begin(); it3 != p->bodyDecl.end(); it3++)
		outv(*it3, count + 1);
	std::vector<FunctionAST*>::iterator it4;
	outtab(count);
	std::cout << "	SUBPROGRAM_SUBPROGRAM声明：" << std::endl;
	for (it4 = p->functions.begin(); it4 != p->functions.end(); it4++)
		outf(*it4, count + 1);
	std::vector<ExprAST*>::iterator it5;
	outtab(count);
	std::cout << "	SUBPROGRAM_执行语句ExprAST：" << std::endl;
	for (it5 = p->body.begin(); it5 != p->body.end(); it5++)
		outb(*it5);
}

void Parser::outb(ExprAST* p) {

}


void Parser::out() {
	std::cout << "PROGRAM NAME:" << name.data() << " arg:";
	std::vector<std::string>::iterator it = arg.begin();
	for (it = arg.begin(); it != arg.end(); it++)
		std::cout << " " << (*it).data();
	std::cout << std::endl;
	std::vector<ConstAST*>::iterator it1;
	std::cout << "CONST声明：" << std::endl;
	for (it1 = cAst_vector.begin(); it1 != cAst_vector.end(); it1++)
		outc(*it1, 0);
	std::vector<ChangeNameTypeAST*>::iterator it2;
	std::cout << "TYPE 声明：" << std::endl;
	for (it2 = nameAst_vector.begin(); it2 != nameAst_vector.end(); it2++)
		outt(*it2, 0);
	std::vector<VariableDeclAST*>::iterator it3;
	std::cout << "VAR  声明：" << std::endl;
	for (it3 = varD_vector.begin(); it3 != varD_vector.end(); it3++)
		outv(*it3, 0);
	std::vector<FunctionAST*>::iterator it4;
	std::cout << "SUBPROGRAM声明：" << std::endl;
	for (it4 = fun_vector.begin(); it4 != fun_vector.end(); it4++)
		outf(*it4, 0);
	std::vector<ExprAST*>::iterator it5;
	std::cout << "执行语句ExprAST：" << std::endl;
	for (it5 = boDy.begin(); it5 != boDy.end(); it5++)
		outb(*it5);
}