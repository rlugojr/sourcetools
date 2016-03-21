#ifndef SOURCE_TOOLS_DIAGNOSTICS_CHECKERS_H
#define SOURCE_TOOLS_DIAGNOSTICS_CHECKERS_H

#include <sourcetools/parse/Node.h>
#include <sourcetools/diagnostics/Diagnostic.h>

namespace sourcetools {
namespace diagnostics {
namespace checkers {

class CheckerBase
{
public:
  typedef tokens::Token Token;
  typedef tokens::TokenType TokenType;
  typedef parser::Node Node;

  virtual void apply(const Node* pNode, Diagnostics* pDiagnostics) const = 0;
  virtual ~CheckerBase() {}
};

/**
 * Warn about code of the form:
 *
 *    x == NULL
 *
 * The user likely intended to check if a value was NULL,
 * and in such a case should use `is.null()` instead.
 */
class ComparisonWithNullChecker : public CheckerBase
{
public:
  void apply(const Node* pNode, Diagnostics* pDiagnostics) const
  {
    const Token& token = pNode->token();
    bool isEquals =
      token.isType(tokens::OPERATOR_EQUAL) ||
      token.isType(tokens::OPERATOR_NOT_EQUAL);

    if (!isEquals)
      return;

    if (pNode->children().size() != 2)
      return;

    Node* pLhs = pNode->children()[0];
    Node* pRhs = pNode->children()[1];

    if (pLhs->token().isType(tokens::KEYWORD_NULL) ||
        pRhs->token().isType(tokens::KEYWORD_NULL))
    {
      pDiagnostics->addWarning(
        "Use 'is.null()' to check if an object is NULL",
        pNode->range());
    }
  }
};

/**
 * Warn about code of the form:
 *
 *    if (x = 1) { ... }
 *
 * The user likely intended to write 'if (x == 1)'.
 */
class AssignmentInIfChecker : public CheckerBase
{
public:
  void apply(const Node* pNode, Diagnostics* pDiagnostics) const
  {
    if (!pNode->token().isType(tokens::KEYWORD_IF))
      return;

    if (pNode->children().size() < 1)
      return;

    Node* pCondition = pNode->children()[0];
    if (!pCondition->token().isType(tokens::OPERATOR_ASSIGN_LEFT_EQUALS))
      return;

    pDiagnostics->addWarning(
      "Using '=' for assignment in 'if' condition",
      pCondition->range());

  }
};

/**
 * Warn about vectorized '&' or '|' used in
 * 'if' statements. The scalar forms, '&&' and '||',
 * are likely preferred.
 */
class ScalarOpsInIfChecker : public CheckerBase
{
public:
  void apply(const Node* pNode, Diagnostics* pDiagnostics) const
  {
    if (!pNode->token().isType(tokens::KEYWORD_IF))
      return;

    if (pNode->children().size() < 1)
      return;

    Node* pCondition = pNode->children()[0];
    const Token& token = pCondition->token();
    if (token.isType(tokens::OPERATOR_AND_VECTOR))
    {
      pDiagnostics->addInfo(
        "Prefer '&&' to '&' in 'if' statement condition",
        pCondition->range());
    }
    else if (token.isType(tokens::OPERATOR_OR_VECTOR))
    {
      pDiagnostics->addInfo(
        "Prefer '||' to '|' in 'if' statement condition",
        pCondition->range());
    }
  }
};

/**
 * Warn about unused computations, e.g.
 *
 *    foo <- function(x) {
 *       x < 1
 *       print(x)
 *    }
 *
 * For example, in the above code, it's possible that the user
 * intended to assign 1 to x, or use that result elsewhere.
 *
 * Don't warn if the expression shows up as the last statement
 * within a parent function's body.
 */
class UnusedResultChecker : public CheckerBase
{
public:
  void apply(const Node* pNode, Diagnostics* pDiagnostics) const
  {
    if (pNode->parent() == NULL)
      return;

    const Token& parentToken = pNode->parent()->token();
    bool isTopLevelContext =
      parentToken.isType(tokens::ROOT) ||
      parentToken.isType(tokens::LBRACE);

    if (!isTopLevelContext)
      return;

    if (parentToken.isType(tokens::LBRACE))
    {
      const std::vector<Node*> siblings = pNode->parent()->children();
      if (pNode == siblings[siblings.size() - 1])
        return;
    }

    const Token& token = pNode->token();
    if (!tokens::isOperator(token))
      return;

    if (tokens::isAssignmentOperator(token))
      return;


    pDiagnostics->addInfo(
      "result of computation is not used",
      pNode->range());
  }
};

} // namespace checkers
} // namespace diagnostics
} // namespace sourcetools

#endif /* SOURCE_TOOLS_DIAGNOSTICS_CHECKERS_H */