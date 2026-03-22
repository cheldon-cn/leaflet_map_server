#include "ogc/draw/render_task.h"
#include <algorithm>

namespace ogc {
namespace draw {

RenderTask::RenderTask()
    : m_priority(RenderTaskPriority::kNormal)
    , m_state(RenderTaskState::kPending)
    , m_progress(0.0)
    , m_userData(nullptr)
    , m_timeout(0)
    , m_createdTime(std::chrono::steady_clock::now()) {
}

RenderTask::RenderTask(const std::string& id, const DrawParams& params)
    : m_id(id)
    , m_params(params)
    , m_priority(RenderTaskPriority::kNormal)
    , m_state(RenderTaskState::kPending)
    , m_progress(0.0)
    , m_userData(nullptr)
    , m_timeout(0)
    , m_createdTime(std::chrono::steady_clock::now()) {
}

void RenderTask::SetId(const std::string& id) {
    m_id = id;
}

std::string RenderTask::GetId() const {
    return m_id;
}

void RenderTask::SetParams(const DrawParams& params) {
    m_params = params;
}

DrawParams RenderTask::GetParams() const {
    return m_params;
}

void RenderTask::SetExtent(const Envelope& extent) {
    m_extent = extent;
}

Envelope RenderTask::GetExtent() const {
    return m_extent;
}

void RenderTask::SetPriority(RenderTaskPriority priority) {
    m_priority = priority;
}

RenderTaskPriority RenderTask::GetPriority() const {
    return m_priority;
}

void RenderTask::SetState(RenderTaskState state) {
    m_state.store(state);
}

RenderTaskState RenderTask::GetState() const {
    return m_state.load();
}

void RenderTask::SetProgress(double progress) {
    m_progress.store(std::max(0.0, std::min(1.0, progress)));
}

double RenderTask::GetProgress() const {
    return m_progress.load();
}

void RenderTask::SetResult(const DrawResult& result) {
    m_result = result;
}

DrawResult RenderTask::GetResult() const {
    return m_result;
}

void RenderTask::SetError(const std::string& error) {
    m_error = error;
}

std::string RenderTask::GetError() const {
    return m_error;
}

void RenderTask::SetStats(const RenderTaskStats& stats) {
    m_stats = stats;
}

RenderTaskStats RenderTask::GetStats() const {
    return m_stats;
}

void RenderTask::SetUserData(void* data) {
    m_userData = data;
}

void* RenderTask::GetUserData() const {
    return m_userData;
}

void RenderTask::SetTag(const std::string& tag) {
    m_tag = tag;
}

std::string RenderTask::GetTag() const {
    return m_tag;
}

void RenderTask::SetCallback(RenderTaskCallback callback) {
    m_callback = callback;
}

RenderTaskCallback RenderTask::GetCallback() const {
    return m_callback;
}

void RenderTask::SetTimeout(int64_t milliseconds) {
    m_timeout = milliseconds;
}

int64_t RenderTask::GetTimeout() const {
    return m_timeout;
}

bool RenderTask::IsPending() const {
    return m_state.load() == RenderTaskState::kPending;
}

bool RenderTask::IsRunning() const {
    return m_state.load() == RenderTaskState::kRunning;
}

bool RenderTask::IsCompleted() const {
    return m_state.load() == RenderTaskState::kCompleted;
}

bool RenderTask::IsFailed() const {
    return m_state.load() == RenderTaskState::kFailed;
}

bool RenderTask::IsCancelled() const {
    return m_state.load() == RenderTaskState::kCancelled;
}

bool RenderTask::IsTimedOut() const {
    if (m_timeout <= 0) {
        return false;
    }
    
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - m_createdTime).count();
    
    return elapsed > m_timeout;
}

void RenderTask::Cancel() {
    RenderTaskState expected = RenderTaskState::kPending;
    m_state.compare_exchange_strong(expected, RenderTaskState::kCancelled);
    
    expected = RenderTaskState::kQueued;
    m_state.compare_exchange_strong(expected, RenderTaskState::kCancelled);
}

void RenderTask::NotifyCompletion() {
    if (m_callback) {
        m_callback(std::const_pointer_cast<RenderTask>(shared_from_this()));
    }
}

int RenderTask::GetPriorityValue() const {
    return static_cast<int>(m_priority);
}

bool RenderTask::operator<(const RenderTask& other) const {
    return GetPriorityValue() < other.GetPriorityValue();
}

void RenderTask::UpdateState(RenderTaskState state) {
    m_state.store(state);
}

RenderTaskPtr RenderTask::Create() {
    return nullptr;
}

RenderTaskPtr RenderTask::Create(const std::string& id, const DrawParams& params) {
    (void)id;
    (void)params;
    return nullptr;
}

}
}
