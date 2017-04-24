/* Predefined lexem */

#pragma once
#include "lexer_property_container.h"

translator::PropertyContainer predefined_lexem(
    std::unordered_map<std::string, int>{
        {";", int(';')}, {".", int('.')}, {"<", int('<')}, {">", int('>')},
        {"=", int('=')},  {"[", int('[')}, {"]", int(']')}, {":", int(':')},
        {":=", 301},      {"<=", 302},     {">=", 303},     {"<>", 304},
        {"PROGRAM", 401}, {"BEGIN", 402},  {"END", 403},    {"VAR", 404},
        {"OR", 405},      {"AND", 406},    {"NOT", 407},    {"INTEGER", 408}});