#include "environment.hpp"

#include <cassert>
#include <cmath>
#include <iostream>
#include <string>
#include <map>

#include "environment.hpp"
#include "semantic_error.hpp"

/*********************************************************************** 
Helper Functions
**********************************************************************/

// predicate, the number of args is nargs
bool nargs_equal(const std::vector<Expression> & args, unsigned nargs){
  return args.size() == nargs;
}

/*********************************************************************** 
Each of the functions below have the signature that corresponds to the
typedef'd Procedure function pointer.
**********************************************************************/

// the default procedure always returns an expresison of type None
Expression default_proc(const std::vector<Expression> & args){
  args.size(); // make compiler happy we used this parameter
  return Expression();
};

Expression add(const std::vector<Expression> & args){

  // check all aruments are numbers or complex, while adding
  std::complex<double> result (0.0,0.0);
  for( auto & a :args){
    if(a.isHeadNumber()){
      result += a.head().asNumber();      
    }
    else if(a.isHeadComplex()){
      result += a.head().asComplex();
    }
    else{
      throw SemanticError("Error in call to add, argument not a number");
    }
  }

  if(result.imag() == 0) {
    double newresult = result.real();
    return Expression(newresult);
  }
  return Expression(result);
};

Expression mul(const std::vector<Expression> & args){
 
  // check all aruments are numbers or complex, while multiplying
  bool complexArgs = false;
  std::complex<double> result (1.0,0.0);
  for( auto & a :args){
    if(a.isHeadNumber()){
      result *= a.head().asNumber();      
    }
    else if(a.isHeadComplex()){
      complexArgs = true;
      result *= a.head().asComplex();
    }
    else{
      throw SemanticError("Error in call to mul, argument not a number");
    }
  }
  
  if(result.imag() == 0 && complexArgs == false) {
    double newresult = result.real();
    return Expression(newresult);
  }
  return Expression(result);
};

Expression subneg(const std::vector<Expression> & args){

  std::complex<double> result (0.0,0.0);
  bool complexArgs = false;
  // preconditions
  if(nargs_equal(args,1)){
    if(args[0].isHeadNumber()){
      result = -args[0].head().asNumber();
    }
    else if(args[0].isHeadComplex()){
      complexArgs = true;
      result = -args[0].head().asComplex();
    }
    else{
      throw SemanticError("Error in call to negate: invalid argument.");
    }
  }
  // if there are 2 arguments proceed with subtraction only if the 2 args are either number or complex
  else if(nargs_equal(args,2)){
    if( (args[0].isHeadNumber()) && (args[1].isHeadNumber()) ){
      result = args[0].head().asNumber() - args[1].head().asNumber();
    }
    else if( (args[0].isHeadComplex()) && (args[1].isHeadComplex()) ){
      complexArgs = true;
      result = args[0].head().asComplex() - args[1].head().asComplex();
    }
    else if ( (args[0].isHeadComplex()) && (args[1].isHeadNumber()) ){
      complexArgs = true;
      result = args[0].head().asComplex() - args[1].head().asNumber();
    }
    else if ( (args[0].isHeadNumber()) && (args[1].isHeadComplex()) ){
      result = args[0].head().asNumber() - args[1].head().asComplex();
    }
    else{      
      throw SemanticError("Error in call to subtraction: invalid argument.");
    }
  }
  else{
    throw SemanticError("Error in call to subtraction or negation: invalid number of arguments.");
  }
  // Returns number if no complex
  if(result.imag() == 0 && complexArgs == false) {
    double newresult = result.real();
    return Expression(newresult);
  }
  return Expression(result);
};


Expression div(const std::vector<Expression> & args){

  std::complex<double> result (1.0,0.0); 
  bool complexArgs = false;
  if (nargs_equal(args, 1)) {
	  if ((args[0].isHeadComplex())) {
		  complexArgs = true;
		  result /= args[0].head().asComplex();
	  }
	  else if (args[0].isHeadNumber()) {
		  result = 1 / args[0].head().asNumber();
	  }
	  else {
		  throw SemanticError("Error in call to division: invalid argument.");
	  }
  }
  // check all arguments are number or complex while dividing
  else if(nargs_equal(args, 2)) {
	  result = std::pow(args[0].head().asComplex(),2);
	  for (auto & a : args) {
		  if (a.isHeadNumber()) {
			  result /= a.head().asNumber();
		  }
		  else if (a.isHeadComplex()) {
			  complexArgs = true;
			  result /= a.head().asComplex();
		  }
		  else {
			  throw SemanticError("Error in call to division: invalid argument.");
		  }
	  }
  }
  else {
	  throw SemanticError("Error in call to division: invalid number of arguments.");
  }
  // Returns number if no complex
  if(result.imag() == 0 && complexArgs == false) {
    double newresult = result.real();
    return Expression(newresult);
  }
  return Expression(result);
};

