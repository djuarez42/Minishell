#!/bin/bash

# Minishell Error Log Aggregator
# This script collects and analyzes all failed tests and errors

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
PURPLE='\033[0;35m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

# Configuration
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
TESTER_DIR="$SCRIPT_DIR/42_minishell_tester"
RESULTS_DIR="$TESTER_DIR/test_results"
TIMESTAMP=$(date +%Y%m%d_%H%M%S)
LOG_DIR="$SCRIPT_DIR/error_logs"


# Create log directory if it doesn't exist
mkdir -p "$LOG_DIR"

# sanity: tester must exist
if [[ ! -x "$TESTER_DIR/tester.sh" ]]; then
    echo "Error: tester not found or not executable at: $TESTER_DIR/tester.sh" >&2
    echo "Make sure the tester is present and executable, or set TESTER_DIR correctly." >&2
fi

echo -e "${BLUE}🔍 Minishell Error Log Aggregator${NC}"
echo -e "${BLUE}====================================${NC}"
echo ""

# Function to run tests and capture all errors
run_comprehensive_test() {
    echo -e "${YELLOW}📋 Running comprehensive tests...${NC}"
    
    # Create a comprehensive test log
    COMPREHENSIVE_LOG="$LOG_DIR/comprehensive_errors_$TIMESTAMP.log"
    
    echo "=== COMPREHENSIVE MINISHELL ERROR LOG ===" > "$COMPREHENSIVE_LOG"
    echo "Generated: $(date)" >> "$COMPREHENSIVE_LOG"
    echo "=========================================" >> "$COMPREHENSIVE_LOG"
    echo "" >> "$COMPREHENSIVE_LOG"
    
    # Run all test categories and capture errors
    echo -e "${CYAN}🧪 Testing all categories...${NC}"
    
    # Test all mandatory features
    echo "=== MANDATORY TESTS ===" >> "$COMPREHENSIVE_LOG"
    (cd "$TESTER_DIR" && ./tester.sh m) 2>&1 | tee -a "$COMPREHENSIVE_LOG"
    
    echo "" >> "$COMPREHENSIVE_LOG"
    echo "=== BONUS TESTS ===" >> "$COMPREHENSIVE_LOG"
    (cd "$TESTER_DIR" && ./tester.sh bonus) 2>&1 | tee -a "$COMPREHENSIVE_LOG"

    echo -e "${GREEN}✅ Comprehensive test completed${NC}"
    echo -e "${BLUE}📄 Log saved to: $COMPREHENSIVE_LOG${NC}"
}

# Function to analyze existing logs
analyze_existing_logs() {
    echo -e "${YELLOW}📊 Analyzing existing error logs...${NC}"
    
    ANALYSIS_LOG="$LOG_DIR/error_analysis_$TIMESTAMP.md"
    
    echo "# Minishell Error Analysis Report" > "$ANALYSIS_LOG"
    echo "**Generated:** $(date)" >> "$ANALYSIS_LOG"
    echo "" >> "$ANALYSIS_LOG"
    
    if [ -d "$RESULTS_DIR" ]; then
        echo "## Existing Test Results" >> "$ANALYSIS_LOG"
        echo "" >> "$ANALYSIS_LOG"
        
        # Count total failures
        TOTAL_FAILURES=$(find "$RESULTS_DIR" -name "*failures*.txt" -exec wc -l {} + | tail -1 | awk '{print $1}')
        echo "- **Total failure entries:** $TOTAL_FAILURES" >> "$ANALYSIS_LOG"
        
        # List all failure files
        echo "### Failure Files Found:" >> "$ANALYSIS_LOG"
        find "$RESULTS_DIR" -name "*failures*.txt" | while read file; do
            echo "- \`$(basename "$file")\` - $(wc -l < "$file") failures" >> "$ANALYSIS_LOG"
        done
        
        echo "" >> "$ANALYSIS_LOG"
        echo "### Recent Failures:" >> "$ANALYSIS_LOG"
        echo "\`\`\`" >> "$ANALYSIS_LOG"
        find "$RESULTS_DIR" -name "*failures*.txt" -exec tail -20 {} \; >> "$ANALYSIS_LOG"
        echo "\`\`\`" >> "$ANALYSIS_LOG"
    else
        echo "No existing test results found." >> "$ANALYSIS_LOG"
    fi
    
    echo -e "${GREEN}✅ Analysis completed${NC}"
    echo -e "${BLUE}📄 Analysis saved to: $ANALYSIS_LOG${NC}"
}

