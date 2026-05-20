#ifndef OBSERVER_H
#define OBSERVER_H
#include<vector>
#include<algorithm>

/// OBSERVER DESIGN PATTERN
/// Image is the Observable and the UI iwndows acts as an observer
/// class template 
template<typename T>
class Observer{
    public:
    virtual ~Observer()=default;
    virtual void update(T& val)=0;
};

template<typename T>
class Observable{
    private:
    std::vector<Observer<T>*> observers;
    public:
    void addObserver(Observer<T>* obs)
    {
        if(obs) observers.push_back(obs);
    }
    void removeObserver(Observer<T>* obs){
        observers.erase(std::remove(observers.begin(),observers.end(),obs),observers.end());

    }
    /// "notifies" the updates to all observers
    void notify(T& val){
        for(Observer<T>* obs: observers)
        {if(obs)
        obs->update(val);
        }
    }
};
#endif