Expression sqrt(const std::vector<Expression> & args){
  std::complex<double> result (0.0,0.0);
  // check if argument is number, negative number, or complex when multiplying
  if(nargs_equal(args,1)){
    if(args[0].isHeadNumber() && args[0].head().asNumber() >= 0){
      result = std::sqrt(args[0].head().asNumber());
    }
    else if(args[0].isHeadNumber() && args[0].head().asNumber() < 0){
      std::complex<double> negativeNumber (args[0].head().asNumber(),0.0);
      result = std::sqrt(negativeNumber);
    }
    else if(args[0].isHeadComplex()){
      result = std::sqrt(args[0].head().asComplex());
    }
  }
  else{
    throw SemanticError("Error in call to square root: argument cannot be negative.");
  }
  // Returns number if no complex
  if(result.imag() == 0) {
    double newresult = result.real();
    return Expression(newresult);
  }
  return Expression(result);
};

Expression pow(const std::vector<Expression> & args){
  std::complex<double> result (0.0,0.0);
  bool complexArgs = false;
  // check if number or complex when exponentiating
  if(nargs_equal(args,2)){
    if( (args[0].isHeadNumber()) && (args[1].isHeadNumber()) ){
      result = std::pow(args[0].head().asNumber(),args[1].head().asNumber());
    }
    else if( (args[0].isHeadNumber()) && (args[1].isHeadComplex()) ){
      complexArgs = true;
      result = std::pow(args[0].head().asNumber(),args[1].head().asComplex());
    }
    else if( (args[0].isHeadComplex()) && (args[1].isHeadNumber()) ){
      complexArgs = true;
      result = std::pow(args[0].head().asComplex(),args[1].head().asNumber());
    }
    else if( (args[0].isHeadComplex()) && (args[1].isHeadComplex()) ){
      complexArgs = true;
      result = std::pow(args[0].head().asComplex(),args[1].head().asComplex());
    }
    else{      
      throw SemanticError("Error in call to power: invalid argument.");
    }
  }
  else{
    throw SemanticError("Error in call to power: invalid number of arguments.");
  }
  // Returns number if no complex
  if(result.imag() == 0 && complexArgs == false) {
    double newresult = result.real();
    return Expression(newresult);
  }
  return Expression(result);
};

Expression ln(const std::vector<Expression> & args){
  double result = 0;
  // check if argument is a positive number when doing ln
  if(nargs_equal(args,1)){
    if( args[0].head().asNumber() > 0){
      if( (args[0].isHeadNumber()) ){
        result = std::log(args[0].head().asNumber());
      }
      /*else{      
        throw SemanticError("Error in call to ln: invalid argument.");
      }*/
    }
    else{
      throw SemanticError("Error in call to ln: negative numbers are not allowed.");
    }
  }
  else{
    throw SemanticError("Error in call to ln: invalid number of arguments.");
  }
  return Expression(result);
};

Expression sin(const std::vector<Expression> & args){
  double result = 0;
  // check if argument is a number when doing sin
  if(nargs_equal(args,1)){
      if( (args[0].isHeadNumber()) ){
        result = std::sin(args[0].head().asNumber());
      }
      else{      
        throw SemanticError("Error in call to sin: invalid argument.");
      }
  }
  else{
    throw SemanticError("Error in call to sin: invalid number of arguments.");
  }
  return Expression(result);
};

Expression cos(const std::vector<Expression> & args){
  double result = 0;
  // check if argument is a number when doing cos
  if(nargs_equal(args,1)){
      if( (args[0].isHeadNumber()) ){
        result = std::cos(args[0].head().asNumber());
      }
      else{      
        throw SemanticError("Error in call to cos: invalid argument.");
      }
  }
  else{
    throw SemanticError("Error in call to cos: invalid number of arguments.");
  }
  return Expression(result);
};

