import React, { useState } from 'react'
import {
  Card,
  Form,
  Input,
  Button,
  Switch,
  Select,
  InputNumber,
  message,
  Tabs,
  Space,
  Tag,
} from 'antd'
import {
  SaveOutlined,
  ReloadOutlined,
  DatabaseOutlined,
  CloudOutlined,
  SecurityScanOutlined,
  NotificationOutlined,
} from '@ant-design/icons'

const { Option } = Select
const { TabPane } = Tabs

const Settings: React.FC = () => {
  const [form] = Form.useForm()
  const [loading, setLoading] = useState(false)

  // 模拟系统配置
  const systemConfig = {
    // 数据库配置
    database: {
      host: 'localhost',
      port: 5432,
      database: 'migration_db',
      username: 'postgres',
      password: '********',
      pool_size: 20,
    },
    // 文件存储配置
    storage: {
      type: 'local', // local, minio, s3
      upload_dir: './uploads',
      max_upload_size: 100, // MB
      allowed_extensions: ['.aprx', '.zip', '.qgs', '.gpkg'],
    },
    // 迁移配置
    migration: {
      timeout: 3600, // 秒
      max_workers: 4,
      batch_size: 10000,
      default_output_format: 'GPKG',
    },
    // 安全配置
    security: {
      require_authentication: true,
      session_timeout: 30, // 分钟
      rate_limit: 100, // 每分钟请求数
      cors_origins: ['http://localhost:3000', 'http://localhost:8000'],
    },
    // 通知配置
    notification: {
      email_enabled: false,
      email_host: 'smtp.example.com',
      email_port: 587,
      email_username: 'user@example.com',
      email_password: '********',
      webhook_enabled: false,
      webhook_url: '',
    },
  }

  const handleSave = () => {
    form.validateFields().then((values) => {
      setLoading(true)
      // 模拟保存操作
      setTimeout(() => {
        message.success('配置保存成功')
        setLoading(false)
      }, 1000)
    })
  }

  const handleReset = () => {
    form.setFieldsValue(systemConfig)
    message.info('配置已重置为默认值')
  }

  return (
    <div>
      <div style={{ display: 'flex', justifyContent: 'space-between', marginBottom: '16px' }}>
        <h1 style={{ margin: 0 }}>系统设置</h1>
        <Space>
          <Button icon={<ReloadOutlined />} onClick={handleReset}>
            重置
          </Button>
          <Button
            type="primary"
            icon={<SaveOutlined />}
            onClick={handleSave}
            loading={loading}
          >
            保存配置
          </Button>
        </Space>
      </div>

      <Tabs defaultActiveKey="database">
        <TabPane
          tab={
            <span>
              <DatabaseOutlined />
              数据库
            </span>
          }
          key="database"
        >
          <Card>
            <Form
              form={form}
              layout="vertical"
              initialValues={systemConfig.database}
            >
              <Form.Item
                name="host"
                label="数据库主机"
                rules={[{ required: true, message: '请输入数据库主机' }]}
              >
                <Input placeholder="例如: localhost" />
              </Form.Item>
              <Form.Item
                name="port"
                label="数据库端口"
                rules={[{ required: true, message: '请输入数据库端口' }]}
              >
                <InputNumber min={1} max={65535} style={{ width: '100%' }} />
              </Form.Item>
              <Form.Item
                name="database"
                label="数据库名称"
                rules={[{ required: true, message: '请输入数据库名称' }]}
              >
                <Input placeholder="例如: migration_db" />
              </Form.Item>
              <Form.Item
                name="username"
                label="用户名"
                rules={[{ required: true, message: '请输入用户名' }]}
              >
                <Input placeholder="例如: postgres" />
              </Form.Item>
              <Form.Item
                name="password"
                label="密码"
                rules={[{ required: true, message: '请输入密码' }]}
              >
                <Input.Password placeholder="请输入密码" />
              </Form.Item>
              <Form.Item
                name="pool_size"
                label="连接池大小"
                rules={[{ required: true, message: '请输入连接池大小' }]}
              >
                <InputNumber min={1} max={100} style={{ width: '100%' }} />
              </Form.Item>
            </Form>
          </Card>
        </TabPane>

        <TabPane
          tab={
            <span>
              <CloudOutlined />
              文件存储
            </span>
          }
          key="storage"
        >
          <Card>
            <Form
              form={form}
              layout="vertical"
              initialValues={systemConfig.storage}
            >
              <Form.Item
                name="type"
                label="存储类型"
                rules={[{ required: true, message: '请选择存储类型' }]}
              >
                <Select>
                  <Option value="local">本地存储</Option>
                  <Option value="minio">MinIO</Option>
                  <Option value="s3">Amazon S3</Option>
                </Select>
              </Form.Item>
              <Form.Item
                name="upload_dir"
                label="上传目录"
                rules={[{ required: true, message: '请输入上传目录' }]}
              >
                <Input placeholder="例如: ./uploads" />
              </Form.Item>
              <Form.Item
                name="max_upload_size"
                label="最大上传大小 (MB)"
                rules={[{ required: true, message: '请输入最大上传大小' }]}
              >
                <InputNumber
                  min={1}
                  max={1024}
                  style={{ width: '100%' }}
                  addonAfter="MB"
                />
              </Form.Item>
              <Form.Item
                name="allowed_extensions"
                label="允许的文件扩展名"
              >
                <Select mode="tags" placeholder="输入扩展名后按回车">
                  {systemConfig.storage.allowed_extensions.map((ext) => (
                    <Option key={ext} value={ext}>
                      {ext}
                    </Option>
                  ))}
                </Select>
              </Form.Item>
            </Form>
          </Card>
        </TabPane>

        <TabPane
          tab={
            <span>
              <DatabaseOutlined />
              迁移配置
            </span>
          }
          key="migration"
        >
          <Card>
            <Form
              form={form}
              layout="vertical"
              initialValues={systemConfig.migration}
            >
              <Form.Item
                name="timeout"
                label="任务超时时间 (秒)"
                rules={[{ required: true, message: '请输入任务超时时间' }]}
              >
                <InputNumber
                  min={60}
                  max={86400}
                  style={{ width: '100%' }}
                  addonAfter="秒"
                />
              </Form.Item>
              <Form.Item
                name="max_workers"
                label="最大工作线程数"
                rules={[{ required: true, message: '请输入最大工作线程数' }]}
              >
                <InputNumber
                  min={1}
                  max={16}
                  style={{ width: '100%' }}
                />
              </Form.Item>
              <Form.Item
                name="batch_size"
                label="批量处理大小"
                rules={[{ required: true, message: '请输入批量处理大小' }]}
              >
                <InputNumber
                  min={1000}
                  max={100000}
                  style={{ width: '100%' }}
                />
              </Form.Item>
              <Form.Item
                name="default_output_format"
                label="默认输出格式"
                rules={[{ required: true, message: '请选择默认输出格式' }]}
              >
                <Select>
                  <Option value="GPKG">GeoPackage (GPKG)</Option>
                  <Option value="SHP">Shapefile (SHP)</Option>
                  <Option value="GEOJSON">GeoJSON</Option>
                </Select>
              </Form.Item>
            </Form>
          </Card>
        </TabPane>

        <TabPane
          tab={
            <span>
              <SecurityScanOutlined />
              安全配置
            </span>
          }
          key="security"
        >
          <Card>
            <Form
              form={form}
              layout="vertical"
              initialValues={systemConfig.security}
            >
              <Form.Item
                name="require_authentication"
                label="要求认证"
                valuePropName="checked"
              >
                <Switch />
              </Form.Item>
              <Form.Item
                name="session_timeout"
                label="会话超时时间 (分钟)"
                rules={[{ required: true, message: '请输入会话超时时间' }]}
              >
                <InputNumber
                  min={1}
                  max={1440}
                  style={{ width: '100%' }}
                  addonAfter="分钟"
                />
              </Form.Item>
              <Form.Item
                name="rate_limit"
                label="API速率限制"
                rules={[{ required: true, message: '请输入API速率限制' }]}
              >
                <InputNumber
                  min={1}
                  max={1000}
                  style={{ width: '100%' }}
                  addonAfter="请求/分钟"
                />
              </Form.Item>
              <Form.Item
                name="cors_origins"
                label="CORS允许来源"
              >
                <Select mode="tags" placeholder="输入URL后按回车">
                  {systemConfig.security.cors_origins.map((origin) => (
                    <Option key={origin} value={origin}>
                      {origin}
                    </Option>
                  ))}
                </Select>
              </Form.Item>
            </Form>
          </Card>
        </TabPane>

        <TabPane
          tab={
            <span>
              <NotificationOutlined />
              通知配置
            </span>
          }
          key="notification"
        >
          <Card>
            <Form
              form={form}
              layout="vertical"
              initialValues={systemConfig.notification}
            >
              <Form.Item
                name="email_enabled"
                label="启用邮件通知"
                valuePropName="checked"
              >
                <Switch />
              </Form.Item>
              <Form.Item
                name="email_host"
                label="邮件服务器"
                rules={[{ required: form.getFieldValue('email_enabled'), message: '请输入邮件服务器' }]}
              >
                <Input placeholder="例如: smtp.example.com" />
              </Form.Item>
              <Form.Item
                name="email_port"
                label="邮件端口"
                rules={[{ required: form.getFieldValue('email_enabled'), message: '请输入邮件端口' }]}
              >
                <InputNumber min={1} max={65535} style={{ width: '100%' }} />
              </Form.Item>
              <Form.Item
                name="email_username"
                label="邮件用户名"
                rules={[{ required: form.getFieldValue('email_enabled'), message: '请输入邮件用户名' }]}
              >
                <Input placeholder="例如: user@example.com" />
              </Form.Item>
              <Form.Item
                name="email_password"
                label="邮件密码"
                rules={[{ required: form.getFieldValue('email_enabled'), message: '请输入邮件密码' }]}
              >
                <Input.Password placeholder="请输入邮件密码" />
              </Form.Item>
              <Form.Item
                name="webhook_enabled"
                label="启用Webhook通知"
                valuePropName="checked"
              >
                <Switch />
              </Form.Item>
              <Form.Item
                name="webhook_url"
                label="Webhook URL"
                rules={[{ required: form.getFieldValue('webhook_enabled'), message: '请输入Webhook URL' }]}
              >
                <Input placeholder="例如: https://hook.example.com" />
              </Form.Item>
            </Form>
          </Card>
        </TabPane>
      </Tabs>

      <Card title="系统信息" style={{ marginTop: '24px' }}>
        <div>
          <p>
            <strong>当前版本:</strong> <Tag color="blue">v1.0.0</Tag>
          </p>
          <p>
            <strong>最后配置更新时间:</strong> {new Date().toLocaleString()}
          </p>
          <p>
            <strong>系统状态:</strong> <Tag color="green">运行正常</Tag>
          </p>
          <p style={{ color: '#666', fontSize: '12px', marginTop: '16px' }}>
            注意：修改配置后需要重启服务才能生效（某些配置可能需要重新登录）。
          </p>
        </div>
      </Card>
    </div>
  )
}

export default Settings