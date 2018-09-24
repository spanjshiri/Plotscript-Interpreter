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

	INFO("trying div with one number to throw a semantic error")
	args.clear();
	args.emplace_back(5.0);
	REQUIRE_THROWS_AS(pdiv(args), SemanticError);

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

	INFO("trying div procedure to throw semantic error")
	args.clear();
	args.emplace_back(Atom("hi"));
	args.emplace_back(2.0);
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

	INFO("trying pow with one positive number")
	args.emplace_back(I);
	args.emplace_back(5.0);
	REQUIRE(ppow(args) == Expression(I));
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