Expression tan(const std::vector<Expression> & args){
  double result = 0;
  // check if argument is a number when doing tan
  if(nargs_equal(args,1)){
      if( (args[0].isHeadNumber()) ){
        result = std::tan(args[0].head().asNumber());
      }
      else{      
        throw SemanticError("Error in call to tan: invalid argument.");
      }
  }
  else{
    throw SemanticError("Error in call to tan: invalid number of arguments.");
  }
  return Expression(result);
};

// Returns real part of complex as a number, error if argument not complex
Expression real(const std::vector<Expression> & args){
  if(nargs_equal(args,1)){
    if( (args[0].isHeadComplex()) ){
      double result = args[0].head().asComplex().real();
      return Expression(result);
    }
    else{
      throw SemanticError("Error in call to real: invalid argument");
    }
  }
  else{
    throw SemanticError("Error in call to real: invalid number of arguments.");
  }
};

// Returns imag part of complex as a number, error if argument not complex
Expression imag(const std::vector<Expression> & args){
  if(nargs_equal(args,1)){
    if( (args[0].isHeadComplex()) ){
      double result = args[0].head().asComplex().imag();
      return Expression(result);
    }
    else{
      throw SemanticError("Error in call to imag: invalid argument");
    }
  }
  else{
    throw SemanticError("Error in call to imag: invalid number of arguments.");
  }
};

// Returns magnitude(absolute value) of complex as a number, error if argument not complex
Expression mag(const std::vector<Expression> & args){
  if(nargs_equal(args,1)){
    if( (args[0].isHeadComplex()) ){
      double result = std::abs(args[0].head().asComplex());
      return Expression(result);
    }
    else{
      throw SemanticError("Error in call to mag: invalid argument");
    }
  }
  else{
    throw SemanticError("Error in call to mag: invalid number of arguments.");
  }
};

// Returns arg(phase angle) of complex as a number, error if argument not complex
Expression arg(const std::vector<Expression> & args){
  if(nargs_equal(args,1)){
    if( (args[0].isHeadComplex()) ){
      double result = std::arg(args[0].head().asComplex());
      return Expression(result);
    }
    else{
      throw SemanticError("Error in call to arg: invalid argument");
    }
  }
  else{
    throw SemanticError("Error in call to arg: invalid number of arguments.");
  }
};

// Returns conjugate of complex as a number, error if argument not complex
Expression conj(const std::vector<Expression> & args){
  if(nargs_equal(args,1)){
    if( (args[0].isHeadComplex()) ){
      std::complex<double> result (args[0].head().asComplex());
      result = std::conj(result);
      return Expression(result);
    }
    else{
      throw SemanticError("Error in call to conj: invalid argument");
    }
  }
  else{
    throw SemanticError("Error in call to conj: invalid number of arguments.");
  }
};

// Returns list as a vector of expressions
Expression list(const std::vector<Expression> & args) {
	std::vector<Expression> result = args;
	return Expression(result);
};

// Returns the first entry in a list
Expression first(const std::vector<Expression> & args) {
	std::vector<Expression> result = args;
	if (nargs_equal(args, 1) && (result[0].isHeadNumber() || result[0].isHeadComplex() || result[0].isHeadSymbol())) {
		throw SemanticError("Error in call to first: argument is not a list.");
	}
	if (nargs_equal(args, 1)) {
		if (args[0].tailConstBegin() == args[0].tailConstEnd()) {
			throw SemanticError("Error in call to first: arugment to empty list.");
		}
		else {
			return Expression(*result[0].tailConstBegin());
		}
	}
	else {
		throw SemanticError("Error in call to first: invalid number of arguments.");
	}
	
}

// Returns all entries after the first in the list
Expression rest(const std::vector<Expression> & args) {
	std::vector<Expression> result;
	if (nargs_equal(args, 1) && (!args[0].isHeadList())) {
		throw SemanticError("Error in call to rest: argument is not a list.");
	}
	if (nargs_equal(args, 1) ) {
		if (args[0].tailConstBegin() == args[0].tailConstEnd()) {
			throw SemanticError("Error in call to rest: arugment to empty list.");
		}
		else {
			for (auto e = (args[0].tailConstBegin() + 1); e != args[0].tailConstEnd(); e++) {
				result.push_back(Expression(*e));
			}
		}
	}
	else {
		throw SemanticError("Error in call to rest: invalid number of arguments.");
	}
	return Expression(result);
}

