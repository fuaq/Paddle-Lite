/* Copyright (c) 2016 PaddlePaddle Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License. */

#pragma once
#include <string>
#include <typeindex>
#include "lite/backends/x86/fluid/float16.h"
#include "lite/core/framework.pb.h"
#include "lite/utils/cp_logging.h"

namespace paddle {
namespace lite {
namespace fluid {

template <typename T>
struct DataTypeTrait {};

// Stub handle for void
template <>
struct DataTypeTrait<void> {
  constexpr static auto DataType = framework::proto::VarType::RAW;
};

#define _ForEachDataTypeHelper_(callback, cpp_type, proto_type) \
  callback(cpp_type, ::paddle::framework::proto::VarType::proto_type);

#define _ForEachDataType_(callback)                                        \
  _ForEachDataTypeHelper_(callback, float, FP32);                          \
  _ForEachDataTypeHelper_(callback, ::paddle::lite::fluid::float16, FP16); \
  _ForEachDataTypeHelper_(callback, double, FP64);                         \
  _ForEachDataTypeHelper_(callback, int, INT32);                           \
  _ForEachDataTypeHelper_(callback, int64_t, INT64);                       \
  _ForEachDataTypeHelper_(callback, bool, BOOL);                           \
  _ForEachDataTypeHelper_(callback, uint8_t, UINT8);                       \
  _ForEachDataTypeHelper_(callback, int16_t, INT16);                       \
  _ForEachDataTypeHelper_(callback, int8_t, INT8)

#define DefineDataTypeTrait(cpp_type, proto_type) \
  template <>                                     \
  struct DataTypeTrait<cpp_type> {                \
    constexpr static auto DataType = proto_type;  \
  }

_ForEachDataType_(DefineDataTypeTrait);

#undef DefineDataTypeTrait

extern framework::proto::VarType::Type ToDataType(std::type_index type);
extern std::type_index ToTypeIndex(framework::proto::VarType::Type type);

template <typename Visitor>
inline void VisitDataType(framework::proto::VarType::Type type,
                          Visitor visitor) {
#define VisitDataTypeCallback(cpp_type, proto_type) \
  do {                                              \
    if (type == proto_type) {                       \
      visitor.template apply<cpp_type>();           \
      return;                                       \
    }                                               \
  } while (0)

  _ForEachDataType_(VisitDataTypeCallback);
#undef VisitDataTypeCallback
  LOG(FATAL) << "Not supported " << type;
}

extern std::string DataTypeToString(const framework::proto::VarType::Type type);
extern size_t SizeOfType(framework::proto::VarType::Type type);
inline std::ostream& operator<<(std::ostream& out,
                                const framework::proto::VarType::Type& type) {
  out << DataTypeToString(type);
  return out;
}

}  // namespace fluid
}  // namespace lite
}  // namespace paddle
