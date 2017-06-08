#pragma once
#include <exception>
#include <iomanip>
#include <iostream>
#include <stack>
#include <string>
#include "lexer_data.h"
#include "parser_containers.h"

namespace translator {
struct ParserResult {
  ParserTree syntax;
  PropertyContainer identifiers;
};

class Parser {
  LexemData _data;
  int _pos;
  ParserResult _res;

  inline LexemToken token_at(int pos) { return _data.tokens[pos]; }

  inline auto symbol_at(int pos) const { return _data.tokens[pos].symbol; }
  inline auto name_at(int pos) const { return _data.tokens[pos].name; }

  inline bool previous_empty() const {
    return (_res.syntax._lastAdded->type == ParserTokenType::Empty);
  }
#define SYNTAX_EXCEPTION(s)                                              \
  std::cout << '[' << token_at(_pos).row << ':' << token_at(_pos).column \
            << ']' << std::setw(25) << "Syntax error: Expected \'" << s  \
            << "\', got \'" << token_at(_pos).name << "\'("              \
            << token_at(_pos).symbol << ")\n";                           \
  result = false;

#define FIND_COMPARE_SYMBOL(c) (_data.lexem_codes[c] == symbol_at(_pos))

#define INCPOS                         \
  if (_pos >= _data.tokens.size()) {   \
    SYNTAX_EXCEPTION("Out of bounds"); \
    return false;                      \
  } else {                             \
    ++_pos;                            \
  }

  bool Empty() {
    _res.syntax.add(PARSER_NOVALUE, ParserTokenType::Empty);
    _res.syntax.headup();
    return true;
  }
  bool SignalProgram() { return Program(); }
  bool Program() {
    bool result = true;
    _res.syntax.add(PARSER_NOVALUE, ParserTokenType::Program);
    if (FIND_COMPARE_SYMBOL("PROGRAM")) {
      INCPOS;
      result = ProcedureIdentifier();
    } else {
      SYNTAX_EXCEPTION("PROGRAM");
    }
    if (FIND_COMPARE_SYMBOL(";")) {
      INCPOS;
      result = Block();
    } else {
      SYNTAX_EXCEPTION(";");
    }
    if (FIND_COMPARE_SYMBOL(".")) {
      INCPOS;
    } else {
      SYNTAX_EXCEPTION(".");
    }
    _res.syntax.headup();
    return result;
  }
  bool Block() {
    bool result = true;
    _res.syntax.add(PARSER_NOVALUE, ParserTokenType::Block);
    VariableDeclarations();
    if (FIND_COMPARE_SYMBOL("BEGIN")) {
      INCPOS;
      result = StatementsList();
      if (FIND_COMPARE_SYMBOL("END")) {
        INCPOS;
      } else {
        SYNTAX_EXCEPTION("END");
      }
    } else {
      SYNTAX_EXCEPTION("BEGIN");
    }
    _res.syntax.headup();
    return result;
  }

  bool VariableDeclarations() {
    bool result = true;
    _res.syntax.add(PARSER_NOVALUE, ParserTokenType::VariableDeclarations);
    if (FIND_COMPARE_SYMBOL("VAR")) {
      INCPOS;
      DeclarationsList();
    } else {
      Empty();
    }
    _res.syntax.headup();
    return result;
  }

  bool DeclarationsList() {
    bool result = true;
    int i = 0;
    // TODO: while
    while (result) {
      _res.syntax.add(PARSER_NOVALUE, ParserTokenType::DeclarationsList);
      result = Declaration();
      ++i;
    }
    while (--i >= 0) {
      _res.syntax.headup();
    }
    _res.syntax.headup();
    return true;
  }
  bool Declaration() {
    bool result = true;
    _res.syntax.add(PARSER_NOVALUE, ParserTokenType::Declaration);
    result = VariableIdentifier();
    if (!result) {
      return false;
    }
    if (FIND_COMPARE_SYMBOL(":")) {
      INCPOS;
      if (FIND_COMPARE_SYMBOL("INTEGER")) {
        INCPOS;
        if (FIND_COMPARE_SYMBOL(";")) {
          INCPOS;
        } else {
          SYNTAX_EXCEPTION(";");
        }
      } else {
        SYNTAX_EXCEPTION("INTEGER");
      }
    } else {
      SYNTAX_EXCEPTION(":");
    }
    _res.syntax.headup();
    return result;
  }

  bool StatementsList() {
    bool result = true;
    int i = 0;
    while (result) {
      _res.syntax.add(PARSER_NOVALUE, ParserTokenType::StatementsList);
      result = Statements();
      ++i;
    }
    while (--i > 0) {
      _res.syntax.headup();
    }
    _res.syntax.headup();
    return result;
  }