// Returns the length of the list
Expression length(const std::vector<Expression> & args) {
	std::vector<Expression> result;
	int count = 0;
	if (nargs_equal(args, 1) && (args[0].isHeadList())) {
		for (auto e = (args[0].tailConstBegin()); e != args[0].tailConstEnd(); e++) {
			count++;
		}
	}
	else {
		throw SemanticError("Error in call to length: argument is not a list.");
	}
	return Expression(count);
}

// Adds an expression to the end of the list
Expression append(const std::vector<Expression> & args) {
	std::vector<Expression> result;
	if ((nargs_equal(args, 2)) && (args[0].isHeadList()) && (!args[1].isHeadList())) {
		for (auto e = (args[0].tailConstBegin()); e != args[0].tailConstEnd(); e++) {
			result.push_back(Expression(*e));
		}
		result.push_back(Expression(args[1]));
	}
	else if ((!nargs_equal(args, 2))) {
		throw SemanticError("Error in call to append: invalid number of arguments, must be binary.");
	}
	else {
		throw SemanticError("Error in call to append: first argument is not a list.");
	}
	return Expression(result);
}

// Joins two lists together
Expression join(const std::vector<Expression> & args) {
	std::vector<Expression> result;
	if ((nargs_equal(args, 2)) && (args[0].isHeadList()) && (args[1].isHeadList())) {
		for (auto e = (args[0].tailConstBegin()); e != args[0].tailConstEnd(); e++) {
			result.push_back(Expression(*e));
		}
		for (auto f = (args[1].tailConstBegin()); f != args[1].tailConstEnd(); f++) {
			result.push_back(Expression(*f));
		}
	}
	else if ((!nargs_equal(args, 2))) {
		throw SemanticError("Error in call to join: invalid number of arguments, must be binary.");
	}
	else {
		throw SemanticError("Error in call to join: first argument is not a list.");
	}
	return Expression(result);
}

// Creates a list with passed parameter of start,end, and increment
Expression range(const std::vector<Expression> & args) {
	std::vector<Expression> result;
	if ((nargs_equal(args, 3)) && (args[0].isHeadNumber()) && (args[1].isHeadNumber()) && (args[2].isHeadNumber())) {
		if ((args[0].head().asNumber()) >= (args[1].head().asNumber())) {
			throw SemanticError("Error in call to range: first argument must be less then second argument.");
		}
		if ((args[2].head().asNumber()) <= 0) {
			throw SemanticError("Error in call to range: third argument must be strictly positive.");
		}
		for (double i = args[0].head().asNumber(); i <= args[1].head().asNumber(); i = i + args[2].head().asNumber()) {
			result.push_back(Expression(i));
		}
	}
	else if ((!nargs_equal(args, 3))) {
		throw SemanticError("Error in call to range: invalid number of arguments, must be ternary.");
	}
	else if ((!args[0].isHeadNumber()) || (!args[1].isHeadNumber()) || (!args[2].isHeadNumber())) {
		throw SemanticError("Error in call to range: all arguments must be numbers.");
	}
	return Expression(result);
}

// A Helper function used in environment to help solve shadowing when using lambda
void Environment::findProc(const std::string & str, Environment & env) {
	if (env.envmap.find(str) != env.envmap.end()) {
		env.envmap.erase(str);
	}
}

const double PI = std::atan2(0, -1);
const double EXP = std::exp(1);
const std::complex<double> I (0.0,1.0);

Environment::Environment(){
  reset();
}

//copy construtor for Environment
Environment::Environment(const Environment & a) {
	envmap = a.envmap;
}

Environment & Environment::operator=(const Environment & a) {
	// prevent self-assignment
	if (this != &a) {
		envmap = a.envmap;
	}

	return *this;
}

bool Environment::is_known(const Atom & sym) const{
  if(!sym.isSymbol()) return false;
  
  return envmap.find(sym.asSymbol()) != envmap.end();
}

