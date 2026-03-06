#!/usr/bin/env python
"""
ArcGIS Pro 工程文件解析脚本
主程序入口
"""
import os
import sys
import argparse

# 添加当前目录到 Python 路径，以便导入 aprx_parser
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

from aprx_parser import parse_aprx, generate_xml


def main():
    """主函数"""
    parser = argparse.ArgumentParser(
        description='解析 ArcGIS Pro (.aprx) 工程文件，提取地图和图层信息并生成 XML'
    )
    
    parser.add_argument(
        'input_aprx',
        help='输入 .aprx 工程文件路径'
    )
    
    parser.add_argument(
        '-o', '--output',
        help='输出 XML 文件路径（默认：与输入文件同目录，扩展名为 .xml）',
        default=None
    )
    
    parser.add_argument(
        '-v', '--verbose',
        help='显示详细输出信息',
        action='store_true'
    )
    
    args = parser.parse_args()
    
    # 检查输入文件是否存在
    if not os.path.exists(args.input_aprx):
        print(f"错误: 输入文件不存在: {args.input_aprx}")
        sys.exit(1)
    
    # 确定输出文件路径
    if args.output is None:
        base_name = os.path.splitext(args.input_aprx)[0]
        args.output = f"{base_name}_layers.xml"
    
    try:
        if args.verbose:
            print(f"开始解析工程文件: {args.input_aprx}")
        
        # 解析 .aprx 文件
        project_data = parse_aprx(args.input_aprx)
        
        if args.verbose:
            print(f"工程名称: {project_data['project_name']}")
            print(f"地图数量: {len(project_data['maps'])}")
            total_layers = sum(len(m['layers']) for m in project_data['maps'])
            print(f"图层总数: {total_layers}")
        
        # 生成 XML 文件
        xml_content = generate_xml(project_data, args.output)
        
        if args.verbose:
            print(f"XML 文件已生成: {args.output}")
            print("\nXML 结构预览:")
            lines = xml_content.split('\n')
            for line in lines[:10]:  # 只显示前10行
                print(line)
            if len(lines) > 10:
                print("...")
        else:
            print(f"成功生成 XML 文件: {args.output}")
        
    except FileNotFoundError as e:
        print(f"文件错误: {e}")
        sys.exit(1)
    except RuntimeError as e:
        print(f"解析错误: {e}")
        sys.exit(1)
    except Exception as e:
        print(f"未预期的错误: {e}")
        sys.exit(1)


if __name__ == '__main__':
    main()