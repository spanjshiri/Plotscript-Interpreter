#include "environment.hpp"

#include <cassert>
#include <cmath>

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
  std::complex<double> result (0.0,0.0);
  for( auto & a :args){
    if(a.isHeadNumber()){
      result *= a.head().asNumber();      
    }
    else if(a.isHeadComplex()){
      result *= a.head().asComplex();
    }
    else{
      throw SemanticError("Error in call to mul, argument not a number");
    }
  }
  
  if(result.imag() == 0) {
    double newresult = result.real();
    return Expression(newresult);
  }
  return Expression(result);
};

Expression subneg(const std::vector<Expression> & args){

  std::complex<double> result (0.0,0.0);

  // preconditions
  if(nargs_equal(args,1)){
    if(args[0].isHeadNumber()){
      result = -args[0].head().asNumber();
    }
    else if(args[0].isHeadComplex()){
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
      result = args[0].head().asComplex() - args[1].head().asComplex();
    }
    else if ( (args[0].isHeadComplex()) && (args[1].isHeadNumber()) ){
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

  if(result.imag() == 0) {
    double newresult = result.real();
    return Expression(newresult);
  }
  return Expression(result);
};

Expression div(const std::vector<Expression> & args){

  std::complex<double> result (0.0,0.0); 

  if(nargs_equal(args,2)){
    if( (args[0].isHeadNumber()) && (args[1].isHeadNumber()) ){
      result = args[0].head().asNumber() / args[1].head().asNumber();
    }
    else{      
      throw SemanticError("Error in call to division: invalid argument.");
    }
  }
  else{
    throw SemanticError("Error in call to division: invalid number of arguments.");
  }
  if(result.imag() == 0) {
    double newresult = result.real();
    return Expression(newresult);
  }
  return Expression(result);
};

Expression sqrt(const std::vector<Expression> & args){
  double result = 0;

  if(nargs_equal(args,1) && args[0].head().asNumber() >= 0){
    if(args[0].isHeadNumber()){
      result = std::sqrt(args[0].head().asNumber());
    }
  }
  else{
    throw SemanticError("Error in call to square root: argument cannot be negative.");
  }
  return Expression(result);
}

Expression pow(const std::vector<Expression> & args){
  double result = 0;
  
  if(nargs_equal(args,2)){
    if( (args[0].isHeadNumber()) && (args[1].isHeadNumber()) ){
      result = std::pow(args[0].head().asNumber(),args[1].head().asNumber());
    }
    else{      
      throw SemanticError("Error in call to power: invalid argument.");
    }
  }
  else{
    throw SemanticError("Error in call to power: invalid number of arguments.");
  }
  return Expression(result);
}

Expression ln(const std::vector<Expression> & args){
  double result = 0;
  
  if(nargs_equal(args,1)){
    if( args[0].head().asNumber() > 0){
      if( (args[0].isHeadNumber()) ){
        result = std::log(args[0].head().asNumber());
      }
      else{      
        throw SemanticError("Error in call to ln: invalid argument.");
      }
    }
    else{
      throw SemanticError("Error in call to ln: negative numbers are not allowed.");
    }
  }
  else{
    throw SemanticError("Error in call to ln: invalid number of arguments.");
  }
  return Expression(result);
}

Expression sin(const std::vector<Expression> & args){
  double result = 0;
  
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
}

Expression cos(const std::vector<Expression> & args){
  double result = 0;
  
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
}

Expression tan(const std::vector<Expression> & args){
  double result = 0;
  
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
}

// Returns real part of complex as a number, error if argument not complex
Expression real(const std::vector<Expression> & args){
  if( (args[0].isHeadComplex()) ){
    std::complex<double> result (args[0].head().asComplex());
    double newresult = result.real();
    return Expression(newresult);
  }
  else{
    throw SemanticError("Error in call to real: invalid argument");
  }
}

// Returns imag part of complex as a number, error if argument not complex
Expression imag(const std::vector<Expression> & args){
  if( (args[0].isHeadComplex()) ){
    std::complex<double> result (args[0].head().asComplex());
    double newresult = result.imag();
    return Expression(newresult);
  }
  else{
    throw SemanticError("Error in call to real: invalid argument");
  }
}

// Returns magnitude(absolute value) of complex as a number, error if argument not complex
Expression mag(const std::vector<Expression> & args){
  if( (args[0].isHeadComplex()) ){
    std::complex<double> result (args[0].head().asComplex());
    double newresult = std::abs(result);
    return Expression(newresult);
  }
  else{
    throw SemanticError("Error in call to real: invalid argument");
  }
}

// Returns arg(phase angle) of complex as a number, error if argument not complex
Expression arg(const std::vector<Expression> & args){
  if( (args[0].isHeadComplex()) ){
    std::complex<double> result (args[0].head().asComplex());
    double newresult = std::arg(result);
    return Expression(newresult);
  }
  else{
    throw SemanticError("Error in call to real: invalid argument");
  }
}

// Returns conjugate of complex as a number, error if argument not complex
Expression conj(const std::vector<Expression> & args){
  if( (args[0].isHeadComplex()) ){
    std::complex<double> result (args[0].head().asComplex());
    result = std::conj(result);
    return Expression(result);
  }
  else{
    throw SemanticError("Error in call to real: invalid argument");
  }
}

const double PI = std::atan2(0, -1);
const double EXP = std::exp(1);
const std::complex<double> I (0.0,1.0);

Environment::Environment(){

  reset();
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
    throw SemanticError("Attempt to overwrite symbol in environemnt");
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
}