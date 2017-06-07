#pragma once
#include <stack>
#include <string>
#include <exception>
#include "parser_containers.h"
#include "lexer_data.h"

namespace translator {
  struct ParserResult
  {
    ParserTree syntax;
    PropertyContainer identifiers;
  };

  class Parser
  {
    LexemData _data;
    int _pos;
    ParserResult _res;

    inline LexemToken token_at(int pos) {
      return _data.tokens[pos];
    }

    inline int symbol_at(int pos) const {
      return _data.tokens[pos].symbol;
    }

    inline bool previous_empty() const {
      return (_res.syntax._lastAdded->type == ParserTokenType::Empty);
    }
#define SYNTAX_EXCEPTION(s) \
    std::cout << "Syntax error!" << "Expected " << s << " at "\
    << token_at(_pos).row << ':' << token_at(_pos).column << ", got "\
    << token_at(_pos).name << std::endl;

#define FIND_COMPARE_SYMBOL(c) \
    (_data.lexem_codes[c] == symbol_at(_pos))\

#define INCPOS if(_pos >= _data.tokens.size())\
 { SYNTAX_EXCEPTION("Out of bounds"); return; }\
 else { ++_pos; }

    void Empty() {
      _res.syntax.add(-1, ParserTokenType::Empty);
      _res.syntax.headup();
    }
    void SignalProgram() {
      return Program();
    }
    void Program() {
      _res.syntax.add(-1, ParserTokenType::Program);
      if (FIND_COMPARE_SYMBOL("PROGRAM")) {
        INCPOS;
        ProcedureIdentifier();
      } else {
        SYNTAX_EXCEPTION("PROGRAM");
      }
      if (FIND_COMPARE_SYMBOL(";")) {
        INCPOS;
        Block();
      } else {
        SYNTAX_EXCEPTION(";");
      }
      if (FIND_COMPARE_SYMBOL(".")) {
        INCPOS;
      } else {
        SYNTAX_EXCEPTION(".");
      }
      _res.syntax.headup();
    }
    void Block() {
      _res.syntax.add(-1, ParserTokenType::Block);
      VariableDeclarations();
      if (!FIND_COMPARE_SYMBOL("BEGIN")) {
        INCPOS;
        StatementsList();
      } else {
        SYNTAX_EXCEPTION("BEGIN");
      }
      if (!FIND_COMPARE_SYMBOL("END")) {
        INCPOS;
      } else {
        SYNTAX_EXCEPTION("END");
      }
      _res.syntax.headup();
    }
    void VariableDeclarations() {
      _res.syntax.add(-1, ParserTokenType::VariableDeclarations);
      if (FIND_COMPARE_SYMBOL("VAR")) {
        INCPOS;
        StatementsList();
      } else {
        Empty();
      }
      _res.syntax.headup();
    }
    void DeclarationsList() {
      _res.syntax.add(-1, ParserTokenType::DeclarationsList);
      Declaration();
      if (_res.syntax._lastAdded->type != ParserTokenType::Empty) {
        DeclarationsList();
      }
      _res.syntax.headup();
    }
    void Declaration() {
      _res.syntax.add(-1, ParserTokenType::Declaration);
      VariableIdentifier();
      if (_res.syntax._lastAdded->type == ParserTokenType::Declaration) {
        Empty();
        return;
      }
      if (FIND_COMPARE_SYMBOL(":")) {
        INCPOS;
        if (FIND_COMPARE_SYMBOL("INTEGER")) {
          INCPOS;
          if (!FIND_COMPARE_SYMBOL(";")) {
            SYNTAX_EXCEPTION(";");
          }
        } else {
          SYNTAX_EXCEPTION("INTEGER");
        }
      } else {
        SYNTAX_EXCEPTION(":");
      }
      _res.syntax.headup();
    }
    void StatementsList() {
      _res.syntax.add(-1, ParserTokenType::StatementsList);
      Statements();
      if (_res.syntax._lastAdded->type != ParserTokenType::Empty) {
        StatementsList();
      }
      _res.syntax.headup();
    }
    void Statements() {
      _res.syntax.add(-1, ParserTokenType::Statements);
      VariableIdentifier();
      if (_res.syntax._lastAdded->type == ParserTokenType::Statements) {
        Empty();
      } else if (!FIND_COMPARE_SYMBOL(":=")) {
        INCPOS;
        ConditionalExpression();
      } else {
        SYNTAX_EXCEPTION(":=");
      }
      _res.syntax.headup();
    }
    void ConditionalExpression() {
      _res.syntax.add(-1, ParserTokenType::ConditionalExpression);
      LogicalSummand();
      Logical();
      _res.syntax.headup();
    }
    void Logical() {
      _res.syntax.add(-1, ParserTokenType::Logical);
      if (FIND_COMPARE_SYMBOL("OR")) {
        INCPOS;
        LogicalSummand();
        Logical();
      } else {
        Empty();
      }
      _res.syntax.headup();
    }
    void LogicalSummand() {
      _res.syntax.add(-1, ParserTokenType::LogicalSummand);
      LogicalMultiplier();
      LogicalMultipliersList();
      _res.syntax.headup();
    }

