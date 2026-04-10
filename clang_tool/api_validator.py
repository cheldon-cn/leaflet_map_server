#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
API Validator Module
Static analysis to detect API mismatches between test code and SDK index
"""

import os
import json
import logging
import re
from typing import Dict, List, Optional, Any, Set, Tuple
from pathlib import Path
from dataclasses import dataclass, asdict
from enum import Enum
from collections import defaultdict

try:
    from clang.cindex import Index, CursorKind, TypeKind, TranslationUnit
except ImportError:
    print("Error: python-libclang not installed. Install with: pip install libclang")
    raise


class MismatchSeverity(Enum):
    CRITICAL = "critical"
    WARNING = "warning"
    INFO = "info"
    SUGGESTION = "suggestion"


class MismatchType(Enum):
    FUNCTION_NAME_MISMATCH = "function_name_mismatch"
    PARAMETER_COUNT_MISMATCH = "parameter_count_mismatch"
    PARAMETER_TYPE_MISMATCH = "parameter_type_mismatch"
    RETURN_TYPE_MISMATCH = "return_type_mismatch"
    MISSING_API = "missing_api"
    DEPRECATED_API = "deprecated_api"
    UNDECLARED_API = "undeclared_api"


@dataclass
class ParameterMismatch:
    position: int
    expected_type: str
    actual_type: str
    
    def to_dict(self) -> Dict[str, Any]:
        return asdict(self)


@dataclass
class MismatchIssue:
    mismatch_type: MismatchType
    severity: MismatchSeverity
    api_name: str
    file_path: str
    line_number: int
    column: int
    description: str
    expected: Optional[str]
    actual: Optional[str]
    parameter_mismatches: List[ParameterMismatch]
    suggestion: str
    
    def to_dict(self) -> Dict[str, Any]:
        return {
            'mismatch_type': self.mismatch_type.value,
            'severity': self.severity.value,
            'api_name': self.api_name,
            'location': {
                'file': self.file_path,
                'line': self.line_number,
                'column': self.column
            },
            'description': self.description,
            'expected': self.expected,
            'actual': self.actual,
            'parameter_mismatches': [pm.to_dict() for pm in self.parameter_mismatches],
            'suggestion': self.suggestion
        }


class APIValidator:
    def __init__(self, config: Dict[str, Any], api_index: Dict[str, Any], project_root: str):
        self.config = config
        self.api_index = api_index
        self.project_root = Path(project_root)
        self.logger = logging.getLogger(__name__)
        
        self.functions_index = api_index.get('functions', {})
        
        validation_config = config.get('validation', {})
        self.mismatch_criteria = validation_config.get('mismatch_criteria', {})
        self.type_equivalence = validation_config.get('type_equivalence', {})
        self.strict_mode = validation_config.get('strict_mode', False)
        self.allow_implicit_cast = validation_config.get('allow_implicit_cast', True)
        
        self.issues: List[MismatchIssue] = []
        self.index = Index.create()
        
    def resolve_path(self, path_str: str) -> Path:
        path = Path(path_str)
        if path.is_absolute():
            return path
        return self.project_root / path
    
    def get_test_files(self) -> List[Path]:
        test_files = []
        tests_config = self.config.get('tests', {})
        test_dirs = tests_config.get('test_dirs', [])
        test_patterns = tests_config.get('test_patterns', ['test_*.cpp'])
        exclude_patterns = tests_config.get('exclude_patterns', [])
        
        for test_dir in test_dirs:
            dir_path = self.resolve_path(test_dir)
            if not dir_path.exists():
                self.logger.warning(f"Test directory not found: {dir_path}")
                continue
                
            for pattern in test_patterns:
                for test_file in dir_path.rglob(pattern):
                    relative_path = test_file.relative_to(self.project_root)
                    should_exclude = any(
                        relative_path.match(excl) or test_file.match(excl)
                        for excl in exclude_patterns
                    )
                    if not should_exclude:
                        test_files.append(test_file)
        
        self.logger.info(f"Found {len(test_files)} test files to validate")
        return test_files
    
    def normalize_type(self, type_str: str) -> str:
        type_str = type_str.strip()
        type_str = re.sub(r'\s+', ' ', type_str)
        type_str = re.sub(r'\s*\*\s*', '*', type_str)
        type_str = re.sub(r'\s*const\s+', 'const ', type_str)
        
        return type_str
    
    def are_types_compatible(self, expected: str, actual: str) -> bool:
        expected_norm = self.normalize_type(expected)
        actual_norm = self.normalize_type(actual)
        
        if expected_norm == actual_norm:
            return True
        
        for base_type, equivalent_types in self.type_equivalence.items():
            if expected_norm in equivalent_types and actual_norm in equivalent_types:
                return True
        
        if self.allow_implicit_cast:
            if '*' in expected_norm and '*' in actual_norm:
                if expected_norm.replace('const ', '').strip() == actual_norm.replace('const ', '').strip():
                    return True
        
        return False
    
    def validate_function_call(self, func_name: str, call_args: List[str], 
                               file_path: str, line: int, column: int) -> Optional[MismatchIssue]:
        if func_name not in self.functions_index:
            if self.mismatch_criteria.get('missing_api', True):
                return MismatchIssue(
                    mismatch_type=MismatchType.MISSING_API,
                    severity=MismatchSeverity.CRITICAL,
                    api_name=func_name,
                    file_path=file_path,
                    line_number=line,
                    column=column,
                    description=f"Function '{func_name}' not found in SDK API index",
                    expected=None,
                    actual=None,
                    parameter_mismatches=[],
                    suggestion=f"Check if '{func_name}' is a valid SDK API or if the API index needs to be updated"
                )
            return None
        
        func_info = self.functions_index[func_name]
        expected_params = func_info.get('parameters', [])
        actual_param_count = len(call_args)
        expected_param_count = len(expected_params)
        
        is_variadic = func_info.get('is_variadic', False)
        
        if not is_variadic and actual_param_count != expected_param_count:
            if self.mismatch_criteria.get('parameter_count_mismatch', True):
                return MismatchIssue(
                    mismatch_type=MismatchType.PARAMETER_COUNT_MISMATCH,
                    severity=MismatchSeverity.CRITICAL,
                    api_name=func_name,
                    file_path=file_path,
                    line_number=line,
                    column=column,
                    description=f"Parameter count mismatch for '{func_name}'",
                    expected=f"{expected_param_count} parameters",
                    actual=f"{actual_param_count} parameters",
                    parameter_mismatches=[],
                    suggestion=f"Expected {expected_param_count} parameters, got {actual_param_count}. "
                              f"Check function signature: {func_info.get('return_type', '')} {func_name}(...)"
                )
        
        param_mismatches = []
        for i, (expected_param, actual_arg) in enumerate(zip(expected_params, call_args)):
            expected_type = expected_param.get('type_name', '')
            actual_type = actual_arg
            
            if not self.are_types_compatible(expected_type, actual_type):
                if self.mismatch_criteria.get('parameter_type_mismatch', True):
                    param_mismatches.append(ParameterMismatch(
                        position=i + 1,
                        expected_type=expected_type,
                        actual_type=actual_type
                    ))
        
        if param_mismatches:
            return MismatchIssue(
                mismatch_type=MismatchType.PARAMETER_TYPE_MISMATCH,
                severity=MismatchSeverity.WARNING if self.allow_implicit_cast else MismatchSeverity.CRITICAL,
                api_name=func_name,
                file_path=file_path,
                line_number=line,
                column=column,
                description=f"Parameter type mismatch for '{func_name}'",
                expected=', '.join([pm.expected_type for pm in param_mismatches]),
                actual=', '.join([pm.actual_type for pm in param_mismatches]),
                parameter_mismatches=param_mismatches,
                suggestion=f"Check parameter types for '{func_name}'. "
                          f"Expected types: {', '.join([p.get('type_name', '') for p in expected_params])}"
            )
        
        return None
    
    def extract_function_calls_from_ast(self, cursor, file_path: str) -> List[Tuple[str, List[str], int, int]]:
        calls = []
        
        for child in cursor.walk_preorder():
            if child.kind == CursorKind.CALL_EXPR:
                func_name = child.spelling
                if func_name.startswith('ogc_'):
                    args = []
                    for arg in child.get_arguments():
                        args.append(arg.type.spelling)
                    
                    line = child.location.line
                    column = child.location.column
                    calls.append((func_name, args, line, column))
        
        return calls
    
    def parse_and_validate_test_file(self, test_file: Path) -> List[MismatchIssue]:
        self.logger.info(f"Validating test file: {test_file}")
        issues = []
        
        args = ['-std=c++11', '-I' + str(self.project_root / 'install' / 'include')]
        
        try:
            tu = self.index.parse(
                str(test_file),
                args=args,
                options=TranslationUnit.PARSE_SKIP_FUNCTION_BODIES
            )
            
            if tu.diagnostics:
                for diag in tu.diagnostics:
                    if diag.severity >= diag.Error:
                        self.logger.warning(f"Parse error in {test_file}: {diag.spelling}")
            
            file_path = str(test_file.relative_to(self.project_root))
            
            calls = self.extract_function_calls_from_ast(tu.cursor, file_path)
            
            for func_name, call_args, line, column in calls:
                issue = self.validate_function_call(
                    func_name, call_args, file_path, line, column
                )
                if issue:
                    issues.append(issue)
                    
        except Exception as e:
            self.logger.error(f"Failed to parse {test_file}: {e}")
        
        return issues
    
    def validate_all_tests(self) -> List[MismatchIssue]:
        test_files = self.get_test_files()
        
        for test_file in test_files:
            file_issues = self.parse_and_validate_test_file(test_file)
            self.issues.extend(file_issues)
        
        self.logger.info(f"Found {len(self.issues)} API mismatch issues")
        return self.issues
    
    def categorize_issues(self) -> Dict[str, List[MismatchIssue]]:
        categorized = defaultdict(list)
        for issue in self.issues:
            categorized[issue.severity.value].append(issue)
        return dict(categorized)
    
    def get_statistics(self) -> Dict[str, Any]:
        categorized = self.categorize_issues()
        
        stats = {
            'total_issues': len(self.issues),
            'by_severity': {
                severity: len(issues) 
                for severity, issues in categorized.items()
            },
            'by_type': defaultdict(int)
        }
        
        for issue in self.issues:
            stats['by_type'][issue.mismatch_type.value] += 1
        
        stats['by_type'] = dict(stats['by_type'])
        
        return stats
    
    def save_validation_results(self, output_path: Optional[Path] = None) -> Path:
        if output_path is None:
            output_dir = self.config.get('report', {}).get('output_dir', 'install/bin/clang_tool/reports')
            output_path = self.project_root / output_dir / 'validation_results.json'
        
        output_path.parent.mkdir(parents=True, exist_ok=True)
        
        results = {
            'statistics': self.get_statistics(),
            'issues': [issue.to_dict() for issue in self.issues]
        }
        
        with open(output_path, 'w', encoding='utf-8') as f:
            json.dump(results, f, indent=2, ensure_ascii=False)
        
        self.logger.info(f"Validation results saved to: {output_path}")
        return output_path


def main():
    import argparse
    
    parser = argparse.ArgumentParser(description='Validate API usage in test files')
    parser.add_argument('--config', '-c', default='config.json', help='Configuration file path')
    parser.add_argument('--index', '-i', default='api_index.json', help='API index file path')
    parser.add_argument('--output', '-o', help='Output file path')
    parser.add_argument('--verbose', '-v', action='store_true', help='Verbose output')
    
    args = parser.parse_args()
    
    logging.basicConfig(
        level=logging.DEBUG if args.verbose else logging.INFO,
        format='%(asctime)s - %(name)s - %(levelname)s - %(message)s'
    )
    
    config_path = Path(args.config)
    if not config_path.is_absolute():
        config_path = Path(__file__).parent / config_path
    
    with open(config_path, 'r', encoding='utf-8') as f:
        config = json.load(f)
    
    index_path = Path(args.index)
    if not index_path.is_absolute():
        index_path = Path(__file__).parent / 'index' / args.index
    
    with open(index_path, 'r', encoding='utf-8') as f:
        api_index = json.load(f)
    
    validator = APIValidator(config, api_index, str(Path(__file__).parent.parent.parent))
    issues = validator.validate_all_tests()
    
    output_path = Path(args.output) if args.output else None
    validator.save_validation_results(output_path)
    
    print(f"\nValidation Summary:")
    print(f"  Total issues: {len(issues)}")
    stats = validator.get_statistics()
    for severity, count in stats['by_severity'].items():
        print(f"  {severity.upper()}: {count}")


if __name__ == '__main__':
    main()
