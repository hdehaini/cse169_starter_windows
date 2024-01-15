class DOF {
public:
    DOF();
    ~DOF();
    
    void SetValue(float v);
    float GetValue();
    void SetMinMax(float min, float max);

private:
    float value;
    float min;
    float max;
};