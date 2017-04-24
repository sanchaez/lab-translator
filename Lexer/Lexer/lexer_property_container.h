#pragma once
#include <cctype>
#include <set>
#include <unordered_map>
#include <vector>

namespace translator {
/// Container for lexems and codes
class PropertyContainer {
 public:
  PropertyContainer(const int inc = 0) : m_current_index(inc) {}
  PropertyContainer(std::vector<std::string>& data, const int inc = 0)
      : m_current_index(inc) {
    build_internal_maps(data, inc);
    gen_allowed_symbols();
  }

  // The map must be one-to-one for PropertyContainer to construct.
  PropertyContainer(std::unordered_map<std::string, int>& data_map)
      : m_lexem2code_map(data_map) {
    for (auto& x : data_map) {
      m_code2lexem_map[x.second] = x.first;
    }
    gen_allowed_symbols();
  }
  PropertyContainer(std::unordered_map<int, std::string>& data_map)
      : m_code2lexem_map(data_map) {
    for (auto& x : data_map) {
      m_lexem2code_map[x.second] = x.first;
    }
    gen_allowed_symbols();
  }

  /// Append lexem to container
  /// Lexem code is generated automatically
  void append(const std::string& lexem) {
    // TODO: jump over already defined lexems
    set(lexem, m_current_index);
    ++m_current_index;
  }

  /// Set lexem/code pair
  void set(const std::string& lexem, int index) {
    /*
    //remove previous
    remove(lexem);
    remove(index);
    */
    m_code2lexem_map[index] = lexem;
    m_lexem2code_map[lexem] = index;
  }

  /// Remove lexem
  void remove(const std::string& lexem) {
    if (m_lexem2code_map.count(lexem)) {
      auto code = m_lexem2code_map[lexem];
      m_code2lexem_map.erase(code);
      m_lexem2code_map.erase(lexem);
    }
  }

  /// Remove code
  void remove(const int code) {
    if (m_code2lexem_map.count(code)) {
      auto lexem = m_code2lexem_map[code];
      m_lexem2code_map.erase(lexem);
      m_code2lexem_map.erase(code);
    }
  }

  /// Used to find lexem by code
  auto operator[](const int code) const {
    return m_code2lexem_map.count(code) <= 0 ? m_code2lexem_map.at(code)
                                             : std::string();
  }

  /// Used to find lexem by value
  auto operator[](const std::string& lexem) const {
    return m_lexem2code_map.count(lexem) <= 0 ? m_lexem2code_map.at(lexem) : -1;
  }

  /// Determine if symbol is allowed
  bool isallowed(char c) { return (allowed_symbols.count(c) > 0); }

 private:
  /// Builds an internal map used to quickly find code by lexem
  void build_internal_maps(const std::vector<std::string>& data,
                           const int increment) {
    int i = m_current_index;
    for (const auto& x : data) {
      append(x);
    }
  }

  // Generates allowed symbols from data
  void gen_allowed_symbols() {
    for (auto& x : m_lexem2code_map) {
      for (char c : x.first) {
        if (!isalnum(c)) {
          allowed_symbols.insert(c);
        }
      }
    }
  }

  int m_current_index;
  std::set<char> allowed_symbols;
  std::unordered_map<int, std::string> m_code2lexem_map;
  std::unordered_map<std::string, int> m_lexem2code_map;
};
}  // namespace translator