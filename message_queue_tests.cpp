#include "catch.hpp"

#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <utility>

#include "semantic_error.hpp"
#include "message_queue.hpp"
#include "expression.hpp"

TEST_CASE( "Test thread safe message queue empty function", "[message_queue]" ) {
  MessageQueue<std::pair<std::string,Expression>> q;
  REQUIRE(q.empty());
}

TEST_CASE( "Test thread safe push function", "[message_queue]" ) {
  MessageQueue<std::pair<std::string,Expression>> q;
  std::pair<std::string,Expression> p = {"no money?",Expression()};
  q.push(p);
  REQUIRE(!q.empty());
}

TEST_CASE( "Test thread safe try and pop return false function", "[message_queue]" ) {
  MessageQueue<std::pair<std::string,Expression>> q;
  std::pair<std::string,Expression> p = {"no money?",Expression()};
  REQUIRE(!q.try_pop(p));
}

TEST_CASE( "Test thread safe try and pop return true function", "[message_queue]" ) {
  MessageQueue<std::pair<std::string,Expression>> q;
  std::pair<std::string,Expression> p = {"no money?",Expression()};
  q.push(p);
  REQUIRE(q.try_pop(p));
}

TEST_CASE( "Test thread safe wait and pop return false function", "[message_queue]" ) {
  MessageQueue<std::pair<std::string,Expression>> q;
  std::pair<std::string,Expression> p = {"no money?",Expression()};
  std::pair<std::string,Expression> p2;
  q.push(p);
  q.wait_and_pop(p2);
  REQUIRE(q.empty());
}




