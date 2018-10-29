#include "catch.hpp"

#include "environment.hpp"
#include "semantic_error.hpp"

#include <cmath>

TEST_CASE( "Test default constructor", "[environment]" ) {

  Environment env;

  REQUIRE(env.is_known(Atom("pi")));
  REQUIRE(env.is_exp(Atom("pi")));

  REQUIRE(!env.is_known(Atom("hi")));
  REQUIRE(!env.is_exp(Atom("hi")));

  REQUIRE(env.is_proc(Atom("+")));
  REQUIRE(env.is_proc(Atom("-")));
  REQUIRE(env.is_proc(Atom("*")));
  REQUIRE(env.is_proc(Atom("/")));
  REQUIRE(!env.is_proc(Atom("op")));
}

TEST_CASE( "Test assignment operator", "[environment]") {
	Environment env;
	env.add_exp(Atom("cool"),Expression(3.0));
	Environment env2;
	env2 = env;
	REQUIRE(env2.is_exp(Atom("cool")));
}

TEST_CASE( "Test get expression", "[environment]" ) {
  Environment env;

  REQUIRE(env.get_exp(Atom("pi")) == Expression(std::atan2(0, -1)));
  REQUIRE(env.get_exp(Atom("hi")) == Expression());
}

TEST_CASE( "Test add expression", "[environment]" ) {
  Environment env;

  Expression a(Atom(1.0));
  env.add_exp(Atom("one"), a);
  REQUIRE(env.is_known(Atom("one")));
  REQUIRE(env.is_exp(Atom("one")));
  REQUIRE(env.get_exp(Atom("one")) == a);

  Expression b(Atom("hello"));
  env.add_exp(Atom("hi"), b);
  REQUIRE(env.is_known(Atom("hi")));
  REQUIRE(env.is_exp(Atom("hi")));
  REQUIRE(env.get_exp(Atom("hi")) == b);

  REQUIRE_THROWS_AS(env.add_exp(Atom(1.0), b), SemanticError);
}

TEST_CASE( "Test get built-in procedure", "[environment]" ) {
  Environment env;

  INFO("default procedure")
  Procedure p1 = env.get_proc(Atom("doesnotexist"));
  Procedure p2 = env.get_proc(Atom("alsodoesnotexist"));
  REQUIRE(p1 == p2);
  std::vector<Expression> args;
  REQUIRE(p1(args) == Expression());
  REQUIRE(p2(args) == Expression());

  INFO("trying add procedure")
  Procedure padd = env.get_proc(Atom("+"));
  args.emplace_back(1.0);
  args.emplace_back(2.0);
  REQUIRE(padd(args) == Expression(3.0));
}

TEST_CASE( "Test add procedure", "[environment]" ) {
  Environment env;
  std::vector<Expression> args;

  INFO("trying add procedure with a complex and a number")
  Procedure padd = env.get_proc(Atom("+"));
  Expression I = env.get_exp(Atom("I"));
  args.emplace_back(1.0);
  args.emplace_back(I);
  REQUIRE(padd(args) == Expression(std::complex<double> (1.0,1.0)));

  INFO("trying add procedure to throw semantic error")
  args.clear();
  Expression a(Atom("hello"));
  env.add_exp(Atom("hi"), a);
  args.emplace_back(Atom("hi"));
  args.emplace_back(2.0);
  REQUIRE_THROWS_AS(padd(args), SemanticError);
}

TEST_CASE("Test mul procedure", "[environment]") {
	Environment env;
	std::vector<Expression> args;

	INFO("trying mul procedure with a complex and a number")
	Procedure pmul = env.get_proc(Atom("*"));
	Expression I = env.get_exp(Atom("I"));
	args.emplace_back(1.0);
	args.emplace_back(I);
	REQUIRE(pmul(args) == Expression(std::complex<double>(0.0, 1.0)));

	INFO("trying mul procedure to throw semantic error")
	args.clear();
	Expression a(Atom("hello"));
	env.add_exp(Atom("hi"), a);
	args.emplace_back(Atom("hi"));
	args.emplace_back(2.0);
	REQUIRE_THROWS_AS(pmul(args), SemanticError);
}

