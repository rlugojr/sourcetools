context("Parser")

test_that("agreement on parse of expressions", {

  results <- check_parse(
    "foo ? bar = baz",     # R believes that '?' is higher precedence than '='?
    "foo ? bar <- baz"
  )

  testthat::expect_true(all(results))

})

test_that("parser handles simple control flow", {

  results <- check_parse(
    "if (foo) bar + baz",
    "while (1) 1 + 2",
    "repeat 1 + 2",
    "if (foo) bar else baz",
    "if (foo) bar else if (baz) bat",
    "for (i in 1:10) 1 + 10"
  )

  stopifnot(all(results))

})

test_that("parser handles compound expressions", {

  results <- check_parse(
    "if (foo) while (bar) 1",
    "if (foo) (1 + 2)",
    "{1; 2; 3}",
    "{1 + 2\n3 + 4\n5 + 6}"
  )

  stopifnot(all(results))
})

# generate test cases
if (FALSE) {

  names <- c("foo", "'bar'", "baz")
  unOps <- c("~", "+", "-", "?", "!")
  binOps <- c(
    # "::", ":::", ## TODO: only allowed in certain contexts
    "$", "@", "^", ":", "%foo%",
    "*", "/", "+", "-",
    # "<", ">", "<=", ">=", "==", "!=", ## TODO: The R parser checks these are only found once in expression
    "&", "&&",
    "|", "||",
    "~",
    # "->", "->>", ## TODO: The R parser translates these into `<-` assignments
    "<-", "<<-",
    "=",
    "?"
  )

  n <- 5
  repeat {
    code <- paste(
      paste(sample(unOps, n), collapse = ""),
      paste(sample(names, n, TRUE), sample(binOps, n, TRUE), collapse = " "),
      " end",
      sep = ""
    )

    R <- parse(text = code)
    attributes(R) <- NULL
    ST <- parse_string(code)
    stopifnot(all.equal(R, ST))
  }

}