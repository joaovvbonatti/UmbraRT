#pragma once
#include <string>

#include "Model.h"

class ModelLoader {
public:
    static Model load(const std::string& path);
};
