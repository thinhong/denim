## Resubmission
This is a resubmission. In this version I have:

* Added more information about the package in the DESCRIPTION file

* Added 4 authors of copied/ derived codes to Authors@R with role "ctb" in DESCRIPTION file (Niels Lohman & Florian Loitsch & Bjoern Hoehrmann for json parser, Ingo Berg for muparser files)

* Since no references were used for the methods in the package, references are not included

* Since copyright is held by package authors, Copyright field is not included

## R CMD check results
Duration: 1m 4.5s

❯ checking installed package size ... NOTE
    installed size is  6.1Mb
    sub-directories of 1Mb or more:
      doc    2.4Mb
      help   2.2Mb
      libs   1.4Mb

0 errors ✔ | 0 warnings ✔ | 1 note ✖

* doc and help directories are for the documentation and code examples
* libs contains the compiled C++ code necessary for the package

## revdepcheck results

We checked 0 reverse dependencies, comparing R CMD check results across CRAN and dev versions of this package.

 * We saw 0 new problems
 * We failed to check 0 packages

* This is a new release.
