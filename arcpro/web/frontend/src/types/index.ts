// 用户相关类型
export interface User {
  id: string
  username: string
  email: string
  full_name?: string
  is_active: boolean
  is_superuser: boolean
  created_at: string
  updated_at: string
}

export interface UserLogin {
  username: string
  password: string
}

export interface UserRegister {
  username: string
  email: string
  password: string
  full_name?: string
}

export interface Token {
  access_token: string
  refresh_token: string
  token_type: string
  expires_in: number
}

// 工程相关类型
export interface Project {
  id: string
  user_id: string
  name: string
  description?: string
  original_filename?: string
  file_path?: string
  file_size?: number
  status: 'uploaded' | 'parsing' | 'parsed' | 'error'
  metadata?: Record<string, any>
  created_at: string
  updated_at: string
}

export interface ProjectCreate {
  name: string
  description?: string
  original_filename?: string
  file_size?: number
}

// 迁移任务相关类型
export interface MigrationTask {
  id: string
  project_id: string
  user_id?: string
  name: string
  description?: string
  config: Record<string, any>
  status: 'pending' | 'running' | 'completed' | 'failed' | 'cancelled'
  progress: number
  error_message?: string
  result_path?: string
  logs?: string
  started_at?: string
  completed_at?: string
  created_at: string
  updated_at: string
}

export interface TaskStep {
  id: string
  task_id: string
  step_name: string
  step_order: number
  status: string
  progress: number
  details?: Record<string, any>
  started_at?: string
  completed_at?: string
  created_at: string
}

export interface TaskCreate {
  project_id: string
  name: string
  description?: string
  config: Record<string, any>
}

// 分页响应类型
export interface PaginatedResponse<T> {
  items: T[]
  total: number
  page: number
  size: number
  pages: number
}

// 错误响应类型
export interface ErrorResponse {
  error: string
  message: string
  detail?: string
  timestamp: string
}

// 系统监控类型
export interface SystemHealth {
  status: 'healthy' | 'unhealthy'
  timestamp: string
  service: string
  version: string
}

export interface SystemStats {
  timestamp: string
  system: {
    cpu_percent: number
    memory_percent: number
    memory_total_gb: number
    memory_used_gb: number
    disk_percent: number
    disk_total_gb: number
    disk_used_gb: number
  }
}

export interface TaskMonitoring {
  timestamp: string
  task_stats: Array<{
    status: string
    count: number
  }>
  recent_tasks: Array<{
    id: string
    name: string
    status: string
    progress: number
    created_at: string
  }>
}

// 文件上传类型
export interface FileUploadResponse {
  message: string
  file_id: string
  filename: string
  file_size: number
  content_type: string
}