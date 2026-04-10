#!/bin/bash
# SDK API Index and Validation Tool - Quick Start Script for Linux/Mac
# This script runs the complete workflow

echo "========================================"
echo "SDK API Index and Validation Tool"
echo "========================================"
echo

# Check Python installation
if ! command -v python3 &> /dev/null; then
    echo "ERROR: Python3 is not installed"
    echo "Please install Python 3.7+"
    exit 1
fi

# Check libclang installation
if ! python3 -c "from clang.cindex import Index" 2> /dev/null; then
    echo "ERROR: python-libclang is not installed"
    echo "Installing python-libclang..."
    pip3 install libclang
    if [ $? -ne 0 ]; then
        echo "Failed to install libclang. Please install manually: pip3 install libclang"
        exit 1
    fi
fi

echo "Step 1: Building API Index..."
python3 main.py --index-only
if [ $? -ne 0 ]; then
    echo "ERROR: Failed to build API index"
    exit 1
fi

echo
echo "Step 2: Extracting API Examples..."
python3 example_extractor.py
if [ $? -ne 0 ]; then
    echo "ERROR: Failed to extract examples"
    exit 1
fi

echo
echo "Step 3: Validating APIs..."
python3 main.py --validate-only --skip-index
if [ $? -ne 0 ]; then
    echo "ERROR: Failed to validate APIs"
    exit 1
fi

echo
echo "Step 4: Generating Report..."
python3 main.py --report-only
if [ $? -ne 0 ]; then
    echo "ERROR: Failed to generate report"
    exit 1
fi

echo
echo "========================================"
echo "Workflow Complete!"
echo "========================================"
echo
echo "Output files:"
echo "  - API Index: index/api_index.json"
echo "  - Examples:  index/api_examples.json"
echo "  - Results:   reports/validation_results.json"
echo "  - Report:    reports/api_validation_report.md"
echo

# Open report
read -p "Open report? (y/n): " OPEN_REPORT
if [ "$OPEN_REPORT" = "y" ] || [ "$OPEN_REPORT" = "Y" ]; then
    if command -v xdg-open &> /dev/null; then
        xdg-open reports/api_validation_report.md
    elif command -v open &> /dev/null; then
        open reports/api_validation_report.md
    else
        echo "Report location: reports/api_validation_report.md"
    fi
fi
