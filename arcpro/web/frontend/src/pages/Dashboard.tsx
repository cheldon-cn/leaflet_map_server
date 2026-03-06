import React from 'react'
import { Card, Row, Col, Statistic, Progress, Table, Space, Button } from 'antd'
import {
  ProjectOutlined,
  SyncOutlined,
  CheckCircleOutlined,
  ClockCircleOutlined,
  FileTextOutlined,
  CloudUploadOutlined,
} from '@ant-design/icons'
import { useQuery } from 'react-query'
import api from '../services/api'

const Dashboard: React.FC = () => {
  // 获取统计数据的查询
  const { data: stats, isLoading: statsLoading } = useQuery(
    'dashboardStats',
    async () => {
      // 这里应该调用API获取统计数据
      // 暂时返回模拟数据
      return {
        totalProjects: 12,
        totalTasks: 48,
        completedTasks: 36,
        pendingTasks: 8,
        failedTasks: 4,
        storageUsed: 65, // 百分比
        recentActivities: [
          { id: 1, type: 'project', action: '创建工程', name: '城市规划项目', time: '2024-01-15 10:30' },
          { id: 2, type: 'task', action: '开始迁移', name: '迁移任务#001', time: '2024-01-15 11:15' },
          { id: 3, type: 'task', action: '完成迁移', name: '迁移任务#002', time: '2024-01-15 14:45' },
          { id: 4, type: 'project', action: '上传文件', name: '环境监测项目', time: '2024-01-16 09:20' },
        ],
      }
    }
  )

  // 快速操作
  const quickActions = [
    {
      title: '上传工程',
      icon: <CloudUploadOutlined style={{ fontSize: '24px' }} />,
      description: '上传ArcGIS Pro工程文件',
      path: '/projects',
      color: '#1890ff',
    },
    {
      title: '创建任务',
      icon: <FileTextOutlined style={{ fontSize: '24px' }} />,
      description: '创建新的迁移任务',
      path: '/tasks',
      color: '#52c41a',
    },
    {
      title: '查看监控',
      icon: <ClockCircleOutlined style={{ fontSize: '24px' }} />,
      description: '查看系统运行状态',
      path: '/monitoring',
      color: '#faad14',
    },
    {
      title: '系统设置',
      icon: <ProjectOutlined style={{ fontSize: '24px' }} />,
      description: '配置系统参数',
      path: '/settings',
      color: '#722ed1',
    },
  ]

  return (
    <div>
      <h1 style={{ marginBottom: '24px' }}>仪表板</h1>
      
      {/* 统计卡片 */}
      <Row gutter={[16, 16]} style={{ marginBottom: '24px' }}>
        <Col xs={24} sm={12} md={6}>
          <Card>
            <Statistic
              title="总工程数"
              value={stats?.totalProjects || 0}
              prefix={<ProjectOutlined />}
              valueStyle={{ color: '#1890ff' }}
            />
          </Card>
        </Col>
        <Col xs={24} sm={12} md={6}>
          <Card>
            <Statistic
              title="总任务数"
              value={stats?.totalTasks || 0}
              prefix={<SyncOutlined />}
              valueStyle={{ color: '#52c41a' }}
            />
          </Card>
        </Col>
        <Col xs={24} sm={12} md={6}>
          <Card>
            <Statistic
              title="完成率"
              value={stats?.completedTasks || 0}
              suffix={`/ ${stats?.totalTasks || 0}`}
              prefix={<CheckCircleOutlined />}
              valueStyle={{ color: '#fa8c16' }}
            />
          </Card>
        </Col>
        <Col xs={24} sm={12} md={6}>
          <Card>
            <div style={{ marginBottom: '8px' }}>
              <div style={{ display: 'flex', justifyContent: 'space-between' }}>
                <span>存储使用</span>
                <span>{stats?.storageUsed || 0}%</span>
              </div>
              <Progress percent={stats?.storageUsed || 0} size="small" />
            </div>
          </Card>
        </Col>
      </Row>

      {/* 快速操作 */}
      <Card title="快速操作" style={{ marginBottom: '24px' }}>
        <Row gutter={[16, 16]}>
          {quickActions.map((action, index) => (
            <Col xs={24} sm={12} md={6} key={index}>
              <Card
                hoverable
                style={{
                  textAlign: 'center',
                  borderColor: action.color,
                  borderWidth: '2px',
                }}
                bodyStyle={{ padding: '24px 16px' }}
              >
                <div style={{ color: action.color, marginBottom: '12px' }}>
                  {action.icon}
                </div>
                <h3 style={{ marginBottom: '8px' }}>{action.title}</h3>
                <p style={{ color: '#666', fontSize: '12px' }}>{action.description}</p>
                <Button type="primary" style={{ marginTop: '12px' }}>
                  立即前往
                </Button>
              </Card>
            </Col>
          ))}
        </Row>
      </Card>

      {/* 最近活动 */}
      <Card title="最近活动">
        <Table
          dataSource={stats?.recentActivities}
          loading={statsLoading}
          pagination={false}
          columns={[
            {
              title: '类型',
              dataIndex: 'type',
              key: 'type',
              render: (type) => {
                const iconMap = {
                  project: <ProjectOutlined />,
                  task: <SyncOutlined />,
                }
                const textMap = {
                  project: '工程',
                  task: '任务',
                }
                return (
                  <Space>
                    {iconMap[type as keyof typeof iconMap]}
                    {textMap[type as keyof typeof textMap]}
                  </Space>
                )
              },
            },
            {
              title: '操作',
              dataIndex: 'action',
              key: 'action',
            },
            {
              title: '名称',
              dataIndex: 'name',
              key: 'name',
            },
            {
              title: '时间',
              dataIndex: 'time',
              key: 'time',
            },
          ]}
        />
      </Card>
    </div>
  )
}

export default Dashboard