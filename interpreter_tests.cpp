#include "catch.hpp"

#include <string>
#include <sstream>
#include <fstream>
#include <iostream>

#include "semantic_error.hpp"
#include "interpreter.hpp"
#include "expression.hpp"

Expression run(const std::string & program){
  
  std::istringstream iss(program);
    
  Interpreter interp;
    
  bool ok = interp.parseStream(iss);
  if(!ok){
    std::cerr << "Failed to parse: " << program << std::endl; 
  }
  REQUIRE(ok == true);

  Expression result;
  REQUIRE_NOTHROW(result = interp.evaluate());

  return result;
}

TEST_CASE( "Test Interpreter parser with expected input", "[interpreter]" ) {

  std::string program = "(begin (define r 10) (* pi (* r r)))";

  std::istringstream iss(program);
 
  Interpreter interp;

  bool ok = interp.parseStream(iss);

  REQUIRE(ok == true);
}

TEST_CASE("Test Interpreter parser with handle_lambda", "[interpreter]") {

	std::string program = "(begin (define func (lambda (x y) (* x y))) (func 2 5))";

	std::istringstream iss(program);

	Interpreter interp;

	bool ok = interp.parseStream(iss);

	REQUIRE(ok == true);
	REQUIRE(interp.evaluate() == Expression(10));
}
TEST_CASE("Test Interpreter parser to lambda should return invalid number of arguments", "[interpreter]") {

	INFO("Testing apply with lambda")
	std::string program = "(lambda (x y) (+ x y) (+ x y))";
	std::istringstream iss(program);
	Interpreter interp;
  bool ok = interp.parseStream(iss);
  REQUIRE(ok == true);
	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Test Interpreter parser with apply for lambda", "[interpreter]") {

	INFO("Testing apply with lambda")
	std::string program = "(begin (define complexAsList (lambda (x) (list (real x) (imag x)))) (apply complexAsList (list (+ 1 (* 3 I)))))";
	Expression value = run(program);
	std::vector<Expression> args = { Expression(1),Expression(3) };
	REQUIRE(value == Expression(args));
}

TEST_CASE("test Interpreter parser to apply with simple case", "[interpreter]") {
	std::string program = "(apply + (list 1 2 3 4))";
	Expression value = run(program);
	REQUIRE(value == Expression(10));
}

