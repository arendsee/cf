% Generated by roxygen2: do not edit by hand
% Please edit documentation in R/util.R
\name{.overlaps}
\alias{.overlaps}
\title{Join two tables by overlapping rows}
\usage{
.overlaps(x, xid, xa, xb, y, yid = xid, ya = xa, yb = xb,
  add_id = TRUE)
}
\arguments{
\item{x}{data.frame: table with interval info in each row (with arbitrary
additional columns).}

\item{xid}{character: name of the scaffold column in x}

\item{xa}{character: name of the start position column in x}

\item{xb}{character: name of the stop position column in x}

\item{y}{data.frame: like x, but for a second set of intervals}

\item{yid}{character: name of the scaffold column in y}

\item{ya}{character: name of the start position column in y}

\item{yb}{character: name of the stop position column in y}

\item{add_id}{logical: whether to add a column of indices holding the column
orders of the original tables (named "xid" and "yid", respectively)}
}
\value{
data.frame with columns corresponding to those of x and y and rows
being cases where intervals overlap between the two.
}
\description{
Join two tables by overlapping rows
}