TEST_CASE("Test subneg procedure", "[environment]") {
	Environment env;
	std::vector<Expression> args;

	INFO("trying subneg with one number")
	Procedure psub = env.get_proc(Atom("-"));
	args.emplace_back(1.0);
	REQUIRE(psub(args) == Expression(-1.0));

	INFO("trying subneg with one complex")
	args.clear();
	Expression I = env.get_exp(Atom("I"));
	args.emplace_back(I);
	REQUIRE(psub(args) == Expression(std::complex<double>(0.0, -1.0)));

	INFO("trying subneg with one number to throw a semantic error")
	args.clear();
	Expression a(Atom("hello"));
	env.add_exp(Atom("hi"), a);
	args.emplace_back(Atom("hi"));
	REQUIRE_THROWS_AS(psub(args), SemanticError);

	INFO("trying subneg procedure with a complex minus complex")
	args.clear();
	args.emplace_back(I);
	args.emplace_back(I);
	REQUIRE(psub(args) == Expression(std::complex<double>(0.0, 0.0)));

	INFO("trying subneg procedure with a number minus complex")
	args.clear();
	args.emplace_back(1.0);
	args.emplace_back(I);
	REQUIRE(psub(args) == Expression(std::complex<double>(1.0, -1.0)));

	INFO("trying subneg procedure with a complex minus number")
	args.clear();
	args.emplace_back(I);
	args.emplace_back(1.0);
	REQUIRE(psub(args) == Expression(std::complex<double>(-1.0, 1.0)));

	INFO("trying subneg procedure to throw semantic error")
	args.clear();
	args.emplace_back(Atom("hi"));
	args.emplace_back(2.0);
	REQUIRE_THROWS_AS(psub(args), SemanticError);
}

TEST_CASE("Test div procedure", "[environment]") {
	Environment env;
	Procedure pdiv = env.get_proc(Atom("/"));
	std::vector<Expression> args;
	Expression I = env.get_exp(Atom("I"));
	Expression a(Atom("hello"));
	env.add_exp(Atom("hi"), a);

	INFO("trying div with one number")
	args.clear();
	args.emplace_back(5.0);
	REQUIRE(pdiv(args) == Expression(.2));

	INFO("trying div with one complex")
	args.clear();
	args.emplace_back(I);
	REQUIRE(pdiv(args) == Expression(std::complex<double>(0.0, -1.0)));

	INFO("trying div procedure with a complex divided by complex")
	args.clear();
	args.emplace_back(I);
	args.emplace_back(I);
	REQUIRE(pdiv(args) == Expression(std::complex<double>(1.0, 0.0)));

	INFO("trying div procedure with a number divided by complex")
	args.clear();
	args.emplace_back(1.0);
	args.emplace_back(I);
	REQUIRE(pdiv(args) == Expression(std::complex<double>(0.0, -1.0)));

	INFO("trying div procedure with a complex divided by number")
	args.clear();
	args.emplace_back(I);
	args.emplace_back(1.0);
	REQUIRE(pdiv(args) == Expression(std::complex<double>(0.0, 1.0)));

	INFO("trying div procedure to throw semantic error with more than 3 arguments")
	args.clear();
	args.emplace_back(I);
	args.emplace_back(I);
	args.emplace_back(I);
	REQUIRE_THROWS_AS(pdiv(args), SemanticError);

	INFO("trying div procedure to throw semantic error with multiple arguments")
	args.clear();
	args.emplace_back(Atom("hi"));
	args.emplace_back(2.0);
	REQUIRE_THROWS_AS(pdiv(args), SemanticError);

	INFO("trying div procedure to throw semantic error with one argument")
	args.clear();
	args.emplace_back(Atom("hi"));
	REQUIRE_THROWS_AS(pdiv(args), SemanticError);
}

