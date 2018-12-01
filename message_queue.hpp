#ifndef MESSAGE_QUEUE_HPP
#define MESSAGE_QUEUE_HPP

#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <utility>
#include <condition_variable>
#include <queue>

template<typename T>
class MessageQueue {
public:
    void push(const T & value){
        std::unique_lock<std::mutex> lock(mes_mutex);
        mes_queue.push(value);
        lock.unlock();
        mes_condition_variable.notify_one();
    }
    bool empty() const{
        std::lock_guard<std::mutex> lock(mes_mutex);
        return mes_queue.empty();
    }
    bool try_pop(T& popped_value){
        std::lock_guard<std::mutex> lock(mes_mutex);
        if (mes_queue.empty()) {
            return false;
        }
        popped_value = mes_queue.front();
        mes_queue.pop();
        return true;
    }
    void wait_and_pop(T& popped_value){
        std::unique_lock<std::mutex> lock(mes_mutex);
        while (mes_queue.empty()) {
            mes_condition_variable.wait(lock);
        }
        popped_value = mes_queue.front();
        mes_queue.pop();
    }
private:
    std::queue<T> mes_queue;
    mutable std::mutex mes_mutex;
    std::condition_variable mes_condition_variable;
};

// #include "message_queue.tpp"

#endif