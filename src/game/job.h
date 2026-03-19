#ifndef GAME_JOB_H
#define GAME_JOB_H

#include <vector>

class State;

class Job {
public:
    virtual ~Job() {}
    
    virtual bool update(State* state) = 0;
    virtual void render(State* state) {};
    virtual std::vector<Job*> finish() { return {}; }

    virtual bool isUnique() const { return false; }
    virtual int getType() const = 0; // must return a consistent int per Job subclass
    
    virtual void pause() {}
    virtual void resume() {}
};

#endif // GAME_JOB_H