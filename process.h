#pragma once
extern "C" {
#   include <stdio.h>
};

class Process {
public:
    typedef size_t PID_t;

public:
    static bool IsAlive(PID_t pid);
};
