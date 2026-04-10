#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Report Generator Module
Generates analysis reports for LLM-based code improvement
"""

import os
import json
import logging
from typing import Dict, List, Optional, Any
from pathlib import Path
from datetime import datetime
from collections import defaultdict


class ReportGenerator:
    def __init__(self, config: Dict[str, Any], validation_results: Dict[str, Any], 
                 api_index: Dict[str, Any], examples: Dict[str, Any], project_root: str):
        self.config = config
        self.validation_results = validation_results
        self.api_index = api_index
        self.examples = examples
        self.project_root = Path(project_root)
        self.logger = logging.getLogger(__name__)
        
        self.report_config = config.get('report', {})
        self.categories = self.report_config.get('categories', {})
        self.include_examples = self.report_config.get('include_examples', True)
        self.include_fix_suggestions = self.report_config.get('include_fix_suggestions', True)
        self.max_issues_per_category = self.report_config.get('max_issues_per_category', 50)
        
    def get_category_info(self, severity: str) -> Dict[str, str]:
        return self.categories.get(severity, {
            'description': f'{severity.capitalize()} issues',
            'action': 'Review and fix',
            'color': 'white'
        })
    
    def get_api_signature(self, api_name: str) -> Optional[str]:
        functions = self.api_index.get('functions', {})
        if api_name in functions:
            func_info = functions[api_name]
            params = ', '.join([p.get('type_name', '') for p in func_info.get('parameters', [])])
            return f"{func_info.get('return_type', '')} {api_name}({params})"
        return None
    
    def get_example_code(self, api_name: str, max_examples: int = 2) -> List[str]:
        if not self.include_examples:
            return []
        
        examples_data = self.examples.get('examples', {})
        if api_name not in examples_data:
            return []
        
        examples_list = examples_data[api_name][:max_examples]
        return [ex.get('code_snippet', '') for ex in examples_list]
    
    def generate_fix_suggestion(self, issue: Dict[str, Any]) -> str:
        if not self.include_fix_suggestions:
            return ""
        
        mismatch_type = issue.get('mismatch_type', '')
        api_name = issue.get('api_name', '')
        
        suggestions = {
            'missing_api': f"""
**Fix Suggestion:**
1. Verify if '{api_name}' is the correct API name
2. Check if the API exists in the SDK header files
3. Update the API index if this is a new API
4. Consider using alternative APIs if this API has been removed
""",
            'parameter_count_mismatch': f"""
**Fix Suggestion:**
1. Check the correct function signature: {self.get_api_signature(api_name) or 'API not found'}
2. Update the function call to match the expected parameter count
3. Review the API documentation for correct usage
4. Example correct usage:
```cpp
{self.get_example_code(api_name)[0] if self.get_example_code(api_name) else '// No example available'}
```
""",
            'parameter_type_mismatch': f"""
**Fix Suggestion:**
1. Convert parameter types to match expected types
2. Expected types: {issue.get('expected', 'N/A')}
3. Actual types: {issue.get('actual', 'N/A')}
4. Consider using type casting if appropriate
5. Example correct usage:
```cpp
{self.get_example_code(api_name)[0] if self.get_example_code(api_name) else '// No example available'}
```
""",
            'return_type_mismatch': f"""
**Fix Suggestion:**
1. Update the code to handle the correct return type
2. Expected: {issue.get('expected', 'N/A')}
3. Actual: {issue.get('actual', 'N/A')}
4. Add appropriate type conversion if needed
""",
            'undeclared_api': f"""
**Fix Suggestion:**
1. Include the appropriate header file for '{api_name}'
2. Check if the API is properly declared in the SDK
3. Verify the API name spelling
"""
        }
        
        return suggestions.get(mismatch_type, f"Review and fix the issue with '{api_name}'")
    
    def generate_issue_section(self, issue: Dict[str, Any], index: int) -> str:
        severity = issue.get('severity', 'info')
        category_info = self.get_category_info(severity)
        
        section = f"""
### Issue #{index}: {issue.get('api_name', 'Unknown API')}

