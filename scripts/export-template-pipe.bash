#!/bin/bash

# This script receives all data item-by-item (all fields of one item followed by
# all fields of another and so on).
# Commented code shows how received data can be collected and accessed.
#
# Usage example:
# dit export - | ./export-template-pipe.bash

# This function can use $data array to process item fields as key-value pairs.
function process_item()
{
    for key in "${!data[@]}"; do
        :
        # echo "Key: $key"
        # echo "Value: ${data[$key]}"
    done

    # Display title or report its absence.
    echo "${data[title]:-<no title>}"
}

# Compose an associative array from key-value pairs.
declare -A data
while read -rd $'\0' line; do
    if [ -z "$line" ]; then
        process_item
        unset data
        declare -A data
        continue
    fi

    # echo "Line: $line"
    key="${line%%=*}"
    value="${line#*=}"
    # echo "Key: $key"
    # echo "Value: $value"

    data[$key]="$value"
done
