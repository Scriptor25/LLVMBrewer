cmake_minimum_required(VERSION 3.28)
project(LLVMBrewer)

option(BREWER_BUILD_EXAMPLE "Enable the example target" ON)
option(BREWER_INSTALL "Enable the install targets" OFF)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(LLVM REQUIRED)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

set(LLVM_LIBS)
list(APPEND LLVM_LIBS
        LLVMDemangle
        LLVMSupport
        LLVMTableGen
        LLVMTableGenGlobalISel
        LLVMTableGenCommon
        LLVMCore
        LLVMFuzzerCLI
        LLVMFuzzMutate
        LLVMFileCheck
        LLVMInterfaceStub
        LLVMIRPrinter
        LLVMIRReader
        LLVMCodeGenTypes
        LLVMCodeGen
        LLVMSelectionDAG
        LLVMAsmPrinter
        LLVMMIRParser
        LLVMGlobalISel
        LLVMBinaryFormat
        LLVMBitReader
        LLVMBitWriter
        LLVMBitstreamReader
        LLVMDWARFLinker
        LLVMDWARFLinkerClassic
        LLVMDWARFLinkerParallel
        LLVMExtensions
        LLVMFrontendDriver
        LLVMFrontendHLSL
        LLVMFrontendOpenACC
        LLVMFrontendOpenMP
        LLVMFrontendOffloading
        LLVMTransformUtils
        LLVMInstrumentation
        LLVMAggressiveInstCombine
        LLVMInstCombine
        LLVMScalarOpts
        LLVMipo
        LLVMVectorize
        LLVMObjCARCOpts
        LLVMCoroutines
        LLVMCFGuard
        LLVMHipStdPar
        LLVMLinker
        LLVMAnalysis
        LLVMLTO
        LLVMMC
        LLVMMCParser
        LLVMMCDisassembler
        LLVMMCA
        LLVMObjCopy
        LLVMObject
        LLVMObjectYAML
        LLVMOption
        LLVMRemarks
        LLVMDebuginfod
        LLVMDebugInfoDWARF
        LLVMDebugInfoGSYM
        LLVMDebugInfoLogicalView
        LLVMDebugInfoMSF
        LLVMDebugInfoCodeView
        LLVMDebugInfoPDB
        LLVMSymbolize
        LLVMDebugInfoBTF
        LLVMDWP
        LLVMExecutionEngine
        LLVMInterpreter
        LLVMJITLink
        LLVMMCJIT
        LLVMOrcJIT
        LLVMOrcDebugging
        LLVMOrcShared
        LLVMOrcTargetProcess
        LLVMRuntimeDyld
        LLVMTarget
        LLVMAArch64CodeGen
        LLVMAArch64AsmParser
        LLVMAArch64Disassembler
        LLVMAArch64Desc
        LLVMAArch64Info
        LLVMAArch64Utils
        LLVMAMDGPUCodeGen
        LLVMAMDGPUAsmParser
        LLVMAMDGPUDisassembler
        LLVMAMDGPUTargetMCA
        LLVMAMDGPUDesc
        LLVMAMDGPUInfo
        LLVMAMDGPUUtils
        LLVMARMCodeGen
        LLVMARMAsmParser
        LLVMARMDisassembler
        LLVMARMDesc
        LLVMARMInfo
        LLVMARMUtils
        LLVMAVRCodeGen
        LLVMAVRAsmParser
        LLVMAVRDisassembler
        LLVMAVRDesc
        LLVMAVRInfo
        LLVMBPFCodeGen
        LLVMBPFAsmParser
        LLVMBPFDisassembler
        LLVMBPFDesc
        LLVMBPFInfo
        LLVMHexagonCodeGen
        LLVMHexagonAsmParser
        LLVMHexagonDisassembler
        LLVMHexagonDesc
        LLVMHexagonInfo
        LLVMLanaiCodeGen
        LLVMLanaiAsmParser
        LLVMLanaiDisassembler
        LLVMLanaiDesc
        LLVMLanaiInfo
        LLVMLoongArchCodeGen
        LLVMLoongArchAsmParser
        LLVMLoongArchDisassembler
        LLVMLoongArchDesc
        LLVMLoongArchInfo
        LLVMMipsCodeGen
        LLVMMipsAsmParser
        LLVMMipsDisassembler
        LLVMMipsDesc
        LLVMMipsInfo
        LLVMMSP430CodeGen
        LLVMMSP430Desc
        LLVMMSP430Info
        LLVMMSP430AsmParser
        LLVMMSP430Disassembler
        LLVMNVPTXCodeGen
        LLVMNVPTXDesc
        LLVMNVPTXInfo
        LLVMPowerPCCodeGen
        LLVMPowerPCAsmParser
        LLVMPowerPCDisassembler
        LLVMPowerPCDesc
        LLVMPowerPCInfo
        LLVMRISCVCodeGen
        LLVMRISCVAsmParser
        LLVMRISCVDisassembler
        LLVMRISCVDesc
        LLVMRISCVTargetMCA
        LLVMRISCVInfo
        LLVMSparcCodeGen
        LLVMSparcAsmParser
        LLVMSparcDisassembler
        LLVMSparcDesc
        LLVMSparcInfo
        LLVMSystemZCodeGen
        LLVMSystemZAsmParser
        LLVMSystemZDisassembler
        LLVMSystemZDesc
        LLVMSystemZInfo
        LLVMVECodeGen
        LLVMVEAsmParser
        LLVMVEDisassembler
        LLVMVEInfo
        LLVMVEDesc
        LLVMWebAssemblyCodeGen
        LLVMWebAssemblyAsmParser
        LLVMWebAssemblyDisassembler
        LLVMWebAssemblyDesc
        LLVMWebAssemblyInfo
        LLVMWebAssemblyUtils
        LLVMX86CodeGen
        LLVMX86AsmParser
        LLVMX86Disassembler
        LLVMX86TargetMCA
        LLVMX86Desc
        LLVMX86Info
        LLVMXCoreCodeGen
        LLVMXCoreDisassembler
        LLVMXCoreDesc
        LLVMXCoreInfo
        LLVMAsmParser
        LLVMLineEditor
        LLVMProfileData
        LLVMCoverage
        LLVMPasses
        LLVMTargetParser
        LLVMTextAPI
        LLVMTextAPIBinaryReader
        LLVMDlltoolDriver
        LLVMLibDriver
        LLVMXRay
        LLVMWindowsDriver
        LLVMWindowsManifest
        LTO
        LLVMCFIVerify
        LLVMDiff
        LLVMExegesisX86
        LLVMExegesisAArch64
        LLVMExegesisPowerPC
        LLVMExegesisMips
        LLVMExegesis
        LLVM-C
        Remarks)

separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})

message(STATUS "Libs: ${LLVM_LIBS}")

file(GLOB_RECURSE brewer-src lib/src/*.cpp lib/include/*.hpp)
add_library(brewer STATIC ${brewer-src})
target_compile_definitions(brewer PUBLIC ${LLVM_DEFINITIONS_LIST})
target_include_directories(brewer PUBLIC lib/include ${LLVM_INCLUDE_DIRS})
target_link_libraries(brewer PUBLIC ${LLVM_LIBS})

if (${BREWER_INSTALL})
    install(TARGETS brewer)
endif ()

if (${BREWER_BUILD_EXAMPLE})
    file(GLOB_RECURSE example-src example/src/*.cpp example/include/*.hpp)
    add_executable(example ${example-src})
    target_include_directories(example PRIVATE example/include)
    target_link_libraries(example PRIVATE brewer)

    if (${BREWER_INSTALL})
        install(TARGETS example)
    endif ()
endif ()
