#!/usr/bin/env bats

# File: student_tests.sh
# 
# Create your unit tests suit in this file


@test "Echo a simple message" {
    run "./dsh" <<EOF                
echo "Hello, world!"
EOF

    stripped_output=$(echo "$output" | tr -d '[:space:]')

    expected_output="Hello,world!dsh2>dsh2>cmdloopreturned0"

    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    [ "$stripped_output" = "$expected_output" ]
}

@test "Echo with multiple spaces" {
    run "./dsh" <<EOF                
echo "Hello      world"
EOF

    stripped_output=$(echo "$output" | tr -d '\t\n\r\f\v')

    expected_output="Hello      worlddsh2> dsh2> cmd loop returned 0"

    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    [ "$stripped_output" = "$expected_output" ]
}

@test "Handle empty command" {
    run "./dsh" <<EOF                

EOF

    stripped_output=$(echo "$output" | tr -d '[:space:]')

    expected_output="dsh2>warning:nocommandsprovideddsh2>cmdloopreturned0"

    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    [ "$stripped_output" = "$expected_output" ]
}

@test "Run pwd command" {
    run "./dsh" <<EOF                
pwd
EOF

    stripped_output=$(echo "$output" | tr -d '[:space:]')

    expected_output="$(pwd)dsh2>dsh2>cmdloopreturned0"

    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    [ "$stripped_output" = "$expected_output" ]
}

@test "Exit command terminates shell" {
    run "./dsh" <<EOF                
exit
EOF

    stripped_output=$(echo "$output" | tr -d '[:space:]')

    expected_output="dsh2>cmdloopreturned0"

    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    [ "$stripped_output" = "$expected_output" ]
}

@test "Handle unknown command" {
    run "./dsh" <<EOF                
unknown
EOF

    stripped_output=$(echo "$output" | tr -d '[:space:]')

    expected_output="dsh2>error:externalcommanderrordsh2>dsh2>cmdloopreturned0"

    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    [ "$stripped_output" = "$expected_output" ]
}