TEST_CASE("Test Interpreter parser to throw semantic error for apply", "[interpreter]") {

	INFO("Should return invalid number of arguments in apply")
	std::string program = "(begin (define linear (lambda (a b x) (+ (* a x) b))) (apply linear (3 4 5)))";
	std::istringstream iss(program);
	Interpreter interp;
  bool ok = interp.parseStream(iss);
  REQUIRE(ok == true);
	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Test Interpreter parser to apply should return invalid number of arguments", "[interpreter]") {

	INFO("Should return invalid number of arguments in apply")
	std::string program = "(apply / (list 1 2 4))";
	std::istringstream iss(program);
	Interpreter interp;
  bool ok = interp.parseStream(iss);
  REQUIRE(ok == true);
	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Test Interpreter parser to apply should return first argument is not a procedure", "[interpreter]") {
	INFO("Should return invalid number of arguments in apply")
	std::string program = "(apply (+ z I) (list 0))";
	std::istringstream iss(program);
	Interpreter interp;
  bool ok = interp.parseStream(iss);
  REQUIRE(ok == true);
	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Test Interpreter parser to lambda first argument to define not a symbol", "[interpreter]") {
	INFO("Should return first argument to define not a symbol")
	std::string program = "(lambda 1 1)";
	std::istringstream iss(program);
	Interpreter interp;
  bool ok = interp.parseStream(iss);
  REQUIRE(ok == true);
	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Test Interpreter parser to lambda attempt to redefine a built in procedure", "[interpreter]") {
	INFO("Should return attempt to redefine a built in procedure")
	std::string program = "(lambda + 1)";
	std::istringstream iss(program);
	Interpreter interp;
  bool ok = interp.parseStream(iss);
  REQUIRE(ok == true);
	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("test Interpreter parser to apply with lambda case", "[interpreter]") {
	std::string program = "(begin (define linear (lambda (a b x) (+ (* a x) b))) (apply linear (list 3 4 5)))";
	Expression value = run(program);
	REQUIRE(value == Expression(19));
}

TEST_CASE("test Interpreter parser to map with simple case", "[interpreter]") {
	std::string program = "(map / (list 1 2 4))";
	Expression value = run(program);
	std::vector<Expression> args = { Expression(1),Expression(0.5),Expression(0.25) };
	REQUIRE(value == Expression(args));
}

TEST_CASE("test Interpreter parser to map with lambda case", "[interpreter]") {
	std::string program = "(begin (define f (lambda (x) (sin x))) (map f (list (- pi) (/ (- pi) 2) 0 (/ pi 2) pi)))";
	Expression value = run(program);
	std::vector<Expression> args = { Expression(-1.22465e-16),Expression(-1),Expression(0),Expression(1),Expression(1.22465e-16) };
	REQUIRE(value == Expression(args));
}

TEST_CASE("Test Interpreter parser to map should return second argument to map not a list", "[interpreter]") {
	std::string program = "(map + 3)";
	std::istringstream iss(program);
	Interpreter interp;
  bool ok = interp.parseStream(iss);
  REQUIRE(ok == true);
	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Test Interpreter parser to map should return first argument to map not a procedure", "[interpreter]") {
	std::string program = "(map 3 (list 1 2 3))";
	std::istringstream iss(program);
	Interpreter interp;
  bool ok = interp.parseStream(iss);
  REQUIRE(ok == true);
	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Test Interpreter parser to map should return invalid arguments in apply", "[interpreter]") {

	INFO("Should return invalid number of arguments in apply")
		std::string program = "(begin (define addtwo (lambda (x y) (+ x y))) (map addtwo (list 1 2 3)))";
	std::istringstream iss(program);
	Interpreter interp;
  bool ok = interp.parseStream(iss);
  REQUIRE(ok == true);
	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}
TEST_CASE("Test Interpreter parser to map to return if not a procedure", "[intercepter]"){
	INFO("Should return error as it is not a procedure")
		std::string program1 = "(begin (apply + 3))";
	std::istringstream iss1(program1);
	Interpreter interp1;
  bool ok1 = interp1.parseStream(iss1);
  REQUIRE(ok1 == true);
	REQUIRE_THROWS_AS(interp1.evaluate(), SemanticError);
}

TEST_CASE("test Interpreter parser for set-property", "[interpreter]") {
	std::string program = "(set-property \"number\" \"three\" (3))";
	Expression value = run(program);
	REQUIRE(value == Expression(3));
}
TEST_CASE("Test Interpreter parser for set property error #1", "[interpreter]"){
  INFO("Should return invalid number of arguments in set property")
	std::string program = "(set-property \"note\" \"a complex number\" (+ 1 I) (+ 5))";
	std::istringstream iss(program);
	Interpreter interp;
  
  bool ok = interp.parseStream(iss);
  REQUIRE(ok == true);
	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Test Interpreter parser for set property error #2", "[interpreter]"){
  INFO("Should return invalid first argument in set property")
	std::string program = "(set-property note \"a complex number\" (+ 1 I))";
	std::istringstream iss(program);
	Interpreter interp;

  bool ok = interp.parseStream(iss);
  REQUIRE(ok == true);
	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Test Interpreter parser for symbol does not name a procedure", "[interpreter]") {

	INFO("Should return error: symbol does not name a procedure")
	std::string program = "(cool 3 4)";
	std::istringstream iss(program);
	Interpreter interp;
  bool ok = interp.parseStream(iss);
  REQUIRE(ok == true);
	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("test Interpreter parser for get-property", "[interpreter]") {
	std::string program = "(begin (define a (+ 1 I)) (define b (set-property \"note\" \"a complex number\" a)) (get-property \"note\" b))";
	Expression value = run(program);
	REQUIRE(value == Expression(Atom("\"a complex number\"")));
}

TEST_CASE("Test Interpreter parser for get-property error #1", "[interpreter]") {
	INFO("Should return error: first argument should be a string")
	std::string program = "(begin (define a (+ 1 I)) (define b (set-property \"note\" \"a complex number\" a)) (get-property note b))";
	std::istringstream iss(program);
	Interpreter interp;
  bool ok = interp.parseStream(iss);
  REQUIRE(ok == true);
	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Test Interpreter parser for get-property error #2", "[interpreter]") {
	INFO("Should return error: second argument must be in the property map")
	std::string program = "(get-property \"note\" 3 3)";
	std::istringstream iss(program);
	Interpreter interp;
  bool ok = interp.parseStream(iss);
  REQUIRE(ok == true);
	REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("test Interpreter parser for makeString", "[interpreter]") {
  std::string program = "(+ 1 2)";
	Expression value = run(program);
  std::string str = "(3)";
	REQUIRE(str == value.makeString());
}

TEST_CASE("test Interpreter parser for makeString none type", "[interpreter]") {
  std::string program = "(get-property \"foo\" (+ 1 2))";
	Expression value = run(program);
  std::string str = "NONE";
	REQUIRE(str == value.makeString());
}

TEST_CASE("test Interpreter parser for makeTail", "[interpreter]") {
  std::string program = "(list 1 2 3)";
	Expression value = run(program);
  std::vector<Expression> vec = { Expression(1), Expression(2), Expression(3) };
	REQUIRE(vec == value.makeTail());
}

TEST_CASE("test Interpreter parser for isPoint false", "[interpreter]") {
  std::map<std::string, Expression> propertymap;
  std::string program = "(set-property \"size\" 10 (make-point 0 0))";
  Expression exp(5);
	REQUIRE(exp.isPoint() == false);
}

TEST_CASE("test Interpreter parser for isLine false", "[interpreter]") {
  std::map<std::string, Expression> propertymap;
  std::string program = "(set-property \"size\" 10 (make-point 0 0))";
  Expression exp(5);
	REQUIRE(exp.isLine() == false);
}

TEST_CASE("test Interpreter parser for isText false", "[interpreter]") {
  std::map<std::string, Expression> propertymap;
  std::string program = "(set-property \"size\" 10 (make-point 0 0))";
  Expression exp(5);
	REQUIRE(exp.isText() == false);
}

/*TEST_CASE("test Interpreter parser for getSize", "[interpreter]") {
  std::string program = "(set-property \"size\" 10 (make-point 0 0))";
  Expression value = run(program);
	REQUIRE(value.getSize() == 10.0);
}*/

//(begin (define a (make-point 5 5)) (get-property "object-name" a))
TEST_CASE( "Test Interpreter parser with numerical literals", "[interpreter]" ) {

  std::vector<std::string> programs = {"(1)", "(+1)", "(+1e+0)", "(1e-0)"};
  
  for(auto program : programs){
    std::istringstream iss(program);
 
    Interpreter interp;

    bool ok = interp.parseStream(iss);

    REQUIRE(ok == true);
  }

  {
    std::istringstream iss("(define x 1abc)");
    
    Interpreter interp;

    bool ok = interp.parseStream(iss);

    REQUIRE(ok == false);
  }
}

TEST_CASE( "Test Interpreter parser with truncated input", "[interpreter]" ) {

  {
    std::string program = "(f";
    std::istringstream iss(program);
  
    Interpreter interp;
    bool ok = interp.parseStream(iss);
    REQUIRE(ok == false);
  }
  
  {
    std::string program = "(begin (define r 10) (* pi (* r r";
    std::istringstream iss(program);

    Interpreter interp;
    bool ok = interp.parseStream(iss);
    REQUIRE(ok == false);
  }
}

TEST_CASE( "Test Interpreter parser with extra input", "[interpreter]" ) {

  std::string program = "(begin (define r 10) (* pi (* r r))) )";
  std::istringstream iss(program);

  Interpreter interp;

  bool ok = interp.parseStream(iss);

  REQUIRE(ok == false);
}

TEST_CASE( "Test Interpreter parser with single non-keyword", "[interpreter]" ) {

  std::string program = "hello";
  std::istringstream iss(program);
  
  Interpreter interp;

  bool ok = interp.parseStream(iss);

  REQUIRE(ok == false);
}

TEST_CASE( "Test Interpreter parser with empty input", "[interpreter]" ) {

  std::string program;
  std::istringstream iss(program);
  
  Interpreter interp;

  bool ok = interp.parseStream(iss);

  REQUIRE(ok == false);
}

TEST_CASE( "Test Interpreter parser with empty expression", "[interpreter]" ) {

  std::string program = "( )";
  std::istringstream iss(program);
  
  Interpreter interp;

  bool ok = interp.parseStream(iss);

  REQUIRE(ok == false);
}

TEST_CASE( "Test Interpreter parser with bad number string", "[interpreter]" ) {

  std::string program = "(1abc)";
  std::istringstream iss(program);
  
  Interpreter interp;

  bool ok = interp.parseStream(iss);

  REQUIRE(ok == false);
}

TEST_CASE( "Test Interpreter parser with incorrect input. Regression Test", "[interpreter]" ) {

  std::string program = "(+ 1 2) (+ 3 4)";
  std::istringstream iss(program);
  
  Interpreter interp;

  bool ok = interp.parseStream(iss);

  REQUIRE(ok == false);
}

TEST_CASE( "Test Interpreter result with literal expressions", "[interpreter]" ) {
  
  { // Number
    std::string program = "(4)";
    Expression result = run(program);
    REQUIRE(result == Expression(4.));
  }

  { // Symbol
    std::string program = "(pi)";
    Expression result = run(program);
    REQUIRE(result == Expression(atan2(0, -1)));
  }

}

TEST_CASE( "Test Interpreter result with simple procedures (add)", "[interpreter]" ) {

  { // add, binary case
    std::string program = "(+ 1 2)";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result == Expression(3.));
  }
  
  { // add, 3-ary case
    std::string program = "(+ 1 2 3)";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result == Expression(6.));
  }

  { // add, 6-ary case
    std::string program = "(+ 1 2 3 4 5 6)";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result == Expression(21.));
  }
}
  