**Type:** {issue.get('mismatch_type', 'Unknown').replace('_', ' ').title()}
**Severity:** {severity.upper()}
**Location:** [{issue['location']['file']}:{issue['location']['line']}](file:///{self.project_root}/{issue['location']['file']}#L{issue['location']['line']})

**Description:**
{issue.get('description', 'No description available')}

**Expected:** `{issue.get('expected', 'N/A')}`
**Actual:** `{issue.get('actual', 'N/A')}`
"""
        
        if issue.get('parameter_mismatches'):
            section += "\n**Parameter Details:**\n"
            for pm in issue['parameter_mismatches']:
                section += f"- Parameter {pm['position']}: Expected `{pm['expected_type']}`, got `{pm['actual_type']}`\n"
        
        fix_suggestion = self.generate_fix_suggestion(issue)
        if fix_suggestion:
            section += fix_suggestion
        
        examples = self.get_example_code(issue.get('api_name', ''))
        if examples:
            section += "\n**Correct Usage Examples:**\n"
            for i, example in enumerate(examples, 1):
                section += f"```cpp\n{example}\n```\n"
        
        return section
    
    def generate_category_section(self, severity: str, issues: List[Dict[str, Any]]) -> str:
        if not issues:
            return ""
        
        category_info = self.get_category_info(severity)
        limited_issues = issues[:self.max_issues_per_category]
        
        section = f"""
## {severity.upper()} Issues ({len(issues)} total)

**Description:** {category_info['description']}
**Action Required:** {category_info['action']}

"""
        
        for i, issue in enumerate(limited_issues, 1):
            section += self.generate_issue_section(issue, i)
            section += "\n---\n"
        
        if len(issues) > self.max_issues_per_category:
            section += f"\n*... and {len(issues) - self.max_issues_per_category} more issues in this category*\n"
        
        return section
    
    def generate_summary_section(self) -> str:
        stats = self.validation_results.get('statistics', {})
        
        section = f"""# SDK API Validation Report

**Generated:** {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}
**Project Root:** {self.project_root}

## Executive Summary

| Metric | Count |
|--------|-------|
| Total Issues | {stats.get('total_issues', 0)} |
| Critical | {stats.get('by_severity', {}).get('critical', 0)} |
| Warning | {stats.get('by_severity', {}).get('warning', 0)} |
| Info | {stats.get('by_severity', {}).get('info', 0)} |
| Suggestion | {stats.get('by_severity', {}).get('suggestion', 0)} |

### Issues by Type

"""
        
        by_type = stats.get('by_type', {})
        for issue_type, count in sorted(by_type.items(), key=lambda x: x[1], reverse=True):
            section += f"- **{issue_type.replace('_', ' ').title()}:** {count}\n"
        
        return section
    
    def generate_action_plan(self) -> str:
        issues = self.validation_results.get('issues', [])
        
        critical_issues = [i for i in issues if i.get('severity') == 'critical']
        warning_issues = [i for i in issues if i.get('severity') == 'warning']
        
        section = """
## Recommended Action Plan

### Phase 1: Critical Issues (Must Fix Immediately)
"""
        
        if critical_issues:
            section += f"\n{len(critical_issues)} critical issues found that will cause compilation or runtime errors.\n\n"
            section += "**Priority APIs to fix:**\n"
            api_counts = defaultdict(int)
            for issue in critical_issues:
                api_counts[issue.get('api_name', 'Unknown')] += 1
            
            for api, count in sorted(api_counts.items(), key=lambda x: x[1], reverse=True)[:10]:
                section += f"- `{api}`: {count} issue(s)\n"
        else:
            section += "\n✅ No critical issues found.\n"
        
        section += """
### Phase 2: Warning Issues (Should Fix Before Release)
"""
        
        if warning_issues:
            section += f"\n{len(warning_issues)} warning issues found that may cause unexpected behavior.\n\n"
            section += "**APIs requiring review:**\n"
            api_counts = defaultdict(int)
            for issue in warning_issues:
                api_counts[issue.get('api_name', 'Unknown')] += 1
            
            for api, count in sorted(api_counts.items(), key=lambda x: x[1], reverse=True)[:10]:
                section += f"- `{api}`: {count} issue(s)\n"
        else:
            section += "\n✅ No warning issues found.\n"
        
        section += """
### Phase 3: Code Quality Improvements
"""
        
        info_issues = [i for i in issues if i.get('severity') == 'info']
        suggestion_issues = [i for i in issues if i.get('severity') == 'suggestion']
        
        if info_issues or suggestion_issues:
            section += f"\n{len(info_issues) + len(suggestion_issues)} informational issues found.\n"
            section += "These are optional improvements that can enhance code quality.\n"
        else:
            section += "\n✅ All code quality checks passed.\n"
        
        return section
    
    def generate_llm_instructions(self) -> str:
        section = """
## Instructions for LLM Code Assistant

This report contains API validation issues that need to be fixed. Please follow these guidelines:

### How to Use This Report

1. **Start with Critical Issues**: Fix all critical issues first as they will cause compilation or runtime errors
2. **Review Warnings**: Address warning issues to ensure code correctness
3. **Consider Suggestions**: Apply informational suggestions to improve code quality

### Fix Process

For each issue:
1. Navigate to the file location specified in the issue
2. Review the expected vs actual values
3. Apply the suggested fix
4. Use the provided example code as reference
5. Test the fix to ensure it works correctly

### Important Notes

- Always check the API signature before making changes
- Use the provided examples as Few-Shot references
- Maintain consistency with existing code style
- Ensure all changes compile successfully
- Run tests after making changes

### API Reference

When in doubt about an API, refer to:
- The API signature provided in each issue
- The example code snippets
- The SDK header files in `install/include/ogc/capi/`
"""
        
        return section
    
    def generate_report(self) -> str:
        report = self.generate_summary_section()
        
        issues = self.validation_results.get('issues', [])
        
        issues_by_severity = defaultdict(list)
        for issue in issues:
            issues_by_severity[issue.get('severity', 'info')].append(issue)
        
        for severity in ['critical', 'warning', 'info', 'suggestion']:
            if severity in issues_by_severity:
                report += self.generate_category_section(severity, issues_by_severity[severity])
        
        report += self.generate_action_plan()
        report += self.generate_llm_instructions()
        
        return report
    
    def save_report(self, output_path: Optional[Path] = None) -> Path:
        if output_path is None:
            output_dir = self.report_config.get('output_dir', 'install/bin/clang_tool/reports')
            output_filename = self.report_config.get('output_filename', 'api_validation_report.md')
            output_path = self.project_root / output_dir / output_filename
        
        output_path.parent.mkdir(parents=True, exist_ok=True)
        
        report = self.generate_report()
        
        with open(output_path, 'w', encoding='utf-8') as f:
            f.write(report)
        
        self.logger.info(f"Report saved to: {output_path}")
        return output_path


def main():
    import argparse
    
    parser = argparse.ArgumentParser(description='Generate API validation report')
    parser.add_argument('--config', '-c', default='config.json', help='Configuration file path')
    parser.add_argument('--validation', '-v', default='validation_results.json', help='Validation results file')
    parser.add_argument('--index', '-i', default='api_index.json', help='API index file')
    parser.add_argument('--examples', '-e', default='api_examples.json', help='API examples file')
    parser.add_argument('--output', '-o', help='Output report file path')
    
    args = parser.parse_args()
    
    logging.basicConfig(
        level=logging.INFO,
        format='%(asctime)s - %(name)s - %(levelname)s - %(message)s'
    )
    
    base_path = Path(__file__).parent
    
    config_path = Path(args.config)
    if not config_path.is_absolute():
        config_path = base_path / config_path
    
    with open(config_path, 'r', encoding='utf-8') as f:
        config = json.load(f)
    
    validation_path = Path(args.validation)
    if not validation_path.is_absolute():
        validation_path = base_path / 'reports' / args.validation
    with open(validation_path, 'r', encoding='utf-8') as f:
        validation_results = json.load(f)
    
    index_path = Path(args.index)
    if not index_path.is_absolute():
        index_path = base_path / 'index' / args.index
    with open(index_path, 'r', encoding='utf-8') as f:
        api_index = json.load(f)
    
    examples_path = Path(args.examples)
    if not examples_path.is_absolute():
        examples_path = base_path / 'index' / args.examples
    with open(examples_path, 'r', encoding='utf-8') as f:
        examples = json.load(f)
    
    generator = ReportGenerator(
        config, validation_results, api_index, examples, 
        str(Path(__file__).parent.parent.parent)
    )
    
    output_path = Path(args.output) if args.output else None
    generator.save_report(output_path)
    
    print(f"\nReport generated successfully!")
    print(f"Total issues: {validation_results['statistics']['total_issues']}")


if __name__ == '__main__':
    main()
