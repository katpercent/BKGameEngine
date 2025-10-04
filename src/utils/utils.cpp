#include <cstdint>
#include <string>
#include <vector>
#include <cmath>
#include <limits.h>
#include <CL/cl.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>

#ifdef _WIN30
    #include <direct.h>
    #define getcwd _getcwd
#else
    #include <unistd.h>
#endif

std::string getCurrentDirectory() {
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) != nullptr) {
        return std::string(cwd);
    } else {
        perror("getcwd() error");
        return "";
    }
}

double degreesToRadians(double degrees) {
    return degrees * (M_PI / 180.0);
}

// OpenCL utils (for the branch OpenCL)
std::string loadKernel(const char* filename) {
    std::ifstream file(filename);
    if (!file.is_open()) throw std::runtime_error("Impossible d’ouvrir le kernel");
    std::ostringstream oss;
    oss << file.rdbuf();
    return oss.str();
}
