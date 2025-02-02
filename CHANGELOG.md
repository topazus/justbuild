## Release `1.3.0` (UNRELEASED)

A feature release on top of `1.2.0`, backwards compatible.

### Major new features

- New subcommand `just serve` to start a target-level caching service,
  as described in the corresponding design document. 
- `just-mr` is able to back up and retrieve distribution files
  from a remote execution endpoint. This simplifies usage in an
  environment with restricted internet access.
- `just execute` now supports blob splitting as new RPC call. `just
  install` uses this call to reduce traffic if the remote-execution
  endpoint supports blob splitting and the `--remember` option is given.
  In this way, traffic from the remote-execution endpoint can be reduced
  when subsequently installing artifacts with only small local
  differences.

### Other changes

- New script `just-deduplicate-repos` to avoid blow up of the
  `repos.json` in the case of chained imports with common dependencies.
- The built-in `"generic"` rule now supports an argument `"sh -c"`,
  allowing to specify the invocation of the shell (defaulting to
  `["sh", "-c"]`).
- `just describe` also shows the values of the implicit dependencies.
- When `just-mr` executes the action to generate the desired tree of a
  `"git tree"` repository, it can be specified that certain variables
  of the environment can be inherited.

### Fixes

- The cache key used for an export target is now based on the
  export target itself rather than that of the exported target. The
  latter could lead to spurious cache hits, but only in the case
  where the exported target was an explicit file reference, and a
  regular target with the same name existed as well. Where the new
  cache keys would overlap with the old ones, they would refer to
  the same configured targets. However, we used the fact that we
  changed the target cache key to also clean up the serialization
  format to only contain the JSON object describing repository,
  target, and effective configuration, instead of a singleton list
  containing this object. Therefore, old and new cache keys do not
  overlap at all. In particular, no special care has to be taken
  on upgrading or downgrading. However, old target-level cache
  entries will not be used leading potentially to rebuilding of
  some targets.
- Improved portability and update of the bundled dependencies.
- Various minor improvements and typo fixes in the documentation.
- Fixed a race condition in an internal cache of `just execute`
  used for keeping track of running operations.
- The built-in rule `"install"` now properly enforces that the
  resulting stage is well-formed, i.e., without tree conflicts.
- Local execution and `just execute` now correctly create empty
  directories if they are part of the action's input.
- Fixed overwrite of existing symlinks in the output directory
  when using subcommands `install` and `install-cas`.
- The format for target-cache shards was changed to a canonical form.
  The new and old formats do not overlap, therefore the correctness
  of the builds is not affected. In particular, no special care has
  to be taken on upgrading or downgrading. However, some target-level
  cache entries will not be used leading potentially to rebuilding of
  some targets.

## Release `1.2.0` (2023-08-25)

A feature release on top of `1.1.0`, backwards compatible.

### Major new features

- Actions can now define additional execution properties and in
  that way chose a specific remote execution image, as well as a
  factor to scale the time out. This also applies to the built-in
  `generic` rule. Additionally, the remote-execution endpoint can
  be dispatched based on the remote-execution properties using
  the `--endpoint-configuration` argument.
- Relative non-upwards symbolic links are now treated as first-class
  objects. This introduces a new artifact type and allows the free use
  of such symbolic links throughout the build process.
- `just-mr` can now optionally resolve symlinks contained in archives.

### Other changes

- `just-import-git` now supports an option `--plain` to import a
  repository without dependencies.
- Minor changes to the layout of the local build root; in particular,
  left-over execution directories, as well as left-over temporary
  directories of `just-mr`, will eventually get cleaned up by
  garbage collection.
- `just-mr` now supports unpacking tar archives compressed with
  bzip2, xz, lzip, and lzma.
- The option `-P` of `build` and `install-cas` can be used to
  inspect parts of a tree.
- `just-mr` now supports unpacking 7zip archives (with default
  compression) when provided as `"zip"` type repositories.
- The configuration variable `COMPILER_FAMILY` is replaced by the more
  flexible `TOOLCHAIN_CONFIG`, an object which may contain the field
  `FAMILY`. From now on, this object is used to set the compiler family
  (e.g., for GNU, set `{"TOOLCHAIN_CONFIG":{"FAMILY":"gnu"}}`).

### Fixes

- Removed potential uses of `malloc` between `fork` and `exec`.
  This removes the risk of deadlocks on certain combinations of
  `C++` standard library and `libc`.
- The link flags for the final linking now can be set via the
  configuration variable `FINAL_LDFLAGS`; in particular, the stack
  size can easily be adapted. The default stack size is now set to
  8M, removing an overflow on systems where the default stack size
  was significantly lower.
