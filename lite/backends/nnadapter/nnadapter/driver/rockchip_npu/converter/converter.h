// Copyright (c) 2019 PaddlePaddle Authors. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>
#include "driver/rockchip_npu/utility.h"

namespace nnadapter {
namespace rockchip_npu {

class Converter {
 public:
  explicit Converter(
      rk::nn::Graph* graph,
      std::map<hal::Operand*, std::vector<std::shared_ptr<rk::nn::Tensor>>>*
          tensors)
      : graph_(graph), tensors_(tensors) {}
  ~Converter() {}

  // Convert a NNAdapter model to rknn graph and tensors
  int Apply(hal::Model* model);
  // Mapping a rknn tensor to a NNAdapter operand
  std::string GetTensorName(hal::Operand* operand);
  std::shared_ptr<rk::nn::Tensor> GetMappedTensor(hal::Operand* operand);
  std::shared_ptr<rk::nn::Tensor> UpdateTensorMap(
      hal::Operand* operand, std::shared_ptr<rk::nn::Tensor> tensor);
  // Create and add a named or anonymous rknn tensor into the tensor pool
  std::shared_ptr<rk::nn::Tensor> AddTensor(
      const std::string& name,
      int32_t* dimensions,
      uint32_t dimension_count,
      rk::nn::PrecisionType precision,
      const float* quant_scale = nullptr,
      const int32_t* zero_point = nullptr,
      void* buffer = nullptr,
      rk::nn::DataLayoutType layout = rk::nn::DataLayoutType::NCHW);
  std::shared_ptr<rk::nn::Tensor> AddTensor(
      const std::string& name,
      const NNAdapterOperandType* type,
      void* buffer = nullptr,
      std::vector<int32_t> dimensions = {});
  std::shared_ptr<rk::nn::Tensor> AddConstantTensor(
      void* values,
      int32_t* dimensions,
      uint32_t dimension_count,
      rk::nn::PrecisionType precision,
      const float* quant_scale = nullptr,
      const int32_t* zero_point = nullptr);
  std::shared_ptr<rk::nn::Tensor> AddVariableTensor(
      const std::string& name,
      int32_t* dimensions,
      uint32_t dimension_count,
      rk::nn::PrecisionType precision,
      const float* quant_scale = nullptr,
      const int32_t* zero_point = nullptr);
  // Quant8 constant operand with asymmetric per-layer quantizion
  std::shared_ptr<rk::nn::Tensor> AddQuant8ConstantTensor(
      uint8_t* values,
      int32_t* dimensions,
      uint32_t dimension_count,
      float quant_scale,
      int32_t zero_point);
  // Quant32 constant operand with symmetric per-layer quantizion
  std::shared_ptr<rk::nn::Tensor> AddQuant32ConstantTensor(
      int32_t* values,
      int32_t* dimensions,
      uint32_t dimension_count,
      float quant_scale);
  // Quant8 variable operand with asymmetric per-layer quantizion
  std::shared_ptr<rk::nn::Tensor> AddQuant8VariableTensor(
      const std::string& name,
      int32_t* dimensions,
      uint32_t dimension_count,
      float quant_scale,
      int32_t zero_point);
  // Convert a NNAdapter operand to a rknn tensor
  std::shared_ptr<rk::nn::Tensor> ConvertOperand(
      hal::Operand* operand, std::vector<int32_t> dimensions = {});
  // Add a rknn operator into rknn graph
  std::shared_ptr<rk::nn::Operator> AddOperator(
      rk::nn::OperatorType type,
      std::vector<std::shared_ptr<rk::nn::Tensor>> input_tensors,
      std::vector<std::shared_ptr<rk::nn::Tensor>> output_tensors,
      void* attrs,
      std::string name = "");

 private:
  rk::nn::Graph* graph_{nullptr};
  std::map<hal::Operand*, std::vector<std::shared_ptr<rk::nn::Tensor>>>*
      tensors_;
};

}  // namespace rockchip_npu
}  // namespace nnadapter
