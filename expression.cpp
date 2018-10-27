#include "expression.hpp"

#include <sstream>
#include <list>
#include <iostream>
#include "environment.hpp"
#include "semantic_error.hpp"

Expression::Expression(){}

Expression::Expression(const Atom & a){
  m_head = a;
}

// recursive copy
Expression::Expression(const Expression & a){
  m_head = a.m_head;
  propertymap = a.propertymap;
  for(auto e : a.m_tail){
    m_tail.push_back(e);
  }
}

// List Constructor for Expression Object
Expression::Expression(const std::vector<Expression> & list) {
	m_head.setList();
	m_tail = list;
}

// Lambda Constructor for Expression object
Expression::Expression(const std::vector<Expression> & args, const Atom & a) {
	m_head = a;
	for (auto e : args) {
		m_tail.push_back(e);
	}
}

// Assignment operator for Expression
Expression & Expression::operator=(const Expression & a){

  // prevent self-assignment
  if(this != &a){
    m_head = a.m_head;
    propertymap = a.propertymap;
    m_tail.clear();
    for(auto e : a.m_tail){
      m_tail.push_back(e);
    } 
  }
  
  return *this;
}


Atom & Expression::head(){
  return m_head;
}

const Atom & Expression::head() const{
  return m_head;
}

bool Expression::isHeadNumber() const noexcept{
  return m_head.isNumber();
}

bool Expression::isHeadSymbol() const noexcept{
  return m_head.isSymbol();
}  

bool Expression::isHeadComplex() const noexcept{
  return m_head.isComplex();
}

bool Expression::isHeadList() const noexcept {
  return m_head.isList();
}

bool Expression::isHeadString() const noexcept {
	return m_head.isString();
}


void Expression::append(const Atom & a){
  m_tail.emplace_back(a);
}


Expression * Expression::tail(){
  Expression * ptr = nullptr;
  
  if(m_tail.size() > 0){
    ptr = &m_tail.back();
  }

  return ptr;
}

Expression::ConstIteratorType Expression::tailConstBegin() const noexcept{
  return m_tail.cbegin();
}

Expression::ConstIteratorType Expression::tailConstEnd() const noexcept{
  return m_tail.cend();
}

// Apply function used for simple and lambda kind which calls eval on the expression
Expression apply(const Atom & op, const std::vector<Expression> & args, const Environment & env){
  // need to create a new environment to use for the 1st one
	if (env.is_exp(op)) {
		args[0].head();
		uint64_t index = 0;
		Environment newEnv = env;
		Expression newExp = newEnv.get_exp(op);
		Expression newArgs = *newExp.tailConstBegin();
		std::vector<Expression> vec = {  };
		for (auto f = (newArgs.tailConstBegin()); f != newArgs.tailConstEnd(); f++) {
			index++;
		}
		if (args.size() != index) {
			throw SemanticError("Error: during apply : Error in call to procedure : invalid number of arguments.");
		}
		index = 0;
		for (auto e = (newArgs.tailConstBegin()); e != newArgs.tailConstEnd(); e++) {
			std::string str = (*e).head().asSymbol();
			newEnv.findProc(str, newEnv);
			newEnv.add_exp(Atom(str), args[index]);
			index++;
		}
		return (newExp.tail()->eval(newEnv));
	}
  // head must be a symbol
  if(!op.isSymbol()){
    throw SemanticError("Error during evaluation: procedure name not symbol");
  }
  
  // must map to a proc
  if(!env.is_proc(op)){
    throw SemanticError("Error during evaluation: symbol does not name a procedure");
  }

  // map from symbol to proc
  Procedure proc = env.get_proc(op);
  
  // call proc with args
  return proc(args);
}

