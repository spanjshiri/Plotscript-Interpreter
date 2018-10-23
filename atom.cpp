#include "atom.hpp"

#include <sstream>
#include <cctype>
#include <cmath>
#include <limits>

Atom::Atom(): m_type(NoneKind) {}

Atom::Atom(double value){

  setNumber(value);
}

Atom::Atom(const Token & token): Atom(){
  
  // is token a number?
  double temp;
  std::istringstream iss(token.asString());
  if(iss >> temp){
    // check for trailing characters if >> succeeds
    if(iss.rdbuf()->in_avail() == 0){
      setNumber(temp);
    }
  }
  else{ // else assume symbol
    // make sure does not start with number
    if(!std::isdigit(token.asString()[0])){
		if (token.asString()[0] == '"') {
			setString(token.asString());
		}
		else {
			setSymbol(token.asString());
		}
    }
  }
}

Atom::Atom(const std::string & value): Atom() {
	if (value[0] == '"') {
		setString(value);
  }
	else {
		setSymbol(value);
	}
}

Atom::Atom(std::complex<double> value) {
  setComplex(value);
}

Atom::Atom(const Atom & x): Atom(){
  if(x.isNumber()){
    setNumber(x.numberValue);
  }
  else if(x.isSymbol()){
    setSymbol(x.stringValue);
  }
  else if(x.isComplex()){
    setComplex(x.complexValue);
  }
  else if (x.isString()) {
	setString(x.stringValue);
  }
}

Atom & Atom::operator=(const Atom & x){

  if(this != &x){
    if(x.m_type == NoneKind){
      m_type = NoneKind;
    }
    else if(x.m_type == NumberKind){
      setNumber(x.numberValue);
    }
    else if(x.m_type == SymbolKind){
      setSymbol(x.stringValue);
    }
    else if(x.m_type == ComplexKind){
      setComplex(x.complexValue);
    }
	else if (x.m_type == ListKind) {
		setList();
	}
	else if (x.m_type == StringKind) {
		setString(x.stringValue);
	}
  }
  return *this;
}
  
Atom::~Atom(){

  // we need to ensure the destructor of the symbol string is called
  if(m_type == SymbolKind || m_type == StringKind){
    stringValue.~basic_string();
  }
}

bool Atom::isNone() const noexcept{
  return m_type == NoneKind;
}

bool Atom::isNumber() const noexcept{
  return m_type == NumberKind;
}

bool Atom::isSymbol() const noexcept{
  return m_type == SymbolKind;
}  

bool Atom::isComplex() const noexcept{
  return m_type == ComplexKind;
}

bool Atom::isList() const noexcept {
	return m_type == ListKind;
}

bool Atom::isString() const noexcept {
	return m_type == StringKind;
}


void Atom::setNumber(double value){

  m_type = NumberKind;
  numberValue = value;
}

void Atom::setSymbol(const std::string & value){

  // we need to ensure the destructor of the symbol string is called
  if(m_type == SymbolKind){
    stringValue.~basic_string();
  }
    
  m_type = SymbolKind;

  // copy construct in place
  new (&stringValue) std::string(value);
}

void Atom::setComplex(const std::complex<double> value){
  m_type = ComplexKind;
  complexValue = value;
}

void Atom::setList() {
	m_type = ListKind;
}

void Atom::setLambda() {
	m_type = LambdaKind;
}

void Atom::setString(const std::string & value) {
	// we need to ensure the destructor of the symbol string is called
	if (m_type == StringKind) {
		stringValue.~basic_string();
	}

	m_type = StringKind;

	// copy construct in place
	new (&stringValue) std::string(value);
}

double Atom::asNumber() const noexcept{
  // Convert a complex to a number if the current type is complex
  if(m_type == ComplexKind){
    double comtoNumber = complexValue.real();
    return comtoNumber;
  }
  return (m_type == NumberKind) ? numberValue : 0.0;  
}


std::string Atom::asSymbol() const noexcept{

  std::string result;

  if(m_type == SymbolKind){
    result = stringValue;
  }

  return result;
}

std::string Atom::asString() const noexcept {
	std::string result;
  std::ostringstream oss;

	if (m_type == StringKind) {
		oss << stringValue;
	}
  else if(m_type == NumberKind){
    oss << numberValue;
  }
  else if(m_type == ComplexKind){
    oss << complexValue;
  }
  result = oss.str();
	return result;
}

std::complex<double> Atom::asComplex() const noexcept{
  // Convert a number to a complex if the current type is number
  if(m_type == NumberKind){
    std::complex<double> numToComplex (numberValue,0.0);
    return numToComplex;
  }
  return (m_type == ComplexKind) ? complexValue : (0.0);
}

bool Atom::operator==(const Atom & right) const noexcept{
  
  if(m_type != right.m_type) return false;

  switch(m_type){
  case NoneKind:
    if(right.m_type != NoneKind) return false;
    break;
  case NumberKind:
    {
      if(right.m_type != NumberKind) return false;
      double dleft = numberValue;
      double dright = right.numberValue;
      double diff = fabs(dleft - dright);
      if(std::isnan(diff) ||
	 (diff > std::numeric_limits<double>::epsilon()*2.0)) return false;
    }
    break;
  case SymbolKind:
    {
      if(right.m_type != SymbolKind) return false;

      return stringValue == right.stringValue;
    }
    break;
  case ComplexKind:
  {
    if(right.m_type != ComplexKind) return false;
	std::complex<double> dleft = complexValue;
	std::complex<double> dright = right.complexValue;
	std::complex<double> diff = (dleft - dright);
	if (std::isnan(diff.real()) || std::isnan(diff.imag()) || 
    (diff.real() > std::numeric_limits<double>::epsilon()*2.0) || (diff.imag() > std::numeric_limits<double>::epsilon()*2.0)) return false;
  }
  break;
  case ListKind:
  {
	  if (right.m_type != ListKind) return false;
  }
  break;
  case StringKind:
  {
	  if (right.m_type != StringKind) return false;

	  return stringValue == right.stringValue;
  }
  break;
  default:
    return false;
  }

  return true;
}

bool operator!=(const Atom & left, const Atom & right) noexcept{
  
  return !(left == right);
}


std::ostream & operator<<(std::ostream & out, const Atom & a){

  if(a.isNumber()){
    out << a.asNumber();
  }
  if(a.isSymbol()){
    out << a.asSymbol();
  }
  if(a.isComplex()){
    out << a.asComplex();
  }
  if (a.isString()) {
	out << a.asString();
  }
  return out;
}
