Importing objects to CAS
========================

Motivation
----------

Roots in `just` builds are typically Git trees. Such artifacts should ideally
be available already in CAS to avoid any expensive fetches. It is not uncommon
for checkouts or other pre-fetched data to be present locally and to want to
quickly integrate them into a multi-repository build.

A typical approach is to pack the process that generates the root directory
and all its content into a script (or a sequence of commands) and place it as
the fetch command of a `git tree` repository inside the multi-repository
configuration file provided to `just-mr`. This however comes with two
inefficiencies: firstly, the user has to specify a priori the expected Git
tree identifier of the root, and secondly, if the tree is not already cached
then `just-mr` will have to (re)run the (usually expensive) fetch command and
hash the generated directory in order to store the root in its Git cache.

Proposal
--------

We propose a new `just add-to-cas` subcommand which takes in a filesystem
path and provides its Git hash to standard output. A corresponding
entry is also added to the local (file, executable, or tree) CAS. While
hashing directories into Git trees is the most important use-case, the
subcommand will allow the hashing of blobs as well.

Note that `just-mr` by default looks both into the local CAS and the Git cache
for any blobs or trees it might need to make available. In particular, build
roots with trees already in local CAS would only need to be additionally
imported into the Git cache, which is a cheap local operation.

### CAS locations

`just` provides a default local build root location for where the local CAS
should be stored. However, a useful use-case of this new subcommand would be
to populate a specific (and possibly existing) CAS. Therefore an option to
specify the local build root will be available.

### Remote CAS

Similarly, we want to be able to populate a remote CAS as well, e.g., to
prepare roots for a future build. For this reason, the subcommand will allow
to specify a remote-execution endpoint, with the understanding that the
generated local CAS entry should be synced with the remote. Additionally,
we will support computing hashes also in compatible mode, set via an
appropriate option.

### Symbolic links

If the given path points to a symbolic link, by default the link will be
followed and we will hash the object the symlink resolves to. However, we will
also support, via an appropriate option, to hash the symlink content as-is
instead.

If the given path points to a directory, the treatment of contained symbolic
links will default to allowing only non-upwards symlinks. To mirror options
available in `just-mr` repository descriptions, we will also support options
to either ignore, or fully resolve symlinks in the generated Git trees.

### Other notes

The only mandatory argument is the path to the filesystem object to be hashed,
with all other options optional, as sensible defaults exist already in `just`.

Logging options will also be available for this subcommand.

While the main purpose of this subcommand is to add the hashed Git object to
CAS, and considering that hashing a directory is a non-trivial operation for
the command-line `git` tool, a pure computation of the hashes _without_
generating a CAS entry might still be of interest and available as an option.
This is, however, not useful in typical situations.

Auxiliary changes
-----------------

### `just-mr` to support `archive` subcommand

Build sources many times come in the form of archives, which allow efficient
storage and backup, and permit easy versioning of build dependencies. Also
build artifacts, for example resulting binaries (together with required headers
when building statically), are typically shipped as archives.

The `just-mr` tool will therefore implement a new subcommand `archive` which,
given a tree identifier, with the understanding that the tree is available
locally (either in local CAS or the Git cache), will produce an archive from
the content of that tree.

The archive content will be written to standard output, thus allowing the usual
piping and redirection of binary streams. Optionally, the subcommand will be
able to write to a file instead.

By default, for reproducibility reasons, the archiving format will be a tarball.
Options will be added to produce other archive types, as supported by `just-mr`.