// Adds apply functionality for a list 
Expression Expression::handle_apply(Environment & env) {
	// tail must have size 2 or error
	if (m_tail.size() != 2) {
		throw SemanticError("Error during evaluation: invalid number of arguments to apply");
	}
	std::vector<Expression> vec = {};
	Expression exp = m_tail[1].eval(env);

	if (!exp.isHeadList()) {
		throw SemanticError("Error during evaluation: second argument must be a list");
	}
	if (env.is_exp(m_tail[0].head())) {
		for (auto e = (exp.tailConstBegin()); e != exp.tailConstEnd(); e++) {
			vec.push_back(*e);
		}
		return apply(m_tail[0].head(), vec, env);
	}
	int index = 0;
	for (auto e = (m_tail[0].tailConstBegin()); e != m_tail[0].tailConstEnd(); e++) {
		index++;
	}
	if (!env.is_proc(m_tail[0].head()) || index != 0) {

		throw SemanticError("Error during evaluation: first argument must be a procedure");
	}
	for (auto e = (exp.tailConstBegin()); e != exp.tailConstEnd(); e++) {
		vec.push_back(*e);
	}
	return apply(m_tail[0].head(), vec, env);
	
}

// Adds a property functionality
Expression Expression::set_property(Environment & env){
// tail[0] must be string
  std::string key = m_tail[0].head().asString();
  Expression value = m_tail[1].eval(env);
  Expression exp = m_tail[2].eval(env);
  if(!m_tail[0].isHeadString()){
    throw SemanticError("Error during evaluation: first argument to property not a string");
  }
  if(m_tail.size() != 3){
    throw SemanticError("Error during evaluation: invalid number of arguments to property");
  }
  //m_tail[2].eval(env).propertymap[m_tail[0].head().asString()] = m_tail[1].eval(env);
  exp.propertymap[key] = value;
  return exp;
  //return m_tail[2].eval(env);
}

// Adds a property functionality
Expression Expression::get_property(Environment & env){
  std::string key = m_tail[0].head().asString();
  Expression exp = m_tail[1].eval(env);
  if(!m_tail[0].isHeadString()){
    throw SemanticError("Error during evaluation: first argument to property not a string");
  }
  if(m_tail.size() != 2){
    throw SemanticError("Error during evaluation: invalid number of arguments to property");
  }
  return exp.propertymap[key];
  // if(m_tail[0] == Expression()){
  //   return Expression(none);
  // }
}

// Adds map functionality for a list
Expression Expression::handle_map(Environment & env) {
	std::vector<Expression> vec = {};
	std::vector<Expression> vec2 = {};
	Expression exp = m_tail[1].eval(env);

	if (!exp.isHeadList()) {
		throw SemanticError("Error during evaluation: second argument must be a list");
	}
	// tail must have size 3 or error
	if (m_tail.size() != 2) {
		throw SemanticError("Error during evaluation: invalid number of arguments to apply");
	}
	if (env.is_exp(m_tail[0].head())) {
		for (auto e = (exp.tailConstBegin()); e != exp.tailConstEnd(); e++) {
			vec2.push_back(*e);
			Expression newexp = apply(m_tail[0].head(), vec2, env);
			vec2.clear();
			vec.push_back(newexp);
		}
		return Expression(vec);
	}
	int index = 0;
	for (auto e = (m_tail[0].tailConstBegin()); e != m_tail[0].tailConstEnd(); e++) {
		index++;
	}
	if (!env.is_proc(m_tail[0].head()) || index != 0) {
		throw SemanticError("Error during evaluation: first argument must be a procedure");
	}
	for (auto e = (exp.tailConstBegin()); e != exp.tailConstEnd(); e++) {
		vec2.push_back(*e);
		Expression newexp = apply(m_tail[0].head(), vec2, env);
		vec2.clear();
		vec.push_back(newexp);
	}
	return Expression(vec);
}

Expression Expression::handle_lookup(const Atom & head, const Environment & env){
    if(head.isSymbol()){ // if symbol is in env return value
      if(env.is_exp(head)){
	return env.get_exp(head);
      }
      else{
	throw SemanticError("Error during evaluation: unknown symbol");
      }
    }
    else if(head.isComplex()){
      return Expression(head);
    }
    else if(head.isNumber()){
      return Expression(head);
    }
	else if (head.isString()) {
      return Expression(head);
	}
    else{
      throw SemanticError("Error during evaluation: Invalid type in terminal expression");
    }
}