TEST_CASE( "Test Interpreter special forms: begin and define", "[interpreter]" ) {

  {
    std::string program = "(define answer 42)";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result == Expression(42.));
  }

  {
    std::string program = "(begin (define answer 42)\n(answer))";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result == Expression(42.));
  }
  
  {
    std::string program = "(begin (define answer (+ 9 11)) (answer))";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result == Expression(20.));
  }

  {
    std::string program = "(begin (define a 1) (define b 1) (+ a b))";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result == Expression(2.));
  }
}

TEST_CASE( "Test a medium-sized expression", "[interpreter]" ) {

  {
    std::string program = "(+ (+ 10 1) (+ 30 (+ 1 1)))";
    Expression result = run(program);
    REQUIRE(result == Expression(43.));
  }
}

TEST_CASE( "Test arithmetic procedures", "[interpreter]" ) {

  {
    std::vector<std::string> programs = {"(+ 1 -2)",
					 "(+ -3 1 1)",
					 "(- 1)",
					 "(- 1 2)",
					 "(* 1 -1)",
					 "(* 1 1 -1)",
					 "(/ -1 1)",
					 "(/ 1 -1)"};

    for(auto s : programs){
      Expression result = run(s);
      REQUIRE(result == Expression(-1.));
    }
  }
}