- The man pages are now provided as markdown files, allowing to
  potentially reduce the build dependencies to more standard ones.
- `just-mr` now correctly performs a forced add in order to stage
  all entries in a Git repository. Previously it was possible for
  entries to be skipped inadvertently in, e.g., imported archives
  if `gitignore` files were present.
- Temporary files generated by `just execute` are now created inside
  the local build root.
- `just install-cas` now correctly handles `--raw-tree` also for
  remote-execution endpoints.
- `just install-cas` now, like `just install`, removes an existing
  destination file before installing instead of overwriting.
- Only actions with exit code 0 that generated all required outputs
  are taken from cache, instead of all actions with exit code 0.
  This only affects remote execution, as purely local build didn't
  cache actions with incomplete outputs.

### Changes since `1.2.0~beta3`

- Only actions with exit code 0 that generated all required outputs
  are taken from cache, instead of all actions with exit code 0.
  This only affects remote execution, as purely local build didn't
  cache actions with incomplete outputs.
- Splitting off libraries from the main binary targets to simplify
  cherry-picking future fixes from the head development branch.
- Improvements of the bundled dependency descriptions.
- Update of documentation.

## Release `1.2.0~beta3` (2023-08-22)

Third beta release for the upcoming `1.2.0` release; see release
notes there.

### Changes since `1.2.0~beta2`

- Update and clean up of bundled dependency descriptions
- Improvement of documentation

## Release `1.2.0~beta2` (2023-08-18)

Second beta release for the upcoming `1.2.0` release; see release
notes there.

### Changes since `1.2.0~beta1`

- Clean up of the internal build description of bundled dependencies.
- Clean up of the internal rules, in particular renaming of
  implicit dependency targets.
- Various documentation improvements.

## Release `1.2.0~beta1` (2023-08-16)

First beta release for the upcoming `1.2.0` release; see release
notes there.

## Release `1.1.0` (2023-05-19)

A feature release on top of `1.0.0`, backwards compatible.

### Major new features

- new subcommand `just execute` to start a single node execution
  service
- New subcommand `just gc` to clean up no longer needed cache and
  CAS entries
- `just` now supports authentication to remote execution via TLS
  and mutual TLS
- `just-mr` is now available as C++ binary and supports fetching in parallel

### Important changes

- The option `-D` now accumulates instead of ignoring all but the
  latest occurrence. This is an incompatible change of the command
  line, but not affecting the backwards compatibility of the build.

- The option `-L` of `just-mr` now is an alternative name for option
  `--local-launcher` instead of `--checkout-locations`, and thus
  matching its meaning in `just`. This is an incompatible change of
  the command line, but not affecting the backwards compatibility of
  the build.

### Other changes

- `just install` and `just install-cas` now have a new `--remember`
  option ensuring that the installed artifacts are also mirrored in
  local CAS
- `just analyse` now supports a new option `--dump-export-targets`

### Note

There is a regression in `libgit2` versions `1.6.1` up to and
including `1.6.4` with a fix already committed upstream. This
regression makes `just` unusable when built against those versions.
Therefore, the third-party build description for `libgit2` is still
for version `1.5.2`.

## Release `1.1.0~beta2` (2023-05-15)

Second beta release for the upcoming `1.1.0` release; see release
notes there.

### Changes since `1.1.0~beta1`

- fix a race condition in our use of `libgit2`
- a fix in the error handling of git trees
- fixes to the third-party descriptions of our dependencies; in particular,
  the structure of the `export` targets is cleaned up. These changes should
  not affect package builds.
- various minor fixes to documentation and tests

### Note

There is a regression in `libgit2` versions `1.6.1` upto and
including `1.6.4` with a fix already committed upstream. This
regression makes `just` unusable when built against those versions.
Therefore, the third-party build description for `libgit2` is still
for version `1.5.2`.

## Release `1.1.0~beta1` (2023-04-28)

First beta release for the upcoming `1.1.0` release; see release
notes there.

## Release `1.0.0` (2022-12-12)

Initial stable release.

### Important changes since `1.0.0~beta6`

- built-in rule "tree" added
- clean up of user-defined rules for C++
- various documentation improvements

## Release `1.0.0~beta6` (2022-10-16)

### Important changes since `1.0.0~beta5`

- The "configure" built-in rule now evaluates "target". Also,
  a bug in the computation of the effective configuration
  was fixed.
- Option `--dump-vars` added to `just analyse`
- Rule fixes in propagating `ENV`
- Launcher functionality added to `just-mr`
- `just` now takes the lexicographically first repository as default
  if no main repository is specified

## Release `1.0.0~beta5` (2022-10-19)

First public beta version.
