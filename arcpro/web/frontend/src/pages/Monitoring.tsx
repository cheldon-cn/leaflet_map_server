import React from 'react'
import { Card, Row, Col, Statistic, Table, Progress, Space, Button } from 'antd'
import {
  DatabaseOutlined,
  CloudServerOutlined,
  ClockCircleOutlined,
  CheckCircleOutlined,
  SyncOutlined,
  ReloadOutlined,
} from '@ant-design/icons'
import { useQuery } from 'react-query'
import { monitoringApi } from '../services/api'

const Monitoring: React.FC = () => {
  // 获取系统状态
  const { data: healthData, refetch: refetchHealth } = useQuery(
    'health',
    monitoringApi.healthCheck,
    { refetchInterval: 30000 } // 每30秒刷新一次
  )

  // 获取系统统计
  const { data: statsData, refetch: refetchStats } = useQuery(
    'systemStats',
    monitoringApi.getSystemStats,
    { refetchInterval: 60000 } // 每60秒刷新一次
  )

  // 获取任务监控
  const { data: tasksData, refetch: refetchTasks } = useQuery(
    'taskMonitoring',
    monitoringApi.getTaskMonitoring,
    { refetchInterval: 10000 } // 每10秒刷新一次
  )

  // 系统资源卡片
  const resourceCards = [
    {
      title: 'CPU使用率',
      value: statsData?.system?.cpu_percent || 0,
      suffix: '%',
      icon: <CloudServerOutlined />,
      color: '#1890ff',
      status: (statsData?.system?.cpu_percent || 0) > 80 ? 'error' : 'normal',
    },
    {
      title: '内存使用率',
      value: statsData?.system?.memory_percent || 0,
      suffix: '%',
      icon: <DatabaseOutlined />,
      color: '#52c41a',
      status: (statsData?.system?.memory_percent || 0) > 85 ? 'error' : 'normal',
    },
    {
      title: '磁盘使用率',
      value: statsData?.system?.disk_percent || 0,
      suffix: '%',
      icon: <DatabaseOutlined />,
      color: '#fa8c16',
      status: (statsData?.system?.disk_percent || 0) > 90 ? 'error' : 'normal',
    },
    {
      title: '系统状态',
      value: healthData?.status === 'healthy' ? '正常' : '异常',
      icon: <CheckCircleOutlined />,
      color: healthData?.status === 'healthy' ? '#52c41a' : '#f5222d',
    },
  ]

  // 任务状态统计
  const taskStatusColumns = [
    {
      title: '状态',
      dataIndex: 'status',
      key: 'status',
      render: (status: string) => {
        const statusMap: Record<string, { color: string; text: string }> = {
          pending: { color: 'blue', text: '等待中' },
          running: { color: 'orange', text: '运行中' },
          completed: { color: 'green', text: '已完成' },
          failed: { color: 'red', text: '失败' },
          cancelled: { color: 'gray', text: '已取消' },
        }
        const statusInfo = statusMap[status] || { color: 'default', text: status }
        return <Tag color={statusInfo.color}>{statusInfo.text}</Tag>
      },
    },
    {
      title: '数量',
      dataIndex: 'count',
      key: 'count',
    },
    {
      title: '占比',
      key: 'percentage',
      render: (record: any) => {
        const total = tasksData?.task_stats?.reduce((sum: number, item: any) => sum + item.count, 0) || 1
        const percentage = Math.round((record.count / total) * 100)
        return <Progress percent={percentage} size="small" />
      },
    },
  ]

  // 最近任务表格列
  const recentTasksColumns = [
    {
      title: '任务名称',
      dataIndex: 'name',
      key: 'name',
    },
    {
      title: '状态',
      dataIndex: 'status',
      key: 'status',
      render: (status: string) => {
        const statusMap: Record<string, { color: string; text: string }> = {
          pending: { color: 'blue', text: '等待中' },
          running: { color: 'orange', text: '运行中' },
          completed: { color: 'green', text: '已完成' },
          failed: { color: 'red', text: '失败' },
          cancelled: { color: 'gray', text: '已取消' },
        }
        const statusInfo = statusMap[status] || { color: 'default', text: status }
        return <Tag color={statusInfo.color}>{statusInfo.text}</Tag>
      },
    },
    {
      title: '进度',
      dataIndex: 'progress',
      key: 'progress',
      render: (progress: number) => `${Math.round(progress * 100)}%`,
    },
    {
      title: '创建时间',
      dataIndex: 'created_at',
      key: 'created_at',
      render: (date: string) => new Date(date).toLocaleString(),
    },
  ]

  return (
    <div>
      <div style={{ display: 'flex', justifyContent: 'space-between', marginBottom: '16px' }}>
        <h1 style={{ margin: 0 }}>系统监控</h1>
        <Space>
          <Button
            icon={<ReloadOutlined />}
            onClick={() => {
              refetchHealth()
              refetchStats()
              refetchTasks()
            }}
          >
            刷新
          </Button>
        </Space>
      </div>

      {/* 系统资源 */}
      <Card title="系统资源" style={{ marginBottom: '24px' }}>
        <Row gutter={[16, 16]}>
          {resourceCards.map((card, index) => (
            <Col xs={24} sm={12} md={6} key={index}>
              <Card
                size="small"
                style={{
                  borderColor: card.color,
                  borderWidth: card.status === 'error' ? '2px' : '1px',
                }}
              >
                <Statistic
                  title={card.title}
                  value={card.value}
                  suffix={card.suffix}
                  prefix={card.icon}
                  valueStyle={{ color: card.color }}
                />
                {card.status === 'error' && (
                  <div style={{ color: '#f5222d', fontSize: '12px', marginTop: '8px' }}>
                    警告: 资源使用率过高
                  </div>
                )}
              </Card>
            </Col>
          ))}
        </Row>
      </Card>

      {/* 任务状态统计 */}
      <Row gutter={[16, 16]}>
        <Col xs={24} lg={12}>
          <Card
            title="任务状态统计"
            extra={<Button type="link" icon={<SyncOutlined />} onClick={refetchTasks} />}
          >
            <Table
              columns={taskStatusColumns}
              dataSource={tasksData?.task_stats || []}
              pagination={false}
              rowKey="status"
            />
          </Card>
        </Col>
        <Col xs={24} lg={12}>
          <Card
            title="系统信息"
            extra={<Button type="link" icon={<CloudServerOutlined />} onClick={refetchStats} />}
          >
            {statsData?.system && (
              <div>
                <div style={{ marginBottom: '12px' }}>
                  <strong>内存:</strong>{' '}
                  {`${(statsData.system.memory_used_gb || 0).toFixed(2)} GB / ${(
                    statsData.system.memory_total_gb || 0
                  ).toFixed(2)} GB`}
                </div>
                <div style={{ marginBottom: '12px' }}>
                  <strong>磁盘:</strong>{' '}
                  {`${(statsData.system.disk_used_gb || 0).toFixed(2)} GB / ${(
                    statsData.system.disk_total_gb || 0
                  ).toFixed(2)} GB`}
                </div>
                <div>
                  <strong>最后更新:</strong>{' '}
                  {new Date(statsData.timestamp).toLocaleString()}
                </div>
              </div>
            )}
          </Card>
        </Col>
      </Row>

      {/* 最近任务 */}
      <Card
        title="最近任务"
        style={{ marginTop: '24px' }}
        extra={<Button type="link" icon={<ClockCircleOutlined />} onClick={refetchTasks} />}
      >
        <Table
          columns={recentTasksColumns}
          dataSource={tasksData?.recent_tasks || []}
          pagination={false}
          rowKey="id"
        />
      </Card>

      {/* 系统日志 */}
      <Card title="系统日志" style={{ marginTop: '24px' }}>
        <div style={{ height: '200px', overflowY: 'auto', background: '#f5f5f5', padding: '12px' }}>
          <div style={{ fontFamily: 'monospace', fontSize: '12px' }}>
            {healthData && (
              <div>
                [{new Date(healthData.timestamp).toLocaleTimeString()}] 健康检查: {healthData.status}
              </div>
            )}
            {statsData && (
              <div>
                [{new Date(statsData.timestamp).toLocaleTimeString()}] 系统统计: CPU{' '}
                {statsData.system?.cpu_percent}%, 内存 {statsData.system?.memory_percent}%
              </div>
            )}
            <div>
              [{new Date().toLocaleTimeString()}] 监控面板已加载
            </div>
          </div>
        </div>
      </Card>
    </div>
  )
}

export default Monitoring