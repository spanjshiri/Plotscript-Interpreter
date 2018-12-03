#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <csignal>
#include <cstdlib>

#include "interpreter.hpp"
#include "semantic_error.hpp"
#include "startup_config.hpp"
#include "message_queue.hpp"


typedef MessageQueue<std::string> imq;
typedef MessageQueue<std::pair<std::string,Expression>> omq;

// This global is needed for communication between the signal handler
// and the rest of the code. This atomic integer counts the number of times
// Cntl-C has been pressed by not reset by the REPL code.
// volatile sig_atomic_t global_status_flag = 0;

// *****************************************************************************
// install a signal handler for Cntl-C on Windows
// *****************************************************************************
#if defined(_WIN64) || defined(_WIN32)
#include <windows.h>

// this function is called when a signal is sent to the process
BOOL WINAPI interrupt_handler(DWORD fdwCtrlType) {

  switch (fdwCtrlType) {
  case CTRL_C_EVENT: // handle Cnrtl-C
    // if not reset since last call, exit
    if (global_status_flag > 0) { 
      exit(EXIT_FAILURE);
    }
    ++global_status_flag;
    return TRUE;

  default:
    return FALSE;
  }
}

// install the signal handler
inline void install_handler() { SetConsoleCtrlHandler(interrupt_handler, TRUE); }
// *****************************************************************************

// *****************************************************************************
// install a signal handler for Cntl-C on Unix/Posix
// *****************************************************************************
#elif defined(__APPLE__) || defined(__linux) || defined(__unix) ||             \
    defined(__posix)
#include <unistd.h>

// this function is called when a signal is sent to the process
void interrupt_handler(int signal_num) {

  if(signal_num == SIGINT){ // handle Cnrtl-C
    // if not reset since last call, exit
    if (global_status_flag > 0) {
      exit(EXIT_FAILURE);
    }
    ++global_status_flag;
  }
}

// install the signal handler
inline void install_handler() {

  struct sigaction sigIntHandler;

  sigIntHandler.sa_handler = interrupt_handler;
  sigemptyset(&sigIntHandler.sa_mask);
  sigIntHandler.sa_flags = 0;

  sigaction(SIGINT, &sigIntHandler, NULL);
}
#endif
// *****************************************************************************
class Consumer {
public:
  Consumer(imq *inputQueuePtr, omq *outputQueuePtr, int identifier = 0)
  {
    inputQueue = inputQueuePtr;
    outputQueue = outputQueuePtr;
    id = identifier;
  }
  bool runStatus(bool status){
    return status;
  }
  void changeRunStatus(){
    status = false;
  }
  void operator()(Interpreter i)
  {
    while(status == true){
      Expression tempExp;
      std::string tempStr;
      std::string errStr;
      inputQueue->wait_and_pop(tempStr);
      std::istringstream expression(tempStr);
      if(tempStr == ""){
        changeRunStatus();
        return;
      }
      if(!i.parseStream(expression)){
        tempStr = "Invalid Program. Could not parse.";
      }
      else{
        try{
          Expression exp = i.evaluate();
          tempExp = exp;
        }
        catch(const SemanticError & ex){
          errStr = ex.what();
        }	
      }
      std::pair<std::string,Expression> tempPair = {errStr, tempExp};

      outputQueue->push(tempPair);
    }
  }
  int threadStarted(){
    return id;
  }
  void setstartedThread(){
    id = 1;
  }
  void setstoppedThread(){
    id = 0;
  }
private:
  imq *inputQueue;
  omq *outputQueue;
  int id;
  bool status = true;
};


void prompt(){
  std::cout << "\nplotscript> ";
}

std::string readline(){
  std::string line;
  std::getline(std::cin, line);

  if (std::cin.fail() || std::cin.eof()) {
      std::cin.clear(); // reset cin state
      line.clear(); //clear input string
    }

  return line;
}

void error(const std::string & err_str){
  std::cerr << "Error: " << err_str << std::endl;
}

void info(const std::string & err_str){
  std::cout << "Info: " << err_str << std::endl;
}

