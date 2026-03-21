#include "ogc/feature/field_value.h"
#include <cstring>
#include <cstdlib>
#include <algorithm>

namespace ogc {

namespace {

constexpr size_t kSmallStringSize = 24;
constexpr size_t kSBOThreshold = kSmallStringSize;

}

CNFieldValue::CNFieldValue()
    : type_(CNFieldType::kUnset), storage_type_(StorageType::kNone),
      str_ptr_(nullptr), dt_ptr_(nullptr), bin_vec_(nullptr),
      i32_vec_(nullptr), i64_vec_(nullptr), d_vec_(nullptr),
      str_vec_(nullptr), geom_ptr_(nullptr) {
}

CNFieldValue::CNFieldValue(CNFieldType type)
    : type_(type), storage_type_(StorageType::kNone),
      str_ptr_(nullptr), dt_ptr_(nullptr), bin_vec_(nullptr),
      i32_vec_(nullptr), i64_vec_(nullptr), d_vec_(nullptr),
      str_vec_(nullptr), geom_ptr_(nullptr) {
    switch (type) {
        case CNFieldType::kInteger:
        case CNFieldType::kInteger64:
        case CNFieldType::kReal:
        case CNFieldType::kBoolean:
        case CNFieldType::kDate:
        case CNFieldType::kTime:
        case CNFieldType::kDateTime:
        case CNFieldType::kString:
        case CNFieldType::kWideString:
        case CNFieldType::kBinary:
        case CNFieldType::kIntegerList:
        case CNFieldType::kInteger64List:
        case CNFieldType::kRealList:
        case CNFieldType::kStringList:
        case CNFieldType::kNull:
        case CNFieldType::kUnset:
        case CNFieldType::kUnknown:
            break;
    }
}

CNFieldValue::CNFieldValue(int32_t value)
    : type_(CNFieldType::kInteger), storage_type_(StorageType::kInteger),
      str_ptr_(nullptr), dt_ptr_(nullptr), bin_vec_(nullptr),
      i32_vec_(nullptr), i64_vec_(nullptr), d_vec_(nullptr),
      str_vec_(nullptr), geom_ptr_(nullptr) {
    *reinterpret_cast<int32_t*>(storage_.buffer) = value;
}

CNFieldValue::CNFieldValue(int64_t value)
    : type_(CNFieldType::kInteger64), storage_type_(StorageType::kInteger64),
      str_ptr_(nullptr), dt_ptr_(nullptr), bin_vec_(nullptr),
      i32_vec_(nullptr), i64_vec_(nullptr), d_vec_(nullptr),
      str_vec_(nullptr), geom_ptr_(nullptr) {
    *reinterpret_cast<int64_t*>(storage_.buffer) = value;
}

CNFieldValue::CNFieldValue(double value)
    : type_(CNFieldType::kReal), storage_type_(StorageType::kReal),
      str_ptr_(nullptr), dt_ptr_(nullptr), bin_vec_(nullptr),
      i32_vec_(nullptr), i64_vec_(nullptr), d_vec_(nullptr),
      str_vec_(nullptr), geom_ptr_(nullptr) {
    *reinterpret_cast<double*>(storage_.buffer) = value;
}

CNFieldValue::CNFieldValue(bool value)
    : type_(CNFieldType::kBoolean), storage_type_(StorageType::kBoolean),
      str_ptr_(nullptr), dt_ptr_(nullptr), bin_vec_(nullptr),
      i32_vec_(nullptr), i64_vec_(nullptr), d_vec_(nullptr),
      str_vec_(nullptr), geom_ptr_(nullptr) {
    storage_.buffer[0] = value ? 1 : 0;
}

CNFieldValue::CNFieldValue(const char* value)
    : type_(CNFieldType::kString), storage_type_(StorageType::kString),
      str_ptr_(nullptr), dt_ptr_(nullptr), bin_vec_(nullptr),
      i32_vec_(nullptr), i64_vec_(nullptr), d_vec_(nullptr),
      str_vec_(nullptr), geom_ptr_(nullptr) {
    if (value && std::strlen(value) <= kSBOThreshold) {
        std::memcpy(storage_.buffer, value, std::strlen(value) + 1);
    } else {
        str_ptr_ = new std::string(value ? value : "");
    }
}

CNFieldValue::CNFieldValue(const std::string& value)
    : type_(CNFieldType::kString), storage_type_(StorageType::kString),
      str_ptr_(nullptr), dt_ptr_(nullptr), bin_vec_(nullptr),
      i32_vec_(nullptr), i64_vec_(nullptr), d_vec_(nullptr),
      str_vec_(nullptr), geom_ptr_(nullptr) {
    if (value.size() <= kSBOThreshold) {
        std::memcpy(storage_.buffer, value.data(), value.size());
        storage_.buffer[value.size()] = '\0';
    } else {
        str_ptr_ = new std::string(value);
    }
}

CNFieldValue::CNFieldValue(const CNDateTime& value)
    : type_(CNFieldType::kDateTime), storage_type_(StorageType::kDateTime),
      str_ptr_(nullptr), dt_ptr_(nullptr), bin_vec_(nullptr),
      i32_vec_(nullptr), i64_vec_(nullptr), d_vec_(nullptr),
      str_vec_(nullptr), geom_ptr_(nullptr) {
    dt_ptr_ = new CNDateTime(value);
}

CNFieldValue::CNFieldValue(const std::vector<uint8_t>& value)
    : type_(CNFieldType::kBinary), storage_type_(StorageType::kBinary),
      str_ptr_(nullptr), dt_ptr_(nullptr), bin_vec_(nullptr),
      i32_vec_(nullptr), i64_vec_(nullptr), d_vec_(nullptr),
      str_vec_(nullptr), geom_ptr_(nullptr) {
    bin_vec_ = new std::vector<uint8_t>(value);
}

CNFieldValue::CNFieldValue(const std::vector<int32_t>& value)
    : type_(CNFieldType::kIntegerList), storage_type_(StorageType::kIntegerList),
      str_ptr_(nullptr), dt_ptr_(nullptr), bin_vec_(nullptr),
      i32_vec_(nullptr), i64_vec_(nullptr), d_vec_(nullptr),
      str_vec_(nullptr), geom_ptr_(nullptr) {
    i32_vec_ = new std::vector<int32_t>(value);
}

CNFieldValue::CNFieldValue(const std::vector<int64_t>& value)
    : type_(CNFieldType::kInteger64List), storage_type_(StorageType::kInteger64List),
      str_ptr_(nullptr), dt_ptr_(nullptr), bin_vec_(nullptr),
      i32_vec_(nullptr), i64_vec_(nullptr), d_vec_(nullptr),
      str_vec_(nullptr), geom_ptr_(nullptr) {
    i64_vec_ = new std::vector<int64_t>(value);
}

CNFieldValue::CNFieldValue(const std::vector<double>& value)
    : type_(CNFieldType::kRealList), storage_type_(StorageType::kRealList),
      str_ptr_(nullptr), dt_ptr_(nullptr), bin_vec_(nullptr),
      i32_vec_(nullptr), i64_vec_(nullptr), d_vec_(nullptr),
      str_vec_(nullptr), geom_ptr_(nullptr) {
    d_vec_ = new std::vector<double>(value);
}

CNFieldValue::CNFieldValue(const std::vector<std::string>& value)
    : type_(CNFieldType::kStringList), storage_type_(StorageType::kStringList),
      str_ptr_(nullptr), dt_ptr_(nullptr), bin_vec_(nullptr),
      i32_vec_(nullptr), i64_vec_(nullptr), d_vec_(nullptr),
      str_vec_(nullptr), geom_ptr_(nullptr) {
    str_vec_ = new std::vector<std::string>(value);
}

CNFieldValue::CNFieldValue(GeometryPtr value)
    : type_(CNFieldType::kUnknown), storage_type_(StorageType::kGeometry),
      str_ptr_(nullptr), dt_ptr_(nullptr), bin_vec_(nullptr),
      i32_vec_(nullptr), i64_vec_(nullptr), d_vec_(nullptr),
      str_vec_(nullptr), geom_ptr_(nullptr) {
    geom_ptr_ = new GeometryPtr(std::move(value));
}

CNFieldValue::~CNFieldValue() {
    Clear();
}

void CNFieldValue::Clear() {
    switch (storage_type_) {
        case StorageType::kString:
            if (str_ptr_) {
                delete str_ptr_;
                str_ptr_ = nullptr;
            }
            break;
        case StorageType::kDateTime:
            if (dt_ptr_) {
                delete dt_ptr_;
                dt_ptr_ = nullptr;
            }
            break;
        case StorageType::kBinary:
            if (bin_vec_) {
                delete bin_vec_;
                bin_vec_ = nullptr;
            }
            break;
        case StorageType::kIntegerList:
            if (i32_vec_) {
                delete i32_vec_;
                i32_vec_ = nullptr;
            }
            break;
        case StorageType::kInteger64List:
            if (i64_vec_) {
                delete i64_vec_;
                i64_vec_ = nullptr;
            }
            break;
        case StorageType::kRealList:
            if (d_vec_) {
                delete d_vec_;
                d_vec_ = nullptr;
            }
            break;
        case StorageType::kStringList:
            if (str_vec_) {
                delete str_vec_;
                str_vec_ = nullptr;
            }
            break;
        case StorageType::kGeometry:
            if (geom_ptr_) {
                delete geom_ptr_;
                geom_ptr_ = nullptr;
            }
            break;
        default:
            break;
    }
    storage_type_ = StorageType::kNone;
    type_ = CNFieldType::kUnset;
    std::memset(storage_.buffer, 0, sizeof(storage_.buffer));
}

CNFieldValue::CNFieldValue(const CNFieldValue& other)
    : type_(other.type_), storage_type_(other.storage_type_),
      str_ptr_(nullptr), dt_ptr_(nullptr), bin_vec_(nullptr),
      i32_vec_(nullptr), i64_vec_(nullptr), d_vec_(nullptr),
      str_vec_(nullptr), geom_ptr_(nullptr) {
    switch (other.storage_type_) {
        case StorageType::kInteger:
            std::memcpy(storage_.buffer, other.storage_.buffer, sizeof(int32_t));
            break;
        case StorageType::kInteger64:
            std::memcpy(storage_.buffer, other.storage_.buffer, sizeof(int64_t));
            break;
        case StorageType::kReal:
            std::memcpy(storage_.buffer, other.storage_.buffer, sizeof(double));
            break;
        case StorageType::kBoolean:
            storage_.buffer[0] = other.storage_.buffer[0];
            break;
        case StorageType::kString:
            if (other.str_ptr_) {
                str_ptr_ = new std::string(*other.str_ptr_);
            } else {
                std::memcpy(storage_.buffer, other.storage_.buffer, kSBOThreshold);
            }
            break;
        case StorageType::kDateTime:
            if (other.dt_ptr_) {
                dt_ptr_ = new CNDateTime(*other.dt_ptr_);
            }
            break;
        case StorageType::kBinary:
            if (other.bin_vec_) {
                bin_vec_ = new std::vector<uint8_t>(*other.bin_vec_);
            }
            break;
        case StorageType::kIntegerList:
            if (other.i32_vec_) {
                i32_vec_ = new std::vector<int32_t>(*other.i32_vec_);
            }
            break;
        case StorageType::kInteger64List:
            if (other.i64_vec_) {
                i64_vec_ = new std::vector<int64_t>(*other.i64_vec_);
            }
            break;
        case StorageType::kRealList:
            if (other.d_vec_) {
                d_vec_ = new std::vector<double>(*other.d_vec_);
            }
            break;
        case StorageType::kStringList:
            if (other.str_vec_) {
                str_vec_ = new std::vector<std::string>(*other.str_vec_);
            }
            break;
        case StorageType::kGeometry:
            if (other.geom_ptr_) {
                geom_ptr_ = new GeometryPtr((*other.geom_ptr_)->Clone());
            }
            break;
        default:
            break;
    }
}

CNFieldValue::CNFieldValue(CNFieldValue&& other) noexcept
    : type_(other.type_), storage_type_(other.storage_type_),
      str_ptr_(other.str_ptr_), dt_ptr_(other.dt_ptr_), 
      bin_vec_(other.bin_vec_), i32_vec_(other.i32_vec_),
      i64_vec_(other.i64_vec_), d_vec_(other.d_vec_),
      str_vec_(other.str_vec_), geom_ptr_(other.geom_ptr_) {
    std::memcpy(storage_.buffer, other.storage_.buffer, sizeof(storage_.buffer));
    other.storage_type_ = StorageType::kNone;
    other.type_ = CNFieldType::kUnset;
    other.str_ptr_ = nullptr;
    other.dt_ptr_ = nullptr;
    other.bin_vec_ = nullptr;
    other.i32_vec_ = nullptr;
    other.i64_vec_ = nullptr;
    other.d_vec_ = nullptr;
    other.str_vec_ = nullptr;
    other.geom_ptr_ = nullptr;
    std::memset(other.storage_.buffer, 0, sizeof(other.storage_.buffer));
}

CNFieldValue& CNFieldValue::operator=(const CNFieldValue& other) {
    if (this != &other) {
        Clear();
        type_ = other.type_;
        storage_type_ = other.storage_type_;
        
        switch (other.storage_type_) {
            case StorageType::kInteger:
                std::memcpy(storage_.buffer, other.storage_.buffer, sizeof(int32_t));
                break;
            case StorageType::kInteger64:
                std::memcpy(storage_.buffer, other.storage_.buffer, sizeof(int64_t));
                break;
            case StorageType::kReal:
                std::memcpy(storage_.buffer, other.storage_.buffer, sizeof(double));
                break;
            case StorageType::kBoolean:
                storage_.buffer[0] = other.storage_.buffer[0];
                break;
            case StorageType::kString:
                if (other.str_ptr_) {
                    str_ptr_ = new std::string(*other.str_ptr_);
                } else {
                    std::memcpy(storage_.buffer, other.storage_.buffer, kSBOThreshold);
                }
                break;
            case StorageType::kDateTime:
                if (other.dt_ptr_) {
                    dt_ptr_ = new CNDateTime(*other.dt_ptr_);
                }
                break;
            case StorageType::kBinary:
                if (other.bin_vec_) {
                    bin_vec_ = new std::vector<uint8_t>(*other.bin_vec_);
                }
                break;
            case StorageType::kIntegerList:
                if (other.i32_vec_) {
                    i32_vec_ = new std::vector<int32_t>(*other.i32_vec_);
                }
                break;
            case StorageType::kInteger64List:
                if (other.i64_vec_) {
                    i64_vec_ = new std::vector<int64_t>(*other.i64_vec_);
                }
                break;
            case StorageType::kRealList:
                if (other.d_vec_) {
                    d_vec_ = new std::vector<double>(*other.d_vec_);
                }
                break;
            case StorageType::kStringList:
                if (other.str_vec_) {
                    str_vec_ = new std::vector<std::string>(*other.str_vec_);
                }
                break;
            case StorageType::kGeometry:
                if (other.geom_ptr_) {
                    geom_ptr_ = new GeometryPtr((*other.geom_ptr_)->Clone());
                }
                break;
            default:
                break;
        }
    }
    return *this;
}

CNFieldValue& CNFieldValue::operator=(CNFieldValue&& other) noexcept {
    if (this != &other) {
        Clear();
        type_ = other.type_;
        storage_type_ = other.storage_type_;
        str_ptr_ = other.str_ptr_;
        dt_ptr_ = other.dt_ptr_;
        bin_vec_ = other.bin_vec_;
        i32_vec_ = other.i32_vec_;
        i64_vec_ = other.i64_vec_;
        d_vec_ = other.d_vec_;
        str_vec_ = other.str_vec_;
        geom_ptr_ = other.geom_ptr_;
        std::memcpy(storage_.buffer, other.storage_.buffer, sizeof(storage_.buffer));
        
        other.storage_type_ = StorageType::kNone;
        other.type_ = CNFieldType::kUnset;
        other.str_ptr_ = nullptr;
        other.dt_ptr_ = nullptr;
        other.bin_vec_ = nullptr;
        other.i32_vec_ = nullptr;
        other.i64_vec_ = nullptr;
        other.d_vec_ = nullptr;
        other.str_vec_ = nullptr;
        other.geom_ptr_ = nullptr;
        std::memset(other.storage_.buffer, 0, sizeof(other.storage_.buffer));
    }
    return *this;
}

CNFieldType CNFieldValue::GetType() const {
    return type_;
}

bool CNFieldValue::IsNull() const {
    return type_ == CNFieldType::kNull;
}

bool CNFieldValue::IsUnset() const {
    return type_ == CNFieldType::kUnset;
}

bool CNFieldValue::IsSet() const {
    return type_ != CNFieldType::kUnset && type_ != CNFieldType::kNull;
}

void CNFieldValue::SetNull() {
    Clear();
    type_ = CNFieldType::kNull;
    storage_type_ = StorageType::kNone;
}

void CNFieldValue::Unset() {
    Clear();
    type_ = CNFieldType::kUnset;
    storage_type_ = StorageType::kNone;
}

int32_t CNFieldValue::GetInteger() const {
    if (storage_type_ == StorageType::kInteger) {
        return *reinterpret_cast<const int32_t*>(storage_.buffer);
    }
    return 0;
}

int64_t CNFieldValue::GetInteger64() const {
    if (storage_type_ == StorageType::kInteger64) {
        return *reinterpret_cast<const int64_t*>(storage_.buffer);
    }
    return 0;
}

double CNFieldValue::GetReal() const {
    if (storage_type_ == StorageType::kReal) {
        return *reinterpret_cast<const double*>(storage_.buffer);
    }
    return 0.0;
}

bool CNFieldValue::GetBoolean() const {
    if (storage_type_ == StorageType::kBoolean) {
        return storage_.buffer[0] != 0;
    }
    return false;
}

std::string CNFieldValue::GetString() const {
    if (storage_type_ == StorageType::kString) {
        if (str_ptr_) {
            return *str_ptr_;
        }
        return std::string(reinterpret_cast<const char*>(storage_.buffer));
    }
    return std::string();
}

CNDateTime CNFieldValue::GetDateTime() const {
    if (storage_type_ == StorageType::kDateTime && dt_ptr_) {
        return *dt_ptr_;
    }
    return CNDateTime();
}

std::vector<uint8_t> CNFieldValue::GetBinary() const {
    if (storage_type_ == StorageType::kBinary && bin_vec_) {
        return *bin_vec_;
    }
    return std::vector<uint8_t>();
}

std::vector<int32_t> CNFieldValue::GetIntegerList() const {
    if (storage_type_ == StorageType::kIntegerList && i32_vec_) {
        return *i32_vec_;
    }
    return std::vector<int32_t>();
}

std::vector<int64_t> CNFieldValue::GetInteger64List() const {
    if (storage_type_ == StorageType::kInteger64List && i64_vec_) {
        return *i64_vec_;
    }
    return std::vector<int64_t>();
}

std::vector<double> CNFieldValue::GetRealList() const {
    if (storage_type_ == StorageType::kRealList && d_vec_) {
        return *d_vec_;
    }
    return std::vector<double>();
}

std::vector<std::string> CNFieldValue::GetStringList() const {
    if (storage_type_ == StorageType::kStringList && str_vec_) {
        return *str_vec_;
    }
    return std::vector<std::string>();
}

GeometryPtr CNFieldValue::GetGeometry() const {
    if (storage_type_ == StorageType::kGeometry && geom_ptr_) {
        return (*geom_ptr_)->Clone();
    }
    return GeometryPtr();
}

void CNFieldValue::SetInteger(int32_t value) {
    Clear();
    type_ = CNFieldType::kInteger;
    storage_type_ = StorageType::kInteger;
    *reinterpret_cast<int32_t*>(storage_.buffer) = value;
}

void CNFieldValue::SetInteger64(int64_t value) {
    Clear();
    type_ = CNFieldType::kInteger64;
    storage_type_ = StorageType::kInteger64;
    *reinterpret_cast<int64_t*>(storage_.buffer) = value;
}

void CNFieldValue::SetReal(double value) {
    Clear();
    type_ = CNFieldType::kReal;
    storage_type_ = StorageType::kReal;
    *reinterpret_cast<double*>(storage_.buffer) = value;
}

void CNFieldValue::SetBoolean(bool value) {
    Clear();
    type_ = CNFieldType::kBoolean;
    storage_type_ = StorageType::kBoolean;
    storage_.buffer[0] = value ? 1 : 0;
}

void CNFieldValue::SetString(const char* value) {
    Clear();
    type_ = CNFieldType::kString;
    storage_type_ = StorageType::kString;
    if (value) {
        size_t len = std::strlen(value);
        if (len <= kSBOThreshold) {
            std::memcpy(storage_.buffer, value, len + 1);
        } else {
            str_ptr_ = new std::string(value);
        }
    }
}

void CNFieldValue::SetString(const std::string& value) {
    Clear();
    type_ = CNFieldType::kString;
    storage_type_ = StorageType::kString;
    if (value.size() <= kSBOThreshold) {
        std::memcpy(storage_.buffer, value.data(), value.size());
        storage_.buffer[value.size()] = '\0';
    } else {
        str_ptr_ = new std::string(value);
    }
}

void CNFieldValue::SetDateTime(const CNDateTime& value) {
    Clear();
    type_ = CNFieldType::kDateTime;
    storage_type_ = StorageType::kDateTime;
    dt_ptr_ = new CNDateTime(value);
}

void CNFieldValue::SetBinary(const std::vector<uint8_t>& value) {
    Clear();
    type_ = CNFieldType::kBinary;
    storage_type_ = StorageType::kBinary;
    bin_vec_ = new std::vector<uint8_t>(value);
}

void CNFieldValue::SetIntegerList(const std::vector<int32_t>& value) {
    Clear();
    type_ = CNFieldType::kIntegerList;
    storage_type_ = StorageType::kIntegerList;
    i32_vec_ = new std::vector<int32_t>(value);
}

void CNFieldValue::SetInteger64List(const std::vector<int64_t>& value) {
    Clear();
    type_ = CNFieldType::kInteger64List;
    storage_type_ = StorageType::kInteger64List;
    i64_vec_ = new std::vector<int64_t>(value);
}

void CNFieldValue::SetRealList(const std::vector<double>& value) {
    Clear();
    type_ = CNFieldType::kRealList;
    storage_type_ = StorageType::kRealList;
    d_vec_ = new std::vector<double>(value);
}

void CNFieldValue::SetStringList(const std::vector<std::string>& value) {
    Clear();
    type_ = CNFieldType::kStringList;
    storage_type_ = StorageType::kStringList;
    str_vec_ = new std::vector<std::string>(value);
}

void CNFieldValue::SetGeometry(GeometryPtr value) {
    Clear();
    type_ = CNFieldType::kUnknown;
    storage_type_ = StorageType::kGeometry;
    geom_ptr_ = new GeometryPtr(std::move(value));
}

void CNFieldValue::Swap(CNFieldValue& other) noexcept {
    std::swap(type_, other.type_);
    std::swap(storage_type_, other.storage_type_);
    std::swap(storage_.buffer, other.storage_.buffer);
    std::swap(str_ptr_, other.str_ptr_);
    std::swap(dt_ptr_, other.dt_ptr_);
    std::swap(bin_vec_, other.bin_vec_);
    std::swap(i32_vec_, other.i32_vec_);
    std::swap(i64_vec_, other.i64_vec_);
    std::swap(d_vec_, other.d_vec_);
    std::swap(str_vec_, other.str_vec_);
    std::swap(geom_ptr_, other.geom_ptr_);
}

bool CNFieldValue::TryGetInteger(int32_t& out) const {
    if (storage_type_ == StorageType::kInteger) {
        out = *reinterpret_cast<const int32_t*>(storage_.buffer);
        return true;
    }
    return false;
}

bool CNFieldValue::TryGetInteger64(int64_t& out) const {
    if (storage_type_ == StorageType::kInteger64) {
        out = *reinterpret_cast<const int64_t*>(storage_.buffer);
        return true;
    }
    return false;
}

bool CNFieldValue::TryGetReal(double& out) const {
    if (storage_type_ == StorageType::kReal) {
        out = *reinterpret_cast<const double*>(storage_.buffer);
        return true;
    }
    return false;
}

bool CNFieldValue::TryGetString(std::string& out) const {
    if (storage_type_ == StorageType::kString) {
        if (str_ptr_) {
            out = *str_ptr_;
        } else {
            out = std::string(reinterpret_cast<const char*>(storage_.buffer));
        }
        return true;
    }
    return false;
}

bool CNFieldValue::TryGetDateTime(CNDateTime& out) const {
    if (storage_type_ == StorageType::kDateTime && dt_ptr_) {
        out = *dt_ptr_;
        return true;
    }
    return false;
}

bool CNFieldValue::ConvertToInteger(int32_t& out) const {
    switch (storage_type_) {
        case StorageType::kInteger:
            out = *reinterpret_cast<const int32_t*>(storage_.buffer);
            return true;
        case StorageType::kInteger64:
            out = static_cast<int32_t>(*reinterpret_cast<const int64_t*>(storage_.buffer));
            return true;
        case StorageType::kReal:
            out = static_cast<int32_t>(*reinterpret_cast<const double*>(storage_.buffer));
            return true;
        case StorageType::kBoolean:
            out = storage_.buffer[0] ? 1 : 0;
            return true;
        case StorageType::kString:
            if (str_ptr_) {
                out = static_cast<int32_t>(std::atoi(str_ptr_->c_str()));
            } else {
                out = static_cast<int32_t>(std::atoi(reinterpret_cast<const char*>(storage_.buffer)));
            }
            return true;
        default:
            return false;
    }
}

bool CNFieldValue::ConvertToInteger64(int64_t& out) const {
    switch (storage_type_) {
        case StorageType::kInteger:
            out = *reinterpret_cast<const int32_t*>(storage_.buffer);
            return true;
        case StorageType::kInteger64:
            out = *reinterpret_cast<const int64_t*>(storage_.buffer);
            return true;
        case StorageType::kReal:
            out = static_cast<int64_t>(*reinterpret_cast<const double*>(storage_.buffer));
            return true;
        case StorageType::kBoolean:
            out = storage_.buffer[0] ? 1 : 0;
            return true;
        case StorageType::kString:
            if (str_ptr_) {
                out = std::atoll(str_ptr_->c_str());
            } else {
                out = std::atoll(reinterpret_cast<const char*>(storage_.buffer));
            }
            return true;
        default:
            return false;
    }
}

bool CNFieldValue::ConvertToReal(double& out) const {
    switch (storage_type_) {
        case StorageType::kInteger:
            out = *reinterpret_cast<const int32_t*>(storage_.buffer);
            return true;
        case StorageType::kInteger64:
            out = static_cast<double>(*reinterpret_cast<const int64_t*>(storage_.buffer));
            return true;
        case StorageType::kReal:
            out = *reinterpret_cast<const double*>(storage_.buffer);
            return true;
        case StorageType::kBoolean:
            out = storage_.buffer[0] ? 1.0 : 0.0;
            return true;
        case StorageType::kString:
            if (str_ptr_) {
                out = std::atof(str_ptr_->c_str());
            } else {
                out = std::atof(reinterpret_cast<const char*>(storage_.buffer));
            }
            return true;
        default:
            return false;
    }
}

bool CNFieldValue::ConvertToString(std::string& out) const {
    switch (storage_type_) {
        case StorageType::kInteger:
            out = std::to_string(*reinterpret_cast<const int32_t*>(storage_.buffer));
            return true;
        case StorageType::kInteger64:
            out = std::to_string(*reinterpret_cast<const int64_t*>(storage_.buffer));
            return true;
        case StorageType::kReal:
            out = std::to_string(*reinterpret_cast<const double*>(storage_.buffer));
            return true;
        case StorageType::kBoolean:
            out = storage_.buffer[0] ? "true" : "false";
            return true;
        case StorageType::kString:
            if (str_ptr_) {
                out = *str_ptr_;
            } else {
                out = std::string(reinterpret_cast<const char*>(storage_.buffer));
            }
            return true;
        case StorageType::kDateTime:
            if (dt_ptr_) {
                out = dt_ptr_->ToISO8601();
            }
            return true;
        default:
            return false;
    }
}

bool CNFieldValue::ConvertToDateTime(CNDateTime& out) const {
    if (storage_type_ == StorageType::kDateTime && dt_ptr_) {
        out = *dt_ptr_;
        return true;
    }
    if (storage_type_ == StorageType::kString) {
        std::string str;
        if (TryGetString(str)) {
            out = CNDateTime::FromISO8601(str);
            return out.IsValid();
        }
    }
    return false;
}

} // namespace ogc
