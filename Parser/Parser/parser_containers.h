#pragma once
#include <algorithm>
#include <iostream>
#include <memory>
#include <ostream>
#include <stack>
#include <string>
#include <vector>
#include "lexer_data.h"

namespace translator {
enum class ParserTokenType {
  Empty,
  SignalProgram,
  Program,
  Block,
  VariableDeclarations,
  DeclarationsList,
  Declaration,
  StatementsList,
  Statements,
  ConditionalExpression,
  Logical,
  LogicalSummand,
  LogicalMultipliersList,
  LogicalMultiplier,
  ComparisonOperator,
  Expression,
  VariableIdentifier,
  ProcedureIdentifier,
  Identifier,
  UnsignedInteger,
};
std::ostream& operator<<(std::ostream& stream, LexemToken& rhs) {
  if (rhs.symbol > 0) {
    stream << rhs.name << '(' << rhs.symbol << ")[" << rhs.row << ':'
           << rhs.column << ']';
  }
  return stream;
}
std::ostream& operator<<(std::ostream& stream, ParserTokenType& rhs) {
  switch (rhs) {
    case translator::ParserTokenType::Empty:
      stream << "empty";
      break;
    case translator::ParserTokenType::SignalProgram:
      stream << "signal-program";
      break;
    case translator::ParserTokenType::Program:
      stream << "program";
      break;
    case translator::ParserTokenType::Block:
      stream << "block";
      break;
    case translator::ParserTokenType::VariableDeclarations:
      stream << "variable-declarations";
      break;
    case translator::ParserTokenType::DeclarationsList:
      stream << "declarations-list";
      break;
    case translator::ParserTokenType::Declaration:
      stream << "declaration";
      break;
    case translator::ParserTokenType::StatementsList:
      stream << "statements-list";
      break;
    case translator::ParserTokenType::Statements:
      stream << "statements";
      break;
    case translator::ParserTokenType::ConditionalExpression:
      stream << "conditional-expression";
      break;
    case translator::ParserTokenType::Logical:
      stream << "logical";
      break;
    case translator::ParserTokenType::LogicalSummand:
      stream << "logical-summand";
      break;
    case translator::ParserTokenType::LogicalMultipliersList:
      stream << "logical-multipliers-list";
      break;
    case translator::ParserTokenType::LogicalMultiplier:
      stream << "logical-multiplier";
      break;
    case translator::ParserTokenType::ComparisonOperator:
      stream << "comparison-operator";
      break;
    case translator::ParserTokenType::Expression:
      stream << "expression";
      break;
    case translator::ParserTokenType::VariableIdentifier:
      stream << "variable-identifier";
      break;
    case translator::ParserTokenType::ProcedureIdentifier:
      stream << "procedure-identifier";
      break;
    case translator::ParserTokenType::Identifier:
      stream << "identifier";
      break;
    case translator::ParserTokenType::UnsignedInteger:
      stream << "unsigned-integer";
      break;
    default:
      stream << "unknown";
      break;
  }
  return stream;
}
struct ParserTreeNode;
using pParserTreeNode = std::shared_ptr<ParserTreeNode>;
using pParserTreeNodeWeak = std::weak_ptr<ParserTreeNode>;
using pParserTreeNodeLinks = std::vector<pParserTreeNode>;

#define PARSER_NOVALUE ParserStatement()
struct ParserStatement {
  std::vector<LexemToken> tokens;
  const int row() const { return (tokens.empty()) ? -1 : tokens[0].row; }
  const int column() const { return (tokens.empty()) ? -1 : tokens[0].column; }
  void add(const LexemToken& rhs) { tokens.push_back(rhs); }
  void add(const std::vector<LexemToken>& rhs) {
    tokens.reserve(tokens.size() + rhs.size());
    tokens.insert(tokens.end(), std::make_move_iterator(rhs.begin()),
                  std::make_move_iterator(rhs.end()));
  }
  void add(const ParserStatement& rhs) { add(rhs.tokens); }
};
std::ostream& operator<<(std::ostream& stream, ParserStatement& rhs) {
  if (!rhs.tokens.empty()) {
    stream << '[' << rhs.row() << ':' << rhs.column() << ']';
    // names
    stream << " $";
    for (auto x : rhs.tokens) {
      stream << ' ' << x.name;
    }
    stream << " #";
    // symbols
    for (auto x : rhs.tokens) {
      stream << ' ' << x.symbol;
    }
  }
  return stream;
}

struct ParserTreeNode {
  ParserTreeNode()
      : value(PARSER_NOVALUE),
        links(),
        type(ParserTokenType::Empty),
        parent() {}
  ParserTreeNode(const ParserStatement& v,
                 const ParserTokenType& t,
                 const pParserTreeNodeLinks l,
                 pParserTreeNodeWeak p)
      : value(v), links(l), type(t), parent(p) {}
  ParserTreeNode(const ParserTreeNode& x)
      : ParserTreeNode(x.value, x.type, x.links, x.parent) {}

