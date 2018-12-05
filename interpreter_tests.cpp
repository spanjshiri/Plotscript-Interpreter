#include "catch.hpp"

#include <string>
#include <sstream>
#include <fstream>
#include <iostream>

#include "startup_config.hpp"
#include "semantic_error.hpp"
#include "interpreter.hpp"
#include "expression.hpp"

Expression run(const std::string & program){
    
  Interpreter interp;

  std::ifstream ifs(STARTUP_FILE);

  REQUIRE(interp.parseStream(ifs));

  REQUIRE_NOTHROW(interp.evaluate());

  std::istringstream iss(program);
    
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

TEST_CASE("Test Interpreter parser for first argument to define not a symbol", "[interpreter]"){
  INFO("Should return invalid first argument to define")
	std::string program = "(define 4 3)";
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

TEST_CASE("test the creation of a point", "[interpreter]") {
  Interpreter interp;

  std::map<std::string, Expression> propertymap;
  std::string program = "(set-property \"size\" 10 (make-point 0 5))";
  std::istringstream iss(program);

  bool ok = interp.parseStream(iss);
  REQUIRE(ok == true);
  Expression value = run(program);
  // Expression newValue = result.getPosition();
  // std::vector<Expression> tail = newResult.makeTail();
  // double x = tail[0].head().asNumber();
  // double y = tail[1].head().asNumber();
  double size = 10;

  // REQUIRE(x == 0);
  // REQUIRE(y == 5);
  REQUIRE(value.makeTail()[0].head().asNumber() == 0);
  REQUIRE(value.makeTail()[1].head().asNumber() == 5);
  REQUIRE(value.getSize() == size);
	REQUIRE(value.isText() == false);
  REQUIRE(value.isLine() == false);
  REQUIRE(value.isPoint() == true);
}

TEST_CASE("test the creation of a line", "[interpreter]") {
  Interpreter interp;

  std::map<std::string, Expression> propertymap;
  std::string program = "(set-property \"thickness\" 13 (make-line (make-point 10 5) (make-point -3 -8)))";
  std::istringstream iss(program);
  bool ok = interp.parseStream(iss);
  REQUIRE(ok == true);
  Expression value = run(program);
  std::vector<Expression> tail = value.makeTail();
  std::vector<Expression> p1 = tail[0].makeTail();
  std::vector<Expression> p2 = tail[1].makeTail();
  double x1 = p1[0].head().asNumber();
  double y1 = p1[1].head().asNumber();
  double x2 = p2[0].head().asNumber();
  double y2 = p2[1].head().asNumber();

  double thickness = 13;

  REQUIRE(x1 == 10);
  REQUIRE(y1 == 5);
  REQUIRE(x2 == -3);
  REQUIRE(y2 == -8);
  REQUIRE(value.getThickness() == thickness);
	REQUIRE(value.isText() == false);
  REQUIRE(value.isLine() == true);
  REQUIRE(value.isPoint() == false);
}

TEST_CASE("test the creation of text", "[interpreter]") {
  Interpreter interp;

  std::map<std::string, Expression> propertymap;
  std::string program = "(set-property \"text-scale\" 2 (make-text \"Sulaiman is cool!\"))";
  std::istringstream iss(program);

  bool ok = interp.parseStream(iss);
  REQUIRE(ok == true);
  Expression value = run(program);
  Expression newExp = value.getPosition();
  double scale = value.getTextScale();
  double rotation = value.getTextRotation();
  std::vector<Expression> tail = newExp.makeTail();
  double x = tail[0].head().asNumber();
  double y = tail[1].head().asNumber();
  std::string text = value.head().asString();
  std::string subText = text.substr(1,text.length()-2);


  double scaleCheck = 2;
  double rotationCheck = 0;

  REQUIRE(x == 0);
  REQUIRE(y == 0);
  REQUIRE(scale == scaleCheck);
  REQUIRE(rotation == rotationCheck);
	REQUIRE(value.isText() == true);
  REQUIRE(value.isLine() == false);
  REQUIRE(value.isPoint() == false);
}

TEST_CASE("test the semantic error of discrete plot", "[interpreter]") {
  std::string program = "(discrete-plot 4 (list (list \"title\" \"The Title\") (list \"abscissa-label\" \"X Label\") (list \"ordinate-label\" \"Y Label\") ))";
	std::istringstream iss(program);
	Interpreter interp;
  bool ok = interp.parseStream(iss);
  REQUIRE(ok == true);
  REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

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
