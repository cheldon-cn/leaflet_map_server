import React, { useState } from 'react'
import {
  Card,
  Table,
  Button,
  Space,
  Modal,
  Form,
  Input,
  Select,
  message,
  Tag,
  Progress,
  Popconfirm,
  Tooltip,
  Badge,
} from 'antd'
import {
  PlusOutlined,
  PlayCircleOutlined,
  PauseCircleOutlined,
  EyeOutlined,
  DeleteOutlined,
  SyncOutlined,
  FileTextOutlined,
} from '@ant-design/icons'
import { useQuery, useMutation, useQueryClient } from 'react-query'
import { taskApi, projectApi } from '../services/api'

const { Search } = Input
const { Option } = Select

const Tasks: React.FC = () => {
  const [isModalVisible, setIsModalVisible] = useState(false)
  const [searchText, setSearchText] = useState('')
  const [form] = Form.useForm()
  const queryClient = useQueryClient()

  // 获取任务列表
  const { data: tasksData, isLoading } = useQuery(
    ['tasks', searchText],
    () => taskApi.getTasks({ limit: 100 })
  )

  // 获取工程列表（用于选择）
  const { data: projectsData } = useQuery('projectsForSelect', () =>
    projectApi.getProjects({ limit: 100 })
  )

  // 创建任务
  const createMutation = useMutation(
    (data: any) => taskApi.createTask(data),
    {
      onSuccess: () => {
        message.success('任务创建成功')
        setIsModalVisible(false)
        form.resetFields()
        queryClient.invalidateQueries('tasks')
      },
      onError: (error: any) => {
        message.error(`创建失败: ${error.response?.data?.message || error.message}`)
      },
    }
  )

  // 开始任务
  const startMutation = useMutation(
    (id: string) => taskApi.startTask(id),
    {
      onSuccess: () => {
        message.success('任务开始执行')
        queryClient.invalidateQueries('tasks')
      },
      onError: (error: any) => {
        message.error(`启动失败: ${error.response?.data?.message || error.message}`)
      },
    }
  )

  // 取消任务
  const cancelMutation = useMutation(
    (id: string) => taskApi.cancelTask(id),
    {
      onSuccess: () => {
        message.success('任务已取消')
        queryClient.invalidateQueries('tasks')
      },
      onError: (error: any) => {
        message.error(`取消失败: ${error.response?.data?.message || error.message}`)
      },
    }
  )

  // 删除任务
  const deleteMutation = useMutation(
    (id: string) => taskApi.deleteTask(id),
    {
      onSuccess: () => {
        message.success('任务删除成功')
        queryClient.invalidateQueries('tasks')
      },
      onError: (error: any) => {
        message.error(`删除失败: ${error.response?.data?.message || error.message}`)
      },
    }
  )

  const handleCreate = () => {
    form.validateFields().then((values) => {
      createMutation.mutate({
        project_id: values.project_id,
        name: values.name,
        description: values.description,
        config: {
          output_format: values.output_format,
          convert_data: values.convert_data,
          verbose: values.verbose,
        },
      })
    })
  }

  const handleStart = (id: string) => {
    startMutation.mutate(id)
  }

  const handleCancel = (id: string) => {
    cancelMutation.mutate(id)
  }

  const handleDelete = (id: string) => {
    deleteMutation.mutate(id)
  }

  const columns = [
    {
      title: '任务名称',
      dataIndex: 'name',
      key: 'name',
      render: (text: string, record: any) => (
        <Space>
          <FileTextOutlined />
          <span>{text}</span>
          {record.status === 'running' && (
            <Badge status="processing" text="运行中" />
          )}
        </Space>
      ),
    },
    {
      title: '所属工程',
      dataIndex: 'project_id',
      key: 'project',
      render: (projectId: string) => {
        const project = projectsData?.items?.find((p: any) => p.id === projectId)
        return project ? project.name : projectId
      },
    },
    {
      title: '状态',
      dataIndex: 'status',
      key: 'status',
      render: (status: string, record: any) => {
        const statusMap: Record<string, { color: string; text: string }> = {
          pending: { color: 'blue', text: '等待中' },
          running: { color: 'orange', text: '运行中' },
          completed: { color: 'green', text: '已完成' },
          failed: { color: 'red', text: '失败' },
          cancelled: { color: 'gray', text: '已取消' },
        }
        const statusInfo = statusMap[status] || { color: 'default', text: status }
        return (
          <Space direction="vertical" size="small">
            <Tag color={statusInfo.color}>{statusInfo.text}</Tag>
            {record.status === 'running' && (
              <Progress percent={Math.round(record.progress * 100)} size="small" />
            )}
          </Space>
        )
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
    {
      title: '操作',
      key: 'actions',
      render: (_: any, record: any) => (
        <Space size="small">
          <Button
            type="link"
            icon={<EyeOutlined />}
            onClick={() => message.info(`查看任务 ${record.name}`)}
          >
            查看
          </Button>
          
          {record.status === 'pending' && (
            <Button
              type="link"
              icon={<PlayCircleOutlined />}
              onClick={() => handleStart(record.id)}
              loading={startMutation.isLoading && startMutation.variables === record.id}
            >
              开始
            </Button>
          )}
          
          {record.status === 'running' && (
            <Button
              type="link"
              icon={<PauseCircleOutlined />}
              onClick={() => handleCancel(record.id)}
              loading={cancelMutation.isLoading && cancelMutation.variables === record.id}
            >
              取消
            </Button>
          )}
          
          {record.status === 'completed' && (
            <Button
              type="link"
              icon={<SyncOutlined />}
              onClick={() => message.info(`重新执行任务 ${record.name}`)}
            >
              重新执行
            </Button>
          )}
          
          {['pending', 'completed', 'failed', 'cancelled'].includes(record.status) && (
            <Popconfirm
              title="确定要删除这个任务吗？"
              onConfirm={() => handleDelete(record.id)}
              okText="确定"
              cancelText="取消"
            >
              <Button type="link" danger icon={<DeleteOutlined />}>
                删除
              </Button>
            </Popconfirm>
          )}
        </Space>
      ),
    },
  ]

  return (
    <div>
      <div style={{ display: 'flex', justifyContent: 'space-between', marginBottom: '16px' }}>
        <h1 style={{ margin: 0 }}>迁移任务</h1>
        <Space>
          <Search
            placeholder="搜索任务"
            allowClear
            onSearch={setSearchText}
            style={{ width: 200 }}
          />
          <Button
            type="primary"
            icon={<PlusOutlined />}
            onClick={() => setIsModalVisible(true)}
          >
            新建任务
          </Button>
        </Space>
      </div>

      <Card>
        <Table
          columns={columns}
          dataSource={tasksData?.items || []}
          loading={isLoading}
          rowKey="id"
          pagination={{
            pageSize: 10,
            showSizeChanger: true,
            showTotal: (total) => `共 ${total} 个任务`,
          }}
        />
      </Card>

      {/* 创建任务模态框 */}
      <Modal
        title="新建迁移任务"
        open={isModalVisible}
        onOk={handleCreate}
        onCancel={() => {
          setIsModalVisible(false)
          form.resetFields()
        }}
        confirmLoading={createMutation.isLoading}
        width={600}
      >
        <Form form={form} layout="vertical">
          <Form.Item
            name="project_id"
            label="选择工程"
            rules={[{ required: true, message: '请选择工程' }]}
          >
            <Select placeholder="请选择工程" loading={!projectsData}>
              {projectsData?.items?.map((project: any) => (
                <Option key={project.id} value={project.id}>
                  {project.name} ({project.status})
                </Option>
              ))}
            </Select>
          </Form.Item>
          
          <Form.Item
            name="name"
            label="任务名称"
            rules={[{ required: true, message: '请输入任务名称' }]}
          >
            <Input placeholder="请输入任务名称" />
          </Form.Item>
          
          <Form.Item name="description" label="任务描述">
            <Input.TextArea placeholder="请输入任务描述" rows={3} />
          </Form.Item>
          
          <Form.Item
            name="output_format"
            label="输出格式"
            initialValue="GPKG"
          >
            <Select>
              <Option value="GPKG">GeoPackage (GPKG)</Option>
              <Option value="SHP">Shapefile (SHP)</Option>
              <Option value="GEOJSON">GeoJSON</Option>
            </Select>
          </Form.Item>
          
          <Form.Item
            name="convert_data"
            label="转换数据"
            initialValue={true}
          >
            <Select>
              <Option value={true}>是</Option>
              <Option value={false}>否</Option>
            </Select>
          </Form.Item>
          
          <Form.Item
            name="verbose"
            label="详细输出"
            initialValue={false}
          >
            <Select>
              <Option value={true}>是</Option>
              <Option value={false}>否</Option>
            </Select>
          </Form.Item>
        </Form>
      </Modal>
    </div>
  )
}

export default Tasks