  bool Statements() {
    bool result = true;
    _res.syntax.add(PARSER_NOVALUE, ParserTokenType::Statements);
    result = VariableIdentifier();
    if (result) {
      if (FIND_COMPARE_SYMBOL(":=")) {
        _res.syntax._lastAdded->value = token_at(_pos);
        INCPOS;
        ConditionalExpression();
        if (FIND_COMPARE_SYMBOL(";")) {
          INCPOS;
        } else {
          SYNTAX_EXCEPTION(";");
        }
      } else {
        SYNTAX_EXCEPTION(":=");
      }
    }
    _res.syntax.headup();
    return result;
  }
  bool ConditionalExpression() {
    bool result = true;
    _res.syntax.add(PARSER_NOVALUE, ParserTokenType::ConditionalExpression);
    result = LogicalSummand();
    if (result) {
      result = Logical();
    } else {
      Empty();
    }
    _res.syntax.headup();
    return result;
  }
  bool Logical() {
    bool result = true;
    _res.syntax.add(PARSER_NOVALUE, ParserTokenType::Logical);
    if (FIND_COMPARE_SYMBOL("OR")) {
      _res.syntax._lastAdded->value = token_at(_pos);
      INCPOS;
      result = LogicalSummand();
      result = Logical();
    } else {
      Empty();
    }
    _res.syntax.headup();
    return result;
  }
  bool LogicalSummand() {
    bool result = true;
    _res.syntax.add(PARSER_NOVALUE, ParserTokenType::LogicalSummand);
    result = LogicalMultiplier();
    if (result) {
      result = LogicalMultipliersList();
    }
    _res.syntax.headup();
    return result;
  }

  bool LogicalMultipliersList() {
    bool result = true;
    _res.syntax.add(PARSER_NOVALUE, ParserTokenType::LogicalMultipliersList);
    if (FIND_COMPARE_SYMBOL("AND")) {
      _res.syntax._lastAdded->value = token_at(_pos);
      INCPOS;
      result = LogicalMultiplier();
      if (result) {
        result = LogicalMultipliersList();
      }
    } else {
      Empty();
    }
    _res.syntax.headup();
    return result;
  }

  bool LogicalMultiplier() {
    bool result = true;
    _res.syntax.add(PARSER_NOVALUE, ParserTokenType::LogicalMultiplier);
    if (FIND_COMPARE_SYMBOL("NOT")) {
      _res.syntax._lastAdded->value = token_at(_pos);
      INCPOS;
      result = LogicalMultiplier();
      if (!result) {
        Empty();
      }
    } else if (FIND_COMPARE_SYMBOL("[")) {
      _res.syntax._lastAdded->value = token_at(_pos);
      INCPOS;
      result = ConditionalExpression();
      if (result && FIND_COMPARE_SYMBOL("]")) {
        INCPOS;
      } else {
        SYNTAX_EXCEPTION("]");
      }
    } else {
      result = Expression();
      if (result) {
        result = ComparisonOperator();
        if (result) {
          result = Expression();
          if (!result) {
            SYNTAX_EXCEPTION("Comparison Right Operand");
          }
        } else {
          SYNTAX_EXCEPTION("Comparison Operator");
        }
      } else {
        SYNTAX_EXCEPTION("Comparison Left Operand");
      }
    }
    _res.syntax.headup();
    return result;
  }
  bool ComparisonOperator() {
    bool result = true;
    _res.syntax.add(PARSER_NOVALUE, ParserTokenType::ComparisonOperator);
    if (FIND_COMPARE_SYMBOL(">") || FIND_COMPARE_SYMBOL("<") ||
        FIND_COMPARE_SYMBOL("=") || FIND_COMPARE_SYMBOL(">=") ||
        FIND_COMPARE_SYMBOL("<=") || FIND_COMPARE_SYMBOL("<>")) {
      _res.syntax._lastAdded->value = token_at(_pos);
      INCPOS;
    } else {
      Empty();
      result = false;
    }
    _res.syntax.headup();
    return result;
  }

  bool Expression() {
    bool result = true;
    _res.syntax.add(PARSER_NOVALUE, ParserTokenType::Expression);
    result = VariableIdentifier();
    if (!result) {
      result = UnsignedInteger();
      if (!result) {
        SYNTAX_EXCEPTION("Variable Or Integer");
      }
    }
    _res.syntax.headup();
    return result;
  }
  bool VariableIdentifier() {
    bool result = true;
    _res.syntax.add(PARSER_NOVALUE, ParserTokenType::VariableIdentifier);
    result = Identifier();
    _res.syntax.headup();
    return result;
  }
  bool ProcedureIdentifier() {
    bool result = true;
    _res.syntax.add(PARSER_NOVALUE, ParserTokenType::ProcedureIdentifier);
    result = Identifier();
    _res.syntax.headup();
    return result;
  }
  bool Identifier() {
    bool result = true;
    _res.syntax.add(PARSER_NOVALUE, ParserTokenType::Identifier);
    if (symbol_at(_pos) < 1000) {
      Empty();
      return false;
    }
    _res.syntax._lastAdded->value = token_at(_pos);
    _res.syntax.headup();
    INCPOS;
    return result;
  }
  bool UnsignedInteger() {
    bool result = true;
    _res.syntax.add(PARSER_NOVALUE, ParserTokenType::UnsignedInteger);
    if (symbol_at(_pos) < 500 || symbol_at(_pos) >= 1000) {
      Empty();
      return false;
    }
    _res.syntax._lastAdded->value = token_at(_pos);
    _res.syntax.headup();
    INCPOS;
    return result;
  }

 public:
  Parser(const LexemData& l) : _data(l), _pos(0) {
    _res.identifiers = _data.lexem_codes;
  }
  bool parse() { return SignalProgram(); }
  void print() { _res.syntax.print(); }
};
}  // namespace translator
