#include <iostream>
#include <random>
#include "TrafficLight.h"

/* Implementation of class "MessageQueue" */


template <typename T>
T MessageQueue<T>::receive()
{
    std::unique_lock<std::mutex> lock(_mutex);
    _cond.wait(lock,[this]{
        return !_queue.empty();
    });
    
    T msg = std::move(_queue.back());
    
    return msg;
}

template <typename T>
void MessageQueue<T>::send(T &&msg)
{
    std::lock_guard<std::mutex> lock(_mutex);
    _queue.push_back(std::move(msg));
    _cond.notify_one();
}

TrafficLight::TrafficLight()
{
    this->_currentPhase = TrafficLightPhase::red;
}

TrafficLight::~TrafficLight()
{
    
}


void TrafficLight::waitForGreen()
{
    while (true)
    {
        TrafficLightPhase phase = _queue.receive();
        if(phase == TrafficLightPhase::green) 
            return;
    }
}

TrafficLightPhase TrafficLight::getCurrentPhase()
{
    return _currentPhase;
}

void TrafficLight::simulate()
{
    threads.emplace_back(std::thread(&TrafficLight::cycleThroughPhases,this));
}

// virtual function which is executed in a thread
void TrafficLight::cycleThroughPhases()
{
    auto last_update_time = std::chrono::steady_clock::now();
    
    while(true)
    {
        auto current_time = std::chrono::steady_clock::now();
        auto time_since_last_update = std::chrono::duration_cast<std::chrono::milliseconds>(current_time - last_update_time);

        //get value between 4 and 6 seconds.
        srand(time(0));
        int Cycle_Duration = random() % 3 + 4;
        Cycle_Duration = Cycle_Duration*1000;

        if(time_since_last_update > std::chrono::milliseconds(Cycle_Duration))
        {
            _currentPhase = (_currentPhase == TrafficLightPhase::red) ? TrafficLightPhase::green:TrafficLightPhase::red;
            _queue.send(std::move(_currentPhase));
            last_update_time =  std::chrono::steady_clock::now();
            
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    
}

