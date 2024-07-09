#pragma once

class Animation {
  protected:
    unsigned long animationStartTime = 0;
    unsigned long lastUpdateTime = 0;

  public:
    bool isCompleted = false;

    virtual ~Animation(){

    }

    virtual void tickAnimation() {

    }

    virtual void startAnimation() {
      animationStartTime = millis();
    }
};