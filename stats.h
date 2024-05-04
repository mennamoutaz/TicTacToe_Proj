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

using namespace std;

bool getPlayerStats(sqlite3* db, const std::string& email,
    int& pvp_win_count, int& pvp_lose_count, int& pvp_total_games,
    int& pve_win_count, int& pve_lose_count, int& pve_total_games);
void updatePlayerStats(sqlite3* db, const std::string& email,
    int pvp_win_count, int pvp_lose_count, int pvp_total_games,
    int pve_win_count, int pve_lose_count, int pve_total_games);
void handleGameOutcome(sqlite3* db, const std::string& player1Email,
    const std::string& player2Email, int game_result, int gameMode);