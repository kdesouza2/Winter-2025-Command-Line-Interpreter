#!/usr/bin/env bats

############################ DO NOT EDIT THIS FILE #####################################
# File: assignement_tests.sh
# 
# DO NOT EDIT THIS FILE
#
# Add/Edit Student tests in student_tests.sh
# 
# All tests in this file must pass - it is used as part of grading!
########################################################################################

@test "Pipes" {
    run "./dsh" <<EOF                
ls | grep dshlib.c
EOF

    # Strip all whitespace (spaces, tabs, newlines) from the output
    stripped_output=$(echo "$output" | tr -d '[:space:]')

    # Expected output with all whitespace removed for easier matching
    expected_output="dsh3>dshlib.cdsh3>cmdloopreturned0"

    # These echo commands will help with debugging and will only print
    #if the test fails
    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    # Check exact match
    [ "$stripped_output" = "$expected_output" ]

    # Assertions
    [ "$status" -eq 0 ]
}