Expression Expression::handle_begin(Environment & env){
  
  if(m_tail.size() == 0){
    throw SemanticError("Error during evaluation: zero arguments to begin");
  }

  // evaluate each arg from tail, return the last
  Expression result;
  for(Expression::IteratorType it = m_tail.begin(); it != m_tail.end(); ++it){
    result = it->eval(env);
  }
  
  return result;
}


Expression Expression::handle_define(Environment & env){

  // tail must have size 3 or error
  if(m_tail.size() != 2){
    throw SemanticError("Error during evaluation: invalid number of arguments to define");
  }
  
  // tail[0] must be symbol
  if(!m_tail[0].isHeadSymbol()){
    throw SemanticError("Error during evaluation: first argument to define not symbol");
  }

  // but tail[0] must not be a special-form or procedure
  std::string s = m_tail[0].head().asSymbol();
  if((s == "define") || (s == "begin") || (s == "e") || (s == "pi") || (s == "I")){
    throw SemanticError("Error during evaluation: attempt to redefine a special-form");
  }
  
  if(env.is_proc(m_head)){
    throw SemanticError("Error during evaluation: attempt to redefine a built-in procedure");
  }
	
  // eval tail[1]
  Expression result = m_tail[1].eval(env);

  //if(env.is_exp(m_head)){
  //  throw SemanticError("Error during evaluation: attempt to redefine a previously defined symbol");
  //}
    
  //and add to env
  env.add_exp(m_tail[0].head(), result);
  
  return result;
}

Expression Expression::handle_lambda(Environment & env) {
	// tail must have size 3 or error
	if (m_tail.size() != 2) {
		throw SemanticError("Error during evaluation: invalid number of arguments to define");
	}

	// tail[0] must be symbol
	if (!m_tail[0].isHeadSymbol() && !m_tail[1].isHeadSymbol()) {
		throw SemanticError("Error during evaluation: first argument to define not symbol");
	}

	// but tail[0] must not be a special-form or procedure
	std::string s = m_tail[0].head().asSymbol();

	if (env.is_proc(m_head) || env.is_proc(m_tail[0].head().asSymbol())) {
		throw SemanticError("Error during evaluation: attempt to redefine a built-in procedure");
	}

	if (env.is_exp(m_head)) {
		throw SemanticError("Error during evaluation: attempt to redefine a previously defined symbol");
	}

	//and add to env
	std::vector<Expression> temp = {  };
	std::vector<Expression> result = {  };
	temp.push_back(Expression(m_tail[0].head()));
	for (auto e = (m_tail[0].tailConstBegin()); e != m_tail[0].tailConstEnd(); e++) {
		temp.push_back(Expression(*e));
	}
	result.push_back(temp);
	result.push_back(m_tail[1]);
	Expression new_result = Expression(result);
	new_result.head().setLambda();
	return new_result;
}

// this is a simple recursive version. the iterative version is more
// difficult with the ast data structure used (no parent pointer).
// this limits the practical depth of our AST
Expression Expression::eval(Environment & env){
  if(m_tail.empty()){
	if (m_head.isSymbol() && m_head.asSymbol() == "list") {
		  return Expression(m_tail);
	}
    return handle_lookup(m_head, env);
  }
  // handle lambda special-form
  if (m_head.isSymbol() && m_head.asSymbol() == "lambda") {
	  return handle_lambda(env);
  }
  // handle map special-form
  if (m_head.isSymbol() && m_head.asSymbol() == "map") {
	  return handle_map(env);
  }
  // handle apply special-form
  if (m_head.isSymbol() && m_head.asSymbol() == "apply") {
	  return handle_apply(env);
  }
  // handle map special-form
  if (m_head.isSymbol() && m_head.asSymbol() == "map") {
	  return handle_map(env);
  }
  // handle begin special-form
  else if(m_head.isSymbol() && m_head.asSymbol() == "begin"){
    return handle_begin(env);
  }
  // handle define special-form
  else if(m_head.isSymbol() && m_head.asSymbol() == "define"){
    return handle_define(env);
  }
  // handle set_property special-form
  else if(m_head.isSymbol() && m_head.asSymbol() == "set-property"){
    return set_property(env);
  }
  // handle get_property special-form
  else if(m_head.isSymbol() && m_head.asSymbol() == "get-property"){
    return get_property(env);
  }
  // else attempt to treat as procedure
  else{ 
    std::vector<Expression> results;
    for(Expression::IteratorType it = m_tail.begin(); it != m_tail.end(); ++it){
      results.push_back(it->eval(env));
    }
    return apply(m_head, results, env);
  }
}