TEST_CASE("Test sqrt procedure", "[environment]") {
	Environment env;
	Procedure psqrt = env.get_proc(Atom("sqrt"));
	std::vector<Expression> args;
	Expression I = env.get_exp(Atom("I"));
	Expression a(Atom("hello"));
	env.add_exp(Atom("hi"), a);

	INFO("trying sqrt with one positive number")
	args.emplace_back(9.0);
	REQUIRE(psqrt(args) == Expression(3.0));

	INFO("trying sqrt procedure with one negative number")
	args.clear();
	args.emplace_back(-4.0);
	REQUIRE(psqrt(args) == Expression(std::complex<double>(0.0, 2.0)));

	INFO("trying sqrt procedure one complex value")
	args.clear();
	args.emplace_back(I);
	REQUIRE(psqrt(args) == Expression(std::complex<double>(0.707107, 0.707107)));

	INFO("trying sqrt with more than one value to throw semantic error")
	args.clear();
	args.emplace_back(5.0);
	args.emplace_back(2.0);
	REQUIRE_THROWS_AS(psqrt(args), SemanticError);
}

TEST_CASE("Test pow procedure", "[environment]") {
	Environment env;
	Procedure ppow = env.get_proc(Atom("^"));
	std::vector<Expression> args;
	Expression I = env.get_exp(Atom("I"));
	Expression a(Atom("hello"));
	env.add_exp(Atom("hi"), a);

	INFO("trying pow with base number and exponent number")
	args.emplace_back(2.0);
	args.emplace_back(5.0);
	REQUIRE(ppow(args) == Expression(32.0));

	INFO("trying pow with base number and exponent complex")
	args.clear();
	args.emplace_back(1.0);
	args.emplace_back(I);
	REQUIRE(ppow(args) == Expression(std::complex<double>(1.0, 0.0)));

	INFO("trying pow with base complex and exponent number")
	args.clear();
	args.emplace_back(I);
	args.emplace_back(5.0);
	REQUIRE(ppow(args) == Expression(I));

	INFO("trying pow with base complex and exponent complex")
	args.clear();
	args.emplace_back(I);
	args.emplace_back(I);
	REQUIRE(ppow(args) == Expression(std::complex<double>(.20788, 0.0)));

	INFO("trying pow procedure to throw semantic error for invalid argument")
	args.clear();
	args.emplace_back(Atom("hi"));
	args.emplace_back(2.0);
	REQUIRE_THROWS_AS(ppow(args), SemanticError);

	INFO("trying pow procedure to throw semantic error for invalid number of arguments")
	args.clear();
	args.emplace_back(2.0);
	REQUIRE_THROWS_AS(ppow(args), SemanticError);
}

TEST_CASE("Test ln procedure", "[environment]") {
	Environment env;
	Procedure pln = env.get_proc(Atom("ln"));
	std::vector<Expression> args;
	Expression I = env.get_exp(Atom("I"));
	Expression a(Atom("hello"));
	env.add_exp(Atom("hi"), a);

	INFO("trying ln with a positive number")
	args.emplace_back(1.0);
	REQUIRE(pln(args) == Expression(0.0));

	INFO("trying ln with a negative number to throw a semantic error")
	args.clear();
	args.emplace_back(-1.0);
	REQUIRE_THROWS_AS(pln(args), SemanticError);

	INFO("trying ln to throw semantic error for invalid argument")
	args.clear();
	args.emplace_back(Atom("hi"));
	REQUIRE_THROWS_AS(pln(args), SemanticError);

	INFO("trying ln to throw semantic error for invalid number of arguments")
	args.clear();
	args.emplace_back(2.0);
	args.emplace_back(2.0);
	REQUIRE_THROWS_AS(pln(args), SemanticError);
}

