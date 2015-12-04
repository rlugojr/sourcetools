#include <sourcetools.h>

#define R_NO_REMAP
#include <R.h>
#include <Rinternals.h>

namespace sourcetools {
namespace {

SEXP asSEXP(const tokens::Token& token)
{
  SEXP tokenSEXP;
  PROTECT(tokenSEXP = Rf_allocVector(VECSXP, 4));
  SET_VECTOR_ELT(tokenSEXP, 0, Rf_mkString(token.contents().c_str()));
  SET_VECTOR_ELT(tokenSEXP, 1, Rf_ScalarInteger(token.row()));
  SET_VECTOR_ELT(tokenSEXP, 2, Rf_ScalarInteger(token.column()));

  std::string typeString = toString(token.type());
  SET_VECTOR_ELT(tokenSEXP, 3, Rf_mkString(typeString.c_str()));

  SEXP namesSEXP;
  PROTECT(namesSEXP = Rf_allocVector(STRSXP, 4));

  SET_STRING_ELT(namesSEXP, 0, Rf_mkChar("value"));
  SET_STRING_ELT(namesSEXP, 1, Rf_mkChar("row"));
  SET_STRING_ELT(namesSEXP, 2, Rf_mkChar("column"));
  SET_STRING_ELT(namesSEXP, 3, Rf_mkChar("type"));

  Rf_setAttrib(tokenSEXP, R_NamesSymbol, namesSEXP);

  UNPROTECT(2);
  return tokenSEXP;
}

SEXP asSEXP(const std::vector<tokens::Token>& tokens)
{
  SEXP resultSEXP;
  std::size_t n = tokens.size();
  PROTECT(resultSEXP = Rf_allocVector(VECSXP, n));
  for (std::size_t i = 0; i < n; ++i)
    SET_VECTOR_ELT(resultSEXP, i, asSEXP(tokens[i]));
  UNPROTECT(1);
  Rf_setAttrib(resultSEXP, R_ClassSymbol, Rf_mkString("RTokens"));
  return resultSEXP;
}

} // anonymous namespace
} // namespace sourcetools

extern "C" SEXP sourcetools_tokenize_file(SEXP absolutePathSEXP)
{
  const char* absolutePath = CHAR(STRING_ELT(absolutePathSEXP, 0));
  std::string contents;
  if (!sourcetools::read(absolutePath, &contents))
  {
    Rf_warning("Failed to read file");
    return R_NilValue;
  }

  const auto& tokens = sourcetools::tokenize(contents);
  return sourcetools::asSEXP(tokens);
}

extern "C" SEXP sourcetools_tokenize_string(SEXP stringSEXP)
{
  const char* string = CHAR(STRING_ELT(stringSEXP, 0));
  const auto& tokens = sourcetools::tokenize(string);
  return sourcetools::asSEXP(tokens);
}
