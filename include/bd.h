#pragma once
#include "common.h"
#include <mutex>

class BD
{
private:
    json dat;
    std::string file;

public:
    BD(std::string path);
    ~BD();
    std::mutex lock;
    json& get();
    void save();
};

extern BD conf;
extern BD bd;
