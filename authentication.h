#pragma once

#include <vector>
#include <limits>
#include <string>
#include <iostream>
#include "sqlite3.h"
#include <cstdint>
#include <conio.h>
#include <chrono>
#include <ctime>
#include <thread>
#include <map>
#include <cstdint>
#include "c_time.h"

using namespace std;

uint64_t customHash(const string& str);
string hashPassword(const string& password_to_be_hashed);
string getPassword();
std::string signup(sqlite3* db);
std::string login(sqlite3* db);