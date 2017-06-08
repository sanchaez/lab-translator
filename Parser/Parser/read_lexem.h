#pragma once
#include <fstream>
#include <iostream>
#include <string>
#include "lexer_data.h"

#define FILEERROR(msg)                        \
  std::cout << "Error: " << msg << std::endl; \
  return 0;

namespace translator {
LexemData load_from_file(const std::string& filename) {
  std::ifstream file(filename);
  LexemData m_lexem;
  // check file
  std::string buf;
  try {
    std::getline(file, buf);
    if (buf[1] != '~') {
      FILEERROR("File is not a lexer output!")
    }
  } catch (std::ifstream::failure& e) {
    FILEERROR(e.what())
  }
  // skip headings
  std::getline(file, buf);

  // read loop
  try {
    // lexem array first
    file >> buf;
    while (buf[0] != '~') {
      translator::LexemToken t;
      t.name = buf;
      file >> t.symbol >> t.row >> t.column;
      m_lexem.tokens.push_back(t);
      file >> buf;
    }
    // skip headings
    std::getline(file, buf);
    std::getline(file, buf);
    // lexem hash second
    while (!file.eof()) {
      int index;
      file >> buf >> index;
      m_lexem.lexem_codes.set(buf, index);
    }

  } catch (const std::ifstream::failure& e) {
    FILEERROR(e.what())
  }
  return m_lexem;
}

}  // namespace translator