    void LogicalMultipliersList() {
      _res.syntax.add(-1, ParserTokenType::LogicalMultipliersList);
      if (FIND_COMPARE_SYMBOL("AND")) {
        INCPOS;
        LogicalMultiplier();
        LogicalMultipliersList();
      } else {
        Empty();
      }
      _res.syntax.headup();
    }

    void LogicalMultiplier() {
      _res.syntax.add(-1, ParserTokenType::LogicalMultipliersList);
      if (FIND_COMPARE_SYMBOL("NOT")) {
        INCPOS;
        LogicalMultiplier();
      } else if(FIND_COMPARE_SYMBOL("[")) {
        INCPOS;
        ConditionalExpression();
        if (!FIND_COMPARE_SYMBOL("]")){
          SYNTAX_EXCEPTION("]");
        }
      } else {
        Expression();
        ComparisonOperator();
        Expression();
      }
      _res.syntax.headup();
    }
    void ComparisonOperator() {
      if (FIND_COMPARE_SYMBOL("<") ||
        FIND_COMPARE_SYMBOL("<=") ||
        FIND_COMPARE_SYMBOL("=") ||
        FIND_COMPARE_SYMBOL("<>") ||
        FIND_COMPARE_SYMBOL(">=") ||
        FIND_COMPARE_SYMBOL(">")) {
        INCPOS;
        _res.syntax.add(symbol_at(_pos), 
          ParserTokenType::LogicalMultipliersList);
        _res.syntax.headup();
      } else {
        SYNTAX_EXCEPTION("ComparisonOperator");
      }

    }
    void Expression() {
      _res.syntax.add(symbol_at(_pos), ParserTokenType::ComparisonOperator);
      VariableIdentifier();
      if (_res.syntax._lastAdded->type == ParserTokenType::ComparisonOperator) {
        UnsignedInteger();
        if (_res.syntax._lastAdded->type == ParserTokenType::ComparisonOperator) {
          SYNTAX_EXCEPTION("Expression");
        }
      }
      _res.syntax.headup();
    }
    void VariableIdentifier() {
      Identifier();
      if (_res.syntax._lastAdded->type == ParserTokenType::Identifier) {
        _res.syntax.insert(-1, ParserTokenType::VariableIdentifier, _res.syntax._lastAdded);
        _res.syntax.headup();
      }
    }
    void ProcedureIdentifier() {
      Identifier();
      if (_res.syntax._lastAdded->type == ParserTokenType::Identifier) {
        _res.syntax.insert(-1, ParserTokenType::ProcedureIdentifier, _res.syntax._lastAdded);
        _res.syntax.headup();
      }
    }
    void Identifier() {
      if (symbol_at(_pos) < 1000 && symbol_at(_pos) >= 2000)
        return;
      _res.syntax.add(symbol_at(_pos), ParserTokenType::Identifier);
      INCPOS;
      _res.syntax.headup();
    }
    void UnsignedInteger() {
      if (symbol_at(_pos) < 500 && symbol_at(_pos) >= 1000)
        return;
      _res.syntax.add(symbol_at(_pos), ParserTokenType::UnsignedInteger);
      INCPOS;
      _res.syntax.headup();
    }
         
  public:
    Parser(const LexemData &l) : _data(l), _pos(0)
    { _res.identifiers = _data.lexem_codes; }
    void parse() {
      SignalProgram();
    }
    void print() {
      _res.syntax.print();
    }
  };
}