  int level() {
    auto x = parent.lock();
    int i = 0;
    while (x) {
      x = x->parent.lock();
      ++i;
    }
    return i;
  }

  ParserStatement value;
  ParserTokenType type;
  pParserTreeNodeLinks links;
  pParserTreeNodeWeak parent;
};

struct ParserTree {
  pParserTreeNode _top;
  pParserTreeNode _head;
  pParserTreeNode _lastAdded;
  ParserTree() : _top(std::make_shared<ParserTreeNode>()) {
    _top->type = ParserTokenType::SignalProgram;
    _head = _top;
    _lastAdded = _top;
  }

  // add to specified
  pParserTreeNode& add(pParserTreeNode& what, pParserTreeNode& parent) {
    what->parent = parent;
    parent->links.emplace_back(what);
    _lastAdded = what;
    _head = what;
    return what;
  }

  pParserTreeNode& add(const ParserStatement& v,
                       const ParserTokenType& t,
                       const pParserTreeNodeLinks& l,
                       pParserTreeNode& p) {
    pParserTreeNode ptr = std::make_shared<ParserTreeNode>(v, t, l, p);
    p->links.emplace_back(ptr);
    _lastAdded = ptr;
    _head = ptr;
    return _lastAdded;
  }

  // returns a shared_ptr to added element
  pParserTreeNode& add(pParserTreeNode& what) {
    what->parent = _head;
    _head->links.emplace_back(what);
    _lastAdded = what;
    _head = what;
    return what;
  }

  pParserTreeNode& add(const ParserStatement& v,
                       const ParserTokenType& t,
                       const pParserTreeNodeLinks& l = pParserTreeNodeLinks()) {
    pParserTreeNode ptr = std::make_shared<ParserTreeNode>(v, t, l, _head);
    _head->links.emplace_back(ptr);
    _lastAdded = ptr;
    _head = ptr;
    return _lastAdded;
  }

  void headup() { _head = _head->parent.lock(); }

  // remove from the tree
  void remove(pParserTreeNode& what) {
    _head = what->parent.lock();
    auto parent_links = what->parent.lock()->links;
    auto found = std::find(parent_links.begin(), parent_links.end(), what);
    if (found != parent_links.end()) {
      parent_links.erase(found);
    }
  }

#define VLINE '|'         // char(179)
#define HLINE '-'         // char(196)
#define LEFTCORNER '+'    // char(192)
#define ISECTIONLEFT LEFTCORNER  // char(195)
#define ISECTIONTOP LEFTCORNER   // char(193)
#define FILLCHAR ' '

  // print to std::ostream
  void print(std::ostream& stream = std::cout) {
    std::stack<pParserTreeNode> nodes;
    std::stack<int> nodes_children;
    std::vector<int> level_sizes;
    nodes.push(_top);
    while (!nodes.empty()) {
      pParserTreeNode current = nodes.top();
      int current_level = current->level();
      nodes.pop();
      level_sizes.resize(current_level + 1);
      int level_size = 0;
      // get links and current_level size (reversed)
      for (auto x = current->links.rbegin(); x != current->links.rend(); ++x) {
        nodes.push(*x);
        ++level_size;
      }
      level_sizes[current_level] = level_size;
      // finish all closed lines
      if (nodes.empty()) {
        int i = 0;
        // skip empty space
        while (level_sizes[i] <= 0) {
          stream << FILLCHAR << FILLCHAR;
          ++i;
        }
        stream << FILLCHAR << LEFTCORNER;
        ++i;
        for (; i < current_level - 1; ++i) {
          if (level_sizes[i] > 0) {
            stream << FILLCHAR << ISECTIONTOP;
          } else {
            stream << HLINE << HLINE;
          }
        }
        stream << HLINE;
      } else if (current_level) {
        for (int i = 0; i < current_level - 1; i++) {
          if (level_sizes[i] > 0) {
            stream << FILLCHAR << VLINE;
          } else {
            stream << FILLCHAR << FILLCHAR;
          }
        }
        if (level_sizes[current_level - 1] > 1) {
          stream << FILLCHAR << ISECTIONLEFT;
        } else {
          stream << FILLCHAR << LEFTCORNER;
        }
        stream << HLINE;
        --level_sizes[current_level - 1];
      }
      stream << '<' << current->type << " \"" << current->value << "\">\n";
    }
  }
};
}  // namespace translator