TEST_CASE("Test sin procedure", "[environment]") {
	Environment env;
	Procedure psin = env.get_proc(Atom("sin"));
	std::vector<Expression> args;
	Expression I = env.get_exp(Atom("I"));
	Expression a(Atom("hello"));
	env.add_exp(Atom("hi"), a);

	INFO("trying sin with a number")
	args.emplace_back(0.0);
	REQUIRE(psin(args) == Expression(0.0));

	INFO("trying sin to throw semantic error for invalid argument")
	args.clear();
	args.emplace_back(Atom("hi"));
	REQUIRE_THROWS_AS(psin(args), SemanticError);

	INFO("trying sin to throw semantic error for invalid number of arguments")
	args.clear();
	args.emplace_back(2.0);
	args.emplace_back(2.0);
	REQUIRE_THROWS_AS(psin(args), SemanticError);
}

TEST_CASE("Test cos procedure", "[environment]") {
	Environment env;
	Procedure pcos = env.get_proc(Atom("cos"));
	std::vector<Expression> args;
	Expression I = env.get_exp(Atom("I"));
	Expression a(Atom("hello"));
	env.add_exp(Atom("hi"), a);

	INFO("trying cos with a number")
	args.emplace_back(0.0);
	REQUIRE(pcos(args) == Expression(1.0));

	INFO("trying cos to throw semantic error for invalid argument")
	args.clear();
	args.emplace_back(Atom("hi"));
	REQUIRE_THROWS_AS(pcos(args), SemanticError);

	INFO("trying cos to throw semantic error for invalid number of arguments")
	args.clear();
	args.emplace_back(2.0);
	args.emplace_back(2.0);
	REQUIRE_THROWS_AS(pcos(args), SemanticError);
}

TEST_CASE("Test tan procedure", "[environment]") {
	Environment env;
	Procedure ptan = env.get_proc(Atom("tan"));
	std::vector<Expression> args;
	Expression I = env.get_exp(Atom("I"));
	Expression a(Atom("hello"));
	env.add_exp(Atom("hi"), a);

	INFO("trying tan with a number")
	args.emplace_back(0.0);
	REQUIRE(ptan(args) == Expression(0.0));

	INFO("trying tan to throw semantic error for invalid argument")
	args.clear();
	args.emplace_back(Atom("hi"));
	REQUIRE_THROWS_AS(ptan(args), SemanticError);

	INFO("trying tan to throw semantic error for invalid number of arguments")
	args.clear();
	args.emplace_back(2.0);
	args.emplace_back(2.0);
	REQUIRE_THROWS_AS(ptan(args), SemanticError);
}

TEST_CASE("Test real procedure", "[environment]") {
	Environment env;
	Procedure preal = env.get_proc(Atom("real"));
	std::vector<Expression> args;
	Expression I = env.get_exp(Atom("I"));

	INFO("trying real with a complex")
	args.emplace_back(I);
	REQUIRE(preal(args) == Expression(0.0));

	INFO("trying real to throw semantic error for invalid argument")
	args.clear();
	args.emplace_back(1.0);
	REQUIRE_THROWS_AS(preal(args), SemanticError);

	INFO("trying real to throw semantic error for invalid number of arguments")
	args.clear();
	args.emplace_back(I);
	args.emplace_back(I);
	REQUIRE_THROWS_AS(preal(args), SemanticError);
}

TEST_CASE("Test imag procedure", "[environment]") {
	Environment env;
	Procedure pimag = env.get_proc(Atom("imag"));
	std::vector<Expression> args;
	Expression I = env.get_exp(Atom("I"));

	INFO("trying imag with a complex")
	args.emplace_back(I);
	REQUIRE(pimag(args) == Expression(1.0));

	INFO("trying imag to throw semantic error for invalid argument")
	args.clear();
	args.emplace_back(1.0);
	REQUIRE_THROWS_AS(pimag(args), SemanticError);

	INFO("trying imag to throw semantic error for invalid number of arguments")
	args.clear();
	args.emplace_back(I);
	args.emplace_back(I);
	REQUIRE_THROWS_AS(pimag(args), SemanticError);
}

