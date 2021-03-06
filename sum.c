/**
 * LLVM equivalent of:
 *
 * int sum(int a, int b) {
 *     return a + b;
 * }
 */

#include <llvm-c/Core.h>
#include <llvm-c/ExecutionEngine.h>
#include <llvm-c/Target.h>
#include <llvm-c/Analysis.h>
#include <llvm-c/BitWriter.h>

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char const *argv[]) {
  LLVMModuleRef mod = LLVMModuleCreateWithName("my_module");

  LLVMTypeRef param_types[] = { LLVMInt32Type(), LLVMInt32Type() };
  LLVMTypeRef ret_type = LLVMFunctionType(LLVMInt32Type(), param_types, 2, 0);
  LLVMValueRef sum = LLVMAddFunction(mod, "sum", ret_type);

  LLVMBasicBlockRef entry = LLVMAppendBasicBlock(sum, "entry");

  LLVMContextRef context = LLVMGetGlobalContext();
  LLVMBuilderRef builder = LLVMCreateBuilderInContext(context);

  LLVMPositionBuilderAtEnd(builder, entry);
  LLVMValueRef tmp = LLVMBuildAdd(builder, LLVMGetParam(sum, 0), LLVMGetParam(sum, 1), "tmp");
  LLVMBuildRet(builder, tmp);

  char *error = NULL;
  LLVMVerifyModule(mod, LLVMAbortProcessAction, &error);
  LLVMDisposeMessage(error);

  LLVMExecutionEngineRef engine;
  error = NULL;
  LLVMLinkInMCJIT();
  LLVMInitializeNativeTarget();
  LLVMInitializeX86AsmPrinter();
  LLVMInitializeX86Disassembler();

  if (LLVMCreateExecutionEngineForModule(&engine, mod, &error) != 0) {
    fprintf(stderr, "failed to create execution engine\n");
    abort();
  }
  if (error) {
    fprintf(stderr, "error: %s\n", error);
    LLVMDisposeMessage(error);
    exit(EXIT_FAILURE);
  }

  if (argc < 3) {
    fprintf(stderr, "usage: %s x y\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  LLVMValueRef GlobalVar = LLVMAddGlobal(mod, LLVMArrayType(LLVMInt8Type(), 6), "simple_value");
  LLVMSetInitializer(GlobalVar, LLVMConstString("nyan", 6, 1));
  uint64_t raw = LLVMGetGlobalValueAddress(engine, "simple_value");
  printf("%s\n", raw);

  // LLVMValueRef aaa = LLVMBuildGlobalStringPtr(builder, "there", "nyan");
  // LLVMValueRef aaa = LLVMAddGlobalInAddressSpace(mod, LLVMArrayType(LLVMInt8Type(), 6), "nyan", 6);
  // LLVMValueRef aaa = LLVMBuildGlobalStringPtr(builder, "nyan", "nyan");
  
  // LLVMAddGlobalMapping(engine, aaa, "nyan");
  // LLVMPrintValueToString(aaa);
  // LLVMGetGlobalValueAddress(engine, "nyan");

    /// extern int printf(char*, ...)

  long long x = strtoll(argv[1], NULL, 10);
  long long y = strtoll(argv[2], NULL, 10);

  int (*func_pointer)(int, int);
  func_pointer = (int (*)(int, int))LLVMGetFunctionAddress(engine, "sum");
  printf("%d\n", func_pointer(x, y));

  LLVMDumpModule(mod);
  // Write out bitcode to file
  if (LLVMWriteBitcodeToFile(mod, "sum.bc") != 0) {
    fprintf(stderr, "error writing bitcode to file, skipping\n");
  }

  LLVMDisposeBuilder(builder);
  LLVMDisposeExecutionEngine(engine);
}

// LLVMGenericValueRef testX = LLVMCreateGenericValueOfInt(LLVMInt32Type(), x, 0);
// printf("%d\n", (int)LLVMGenericValueToInt(testX, 0));

  // LLVMValueRef aaa = LLVMAddGlobalInAddressSpace(mod, LLVMArrayType(LLVMInt8Type(), 6), "nyan", 6);

  /* this is a function to emit llvm-ir code */
  // LLVMPrintValueToString(aaa);
  // printf("%s\n", LLVMPrintValueToString(aaa));



  // LLVMValueRef GlobalVar = LLVMAddGlobal(mod, LLVMInt32Type(), "simple_value");
  // LLVMSetInitializer(GlobalVar, LLVMConstInt(LLVMInt32Type(), 42, 0));

  // uint64_t raw = LLVMGetGlobalValueAddress(engine, "simple_value");
  // int32_t *usable  = (int32_t *) raw;
  // printf("%d\n", *usable);