# Function to get specific error types
get_specific_errors() {
    local error_type="$1"
    
    echo -e "${YELLOW}🔍 Searching for $error_type errors...${NC}"
    
    ERROR_LOG="$LOG_DIR/${error_type}_errors_$TIMESTAMP.log"
    
    echo "=== $error_type ERRORS ===" > "$ERROR_LOG"
    echo "Generated: $(date)" >> "$ERROR_LOG"
    echo "=========================" >> "$ERROR_LOG"
    echo "" >> "$ERROR_LOG"
    
    # Search for specific error patterns
    case "$error_type" in
        "segfault"|"crash")
            echo "Searching for segmentation faults and crashes..." >> "$ERROR_LOG"
            find "$RESULTS_DIR" -name "*.txt" -exec grep -l -i "segfault\|crash\|signal\|abort" {} \; | while read file; do
                echo "--- Found in: $file ---" >> "$ERROR_LOG"
                grep -i "segfault\|crash\|signal\|abort" "$file" >> "$ERROR_LOG"
                echo "" >> "$ERROR_LOG"
            done
            ;;
        "memory"|"leak")
            echo "Searching for memory leaks and allocation errors..." >> "$ERROR_LOG"
            find "$RESULTS_DIR" -name "*.txt" -exec grep -l -i "leak\|memory\|malloc\|free" {} \; | while read file; do
                echo "--- Found in: $file ---" >> "$ERROR_LOG"
                grep -i "leak\|memory\|malloc\|free" "$file" >> "$ERROR_LOG"
                echo "" >> "$ERROR_LOG"
            done
            ;;
        "syntax"|"parsing")
            echo "Searching for syntax and parsing errors..." >> "$ERROR_LOG"
            find "$RESULTS_DIR" -name "*.txt" -exec grep -l -i "syntax\|parse\|unexpected\|error" {} \; | while read file; do
                echo "--- Found in: $file ---" >> "$ERROR_LOG"
                grep -i "syntax\|parse\|unexpected\|error" "$file" >> "$ERROR_LOG"
                echo "" >> "$ERROR_LOG"
            done
            ;;
        "output"|"stdout")
            echo "Searching for output mismatches..." >> "$ERROR_LOG"
            find "$RESULTS_DIR" -name "*failures*.txt" -exec cat {} \; >> "$ERROR_LOG"
            ;;
        *)
            echo "Searching for general errors..." >> "$ERROR_LOG"
            find "$RESULTS_DIR" -name "*.txt" -exec cat {} \; >> "$ERROR_LOG"
            ;;
    esac
    
    echo -e "${GREEN}✅ $error_type errors extracted${NC}"
    echo -e "${BLUE}📄 Log saved to: $ERROR_LOG${NC}"
}

# Function to create error summary
create_error_summary() {
    echo -e "${YELLOW}📈 Creating error summary...${NC}"
    
    SUMMARY_LOG="$LOG_DIR/error_summary_$TIMESTAMP.md"
    
    echo "# Minishell Error Summary" > "$SUMMARY_LOG"
    echo "**Generated:** $(date)" >> "$SUMMARY_LOG"
    echo "" >> "$SUMMARY_LOG"
    
    # Count different types of errors
    echo "## Error Statistics" >> "$SUMMARY_LOG"
    echo "" >> "$SUMMARY_LOG"
    
    if [ -d "$RESULTS_DIR" ]; then
        # Count total test files
        TOTAL_TESTS=$(find "$RESULTS_DIR" -name "*.txt" | wc -l)
        echo "- **Total test result files:** $TOTAL_TESTS" >> "$SUMMARY_LOG"
        
        # Count failures by type
        STDOUT_FAILURES=$(find "$RESULTS_DIR" -name "*STD_OUT_failures*.txt" | wc -l)
        STDERR_FAILURES=$(find "$RESULTS_DIR" -name "*STD_ERR_failures*.txt" | wc -l)
        EXIT_FAILURES=$(find "$RESULTS_DIR" -name "*EXIT_failures*.txt" | wc -l)
        
        echo "- **STDOUT failure files:** $STDOUT_FAILURES" >> "$SUMMARY_LOG"
        echo "- **STDERR failure files:** $STDERR_FAILURES" >> "$SUMMARY_LOG"
        echo "- **EXIT failure files:** $EXIT_FAILURES" >> "$SUMMARY_LOG"
        
        echo "" >> "$SUMMARY_LOG"
        echo "## Note" >> "$SUMMARY_LOG"
        echo "This summary shows historical failure files from previous test runs." >> "$SUMMARY_LOG"
        echo "For current test status, run: \`./42_minishell_tester/tester.sh m builtins\`" >> "$SUMMARY_LOG"
        
        # Get most recent failures
        echo "" >> "$SUMMARY_LOG"
        echo "## Most Recent Failures" >> "$SUMMARY_LOG"
        echo "" >> "$SUMMARY_LOG"
        
        # Get most recent failure files (macOS compatible)
        find "$RESULTS_DIR" -name "*failures*.txt" -exec stat -f "%m %N" {} \; | sort -n | tail -3 | while read timestamp file; do
            echo "### $(basename "$file")" >> "$SUMMARY_LOG"
            echo "**Modified:** $(date -r $timestamp)" >> "$SUMMARY_LOG"
            echo "" >> "$SUMMARY_LOG"
            echo "\`\`\`" >> "$SUMMARY_LOG"
            head -10 "$file" >> "$SUMMARY_LOG"
            echo "\`\`\`" >> "$SUMMARY_LOG"
            echo "" >> "$SUMMARY_LOG"
        done
    fi
    
    echo -e "${GREEN}✅ Summary created${NC}"
    echo -e "${BLUE}📄 Summary saved to: $SUMMARY_LOG${NC}"
}