TEST_CASE("Test mag procedure", "[environment]") {
	Environment env;
	Procedure pmag = env.get_proc(Atom("mag"));
	std::vector<Expression> args;
	Expression I = env.get_exp(Atom("I"));

	INFO("trying mag with a complex")
	args.emplace_back(I);
	REQUIRE(pmag(args) == Expression(1.0));

	INFO("trying mag to throw semantic error for invalid argument")
	args.clear();
	args.emplace_back(1.0);
	REQUIRE_THROWS_AS(pmag(args), SemanticError);

	INFO("trying mag to throw semantic error for invalid number of arguments")
	args.clear();
	args.emplace_back(I);
	args.emplace_back(I);
	REQUIRE_THROWS_AS(pmag(args), SemanticError);
}

TEST_CASE("Test arg procedure", "[environment]") {
	Environment env;
	Procedure parg = env.get_proc(Atom("arg"));
	std::vector<Expression> args;
	Expression I = env.get_exp(Atom("I"));

	INFO("trying arg with a complex")
	args.emplace_back(std::complex<double> (0.0,0.0));
	REQUIRE(parg(args) == Expression(0.0));

	INFO("trying arg to throw semantic error for invalid argument")
	args.clear();
	args.emplace_back(1.0);
	REQUIRE_THROWS_AS(parg(args), SemanticError);

	INFO("trying arg to throw semantic error for invalid number of arguments")
	args.clear();
	args.emplace_back(I);
	args.emplace_back(I);
	REQUIRE_THROWS_AS(parg(args), SemanticError);
}

TEST_CASE("Test conj procedure", "[environment]") {
	Environment env;
	Procedure pconj = env.get_proc(Atom("conj"));
	std::vector<Expression> args;
	Expression I = env.get_exp(Atom("I"));

	INFO("trying conj with a complex")
	args.emplace_back(I);
	REQUIRE(pconj(args) == Expression(std::complex<double>(0.0,-1.0)));

	INFO("trying conj to throw semantic error for invalid argument")
	args.clear();
	args.emplace_back(1.0);
	REQUIRE_THROWS_AS(pconj(args), SemanticError);

	INFO("trying conj to throw semantic error for invalid number of arguments")
	args.clear();
	args.emplace_back(I);
	args.emplace_back(I);
	REQUIRE_THROWS_AS(pconj(args), SemanticError);
}

TEST_CASE("Test List procedure", "[environment]") {
	Environment env;
	Procedure plist = env.get_proc(Atom("list"));
	std::vector<Expression> args;
	Expression I = env.get_exp(Atom("I"));

	INFO("trying list to create a list")
	args.clear();
	args.emplace_back(Expression(0.0));
	args.emplace_back(Expression(1.0));
	args.emplace_back(Expression(2.0));
	REQUIRE(plist(args).isHeadList());
	std::vector<Expression> testList = { Expression(0.0), Expression(1.0), Expression(2.0) };
	REQUIRE(plist(args) == Expression(testList));
}

TEST_CASE("Test First procedure", "[environment]") {
	Environment env;
	Procedure plist = env.get_proc(Atom("list"));
	Procedure pfirst = env.get_proc(Atom("first"));
	std::vector<Expression> args;
	Expression I = env.get_exp(Atom("I"));

	INFO("trying first with one list")
	args.clear();
	args.emplace_back(Expression(1.0));
	args.emplace_back(Expression(2.0));
	args.emplace_back(Expression(3.0));
	std::vector<Expression> testList = { plist(args) };
	REQUIRE(pfirst(testList) == Expression(1.0));

	INFO("trying first with one invalid argument")
	args.clear();
	args.emplace_back(Expression(1.0));
	REQUIRE_THROWS_AS(pfirst(args), SemanticError);

	INFO("trying first with empty list")
	args.clear();
	std::vector<Expression> testList1 = { plist(args) };
	REQUIRE_THROWS_AS(pfirst(testList1), SemanticError);

	INFO("trying first with invalid number of arguments")
	args.clear();
	std::vector<Expression> testList2 = { plist(args),plist(testList1) };
	REQUIRE_THROWS_AS(pfirst(testList2), SemanticError);
}

