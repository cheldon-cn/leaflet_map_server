#include "ogc/graph/render/render_task.h"
#include <algorithm>

namespace ogc {
namespace graph {

struct RenderTask::Impl {
    std::string id;
    DrawParams params;
    Envelope extent;
    RenderTaskPriority priority;
    std::atomic<RenderTaskState> state;
    std::atomic<double> progress;
    ogc::draw::DrawResult result;
    std::string error;
    RenderTaskStats stats;
    void* userData;
    std::string tag;
    RenderTaskCallback callback;
    int64_t timeout;
    std::chrono::steady_clock::time_point createdTime;
    
    Impl()
        : priority(RenderTaskPriority::kNormal)
        , state(RenderTaskState::kPending)
        , progress(0.0)
        , userData(nullptr)
        , timeout(0)
        , createdTime(std::chrono::steady_clock::now()) {
    }
    
    Impl(const std::string& id_, const DrawParams& params_)
        : id(id_)
        , params(params_)
        , priority(RenderTaskPriority::kNormal)
        , state(RenderTaskState::kPending)
        , progress(0.0)
        , userData(nullptr)
        , timeout(0)
        , createdTime(std::chrono::steady_clock::now()) {
    }
};

RenderTask::RenderTask()
    : impl_(std::make_unique<Impl>()) {
}

RenderTask::RenderTask(const std::string& id, const DrawParams& params)
    : impl_(std::make_unique<Impl>(id, params)) {
}

RenderTask::~RenderTask() = default;

void RenderTask::SetId(const std::string& id) {
    impl_->id = id;
}

std::string RenderTask::GetId() const {
    return impl_->id;
}

void RenderTask::SetParams(const DrawParams& params) {
    impl_->params = params;
}

DrawParams RenderTask::GetParams() const {
    return impl_->params;
}

void RenderTask::SetExtent(const Envelope& extent) {
    impl_->extent = extent;
}

Envelope RenderTask::GetExtent() const {
    return impl_->extent;
}

void RenderTask::SetPriority(RenderTaskPriority priority) {
    impl_->priority = priority;
}

RenderTaskPriority RenderTask::GetPriority() const {
    return impl_->priority;
}

void RenderTask::SetState(RenderTaskState state) {
    impl_->state.store(state);
}

RenderTaskState RenderTask::GetState() const {
    return impl_->state.load();
}

void RenderTask::SetProgress(double progress) {
    impl_->progress.store(std::max(0.0, std::min(1.0, progress)));
}

double RenderTask::GetProgress() const {
    return impl_->progress.load();
}

void RenderTask::SetResult(const ogc::draw::DrawResult& result) {
    impl_->result = result;
}

ogc::draw::DrawResult RenderTask::GetResult() const {
    return impl_->result;
}

void RenderTask::SetError(const std::string& error) {
    impl_->error = error;
}

std::string RenderTask::GetError() const {
    return impl_->error;
}

void RenderTask::SetStats(const RenderTaskStats& stats) {
    impl_->stats = stats;
}

RenderTaskStats RenderTask::GetStats() const {
    return impl_->stats;
}

void RenderTask::SetUserData(void* data) {
    impl_->userData = data;
}

void* RenderTask::GetUserData() const {
    return impl_->userData;
}

void RenderTask::SetTag(const std::string& tag) {
    impl_->tag = tag;
}

std::string RenderTask::GetTag() const {
    return impl_->tag;
}

void RenderTask::SetCallback(RenderTaskCallback callback) {
    impl_->callback = callback;
}

RenderTaskCallback RenderTask::GetCallback() const {
    return impl_->callback;
}

void RenderTask::SetTimeout(int64_t milliseconds) {
    impl_->timeout = milliseconds;
}

int64_t RenderTask::GetTimeout() const {
    return impl_->timeout;
}

bool RenderTask::IsPending() const {
    return impl_->state.load() == RenderTaskState::kPending;
}

bool RenderTask::IsRunning() const {
    return impl_->state.load() == RenderTaskState::kRunning;
}

bool RenderTask::IsCompleted() const {
    return impl_->state.load() == RenderTaskState::kCompleted;
}

bool RenderTask::IsFailed() const {
    return impl_->state.load() == RenderTaskState::kFailed;
}

bool RenderTask::IsCancelled() const {
    return impl_->state.load() == RenderTaskState::kCancelled;
}

bool RenderTask::IsTimedOut() const {
    if (impl_->timeout <= 0) {
        return false;
    }
    
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - impl_->createdTime).count();
    
    return elapsed > impl_->timeout;
}

void RenderTask::Cancel() {
    RenderTaskState expected = RenderTaskState::kPending;
    impl_->state.compare_exchange_strong(expected, RenderTaskState::kCancelled);
    
    expected = RenderTaskState::kQueued;
    impl_->state.compare_exchange_strong(expected, RenderTaskState::kCancelled);
}

void RenderTask::NotifyCompletion() {
    if (impl_->callback) {
        impl_->callback(std::const_pointer_cast<RenderTask>(shared_from_this()));
    }
}

int RenderTask::GetPriorityValue() const {
    return static_cast<int>(impl_->priority);
}

bool RenderTask::operator<(const RenderTask& other) const {
    return GetPriorityValue() < other.GetPriorityValue();
}

void RenderTask::UpdateState(RenderTaskState state) {
    impl_->state.store(state);
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
