#include "catch.hpp"

#include "expression.hpp"
#include "environment.hpp"

TEST_CASE( "Test default expression", "[expression]" ) {

  Expression exp;

  REQUIRE(!exp.isHeadNumber());
  REQUIRE(!exp.isHeadSymbol());
}

TEST_CASE("Test expression with Lambda Constructor for Expression", "[expression]") {
	std::vector<Expression> args = {};
	args.push_back(Expression(3.0));
	Atom head;
	Expression exp(args, head);

	REQUIRE(!exp.isHeadNumber());
	REQUIRE(!exp.isHeadSymbol());
}

TEST_CASE( "Test double expression", "[expression]" ) {

  Expression exp(6.023);

  REQUIRE(exp.isHeadNumber());
  REQUIRE(!exp.isHeadSymbol());
}


TEST_CASE( "Test symbol expression", "[expression]" ) {

  Expression exp(Atom("asymbol"));

  REQUIRE(!exp.isHeadNumber());
  REQUIRE(exp.isHeadSymbol());
}