std::ostream & operator<<(std::ostream & out, const Expression & exp){
  Environment env;
  if(!exp.isHeadList() && exp.head().isNone()){
    out << "NONE";
  }
  else{
      if(!exp.isHeadComplex()) out << "(";
    out << exp.head();

    if (exp.isHeadSymbol() && env.is_proc(exp.head())) {
      out << " ";
    }

    for(auto e = exp.tailConstBegin(); e != exp.tailConstEnd(); ++e){
      auto it = e + 1;
      if (it == exp.tailConstEnd()) {
        out << *e;
      }
      else {
        out << *e << " ";
      }
    }

    if (!exp.isHeadComplex()) {
	  out << ")";
    }
  }
  return out;
}

bool Expression::operator==(const Expression & exp) const noexcept{
  
  bool result = (m_head == exp.m_head);

  result = result && (m_tail.size() == exp.m_tail.size());

  if(result){
    for(auto lefte = m_tail.begin(), righte = exp.m_tail.begin();
	(lefte != m_tail.end()) && (righte != exp.m_tail.end());
	++lefte, ++righte){
      result = result && (*lefte == *righte);
    }
  }

  return result;
}

bool operator!=(const Expression & left, const Expression & right) noexcept{

  return !(left == right);
}

std::string Expression::makeString() const noexcept{
    Environment env;
    std::string newString;
    // static bool listStartChecked = false;
    // static bool listEndChecked = false;
    if(!this->isHeadList() && this->head().isNone()){
        newString+="NONE";
    }
    else{
      if(!this->isHeadComplex() && !this->isHeadList()){
          newString+="(";
      }
      newString+=this->head().asString();

      for(auto e = this->tailConstBegin(); e != this->tailConstEnd(); ++e){
        newString+=(*e).makeString();
      }

      if (!this->isHeadComplex() && !this->isHeadList()) {
      newString+=")";
      }

  }
  //std::cout << newString << std::endl;
  return newString;
}

std::vector<Expression> Expression::makeTail() const noexcept{
  std::vector<Expression> vec;
  for(auto e = this->tailConstBegin(); e != this->tailConstEnd(); ++e){
    // std::cout << (*e).head().asString() << std::endl;
    vec.push_back(*e);
  }
  return vec;
}

bool Expression::isPoint() const noexcept{
  bool point = false;
  std::cout << "Inside isPoint Before" << std::endl;
  Expression pointExp(Atom("\"point\""));
  if(propertymap.find("\"object-name\"") != propertymap.end()){
    std::cout << "Inside isPoint Before" << std::endl;
    if(propertymap.at("\"object-name\"") == pointExp){
      point = true;
    }
  }
  return point;
}

bool Expression::isLine() const noexcept{
  bool line = false;
  Expression lineExp(Atom("\"line\""));
  if(propertymap.find("\"object-name\"") != propertymap.end()){
    if(propertymap.at("\"object-name\"") == lineExp){
      line = true;
    }
  }
  return line;
}

bool Expression::isText() const noexcept{
  bool text = false;
  Expression textExp(Atom("\"text\""));
  if(propertymap.find("\"object-name\"") != propertymap.end()){
    if(propertymap.at("\"object-name\"") == textExp){
      text = true;
    }
  }
  return text;
}

double Expression::getSize() const noexcept{
  return propertymap.at("\"size\"").head().asNumber();
}

double Expression::getThickness() const noexcept{
  return propertymap.at("\"thickness\"").head().asNumber();
}

Expression Expression::getPosition() const noexcept{
  return propertymap.at("\"position\"");
}


