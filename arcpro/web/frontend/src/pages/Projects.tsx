import React, { useState } from 'react'
import {
  Card,
  Table,
  Button,
  Space,
  Modal,
  Form,
  Input,
  Upload,
  message,
  Tag,
  Popconfirm,
  Tooltip,
} from 'antd'
import {
  PlusOutlined,
  UploadOutlined,
  EyeOutlined,
  EditOutlined,
  DeleteOutlined,
  SyncOutlined,
  FileTextOutlined,
} from '@ant-design/icons'
import { useQuery, useMutation, useQueryClient } from 'react-query'
import { projectApi } from '../services/api'

const { Search } = Input

const Projects: React.FC = () => {
  const [isModalVisible, setIsModalVisible] = useState(false)
  const [searchText, setSearchText] = useState('')
  const [form] = Form.useForm()
  const queryClient = useQueryClient()

  // 获取工程列表
  const { data: projectsData, isLoading } = useQuery(
    ['projects', searchText],
    () => projectApi.getProjects({ limit: 100 })
  )

  // 创建工程
  const createMutation = useMutation(
    (formData: FormData) => projectApi.createProject(formData),
    {
      onSuccess: () => {
        message.success('工程创建成功')
        setIsModalVisible(false)
        form.resetFields()
        queryClient.invalidateQueries('projects')
      },
      onError: (error: any) => {
        message.error(`创建失败: ${error.response?.data?.message || error.message}`)
      },
    }
  )

  // 删除工程
  const deleteMutation = useMutation(
    (id: string) => projectApi.deleteProject(id),
    {
      onSuccess: () => {
        message.success('工程删除成功')
        queryClient.invalidateQueries('projects')
      },
      onError: (error: any) => {
        message.error(`删除失败: ${error.response?.data?.message || error.message}`)
      },
    }
  )

  // 解析工程
  const parseMutation = useMutation(
    (id: string) => projectApi.parseProject(id),
    {
      onSuccess: () => {
        message.success('工程解析成功')
        queryClient.invalidateQueries('projects')
      },
      onError: (error: any) => {
        message.error(`解析失败: ${error.response?.data?.message || error.message}`)
      },
    }
  )

  const handleCreate = () => {
    form.validateFields().then((values) => {
      const formData = new FormData()
      formData.append('name', values.name)
      formData.append('description', values.description || '')
      if (values.file && values.file[0]) {
        formData.append('file', values.file[0].originFileObj)
      }
      createMutation.mutate(formData)
    })
  }

  const handleDelete = (id: string) => {
    deleteMutation.mutate(id)
  }

  const handleParse = (id: string) => {
    parseMutation.mutate(id)
  }

  const columns = [
    {
      title: '工程名称',
      dataIndex: 'name',
      key: 'name',
      render: (text: string, record: any) => (
        <Space>
          <FileTextOutlined />
          <span>{text}</span>
          {record.status === 'parsed' && (
            <Tag color="green">已解析</Tag>
          )}
        </Space>
      ),
    },
    {
      title: '文件',
      dataIndex: 'original_filename',
      key: 'filename',
      render: (filename: string) => (
        <Tooltip title={filename}>
          <span style={{ maxWidth: '200px', overflow: 'hidden', textOverflow: 'ellipsis' }}>
            {filename}
          </span>
        </Tooltip>
      ),
    },
    {
      title: '状态',
      dataIndex: 'status',
      key: 'status',
      render: (status: string) => {
        const statusMap: Record<string, { color: string; text: string }> = {
          uploaded: { color: 'blue', text: '已上传' },
          parsing: { color: 'orange', text: '解析中' },
          parsed: { color: 'green', text: '已解析' },
          error: { color: 'red', text: '错误' },
        }
        const statusInfo = statusMap[status] || { color: 'default', text: status }
        return <Tag color={statusInfo.color}>{statusInfo.text}</Tag>
      },
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
            onClick={() => message.info(`查看工程 ${record.name}`)}
          >
            查看
          </Button>
          <Button
            type="link"
            icon={<SyncOutlined />}
            onClick={() => handleParse(record.id)}
            loading={parseMutation.isLoading && parseMutation.variables === record.id}
          >
            解析
          </Button>
          <Button
            type="link"
            icon={<EditOutlined />}
            onClick={() => message.info(`编辑工程 ${record.name}`)}
          >
            编辑
          </Button>
          <Popconfirm
            title="确定要删除这个工程吗？"
            onConfirm={() => handleDelete(record.id)}
            okText="确定"
            cancelText="取消"
          >
            <Button type="link" danger icon={<DeleteOutlined />}>
              删除
            </Button>
          </Popconfirm>
        </Space>
      ),
    },
  ]

  return (
    <div>
      <div style={{ display: 'flex', justifyContent: 'space-between', marginBottom: '16px' }}>
        <h1 style={{ margin: 0 }}>工程管理</h1>
        <Space>
          <Search
            placeholder="搜索工程"
            allowClear
            onSearch={setSearchText}
            style={{ width: 200 }}
          />
          <Button
            type="primary"
            icon={<PlusOutlined />}
            onClick={() => setIsModalVisible(true)}
          >
            新建工程
          </Button>
        </Space>
      </div>

      <Card>
        <Table
          columns={columns}
          dataSource={projectsData?.items || []}
          loading={isLoading}
          rowKey="id"
          pagination={{
            pageSize: 10,
            showSizeChanger: true,
            showTotal: (total) => `共 ${total} 个工程`,
          }}
        />
      </Card>

      {/* 创建工程模态框 */}
      <Modal
        title="新建工程"
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
            name="name"
            label="工程名称"
            rules={[{ required: true, message: '请输入工程名称' }]}
          >
            <Input placeholder="请输入工程名称" />
          </Form.Item>
          <Form.Item name="description" label="工程描述">
            <Input.TextArea placeholder="请输入工程描述" rows={3} />
          </Form.Item>
          <Form.Item
            name="file"
            label="工程文件"
            rules={[{ required: true, message: '请选择工程文件' }]}
            valuePropName="fileList"
            getValueFromEvent={(e) => (Array.isArray(e) ? e : e?.fileList)}
          >
            <Upload
              maxCount={1}
              beforeUpload={() => false} // 阻止自动上传
              accept=".aprx,.zip"
            >
              <Button icon={<UploadOutlined />}>选择文件</Button>
              <div style={{ marginTop: '8px', color: '#999' }}>
                支持 .aprx 和 .zip 格式，最大100MB
              </div>
            </Upload>
          </Form.Item>
        </Form>
      </Modal>
    </div>
  )
}

export default Projects