# Development Practices

## Directory Hierarchy

Follow the Maven directory-hierarchy convention for all languages.

 - Repository Root
 - ``src``
   - ``main``
     - ``java``: Java code, with intermediary reverse-domain-name directories.
     - ``cpp``: C++ code and headers. Intermediate directories are allowed where desired, but should not use reverse-domain-naming.
     - ``scripts``: Bash/Awk/Python/Perl, etc scripts for use by build/deployment automation.
       This is a suggested location, and other locations may be used, as necessitated by build and automation tools.
   - ``test``
     - ``java``: Java unit tests, with intermediary reverse-domain-name directories.
     - ``cpp``: C++ Unit tests.
     - ``integration``: Integration test code, etc.
 - ``<sub-project directories>``
   - follow the same ``src/main/[java|cpp]`` directory hierarchy

When naming intermediate directories inside of ``src/main/[java|cpp]``, use ``alllowercase`` directory names.
For naming source files in both Java and C++, reuse the spelling of the primary class, function, or family defined by the file.

## C++
The following practices will be adhered to for all C++ Development.

Use the following naming conventions for new C++ code.

 - Never define a function or variable starting with an underscore (``_``), or containing two sequential underscores (``__``).
   - These conflict with reserved identifiers for future standard use and for libc internal use. POSIX additionally reserves certain patterns of identifiers, however we are not currently attempting to respect thise. See [here](https://stackoverflow.com/questions/228783/what-are-the-rules-about-using-an-underscore-in-a-c-identifier) for a more indepth explanation.
 - All functions must start with a verb.
 - All variables/attributes must start with a noun (or adjective which modifies a noun).
 - typedef name: ``lowerCamelName_t``
 - class/struct names: ``UpperCamelCase``
 - **Always** use ``this`` pointer for member access.
 - Member functions: ``this->useLowerCaseCamel``
 - Member attributes: ``this->lowerCaseCamel``
 - Standalone functions: ``useLowerCaseCamel``
 - Local variables: ``snake_case`` or ``abbrev``
 - Global variables: ``Upper_Snake_Case``
 - Static constant variables, and constant expressions: ``CAPS_SNAKE_CASE``
 - Prefer using ``enum class`` over constants. Use ``UpperCamelCase`` for the enum class name, and ``lowerCamelCase`` (noun) for enum values.
 - Well known mathematical names may override verb/noun agreement rules. For example ``extendedEuclid(...)``.
 - arbitrary single letter identifiers may be used for mathematical expressions, so long as an explaining reference is given.
   - Examples of acceptable references would be a company Wiki or Knowledge Base article, or an academic paper referenced through a durable medium.
   - When referencing a page on gitlab.stealthsoftwareinc.com make sure take a link which includes a commit-sha, as pages maybe moved or edited after you reference it.
   - [IACR ePrint](https://eprint.iacr.org/) and the [Wayback Machine](http://web.archive.org/) are examples of sufficiently durable mediums.
     Faculty pages of educational institutes are to be avoided as links can go stale when individuals transfer from one institution to another (There is a button on the Wayback Machine to save a page).

At discretion of implementing engineer, use one of the following conventions for grouping classes in files.

 - For small(ish) classes, group when related by inheritance or composition.
 - One class per file.

Use the following guidelines when using ``#include`` directives.

 - Avoid using paths relative to the source file (e.g. ``./File.h`` or ``../File.h``).
 - Prefer using angle brackets instead of double quotes (e.g. ``#include <File.h>``).
 - Attempt to ``#include`` all files which you use, try not to rely on transitive inclusion.
 - Attempt to locate the ``#include`` section of a header file inside the header guard.
 - Group your includes in the following sections
   - POSIX, Win32 or other system specific library includes
   - C and C++ includes (in C++ prefer using ``cstdxxx`` over ``stdxxx.h``)
   - 3rd party includes
   - Project specific includes

## Git

### Branch Naming
Use the following conventions for naming branches.

 - ``master`` The main branch for tracking functioning code.
 - ``wiki/<title>`` for changes to the SAFRN Wiki.
 - ``bugfix/<title>`` For fixes to bugs.
 - ``feature/<title>`` For new feature development. May involve changes to the wiki without a separate ``wiki/`` branch.
 - ``misc/<title>`` For miscelaneous other changes.

Use the following conventions for managing the flow of branches.

 - ``master`` Never push to master, always use a merge request. (This is enforced by GitLab already)
 - ``wiki/*`` always branch off of latest master, and merge request to master.
 - ``feature/*`` A feature must branch from ``master``, and must merge request back onto ``master``.
   - A ``feature`` branch may merge from ``master`` onto the ``feature/`` branch in cases where feature development depends on later changes from ``master``.
   - Run ``clang-format`` before merging  a feature. For C++, the ``make format`` target is a convenience for this.
 - ``bugfix/*`` Should originate from ``master``.
   - May merge request either to ``master``, or to a relevent ``feature/`` branch, or both.
 - ``misc/*`` may branch and merge as needed, but usage of ``misc/`` branches should be minimized, in favor of more specific branch types.

### Merge Request Netiquette

 - Make sure your code gets reviewed.
   > Don't merge code without having it reviewed (exceptions can be made for automated formatter merges).
   > Also if you make changes to your review after it has been reviewed, make a change log, and ask for a rereview.
 - Allow the original author to close discussions.
   > This goes for both merge request feedback discussions, and the merge request itself.
 - Notify others when changes are made or approved.
   > When you fix your merge request feedback leave a note saying "fixed" or :hammer_and_wrench: (``:hammer_and_wrench:``).
   > If you made many changes, add a commit-sha to your :hammer_and_wrench:.
   > When you add non-review changes add a change log.
   >
   > Likewise, when a Merge Request is approved, add an "LGTM" or :whale: (whale of approval, ``:whale:``)

The following emojis have special meaning during a review, here is a cheat sheet:

| Emoji | Emoji code | plaintext equivelant | Meaning |
| ---- | ---- | ---- | ---- |
| :whale: | ``:whale:`` | LGTM | The merge request has been approved by a reviewer. |
| :hammer_and_wrench:, :hammer:, :wrench: | ``:hammer_and_wrench:`` | fixed | A feedback has been fixed or addressed. Often accompanied by a commit sha or other explanation. |
| :key: | ``:key:`` | Requires maintainer | The author or reviewer requires assistance from a maintainer, because permissions do not allow them to perform an action. (You must tag the maintainer using ``@`` and indicate the necessary action). |
| :gift: | ``:gift:`` | Transfer review | One reviewer transfers a feedback to another reviewer, giving the new reviewer permission and responsibility for closing the feedback. |

Here is an example:

 - The _author_ should tag or assign _reviewers_.
 - The _reviewers_ will add comments to the review.
 - Upon addressing a comment the _author_ should reply to the comment with a "fixed" or "fixed <commit-sha>" comment, or the _author_ may add a dissenting opinion to the comment.
   The _author_ may choose to use the alternate emoji spelling of "fixed" as :hammer_and_wrench: (markdown source: ``:hammer_and_wrench:``).
 - The _author_ should never resolve a _reviewer's_ comment, this action is reserved for the _reviewer who opened the comment_.
   This allows a _reviewer_ to request additional changes for a comment.
   Note, we've encountered situations where a _reviewer_ has not had adequate permissions to resolve a thread.
   In this case, the _reviewer_ should reply to the comment with "accepted", or alternate emoji spelling :key: (markdown source: ``:key:``), and tag (``@``) a _maintainer_.
 - Upon satisfactory review each _reviewer_ should add a comment saying "LGTM" (looks good to me).
   A _reviewer_ may choose the alternate emoji spelling :whale: (markdown source: ``:whale:``, etemology: I was going to do ``:seal:`` for "seal of approval", but there is no such seal emoji, so now we get the "whale of approval").
 - A _reviewer_ should not merge the request. This action is reserved for the _author_ in the case that additional changes are desired.
 - If an _author_ makes changes after getting "LGTM" or :whale: from a _reviewer_, then the author should reply to the :whale: indicating the changes which have been made, and allow the _reviewer_ a chance to re-review.
 - Once the _author_ is satisfied, all comments are resolved, and has received the :whale: from all _reviewers_, then the _author_ may merge the request, or ask a _maintainer_ to merge the request, if permissions issues arise.