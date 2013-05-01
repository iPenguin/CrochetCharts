#!/bin/bash
#
# Additional pre-commit tests that should be preformed.
#
# The following tests were copied and modified from the cmake hook scripts.
#
#-----------------------------------------------------------------------------
# Check for committer identity.
advice='
Use the commands

        git config --global user.name '\''Your Name'\''
        git config --global user.email '\''you@yourdomain.com'\''

to introduce yourself to Git before committing.'

# Ensure name and email are available.
git config --get user.name > /dev/null &&
git config --get user.email > /dev/null ||
die 'Identity not configured!' "$advice"

# Validate the name and email.
git config --get user.name | grep ' ' > /dev/null ||
die 'Set user.name to your Real Name (with a space), not a userid.' "$advice"
git config --get user.email | grep '^[^@]*@[^@]*$' > /dev/null ||
die 'Set user.email to an email address (userid@validdomain.com).' "$advice"

#-------------------------------------------------------------------------------
line_too_long=120
bad=$(regex=".{$line_too_long}" &&
git diff-index --cached HEAD --name-only --diff-filter=AM \
    --pickaxe-regex -S"$regex" -- 'src/*.h' 'src/*.cpp' 'tests/*.h' 'tests/*.cpp' |
while read file; do
  lines_too_long=$(git diff-index -p --cached HEAD \
                   --pickaxe-regex -S"$regex" -- "$file")
  if echo "$lines_too_long" | egrep -q '^\+'"$regex"; then
    echo "$lines_too_long"
  fi
done)
test -z "$bad" ||
die 'The following changes add lines too long for our C++ style:

'"$bad"'

Use lines strictly less than '"$line_too_long"' characters in C++ code.'

#-------------------------------------------------------------------------------
# Approximate whitespace=tab-in-indent check with Git < 1.7.2.
git --version | grep -q " \(1\.[0-6]\|1\.7\.[01]\)" &&
approx_tab_in_indent=true || approx_tab_in_indent=false
check_tab() {
        lines=$(git diff-index -p --cached $against -- "$1" |
                grep '^+        ') &&
        echo "$lines" |
        while read line; do
                echo "$1: tab in indent." &&
                echo "$line"
        done
}

# Reject addition of a line without a newline at end-of-file.
check_no_lf_at_eof() {
        lines=$(git diff-index -p --cached $against -- "$1" | tail -2)
        if echo "$lines" | head -1 | grep -q '^+' &&
           echo "$lines" | tail -1 | grep -q '^\\ No newline'; then
                echo "$1: No newline at end of file"
        fi
}

# Custom whitespace checks.
check_whitespace() {
        ws=$(git check-attr whitespace -- "$file" |
             sed 's/^[^:]*: whitespace: //')
        if $approx_tab_in_indent; then
                case ",$ws," in
                        *,tab-in-indent,*) check_tab "$1" ;;
                esac
        fi
        case ",$ws," in
                *,no-lf-at-eof,*) check_no_lf_at_eof "$1" ;;
        esac
}

bad=$(git diff-index --name-only --cached $against -- |
while read file; do
        check_whitespace "$file"
done)
test -z "$bad" || die "$bad"
