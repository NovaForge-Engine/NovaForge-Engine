#pragma once 


class ISystem{
    virtual void Init() = 0;
    virtual void Shutdown() = 0;
    virtual void Update() = 0;
    virtual void FixedUpdate() = 0;
};
