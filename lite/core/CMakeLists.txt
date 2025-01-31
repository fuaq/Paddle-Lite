if (WITH_TESTING)
  add_subdirectory (test)
endif ()
lite_cc_library (target_wrapper SRCS target_wrapper.cc
  DEPS target_wrapper_host place fbs_headers
  X86_DEPS target_wrapper_x86
  CUDA_DEPS target_wrapper_cuda
  XPU_DEPS target_wrapper_xpu
  CL_DEPS cl_target_wrapper
  METAL_DEPS metal_target_wrapper
  FPGA_DEPS fpga_target_wrapper
  INTEL_FPGA_DEPS intel_fpga_target_wrapper
  BM_DEPS target_wrapper_bm
  MLU_DEPS target_wrapper_mlu
  NNADAPTER_DEPS nnadapter_wrapper
)

lite_cc_library (memory SRCS memory.cc DEPS target_wrapper)

set (tensor_extra_deps "")
if (LITE_WITH_FPGA)
  set (tensor_extra_deps lite_tensor_fpga)
endif ()
lite_cc_library (tensor SRCS tensor.cc dim.cc DEPS memory ${tensor_extra_deps})

if (NOT LITE_ON_TINY_PUBLISH)
  proto_library (framework_proto SRCS framework.proto)
endif ()

lite_cc_library (variable SRCS variable.cc DEPS tensor)
lite_cc_library (types SRCS types.cc)
lite_cc_library (op_registry SRCS op_registry.cc DEPS kernel)
lite_cc_library (scope SRCS scope.cc DEPS tensor)
lite_cc_library (device_info SRCS device_info.cc DEPS tensor)

if (LITE_WITH_ARM)
  lite_cc_library (context SRCS context.cc
    DEPS tensor any device_info target_wrapper
    CL_DEPS cl_context
  )
elseif (LITE_WITH_X86)
  lite_cc_library (context SRCS context.cc
    DEPS tensor any device_info target_wrapper eigen3
    CL_DEPS cl_context
    CUDA_DEPS cuda_context
  )
elseif (LITE_WITH_SW)
  lite_cc_library (context SRCS context.cc
    DEPS tensor any device_info target_wrapper
  )
else ()
  meaasge (FATAL_ERROR "unsupported host device")
endif ()

#-------------------------------------------- GET CODE META INFO ------------------------------------------
if (LITE_WITH_CODE_META_INFO)
  execute_process (
    COMMAND git describe --tags --exact-match
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE PADDLE_LITE_TAG
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )

  execute_process (
    COMMAND git rev-parse --abbrev-ref HEAD
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE PADDLE_LITE_BRANCH
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )

  execute_process (
    COMMAND git log -1 --format=%h
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE PADDLE_LITE_COMMIT
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )

  message (STATUS "tag: ${PADDLE_LITE_TAG}")
  message (STATUS "branch: ${PADDLE_LITE_BRANCH}")
  message (STATUS "commit: ${PADDLE_LITE_COMMIT}")
endif ()
configure_file (version.h.in version.h)
#----------------------------------------------- NOT CHANGE -----------------------------------------------
lite_cc_library (kernel SRCS kernel.cc
  DEPS context type_system target_wrapper any op_params tensor
  PROFILE_DEPS lite_profiler
)
lite_cc_library (op SRCS op_lite.cc
  DEPS scope op_registry target_wrapper kernel cpp_op_desc tensor utils
)



lite_cc_library (type_system SRCS type_system.cc DEPS tensor target_wrapper)

lite_cc_library (program SRCS program.cc
  DEPS op kernel model_parser ${ops} ${cpp_wrapper}
  PROFILE_DEPS lite_profiler
  CUDA_DEPS nvtx_wrapper cuda_type_trans
)

if (NOT LITE_ON_TINY_PUBLISH)
  add_subdirectory (optimizer)
  add_subdirectory (profile)

endif()

if (LITE_WITH_XCODE)
  add_subdirectory (profile)
endif ()

if (LITE_WITH_NPU OR LITE_WITH_XTCL OR LITE_WITH_BM OR LITE_WITH_RKNPU OR LITE_WITH_MLU OR LITE_WITH_APU OR LITE_WITH_HUAWEI_ASCEND_NPU OR LITE_WITH_IMAGINATION_NNA OR LITE_WITH_NNADAPTER)
  lite_cc_library(subgraph_bridge_registry
    SRCS subgraph_bridge_registry.cc
    DEPS op
  )
  lite_cc_library(subgraph_engine_base
    SRCS subgraph_engine_base.cc
    DEPS tensor op scope program
  )
endif ()

# for mobile, unnecessary to compile the following testings.
if (LITE_WITH_LIGHT_WEIGHT_FRAMEWORK)
  return ()
endif ()

lite_cc_test (test_scope SRCS scope_test.cc DEPS scope)
lite_cc_test (test_kernel SRCS kernel_test.cc DEPS kernel target_wrapper any)
lite_cc_test (test_op SRCS op_lite_test.cc DEPS op)
lite_cc_test (test_tensor SRCS lite_tensor_test.cc DEPS tensor)
lite_cc_test (test_type_system SRCS type_system_test.cc DEPS type_system utils)
lite_cc_test (test_types SRCS types_test.cc DEPS types)
lite_cc_test (test_memory SRCS memory_test.cc DEPS memory)
lite_cc_test (test_context SRCS context_test.cc DEPS context)
