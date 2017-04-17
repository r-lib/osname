
#' Human Readable Operating System Name
#'
#' The current version was adapted from `utils::sessionInfo()`.
#'
#' @return A length one character vector.
#'
#' @export

os_name <- function() {
  if (.Platform$OS.type == "windows") {
    utils::win.version()

  } else if (nzchar(Sys.which("uname"))) {
    uname <- system("uname -a", intern = TRUE)
    os <- sub(" .*", "", uname)

    switch(
      os,

      Linux =
        if (file.exists("/etc/os-release")) {
          tmp <- readLines("/etc/os-release")
          t2 <- if (any(startsWith(tmp, "PRETTY_NAME="))) {
            sub("^PRETTY_NAME=", "", grep("^PRETTY_NAME=", tmp, value = TRUE)[1L])
          } else if (any(startsWith(tmp, "NAME"))) {
            sub("^NAME=", "", grep("^NAME=", tmp, value = TRUE)[1L])
          } else {
            "Linux (unknown distro)"
          }
          sub("\"(.*)\"", "\\1", t2)
        } else if (file.exists("/etc/system-release")) {
          readLines("/etc/system-release")
        },

      Darwin = {
        ver <- readLines("/System/Library/CoreServices/SystemVersion.plist")
        ind <- grep("ProductUserVisibleVersion", ver)
        ver <- ver[ind + 1L]
        ver <- sub(".*<string>", "", ver)
        ver <- sub("</string>$", "", ver)
        ver1 <- strsplit(ver, ".", fixed = TRUE)[[1L]][2L]
        sprintf(
          "%s %s %s",
          ifelse(as.numeric(ver1) < 12, "OS X", "macOS"),
          switch(ver1, `4` = "Tiger", `5` = "Leopard", `6` = "Snow Leopard",
                 `7` = "Lion", `8` = "Mountain Lion", `9` = "Mavericks",
                 `10` = "Yosemite", `11` = "El Capitan", `12` = "Sierra",
                 ""), ver)
      },

      SunOS = {
        ver <- system("uname -r", intern = TRUE)
        paste("Solaris", strsplit(ver, ".", fixed = TRUE)[[1L]][2L])
      },

      uname
    )
  }
}