int eval_from_stream(std::istream & stream, Interpreter interp){

  //Interpreter interp;
  
  if(!interp.parseStream(stream)){
    error("Invalid Program. Could not parse.");
    return EXIT_FAILURE;
  }
  else{
    try{
      Expression exp = interp.evaluate();
      std::cout << exp << std::endl;
    }
    catch(const SemanticError & ex){
      std::cerr << ex.what() << std::endl;
      return EXIT_FAILURE;
    }	
  }

  return EXIT_SUCCESS;
}

int eval_from_file(std::string filename, Interpreter interp){
      
  std::ifstream ifs(filename);
  if(!ifs){
    error("Could not open file for reading.");
    return EXIT_FAILURE;
  }
  
  return eval_from_stream(ifs, interp);
}

int eval_from_command(std::string argexp, Interpreter interp){

  std::istringstream expression(argexp);

  return eval_from_stream(expression, interp);
}

// A REPL is a repeated read-eval-print loop
void repl(Interpreter interp){
  
  imq *input = new imq;
  omq *output = new omq;
  std::pair<std::string,Expression> tempPair = {};
  Consumer con(input, output);

  std::thread consumer_th1(con,interp);
  con.setstartedThread();

  while(!std::cin.eof()){
    global_status_flag = 0;
    
    prompt();
    std::string line = readline();

    if(line.empty()) continue;

    if(line == "%start"){
      if(con.threadStarted() == 0){
        consumer_th1 = std::thread(con,interp);
        con.setstartedThread();
      }
      continue;
    }
    if(line == "%stop"){
      std::string empty;
      if(con.threadStarted() == 1){
        con.setstoppedThread();
        input->push(empty);
        consumer_th1.join();
        if(!input->empty()){
          input->wait_and_pop(empty);
        }
      }
      continue;
    }
    if(line == "%reset"){
      std::string empty;
      if(con.threadStarted() == 1){
        con.setstoppedThread();
        input->push(empty);
        consumer_th1.join();
        if(!input->empty()){
          input->wait_and_pop(empty);
        }
      }
      if(con.threadStarted() == 0){
        consumer_th1 = std::thread(con,interp);
        con.setstartedThread();
      }
      continue;
    }
    if(line == "%exit"){
      std::string empty;
      if(con.threadStarted() == 1){
        con.setstoppedThread();
        input->push(empty);
        consumer_th1.join();
        if(!input->empty()){
          input->wait_and_pop(empty);
        }
      }
      exit(EXIT_SUCCESS);
    }
    if(con.threadStarted() == 0){
      std::cout << "Error: interpreter kernel not running" << std::endl;
      continue;
    }
    input->push(line);

    output->wait_and_pop(tempPair);

    if(tempPair.first == ""){
      std::cout << tempPair.second << std::endl;
    }
    else{
      std::cout << tempPair.first << std::endl;
    }
    
  }
  consumer_th1.join();
  delete input;
  delete output;
}

int main(int argc, char *argv[])
{
  install_handler();
   Interpreter interp;
   std::ifstream ifs(STARTUP_FILE);
    if(!interp.parseStream(ifs)){
    error("Invalid Program. Could not parse.");
    return EXIT_FAILURE;
  }
  else{
    try{
      Expression exp = interp.evaluate();
    }
    catch(const SemanticError & ex){
      std::cerr << ex.what() << std::endl;
      return EXIT_FAILURE;
    }	
  }
  if(argc == 2){
    return eval_from_file(argv[1], interp);
  }
  else if(argc == 3){
    if(std::string(argv[1]) == "-e"){
      return eval_from_command(argv[2], interp);
    }
    else{
      error("Incorrect number of command line arguments.");
    }
  }
  else{
    repl(interp);
  }
    
  return EXIT_SUCCESS;
}

// class Producer {
// public:
//   Producer(imq *inputQueuePtr)
//   {
//     inputQueue = inputQueuePtr;
//   }
//   void operator()(std::string string) const
//   {
//     inputQueue->push(string);
//   }
// private:
//   imq *inputQueue;
// };

