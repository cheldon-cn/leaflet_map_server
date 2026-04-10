#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
API Index Builder Module
Extracts API signatures from SDK header files using libclang
"""

import os
import json
import logging
from typing import Dict, List, Optional, Any
from pathlib import Path
from dataclasses import dataclass, asdict
from enum import Enum

try:
    from clang.cindex import Index, CursorKind, TypeKind, TranslationUnit, Cursor
except ImportError:
    print("Error: python-libclang not installed. Install with: pip install libclang")
    raise


class APIType(Enum):
    FUNCTION = "function"
    ENUM = "enum"
    STRUCT = "struct"
    TYPEDEF = "typedef"
    MACRO = "macro"


@dataclass
class ParameterInfo:
    name: str
    type_name: str
    is_pointer: bool
    is_const: bool
    is_array: bool
    array_size: Optional[int]
    
    def to_dict(self) -> Dict[str, Any]:
        return asdict(self)


@dataclass
class FunctionInfo:
    name: str
    return_type: str
    parameters: List[ParameterInfo]
    is_variadic: bool
    is_inline: bool
    is_static: bool
    file_path: str
    line_number: int
    column: int
    brief_comment: Optional[str]
    
    def to_dict(self) -> Dict[str, Any]:
        return {
            'name': self.name,
            'return_type': self.return_type,
            'parameters': [p.to_dict() for p in self.parameters],
            'is_variadic': self.is_variadic,
            'is_inline': self.is_inline,
            'is_static': self.is_static,
            'location': {
                'file': self.file_path,
                'line': self.line_number,
                'column': self.column
            },
            'brief_comment': self.brief_comment
        }


@dataclass
class EnumValueInfo:
    name: str
    value: int
    
    def to_dict(self) -> Dict[str, Any]:
        return asdict(self)


@dataclass
class EnumInfo:
    name: str
    values: List[EnumValueInfo]
    file_path: str
    line_number: int
    brief_comment: Optional[str]
    
    def to_dict(self) -> Dict[str, Any]:
        return {
            'name': self.name,
            'values': [v.to_dict() for v in self.values],
            'location': {
                'file': self.file_path,
                'line': self.line_number
            },
            'brief_comment': self.brief_comment
        }


@dataclass
class StructFieldInfo:
    name: str
    type_name: str
    offset: int
    is_pointer: bool
    
    def to_dict(self) -> Dict[str, Any]:
        return asdict(self)


@dataclass
class StructInfo:
    name: str
    fields: List[StructFieldInfo]
    size: int
    file_path: str
    line_number: int
    brief_comment: Optional[str]
    
    def to_dict(self) -> Dict[str, Any]:
        return {
            'name': self.name,
            'fields': [f.to_dict() for f in self.fields],
            'size': self.size,
            'location': {
                'file': self.file_path,
                'line': self.line_number
            },
            'brief_comment': self.brief_comment
        }


@dataclass
class TypedefInfo:
    name: str
    underlying_type: str
    file_path: str
    line_number: int
    brief_comment: Optional[str]
    
    def to_dict(self) -> Dict[str, Any]:
        return {
            'name': self.name,
            'underlying_type': self.underlying_type,
            'location': {
                'file': self.file_path,
                'line': self.line_number
            },
            'brief_comment': self.brief_comment
        }


class APIIndexBuilder:
    def __init__(self, config: Dict[str, Any], project_root: str):
        self.config = config
        self.project_root = Path(project_root)
        self.logger = logging.getLogger(__name__)
        
        self.functions: Dict[str, FunctionInfo] = {}
        self.enums: Dict[str, EnumInfo] = {}
        self.structs: Dict[str, StructInfo] = {}
        self.typedefs: Dict[str, TypedefInfo] = {}
        
        clang_config = config.get('clang', {})
        self.compilation_args = clang_config.get('compilation_args', [])
        
        self.index = Index.create()
        
    def resolve_path(self, path_str: str) -> Path:
        path = Path(path_str)
        if path.is_absolute():
            return path
        return self.project_root / path
    
    def get_header_files(self) -> List[Path]:
        header_files = []
        sdk_config = self.config.get('sdk', {})
        include_dirs = sdk_config.get('include_dirs', [])
        header_patterns = sdk_config.get('header_patterns', ['*.h'])
        exclude_patterns = sdk_config.get('exclude_patterns', [])
        
        for include_dir in include_dirs:
            dir_path = self.resolve_path(include_dir)
            if not dir_path.exists():
                self.logger.warning(f"Include directory not found: {dir_path}")
                continue
                
            for pattern in header_patterns:
                for header_file in dir_path.rglob(pattern):
                    relative_path = header_file.relative_to(self.project_root)
                    should_exclude = any(
                        relative_path.match(excl) or header_file.match(excl)
                        for excl in exclude_patterns
                    )
                    if not should_exclude:
                        header_files.append(header_file)
        
        self.logger.info(f"Found {len(header_files)} header files to parse")
        return header_files
    
    def parse_type(self, cursor: Cursor) -> str:
        try:
            return cursor.type.spelling
        except:
            return "unknown"
    
    def extract_parameters(self, cursor: Cursor) -> List[ParameterInfo]:
        params = []
        for arg in cursor.get_arguments():
            type_obj = arg.type
            
            is_pointer = type_obj.kind in [TypeKind.POINTER, TypeKind.LVALUEREFERENCE]
            is_const = type_obj.is_const_qualified() if is_pointer else False
            is_array = type_obj.kind == TypeKind.CONSTANTARRAY
            
            array_size = None
            if is_array:
                try:
                    array_size = type_obj.get_array_size()
                except:
                    pass
            
            param = ParameterInfo(
                name=arg.spelling or "unnamed",
                type_name=self.parse_type(arg),
                is_pointer=is_pointer,
                is_const=is_const,
                is_array=is_array,
                array_size=array_size
            )
            params.append(param)
        
        return params
    
    def extract_function(self, cursor: Cursor, file_path: str) -> Optional[FunctionInfo]:
        try:
            return_type = self.parse_type(cursor)
            parameters = self.extract_parameters(cursor)
            
            brief_comment = cursor.brief_comment if hasattr(cursor, 'brief_comment') else None
            
            func_info = FunctionInfo(
                name=cursor.spelling,
                return_type=return_type,
                parameters=parameters,
                is_variadic=cursor.type.is_function_variadic() if hasattr(cursor.type, 'is_function_variadic') else False,
                is_inline=cursor.is_inline_method() if hasattr(cursor, 'is_inline_method') else False,
                is_static=cursor.is_static_method() if hasattr(cursor, 'is_static_method') else False,
                file_path=file_path,
                line_number=cursor.location.line,
                column=cursor.location.column,
                brief_comment=brief_comment
            )
            
            return func_info
        except Exception as e:
            self.logger.error(f"Error extracting function {cursor.spelling}: {e}")
            return None
    
    def extract_enum(self, cursor: Cursor, file_path: str) -> Optional[EnumInfo]:
        try:
            values = []
            for child in cursor.get_children():
                if child.kind == CursorKind.ENUM_CONSTANT_DECL:
                    values.append(EnumValueInfo(
                        name=child.spelling,
                        value=child.enum_value
                    ))
            
            brief_comment = cursor.brief_comment if hasattr(cursor, 'brief_comment') else None
            
            return EnumInfo(
                name=cursor.spelling,
                values=values,
                file_path=file_path,
                line_number=cursor.location.line,
                brief_comment=brief_comment
            )
        except Exception as e:
            self.logger.error(f"Error extracting enum {cursor.spelling}: {e}")
            return None
    
    def extract_struct(self, cursor: Cursor, file_path: str) -> Optional[StructInfo]:
        try:
            fields = []
            for child in cursor.get_children():
                if child.kind == CursorKind.FIELD_DECL:
                    type_obj = child.type
                    fields.append(StructFieldInfo(
                        name=child.spelling,
                        type_name=self.parse_type(child),
                        offset=child.get_field_offsetof() if hasattr(child, 'get_field_offsetof') else 0,
                        is_pointer=type_obj.kind in [TypeKind.POINTER, TypeKind.LVALUEREFERENCE]
                    ))
            
            brief_comment = cursor.brief_comment if hasattr(cursor, 'brief_comment') else None
            
            return StructInfo(
                name=cursor.spelling,
                fields=fields,
                size=cursor.type.get_size() if hasattr(cursor.type, 'get_size') else 0,
                file_path=file_path,
                line_number=cursor.location.line,
                brief_comment=brief_comment
            )
        except Exception as e:
            self.logger.error(f"Error extracting struct {cursor.spelling}: {e}")
            return None
    
    def extract_typedef(self, cursor: Cursor, file_path: str) -> Optional[TypedefInfo]:
        try:
            underlying_type = cursor.underlying_typedef_type.spelling if hasattr(cursor, 'underlying_typedef_type') else "unknown"
            
            brief_comment = cursor.brief_comment if hasattr(cursor, 'brief_comment') else None
            
            return TypedefInfo(
                name=cursor.spelling,
                underlying_type=underlying_type,
                file_path=file_path,
                line_number=cursor.location.line,
                brief_comment=brief_comment
            )
        except Exception as e:
            self.logger.error(f"Error extracting typedef {cursor.spelling}: {e}")
            return None
    
    def parse_header(self, header_file: Path) -> None:
        self.logger.info(f"Parsing header: {header_file}")
        
        args = self.compilation_args.copy()
        args = [arg.replace('install/', str(self.project_root / 'install') + '/') for arg in args]
        
        try:
            tu = self.index.parse(
                str(header_file),
                args=args,
                options=TranslationUnit.PARSE_DETAILED_PROCESSING_RECORD
            )
            
            if tu.diagnostics:
                for diag in tu.diagnostics:
                    if diag.severity >= diag.Error:
                        self.logger.warning(f"Parse error in {header_file}: {diag.spelling}")
            
            file_path = str(header_file.relative_to(self.project_root))
            
            for cursor in tu.cursor.walk_preorder():
                if cursor.location.file and str(cursor.location.file.name) != str(header_file):
                    continue
                
                if cursor.kind == CursorKind.FUNCTION_DECL:
                    if cursor.spelling.startswith('ogc_'):
                        func_info = self.extract_function(cursor, file_path)
                        if func_info:
                            self.functions[func_info.name] = func_info
                
                elif cursor.kind == CursorKind.ENUM_DECL:
                    if cursor.spelling and cursor.spelling.startswith('ogc_'):
                        enum_info = self.extract_enum(cursor, file_path)
                        if enum_info:
                            self.enums[enum_info.name] = enum_info
                
                elif cursor.kind == CursorKind.STRUCT_DECL:
                    if cursor.spelling and cursor.spelling.startswith('ogc_'):
                        struct_info = self.extract_struct(cursor, file_path)
                        if struct_info:
                            self.structs[struct_info.name] = struct_info
                
                elif cursor.kind == CursorKind.TYPEDEF_DECL:
                    if cursor.spelling and cursor.spelling.startswith('ogc_'):
                        typedef_info = self.extract_typedef(cursor, file_path)
                        if typedef_info:
                            self.typedefs[typedef_info.name] = typedef_info
                            
        except Exception as e:
            self.logger.error(f"Failed to parse {header_file}: {e}")
    
    def build_index(self) -> Dict[str, Any]:
        header_files = self.get_header_files()
        
        for header_file in header_files:
            self.parse_header(header_file)
        
        index_data = {
            'metadata': {
                'version': self.config.get('version', '1.0.0'),
                'total_functions': len(self.functions),
                'total_enums': len(self.enums),
                'total_structs': len(self.structs),
                'total_typedefs': len(self.typedefs)
            },
            'functions': {name: info.to_dict() for name, info in self.functions.items()},
            'enums': {name: info.to_dict() for name, info in self.enums.items()},
            'structs': {name: info.to_dict() for name, info in self.structs.items()},
            'typedefs': {name: info.to_dict() for name, info in self.typedefs.items()}
        }
        
        self.logger.info(f"Index built: {len(self.functions)} functions, {len(self.enums)} enums, "
                        f"{len(self.structs)} structs, {len(self.typedefs)} typedefs")
        
        return index_data
    
    def save_index(self, index_data: Dict[str, Any], output_path: Optional[Path] = None) -> Path:
        if output_path is None:
            output_dir = self.config.get('index', {}).get('output_dir', 'install/bin/clang_tool/index')
            output_filename = self.config.get('index', {}).get('output_filename', 'api_index.json')
            output_path = self.project_root / output_dir / output_filename
        
        output_path.parent.mkdir(parents=True, exist_ok=True)
        
        with open(output_path, 'w', encoding='utf-8') as f:
            json.dump(index_data, f, indent=2, ensure_ascii=False)
        
        self.logger.info(f"Index saved to: {output_path}")
        return output_path


def main():
    import argparse
    
    parser = argparse.ArgumentParser(description='Build API index from SDK header files')
    parser.add_argument('--config', '-c', default='config.json', help='Configuration file path')
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
    
    builder = APIIndexBuilder(config, str(Path(__file__).parent.parent.parent))
    index_data = builder.build_index()
    
    output_path = Path(args.output) if args.output else None
    builder.save_index(index_data, output_path)


if __name__ == '__main__':
    main()
