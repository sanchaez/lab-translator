#pragma once
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
  }

  // The map must be one-to-one for PropertyContainer to construct.
  PropertyContainer(std::unordered_map<std::string, int>& data_map)
      : m_lexem2code_map(data_map) {
    for (auto& x : data_map) {
      m_code2lexem_map[x.second] = x.first;
    }
  }
  PropertyContainer(std::unordered_map<int, std::string>& data_map)
      : m_code2lexem_map(data_map) {
    for (auto& x : data_map) {
      m_lexem2code_map[x.second] = x.first;
    }
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

 private:
  /// Builds an internal map used to quickly find code by lexem
  void build_internal_maps(const std::vector<std::string>& data,
                           const int increment) {
    int i = m_current_index;
    for (const auto& x : data) {
      append(x);
    }
  }

  int m_current_index;
  std::unordered_map<int, std::string> m_code2lexem_map;
  std::unordered_map<std::string, int> m_lexem2code_map;
};
}  // namespace translator