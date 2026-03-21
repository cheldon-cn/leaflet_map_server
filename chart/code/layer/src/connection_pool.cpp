#include "ogc/layer/connection_pool.h"

namespace ogc {

CNConnectionGuard::CNConnectionGuard(CNConnectionPool& pool, int timeout_ms)
    : pool_(pool), conn_(pool.Acquire(timeout_ms)) {
}

CNConnectionGuard::~CNConnectionGuard() {
    if (conn_) {
        pool_.Release(conn_);
    }
}

} // namespace ogc
