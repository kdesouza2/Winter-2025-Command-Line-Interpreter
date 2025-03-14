#!/usr/bin/env bats

# File: student_tests.sh
# 
# Create your unit tests suit in this file

@test "Example: check ls runs without errors" {
    run ./dsh <<EOF                
ls
EOF

    # Assertions
    [ "$status" -eq 0 ]
}

@test "Multiple Piped Commands" {
    run "./dsh" <<EOF
ls | grep dsh | sort
EOF

    stripped_output=$(echo "$output" | tr -d '[:space:]')
    expected_output="dsh4>dshdsh_cli.cdshlib.cdshlib.h"

    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    [[ "$stripped_output" == *"$expected_output"* ]]
    [ "$status" -eq 0 ]
}

@test "Pipe with Multiple Commands and Reversed Order" {
    run "./dsh" <<EOF
ls | sort | head -n 1
EOF

    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"
    [ "$status" -eq 0 ]
}

@test "Handling of Pipes with Exit Command" {
    run "./dsh" <<EOF
echo "hello" | exit
EOF

    echo "Output: $output"
    echo "Exit Status: $status"

    [ "$status" -eq 0 ]  # Should exit without error
}