TEST_CASE("Test Rest procedure", "[environment]") {
	Environment env;
	Procedure plist = env.get_proc(Atom("list"));
	Procedure prest = env.get_proc(Atom("rest"));
	std::vector<Expression> args;
	Expression I = env.get_exp(Atom("I"));

	INFO("trying rest with one list")
	args.clear();
	args.emplace_back(Expression(1.0));
	args.emplace_back(Expression(2.0));
	args.emplace_back(Expression(3.0));
	std::vector<Expression> testList = { plist(args) };
	std::vector<Expression> testList3 = { Expression(2.0),Expression(3.0) };
	REQUIRE(prest(testList) == plist(testList3));

	INFO("trying rest with one invalid argument")
	args.clear();
	args.emplace_back(Expression(1.0));
	REQUIRE_THROWS_AS(prest(args), SemanticError);

	INFO("trying rest with empty list")
	args.clear();
	std::vector<Expression> testList1 = { plist(args) };
	REQUIRE_THROWS_AS(prest(testList1), SemanticError);

	INFO("trying rest with invalid number of arguments")
	args.clear();
	std::vector<Expression> testList2 = { plist(args),plist(testList1) };
	REQUIRE_THROWS_AS(prest(testList2), SemanticError);
}

TEST_CASE("Test Length procedure", "[environment]") {
	Environment env;
	Procedure plist = env.get_proc(Atom("list"));
	Procedure plength = env.get_proc(Atom("length"));
	std::vector<Expression> args;
	Expression I = env.get_exp(Atom("I"));

	INFO("trying length with one list")
	args.clear();
	args.emplace_back(Expression(1.0));
	args.emplace_back(Expression(2.0));
	args.emplace_back(Expression(3.0));
	std::vector<Expression> testList = { plist(args) };
	REQUIRE(plength(testList) == Expression(3.0));

	INFO("trying length with one invalid argument")
	args.clear();
	args.emplace_back(Expression(1.0));
	REQUIRE_THROWS_AS(plength(args), SemanticError);

	INFO("trying length with empty list")
	args.clear();
	std::vector<Expression> testList1 = { plist(args) };
	REQUIRE(plength(testList1) == Expression(0.0));

	INFO("trying rest with invalid number of arguments")
	args.clear();
	std::vector<Expression> testList2 = { plist(args),plist(testList1) };
	REQUIRE_THROWS_AS(plength(testList2), SemanticError);
}

TEST_CASE("Test Append procedure", "[environment]") {
Environment env;
Procedure plist = env.get_proc(Atom("list"));
Procedure pappend = env.get_proc(Atom("append"));
std::vector<Expression> args;
Expression I = env.get_exp(Atom("I"));

INFO("trying append with one list and expression")
args.clear();
args.emplace_back(Expression(1.0));
args.emplace_back(Expression(2.0));
args.emplace_back(Expression(3.0));
std::vector<Expression> testList = { plist(args) };
Expression exp = Expression(4.0);
std::vector<Expression> testList5 = { testList, exp };
std::vector<Expression> testList6 = { plist(args), Expression(4.0) };
REQUIRE(pappend(testList5) == plist(testList6));

INFO("trying append with invalid number of arguments")
Expression exp2 = Expression(5.0);
std::vector<Expression> testList2 = { plist(args), exp, exp2 };
REQUIRE_THROWS_AS(pappend(testList2), SemanticError);

INFO("trying append with invalid number of arguments")
std::vector<Expression> testList3 = { exp, exp2 };
REQUIRE_THROWS_AS(pappend(testList3), SemanticError);

INFO("trying append with one invalid argument")
args.clear();
args.emplace_back(Expression(1.0));
REQUIRE_THROWS_AS(pappend(args), SemanticError);
}