TEST_CASE( "Test some semantically invalid expresions", "[interpreter]" ) {
  
  std::vector<std::string> programs = {"(@ none)", // so such procedure
				       "(- 1 1 2)", // too many arguments
				       "(define begin 1)", // redefine special form
				       "(define pi 3.14)"}; // redefine builtin symbol
    for(auto s : programs){
      Interpreter interp;

      std::istringstream iss(s);
      
      bool ok = interp.parseStream(iss);
      REQUIRE(ok == true);
      
      REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
    }
}

TEST_CASE( "Test for exceptions from semantically incorrect input", "[interpreter]" ) {

  std::string input = R"(
(+ 1 a)
)";

  Interpreter interp;
  
  std::istringstream iss(input);
  
  bool ok = interp.parseStream(iss);
  REQUIRE(ok == true);
  
  REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE( "Test malformed define", "[interpreter]" ) {

    std::string input = R"(
(define a 1 2)
)";

  Interpreter interp;
  
  std::istringstream iss(input);
  
  bool ok = interp.parseStream(iss);
  REQUIRE(ok == true);
  
  REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE( "Test using number as procedure", "[interpreter]" ) {
    std::string input = R"(
(1 2 3)
)";

  Interpreter interp;
  
  std::istringstream iss(input);
  
  bool ok = interp.parseStream(iss);
  REQUIRE(ok == true);
  
  REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}
