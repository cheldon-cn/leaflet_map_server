import React, { useState } from 'react'
import { useNavigate } from 'react-router-dom'
import {
  Card,
  Form,
  Input,
  Button,
  Checkbox,
  message,
  Typography,
  Space,
  Divider,
} from 'antd'
import {
  UserOutlined,
  LockOutlined,
  GoogleOutlined,
  GithubOutlined,
} from '@ant-design/icons'
import { authApi } from '../services/api'

const { Title, Text, Link } = Typography

const Login: React.FC = () => {
  const [loading, setLoading] = useState(false)
  const [isRegister, setIsRegister] = useState(false)
  const navigate = useNavigate()
  const [form] = Form.useForm()

  const handleSubmit = async (values: any) => {
    setLoading(true)
    try {
      if (isRegister) {
        // 注册
        await authApi.register({
          username: values.username,
          email: values.email,
          password: values.password,
          full_name: values.full_name,
        })
        message.success('注册成功，请登录')
        setIsRegister(false)
        form.resetFields(['password', 'confirmPassword'])
      } else {
        // 登录
        const response = await authApi.login(values.username, values.password)
        localStorage.setItem('access_token', response.access_token)
        localStorage.setItem('refresh_token', response.refresh_token)
        message.success('登录成功')
        navigate('/dashboard')
      }
    } catch (error: any) {
      message.error(error.response?.data?.message || '操作失败')
    } finally {
      setLoading(false)
    }
  }

  const handleGoogleLogin = () => {
    message.info('Google登录功能即将上线')
  }

  const handleGithubLogin = () => {
    message.info('GitHub登录功能即将上线')
  }

  return (
    <div
      style={{
        minHeight: '100vh',
        display: 'flex',
        alignItems: 'center',
        justifyContent: 'center',
        background: 'linear-gradient(135deg, #667eea 0%, #764ba2 100%)',
        padding: '20px',
      }}
    >
      <Card
        style={{
          width: '100%',
          maxWidth: 400,
          boxShadow: '0 20px 60px rgba(0,0,0,0.3)',
        }}
        bodyStyle={{ padding: '40px' }}
      >
        <div style={{ textAlign: 'center', marginBottom: '32px' }}>
          <Title level={2} style={{ marginBottom: '8px' }}>
            ArcGIS Pro迁移工具
          </Title>
          <Text type="secondary">
            {isRegister ? '创建新账户' : '欢迎回来，请登录'}
          </Text>
        </div>

        <Form
          form={form}
          layout="vertical"
          onFinish={handleSubmit}
          initialValues={{ remember: true }}
        >
          {isRegister && (
            <Form.Item
              name="email"
              label="邮箱"
              rules={[
                { required: true, message: '请输入邮箱' },
                { type: 'email', message: '请输入有效的邮箱地址' },
              ]}
            >
              <Input
                prefix={<UserOutlined />}
                placeholder="请输入邮箱"
                size="large"
              />
            </Form.Item>
          )}

          {isRegister && (
            <Form.Item
              name="full_name"
              label="姓名"
              rules={[{ required: true, message: '请输入姓名' }]}
            >
              <Input
                prefix={<UserOutlined />}
                placeholder="请输入姓名"
                size="large"
              />
            </Form.Item>
          )}

          <Form.Item
            name="username"
            label="用户名"
            rules={[{ required: true, message: '请输入用户名' }]}
          >
            <Input
              prefix={<UserOutlined />}
              placeholder="请输入用户名"
              size="large"
            />
          </Form.Item>

          <Form.Item
            name="password"
            label="密码"
            rules={[{ required: true, message: '请输入密码' }]}
          >
            <Input.Password
              prefix={<LockOutlined />}
              placeholder="请输入密码"
              size="large"
            />
          </Form.Item>

          {isRegister && (
            <Form.Item
              name="confirmPassword"
              label="确认密码"
              dependencies={['password']}
              rules={[
                { required: true, message: '请确认密码' },
                ({ getFieldValue }) => ({
                  validator(_, value) {
                    if (!value || getFieldValue('password') === value) {
                      return Promise.resolve()
                    }
                    return Promise.reject(new Error('两次输入的密码不一致'))
                  },
                }),
              ]}
            >
              <Input.Password
                prefix={<LockOutlined />}
                placeholder="请确认密码"
                size="large"
              />
            </Form.Item>
          )}

          {!isRegister && (
            <Form.Item>
              <div style={{ display: 'flex', justifyContent: 'space-between' }}>
                <Form.Item name="remember" valuePropName="checked" noStyle>
                  <Checkbox>记住我</Checkbox>
                </Form.Item>
                <Link>忘记密码？</Link>
              </div>
            </Form.Item>
          )}

          <Form.Item>
            <Button
              type="primary"
              htmlType="submit"
              loading={loading}
              size="large"
              block
            >
              {isRegister ? '注册' : '登录'}
            </Button>
          </Form.Item>

          {!isRegister && (
            <>
              <Divider>
                <Text type="secondary">其他登录方式</Text>
              </Divider>
              <Space direction="vertical" style={{ width: '100%' }}>
                <Button
                  icon={<GoogleOutlined />}
                  size="large"
                  block
                  onClick={handleGoogleLogin}
                >
                  使用 Google 登录
                </Button>
                <Button
                  icon={<GithubOutlined />}
                  size="large"
                  block
                  onClick={handleGithubLogin}
                >
                  使用 GitHub 登录
                </Button>
              </Space>
            </>
          )}

          <div style={{ textAlign: 'center', marginTop: '24px' }}>
            <Text type="secondary">
              {isRegister ? '已有账户？' : '没有账户？'}{' '}
              <Link
                onClick={() => {
                  setIsRegister(!isRegister)
                  form.resetFields()
                }}
              >
                {isRegister ? '去登录' : '立即注册'}
              </Link>
            </Text>
          </div>
        </Form>
      </Card>
    </div>
  )
}

export default Login