import axios from 'axios'

// 创建axios实例
const api = axios.create({
  baseURL: '/api/v1',
  timeout: 30000,
  headers: {
    'Content-Type': 'application/json',
  },
})

// 请求拦截器
api.interceptors.request.use(
  (config) => {
    // 从localStorage获取token
    const token = localStorage.getItem('access_token')
    if (token) {
      config.headers.Authorization = `Bearer ${token}`
    }
    return config
  },
  (error) => {
    return Promise.reject(error)
  }
)

// 响应拦截器
api.interceptors.response.use(
  (response) => {
    return response.data
  },
  (error) => {
    // 处理错误响应
    if (error.response) {
      const { status, data } = error.response
      
      switch (status) {
        case 401:
          // token过期，跳转到登录页
          localStorage.removeItem('access_token')
          window.location.href = '/login'
          break
        case 403:
          console.error('权限不足:', data.message)
          break
        case 404:
          console.error('资源未找到:', data.message)
          break
        case 500:
          console.error('服务器错误:', data.message)
          break
        default:
          console.error('请求错误:', data.message)
      }
    } else if (error.request) {
      console.error('网络错误，请检查网络连接')
    } else {
      console.error('请求配置错误:', error.message)
    }
    
    return Promise.reject(error)
  }
)

// 认证API
export const authApi = {
  login: (username: string, password: string) =>
    api.post('/auth/login', { username, password }),
  register: (userData: any) => api.post('/auth/register', userData),
  getCurrentUser: () => api.get('/auth/me'),
  refreshToken: (refreshToken: string) =>
    api.post('/auth/refresh', { refresh_token: refreshToken }),
}

// 工程API
export const projectApi = {
  getProjects: (params?: { skip?: number; limit?: number }) =>
    api.get('/projects', { params }),
  getProject: (id: string) => api.get(`/projects/${id}`),
  createProject: (formData: FormData) =>
    api.post('/projects', formData, {
      headers: { 'Content-Type': 'multipart/form-data' },
    }),
  updateProject: (id: string, data: any) => api.put(`/projects/${id}`, data),
  deleteProject: (id: string) => api.delete(`/projects/${id}`),
  parseProject: (id: string) => api.post(`/projects/${id}/parse`),
}

// 任务API
export const taskApi = {
  getTasks: (params?: { skip?: number; limit?: number }) =>
    api.get('/tasks', { params }),
  getTask: (id: string) => api.get(`/tasks/${id}`),
  createTask: (data: any) => api.post('/tasks', data),
  updateTask: (id: string, data: any) => api.put(`/tasks/${id}`, data),
  deleteTask: (id: string) => api.delete(`/tasks/${id}`),
  startTask: (id: string) => api.post(`/tasks/${id}/start`),
  cancelTask: (id: string) => api.post(`/tasks/${id}/cancel`),
  getTaskProgress: (id: string) => api.get(`/tasks/${id}/progress`),
  getTaskLogs: (id: string) => api.get(`/tasks/${id}/logs`),
}

// 文件API
export const fileApi = {
  uploadFile: (file: File) => {
    const formData = new FormData()
    formData.append('file', file)
    return api.post('/files/upload', formData, {
      headers: { 'Content-Type': 'multipart/form-data' },
    })
  },
  downloadFile: (fileId: string) => api.get(`/files/${fileId}/download`),
  deleteFile: (fileId: string) => api.delete(`/files/${fileId}`),
}

// 监控API
export const monitoringApi = {
  healthCheck: () => api.get('/monitoring/health'),
  getSystemStats: () => api.get('/monitoring/stats'),
  getTaskMonitoring: () => api.get('/monitoring/tasks'),
}

export default api