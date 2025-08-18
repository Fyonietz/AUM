#ifndef AUTH
#define AUTH
#pragma once
#ifdef _WIN32
    #define EXPORT __declspec(dllexport)
#else   
    #define EXPORT __attribute__((visibility("default")))
#endif
#include <boost/tokenizer.hpp>
#include <openssl/evp.h>
#include <sstream>
#include <iomanip>
EXPORT std::string checker(std::string username);
EXPORT std::string hash(const std::string& input);
EXPORT bool auth_cond(bool cond);
#endif // !AUTH