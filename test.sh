#!/bin/bash
export TEST_VAR="test_value"
cat << 'EOF'
$TEST_VAR