bool Environment::is_exp(const Atom & sym) const{
  if(!sym.isSymbol()) return false;
  
  auto result = envmap.find(sym.asSymbol());
  return (result != envmap.end()) && (result->second.type == ExpressionType);
}

Expression Environment::get_exp(const Atom & sym) const{

  Expression exp;
  
  if(sym.isSymbol()){
    auto result = envmap.find(sym.asSymbol());
    if((result != envmap.end()) && (result->second.type == ExpressionType)){
      exp = result->second.exp;
    }
  }

  return exp;
}

void Environment::add_exp(const Atom & sym, const Expression & exp){

  if(!sym.isSymbol()){
    throw SemanticError("Attempt to add non-symbol to environment");
  }
    
  // error if overwriting symbol map
  if(envmap.find(sym.asSymbol()) != envmap.end()){
    throw SemanticError("Attempt to overwrite symbol in environment");
  }

  envmap.emplace(sym.asSymbol(), EnvResult(ExpressionType, exp)); 
}

bool Environment::is_proc(const Atom & sym) const{
  if(!sym.isSymbol()) return false;
  
  auto result = envmap.find(sym.asSymbol());
  return (result != envmap.end()) && (result->second.type == ProcedureType);
}

Procedure Environment::get_proc(const Atom & sym) const{

  //Procedure proc = default_proc;

  if(sym.isSymbol()){
    auto result = envmap.find(sym.asSymbol());
    if((result != envmap.end()) && (result->second.type == ProcedureType)){
      return result->second.proc;
    }
  }

  return default_proc;
}

/*
Reset the environment to the default state. First remove all entries and
then re-add the default ones.
 */
void Environment::reset(){

  envmap.clear();

  // Built-In value of I
  envmap.emplace("I", EnvResult(ExpressionType, Expression(I)));

  // Built-In value of e
  envmap.emplace("e", EnvResult(ExpressionType, Expression(EXP)));
  
  // Built-In value of pi
  envmap.emplace("pi", EnvResult(ExpressionType, Expression(PI)));

  // Procedure: add;
  envmap.emplace("+", EnvResult(ProcedureType, add)); 

  // Procedure: subneg;
  envmap.emplace("-", EnvResult(ProcedureType, subneg)); 

  // Procedure: mul;
  envmap.emplace("*", EnvResult(ProcedureType, mul)); 

  // Procedure: div;
  envmap.emplace("/", EnvResult(ProcedureType, div)); 

  // Procedure: sqrt;
  envmap.emplace("sqrt", EnvResult(ProcedureType, sqrt));

  // Procedure: pow;
  envmap.emplace("^", EnvResult(ProcedureType, pow));

  // Procedure: ln;
  envmap.emplace("ln", EnvResult(ProcedureType, ln));

  // Procedure: sin;
  envmap.emplace("sin", EnvResult(ProcedureType, sin));

  // Procedure: cos;
  envmap.emplace("cos", EnvResult(ProcedureType, cos));

  // Procedure: tan;
  envmap.emplace("tan", EnvResult(ProcedureType, tan));

  // Procedure: real;
  envmap.emplace("real", EnvResult(ProcedureType, real));

  // Procedure: imag;
  envmap.emplace("imag", EnvResult(ProcedureType, imag));

  // Procedure: mag;
  envmap.emplace("mag", EnvResult(ProcedureType, mag));

  // Procedure: arg;
  envmap.emplace("arg", EnvResult(ProcedureType, arg));

  // Procedure: imag;
  envmap.emplace("conj", EnvResult(ProcedureType, conj));

  // Procedure: list;
  envmap.emplace("list", EnvResult(ProcedureType, list));

  // Procedure: first;
  envmap.emplace("first", EnvResult(ProcedureType, first));

  // Procedure: rest;
  envmap.emplace("rest", EnvResult(ProcedureType, rest));

  // Procedure: length;
  envmap.emplace("length", EnvResult(ProcedureType, length));

  // Procedure: append;
  envmap.emplace("append", EnvResult(ProcedureType, append));

  // Procedure: join;
  envmap.emplace("join", EnvResult(ProcedureType, join));

  // Procedure: range;
  envmap.emplace("range", EnvResult(ProcedureType, range));
}