#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
SDK API Index and Validation Tool
Main entry point for the complete workflow
"""

import os
import sys
import json
import logging
import argparse
from pathlib import Path
from typing import Optional

from api_indexer import APIIndexBuilder
from example_extractor import ExampleExtractor
from api_validator import APIValidator
from report_generator import ReportGenerator


class SDKAPITool:
    def __init__(self, config_path: str, project_root: Optional[str] = None):
        self.config_path = Path(config_path)
        self.project_root = Path(project_root) if project_root else self.config_path.parent.parent.parent
        
        with open(self.config_path, 'r', encoding='utf-8') as f:
            self.config = json.load(f)
        
        self._setup_logging()
        
        self.api_index = None
        self.examples = None
        self.validation_results = None
        
    def _setup_logging(self):
        output_config = self.config.get('output', {})
        log_level = getattr(logging, output_config.get('log_level', 'INFO').upper())
        log_file = output_config.get('log_file')
        
        handlers = [logging.StreamHandler(sys.stdout)]
        
        if log_file:
            log_path = self.project_root / log_file
            log_path.parent.mkdir(parents=True, exist_ok=True)
            handlers.append(logging.FileHandler(log_path, encoding='utf-8'))
        
        logging.basicConfig(
            level=log_level,
            format='%(asctime)s - %(name)s - %(levelname)s - %(message)s',
            handlers=handlers
        )
        
        self.logger = logging.getLogger(__name__)
        
    def build_api_index(self, output_path: Optional[str] = None) -> dict:
        self.logger.info("=" * 60)
        self.logger.info("Step 1: Building API Index from SDK Headers")
        self.logger.info("=" * 60)
        
        builder = APIIndexBuilder(self.config, str(self.project_root))
        self.api_index = builder.build_index()
        
        output = Path(output_path) if output_path else None
        saved_path = builder.save_index(self.api_index, output)
        
        self.logger.info(f"API Index built successfully: {len(self.api_index['functions'])} functions")
        return self.api_index
    
    def extract_examples(self, output_path: Optional[str] = None) -> dict:
        self.logger.info("=" * 60)
        self.logger.info("Step 2: Extracting API Usage Examples from Tests")
        self.logger.info("=" * 60)
        
        extractor = ExampleExtractor(self.config, str(self.project_root))
        self.examples = extractor.extract_examples()
        
        output = Path(output_path) if output_path else None
        saved_path = extractor.save_examples(self.examples, output)
        
        self.logger.info(f"Examples extracted: {self.examples['metadata']['total_examples']} examples")
        return self.examples
    
    def validate_apis(self, output_path: Optional[str] = None) -> dict:
        self.logger.info("=" * 60)
        self.logger.info("Step 3: Validating API Usage in Test Files")
        self.logger.info("=" * 60)
        
        if not self.api_index:
            self.logger.error("API index not available. Run build_api_index() first.")
            return None
        
        validator = APIValidator(self.config, self.api_index, str(self.project_root))
        issues = validator.validate_all_tests()
        self.validation_results = {
            'statistics': validator.get_statistics(),
            'issues': [issue.to_dict() for issue in issues]
        }
        
        output = Path(output_path) if output_path else None
        saved_path = validator.save_validation_results(output)
        
        self.logger.info(f"Validation complete: {len(issues)} issues found")
        return self.validation_results
    
    def generate_report(self, output_path: Optional[str] = None) -> str:
        self.logger.info("=" * 60)
        self.logger.info("Step 4: Generating Analysis Report")
        self.logger.info("=" * 60)
        
        if not self.validation_results:
            self.logger.error("Validation results not available. Run validate_apis() first.")
            return None
        
        if not self.api_index:
            self.logger.error("API index not available. Run build_api_index() first.")
            return None
        
        if not self.examples:
            self.logger.warning("Examples not available. Report will not include code examples.")
            self.examples = {'examples': {}}
        
        generator = ReportGenerator(
            self.config, self.validation_results, self.api_index, 
            self.examples, str(self.project_root)
        )
        
        output = Path(output_path) if output_path else None
        saved_path = generator.save_report(output)
        
        self.logger.info(f"Report generated: {saved_path}")
        return str(saved_path)
    
    def run_full_workflow(self, skip_index: bool = False, skip_examples: bool = False) -> dict:
        self.logger.info("=" * 60)
        self.logger.info("SDK API Index and Validation Tool - Full Workflow")
        self.logger.info("=" * 60)
        
        results = {}
        
        if not skip_index:
            results['index'] = self.build_api_index()
        
        if not skip_examples:
            results['examples'] = self.extract_examples()
        
        results['validation'] = self.validate_apis()
        results['report'] = self.generate_report()
        
        self.logger.info("=" * 60)
        self.logger.info("Workflow Complete!")
        self.logger.info("=" * 60)
        
        self._print_summary()
        
        return results
    
    def _print_summary(self):
        if not self.validation_results:
            return
        
        stats = self.validation_results.get('statistics', {})
        
        print("\n" + "=" * 60)
        print("VALIDATION SUMMARY")
        print("=" * 60)
        print(f"Total Issues: {stats.get('total_issues', 0)}")
        print("\nBy Severity:")
        for severity, count in stats.get('by_severity', {}).items():
            print(f"  {severity.upper():12s}: {count}")
        print("\nBy Type:")
        for issue_type, count in stats.get('by_type', {}).items():
            print(f"  {issue_type:30s}: {count}")
        print("=" * 60 + "\n")


def main():
    parser = argparse.ArgumentParser(
        description='SDK API Index and Validation Tool',
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  # Run full workflow
  python main.py --full
  
  # Build API index only
  python main.py --index-only
  
  # Validate and generate report (skip index build)
  python main.py --validate --skip-index
  
  # Use custom config
  python main.py --config my_config.json --full
"""
    )
    
    parser.add_argument('--config', '-c', default='config.json',
                       help='Configuration file path (default: config.json)')
    parser.add_argument('--project-root', '-p',
                       help='Project root directory (default: auto-detect)')
    
    action_group = parser.add_mutually_exclusive_group()
    action_group.add_argument('--full', '-f', action='store_true',
                             help='Run full workflow (index, extract, validate, report)')
    action_group.add_argument('--index-only', action='store_true',
                             help='Build API index only')
    action_group.add_argument('--validate-only', action='store_true',
                             help='Validate APIs only (requires existing index)')
    action_group.add_argument('--report-only', action='store_true',
                             help='Generate report only (requires existing validation results)')
    
    parser.add_argument('--skip-index', action='store_true',
                       help='Skip index building (use existing index)')
    parser.add_argument('--skip-examples', action='store_true',
                       help='Skip example extraction')
    
    parser.add_argument('--output-index', '-oi',
                       help='Output path for API index')
    parser.add_argument('--output-examples', '-oe',
                       help='Output path for examples')
    parser.add_argument('--output-validation', '-ov',
                       help='Output path for validation results')
    parser.add_argument('--output-report', '-or',
                       help='Output path for report')
    
    parser.add_argument('--verbose', '-v', action='store_true',
                       help='Enable verbose output')
    
    args = parser.parse_args()
    
    config_path = Path(args.config)
    if not config_path.is_absolute():
        config_path = Path(__file__).parent / args.config
    
    tool = SDKAPITool(str(config_path), args.project_root)
    
    if args.verbose:
        logging.getLogger().setLevel(logging.DEBUG)
    
    try:
        if args.full:
            tool.run_full_workflow(
                skip_index=args.skip_index,
                skip_examples=args.skip_examples
            )
        elif args.index_only:
            tool.build_api_index(args.output_index)
        elif args.validate_only:
            tool.validate_apis(args.output_validation)
        elif args.report_only:
            tool.generate_report(args.output_report)
        else:
            parser.print_help()
            print("\nTip: Use --full to run the complete workflow")
            
    except KeyboardInterrupt:
        print("\nOperation cancelled by user")
        sys.exit(1)
    except Exception as e:
        logging.error(f"Error: {e}", exc_info=args.verbose)
        sys.exit(1)


if __name__ == '__main__':
    main()
