#ifndef CYCLE_DATABASE_DATABASE_FACTORY_H
#define CYCLE_DATABASE_DATABASE_FACTORY_H

#include "idatabase.h"
#include <memory>
#include <string>

namespace cycle {
namespace database {

class DatabaseFactory {
public:
    static std::shared_ptr<IDatabase> Create(const std::string& type, 
                                            const std::string& connectionString);
    
    static std::shared_ptr<IDatabase> CreateSqlite(const std::string& dbPath);
    
#ifdef HAVE_POSTGRESQL
    static std::shared_ptr<IDatabase> CreatePostgresql(const std::string& connectionString);
#endif
    
    static std::vector<std::string> GetSupportedTypes();
};

} // namespace database
} // namespace cycle

#endif // CYCLE_DATABASE_DATABASE_FACTORY_H
