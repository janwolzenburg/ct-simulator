#pragma once

#include <filesystem>
using std::filesystem::path;

#include <string>
using std::string;

constexpr size_t number_of_projections = 40;
constexpr size_t number_of_pixel = 26;
constexpr double measurefield_size = 400;

path GetPath( const string filename );