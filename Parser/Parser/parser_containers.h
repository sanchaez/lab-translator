#pragma once
#include <iostream>
#include <ostream>
#include <string>
#include <stack>
#include <vector>
#include <algorithm>
#include <memory>
namespace translator {
  enum class ParserTokenType
  {
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
    String,
    UnsignedInteger,
    DigitsString,
    Digit,
    Letter
  };

  std::ostream& operator<<(std::ostream& stream, ParserTokenType &rhs) {
    stream << '$';
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
    case translator::ParserTokenType::String:
      stream << "string";
      break;
    case translator::ParserTokenType::UnsignedInteger:
      stream << "unsigned-integer";
      break;
    case translator::ParserTokenType::DigitsString:
      stream << "digits-string";
      break;
    case translator::ParserTokenType::Digit:
      stream << "digit";
      break;
    case translator::ParserTokenType::Letter:
      stream << "letter";
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


  struct ParserTreeNode
  {
    ParserTreeNode(): value(), links(), type(ParserTokenType::Empty), parent() {}
    ParserTreeNode(int v,
      const ParserTokenType &t,
      const pParserTreeNodeLinks l,
      pParserTreeNodeWeak p) : value(v), links(l), type(t), parent(p) {}
    ParserTreeNode(const ParserTreeNode& x): 
      ParserTreeNode(x.value, x.type, x.links, x.parent) {}

    int level() {
      auto x = parent.lock();
      int i = 0;
      while (x) {
        x = x->parent.lock();
        ++i;
      }
      return i;
    }

    int value;
    ParserTokenType type;
    pParserTreeNodeLinks links;
    pParserTreeNodeWeak parent;
  };

  struct ParserTree
  {
    pParserTreeNode _top;
    pParserTreeNode _head;
    pParserTreeNode _lastAdded;
    ParserTree() : _top(std::make_shared<ParserTreeNode>()) {
      _top->type = ParserTokenType::SignalProgram; 
      _head = _top; 
      _lastAdded = _top; }

    //add to specified
    void add(pParserTreeNode &what, pParserTreeNode &parent) {
      what->parent = parent;
      parent->links.push_back(what);
      _head = what;
    }

    void add(int v,
      const ParserTokenType &t,
      const pParserTreeNodeLinks& l, 
      pParserTreeNode &p) {
      pParserTreeNode ptr = std::make_shared<ParserTreeNode>(v, t, l, p);
      p->links.emplace_back(ptr);
      _lastAdded = ptr;
      _head = ptr;
    }

    //add to head
    void add(pParserTreeNode &what) {
      what->parent = _head;
      _head->links.emplace_back(what);
      _lastAdded = what;
      _head = what;
    }
    void add(int v,
        const ParserTokenType &t,
        const pParserTreeNodeLinks& l = pParserTreeNodeLinks() ) {
      pParserTreeNode ptr = std::make_shared<ParserTreeNode>(v, t, l, _head);
      _head->links.emplace_back(ptr);
      _lastAdded = ptr;
      _head = ptr;
    }
    void headup() {
      _head = _head->parent.lock();
    }
    void insert(int v,
      const ParserTokenType &t,
      pParserTreeNode &child) {
      pParserTreeNodeLinks l = child->links;
      pParserTreeNode x = std::make_shared<ParserTreeNode>(v, t, l, child->parent.lock());
      child->parent = x;
      auto parent_links = x->parent.lock()->links;
      auto found = std::find(parent_links.begin(), parent_links.end(), child);
      if (found != parent_links.end()) {
        parent_links.erase(found);
      }
      add(x, x->parent.lock());
    }
    //remove from the tree
    void remove(pParserTreeNode &what) {
      _head = what->parent.lock();
      auto parent_links = what->parent.lock()->links;
      auto found = std::find(parent_links.begin(), parent_links.end(), what);
      if (found != parent_links.end()) {
        parent_links.erase(found);
      }
    }

    void print() {
      std::stack<pParserTreeNode> nodes;
      nodes.push(_top);
      while (!nodes.empty()) {
        pParserTreeNode current = nodes.top();
        auto level = current->level();
        if (current->links.empty()) {
          for (size_t i = 0; i < level; ++i) {
            std::cout << char(192) << char(196);
          }
          std::cout << char(192) << char(196);
        } else {
          for (size_t i = 0; i < level; ++i) {
            std::cout << char(179) << ' ';
          }
          if (level) {
            std::cout << char(192) << char(196);
          }
        }


        std::cout << current->type << " " << current->value << std::endl;
        nodes.pop();
        for (auto x : current->links) {
          nodes.push(x);
        }
      }
    }
  };
}