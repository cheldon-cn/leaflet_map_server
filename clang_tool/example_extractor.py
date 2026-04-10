#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
API Example Extractor Module
Extracts real API usage examples from test files using libclang
"""

import os
import json
import logging
import re
from typing import Dict, List, Optional, Any, Tuple
from pathlib import Path
from dataclasses import dataclass, asdict
from collections import defaultdict

try:
    from clang.cindex import Index, CursorKind, TranslationUnit
except ImportError:
    print("Error: python-libclang not installed. Install with: pip install libclang")
    raise


@dataclass
class CodeLocation:
    file: str
    line: int
    column: int
    
    def to_dict(self) -> Dict[str, Any]:
        return asdict(self)


@dataclass
class APIExample:
    api_name: str
    code_snippet: str
    context_before: str
    context_after: str
    location: CodeLocation
    test_name: str
    description: Optional[str]
    
    def to_dict(self) -> Dict[str, Any]:
        return {
            'api_name': self.api_name,
            'code_snippet': self.code_snippet,
            'context_before': self.context_before,
            'context_after': self.context_after,
            'location': self.location.to_dict(),
            'test_name': self.test_name,
            'description': self.description
        }


class ExampleExtractor:
    def __init__(self, config: Dict[str, Any], project_root: str):
        self.config = config
        self.project_root = Path(project_root)
        self.logger = logging.getLogger(__name__)
        
        self.examples: Dict[str, List[APIExample]] = defaultdict(list)
        self.index = Index.create()
        
        example_config = config.get('index', {}).get('structure', {}).get('examples', {})
        self.max_examples_per_api = example_config.get('max_examples_per_api', 5)
        self.min_example_lines = example_config.get('min_example_lines', 3)
        self.include_context = example_config.get('include_context', True)
        
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
        
        self.logger.info(f"Found {len(test_files)} test files to analyze")
        return test_files
    
    def extract_function_calls(self, cursor, file_content: List[str]) -> List[Tuple[str, int, int]]:
        calls = []
        
        for child in cursor.walk_preorder():
            if child.kind == CursorKind.CALL_EXPR:
                func_name = child.spelling
                if func_name.startswith('ogc_'):
                    line = child.location.line
                    column = child.location.column
                    calls.append((func_name, line, column))
        
        return calls
    
    def get_code_snippet(self, file_content: List[str], line: int, 
                         context_lines: int = 3) -> Tuple[str, str, str]:
        start_line = max(0, line - context_lines - 1)
        end_line = min(len(file_content), line + context_lines)
        
        context_before = '\n'.join(file_content[start_line:line-1])
        code_snippet = file_content[line-1] if line <= len(file_content) else ""
        context_after = '\n'.join(file_content[line:end_line])
        
        return context_before, code_snippet, context_after
    
    def extract_test_name(self, cursor) -> Optional[str]:
        for child in cursor.walk_preorder():
            if child.kind == CursorKind.FUNCTION_DECL:
                if child.spelling.startswith('TEST_') or child.spelling.startswith('TEST_F'):
                    return child.spelling
        return None
    
    def extract_description_from_comment(self, file_content: List[str], line: int) -> Optional[str]:
        for i in range(max(0, line - 10), line):
            content = file_content[i].strip()
            if content.startswith('* @brief') or content.startswith('//'):
                return content.lstrip('* /@brief').strip()
        return None
    
    def parse_test_file(self, test_file: Path) -> None:
        self.logger.info(f"Parsing test file: {test_file}")
        
        try:
            with open(test_file, 'r', encoding='utf-8', errors='ignore') as f:
                file_content = f.readlines()
        except Exception as e:
            self.logger.error(f"Failed to read {test_file}: {e}")
            return
        
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
            
            calls = self.extract_function_calls(tu.cursor, file_content)
            
            for func_name, line, column in calls:
                if len(self.examples[func_name]) >= self.max_examples_per_api:
                    continue
                
                context_before, code_snippet, context_after = self.get_code_snippet(
                    file_content, line, context_lines=3
                )
                
                test_name = self.extract_test_name(tu.cursor) or test_file.stem
                
                description = self.extract_description_from_comment(file_content, line)
                
                example = APIExample(
                    api_name=func_name,
                    code_snippet=code_snippet.strip(),
                    context_before=context_before.strip() if self.include_context else "",
                    context_after=context_after.strip() if self.include_context else "",
                    location=CodeLocation(
                        file=str(test_file.relative_to(self.project_root)),
                        line=line,
                        column=column
                    ),
                    test_name=test_name,
                    description=description
                )
                
                self.examples[func_name].append(example)
                
        except Exception as e:
            self.logger.error(f"Failed to parse {test_file}: {e}")
    
    def extract_examples(self) -> Dict[str, Any]:
        test_files = self.get_test_files()
        
        for test_file in test_files:
            self.parse_test_file(test_file)
        
        examples_data = {
            'metadata': {
                'total_apis': len(self.examples),
                'total_examples': sum(len(exs) for exs in self.examples.values())
            },
            'examples': {
                api_name: [ex.to_dict() for ex in ex_list]
                for api_name, ex_list in self.examples.items()
            }
        }
        
        self.logger.info(f"Extracted {examples_data['metadata']['total_examples']} examples "
                        f"for {examples_data['metadata']['total_apis']} APIs")
        
        return examples_data
    
    def save_examples(self, examples_data: Dict[str, Any], output_path: Optional[Path] = None) -> Path:
        if output_path is None:
            output_dir = self.config.get('index', {}).get('output_dir', 'install/bin/clang_tool/index')
            output_path = self.project_root / output_dir / 'api_examples.json'
        
        output_path.parent.mkdir(parents=True, exist_ok=True)
        
        with open(output_path, 'w', encoding='utf-8') as f:
            json.dump(examples_data, f, indent=2, ensure_ascii=False)
        
        self.logger.info(f"Examples saved to: {output_path}")
        return output_path
    
    def generate_few_shot_prompt(self, api_name: str, max_examples: int = 3) -> str:
        if api_name not in self.examples:
            return f"No examples found for API: {api_name}"
        
        examples = self.examples[api_name][:max_examples]
        prompt_parts = [f"## Few-Shot Examples for {api_name}\n"]
        
        for i, example in enumerate(examples, 1):
            prompt_parts.append(f"### Example {i} (from {example.test_name})\n")
            if example.description:
                prompt_parts.append(f"Description: {example.description}\n")
            prompt_parts.append(f"```cpp\n{example.code_snippet}\n```\n")
            prompt_parts.append(f"Location: {example.location.file}:{example.location.line}\n\n")
        
        return '\n'.join(prompt_parts)


def main():
    import argparse
    
    parser = argparse.ArgumentParser(description='Extract API usage examples from test files')
    parser.add_argument('--config', '-c', default='config.json', help='Configuration file path')
    parser.add_argument('--output', '-o', help='Output file path')
    parser.add_argument('--api', '-a', help='Generate few-shot prompt for specific API')
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
    
    extractor = ExampleExtractor(config, str(Path(__file__).parent.parent.parent))
    examples_data = extractor.extract_examples()
    
    if args.api:
        prompt = extractor.generate_few_shot_prompt(args.api)
        print(prompt)
    else:
        output_path = Path(args.output) if args.output else None
        extractor.save_examples(examples_data, output_path)


if __name__ == '__main__':
    main()