# Function to show all available logs
show_available_logs() {
    echo -e "${YELLOW}📁 Available error logs:${NC}"
    echo ""
    
    if [ -d "$LOG_DIR" ]; then
        find "$LOG_DIR" -name "*.log" -o -name "*.md" | sort | while read file; do
            echo -e "${CYAN}📄 $(basename "$file")${NC}"
            echo "   Size: $(du -h "$file" | cut -f1)"
            echo "   Modified: $(stat -f "%Sm" "$file")"
            echo ""
        done
    else
        echo -e "${RED}No logs found. Run tests first.${NC}"
    fi
}

# Main menu
show_menu() {
    echo -e "${BLUE}Minishell Error Log Commands:${NC}"
    echo ""
    echo "1. Run comprehensive test and log all errors"
    echo "2. Analyze existing error logs"
    echo "3. Get specific error types (segfault, memory, syntax, output)"
    echo "4. Create error summary"
    echo "5. Show available logs"
    echo "6. Clean old logs"
    echo "7. Exit"
    echo ""
}

# Function to clean old logs
clean_old_logs() {
    echo -e "${YELLOW}🧹 Cleaning old logs...${NC}"
    
    # Keep only last 10 log files
    find "$LOG_DIR" -name "*.log" -o -name "*.md" | sort -r | tail -n +11 | xargs rm -f
    
    echo -e "${GREEN}✅ Old logs cleaned${NC}"
}

# Main execution
case "$1" in
    "comprehensive"|"all")
        run_comprehensive_test
        ;;
    "analyze"|"analysis")
        analyze_existing_logs
        ;;
    "specific"|"type")
        if [ -n "$2" ]; then
            get_specific_errors "$2"
        else
            echo -e "${RED}Please specify error type: segfault, memory, syntax, output${NC}"
        fi
        ;;
    "summary")
        create_error_summary
        ;;
    "list"|"show")
        show_available_logs
        ;;
    "clean")
        clean_old_logs
        ;;
    "help"|"-h"|"--help")
        echo -e "${BLUE}Usage: $0 [command] [options]${NC}"
        echo ""
        echo "Commands:"
        echo "  comprehensive, all    - Run all tests and log errors"
        echo "  analyze, analysis     - Analyze existing logs"
        echo "  specific, type <type> - Get specific error types"
        echo "  summary              - Create error summary"
        echo "  list, show           - Show available logs"
        echo "  clean                - Clean old logs"
        echo "  help                 - Show this help"
        echo ""
        echo "Error types: segfault, memory, syntax, output"
        ;;
    "")
        show_menu
        read -p "Choose an option (1-7): " choice
        case $choice in
            1) run_comprehensive_test ;;
            2) analyze_existing_logs ;;
            3) 
                read -p "Enter error type: " error_type
                get_specific_errors "$error_type"
                ;;
            4) create_error_summary ;;
            5) show_available_logs ;;
            6) clean_old_logs ;;
            7) exit 0 ;;
            *) echo -e "${RED}Invalid option${NC}" ;;
        esac
        ;;
    *)
        echo -e "${RED}Unknown command: $1${NC}"
        echo "Use '$0 help' for usage information"
        ;;
esac
