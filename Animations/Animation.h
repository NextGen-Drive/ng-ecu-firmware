class Animation {
  protected:
    unsigned long animationStartTime = 0;
    unsigned long lastUpdateTime = 0;

  public:
    virtual void tickAnimation() {

    }

    virtual void startAnimation() {

    }
};