#!/bin/bash

# This script is invoked once for each item.
# Commented code shows how received data can be collected and accessed.
#
# Usage example:
# dit export ./export-template.bash

declare -A data

# Compose an associative array from key-value pairs.
for arg; do
    # echo "Arg: $arg"
    key="${arg%%=*}"
    value="${arg#*=}"
    # echo "Key: $key"
    # echo "Value: $value"

    data[$key]="$value"
done

# Visit each array entry.
for key in "${!data[@]}"; do
    :
    # echo "Key: $key"
    # echo "Value: ${data[$key]}"
done

# Display title or report its absence.
echo "${data[title]:-<no title>}"
