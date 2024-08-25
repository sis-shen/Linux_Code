#pragma once

class nocopy
{
public:
    nocopy(){}
private:
    nocopy(const nocopy&) = delete;
    const nocopy& operator=(const nocopy&) = delete;
};