TEST_CASE("Test Join procedure", "[environment]") {
	Environment env;
	Procedure plist = env.get_proc(Atom("list"));
	Procedure pjoin = env.get_proc(Atom("join"));
	std::vector<Expression> args;
	Expression I = env.get_exp(Atom("I"));

	INFO("trying join with two lists")
	args.clear();
	args.emplace_back(Expression(1.0));
	args.emplace_back(Expression(2.0));
	args.emplace_back(Expression(3.0));
	std::vector<Expression> testList = { plist(args), plist(args) };
	std::vector<Expression> testList6 = { Expression(1.0),Expression(2.0),Expression(3.0),Expression(1.0),Expression(2.0),Expression(3.0) };
	REQUIRE(pjoin(testList) == plist(testList6));

	INFO("trying join with invalid number of arguments")
	Expression exp1 = Expression(5.0);
	Expression exp2 = Expression(5.0);
	std::vector<Expression> testList2 = { plist(args), plist(args), plist(args)};
	REQUIRE_THROWS_AS(pjoin(testList2), SemanticError);

	INFO("trying join with invalid number of arguments")
	std::vector<Expression> testList3 = { exp1, exp2 };
	REQUIRE_THROWS_AS(pjoin(testList3), SemanticError);
}

TEST_CASE("Test Range procedure", "[environment]") {
	Environment env;
	Procedure plist = env.get_proc(Atom("list"));
	Procedure prange = env.get_proc(Atom("range"));
	std::vector<Expression> args;
	Expression I = env.get_exp(Atom("I"));

	INFO("trying range with 3 numbers and no errors")
	args.clear();
	args.emplace_back(Expression(1.0));
	args.emplace_back(Expression(2.0));
	args.emplace_back(Expression(3.0));
	std::vector<Expression> testList = { Expression(0.0),Expression(1.0),Expression(0.25) };
	std::vector<Expression> testList6 = { Expression(0.0),Expression(0.25),Expression(0.50),Expression(0.75),Expression(1.0) };
	REQUIRE(prange(testList) == plist(testList6));

	INFO("trying range with 3 numbers and arg0 > arg1 to throw semantic error")
	std::vector<Expression> testList1 = { Expression(1.0),Expression(0.0),Expression(0.25) };
	REQUIRE_THROWS_AS(prange(testList1), SemanticError);

	INFO("trying range with 3 numbers and arg2 <= 0 to throw semantic error")
	std::vector<Expression> testList2 = { Expression(0.0),Expression(2.0),Expression(-1.0) };
	REQUIRE_THROWS_AS(prange(testList2), SemanticError);

	INFO("trying range with invalid number of arguments to throw semantic error")
	std::vector<Expression> testList3 = { Expression(0.0),Expression(2.0) };
	REQUIRE_THROWS_AS(prange(testList3), SemanticError);

	INFO("trying range with atleast one argument not a number to throw semantic error")
	std::vector<Expression> testList4 = { Expression(I),Expression(2.0),Expression(-1.0) };
	REQUIRE_THROWS_AS(prange(testList4), SemanticError);
}

TEST_CASE( "Test reset", "[environment]" ) {
  Environment env;

  Expression a(Atom(1.0));
  env.add_exp(Atom("one"), a);
  Expression b(Atom("hello"));
  env.add_exp(Atom("hi"), b);

  env.reset();
  REQUIRE(!env.is_known(Atom("one")));
  REQUIRE(!env.is_exp(Atom("one")));
  REQUIRE(env.get_exp(Atom("one")) == Expression());
  REQUIRE(!env.is_known(Atom("hi")));
  REQUIRE(!env.is_exp(Atom("hi")));
  REQUIRE(env.get_exp(Atom("hi")) == Expression());
}

TEST_CASE( "Test semeantic errors", "[environment]" ) {

  Environment env;

  {
    Expression exp(Atom("begin"));
    
    REQUIRE_THROWS_AS(exp.eval(env), SemanticError);
  }
}

