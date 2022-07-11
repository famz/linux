#include <linux/module.h>

#include "wasm3.h"

unsigned char fib32_wasm[] = {
  0x00, 0x61, 0x73, 0x6d, 0x01, 0x00, 0x00, 0x00, 0x01, 0x06, 0x01, 0x60,
  0x01, 0x7f, 0x01, 0x7f, 0x03, 0x02, 0x01, 0x00, 0x07, 0x07, 0x01, 0x03,
  0x66, 0x69, 0x62, 0x00, 0x00, 0x0a, 0x1f, 0x01, 0x1d, 0x00, 0x20, 0x00,
  0x41, 0x02, 0x49, 0x04, 0x40, 0x20, 0x00, 0x0f, 0x0b, 0x20, 0x00, 0x41,
  0x02, 0x6b, 0x10, 0x00, 0x20, 0x00, 0x41, 0x01, 0x6b, 0x10, 0x00, 0x6a,
  0x0f, 0x0b
};
unsigned int fib32_wasm_len = 62;

#define FATAL(func, msg) panic(msg)

void run_wasm(void)
{
	IM3Environment env;
	IM3Runtime runtime;
	IM3Module module;
	M3Result result = m3Err_none;
	IM3Function f;
	uint32_t value = 0;
	uint8_t* wasm = (uint8_t*)fib32_wasm;
	size_t fsize = fib32_wasm_len;

	pr_warn("Loading WebAssembly...\n");

	env = m3_NewEnvironment ();
	if (!env) FATAL("m3_NewEnvironment", "failed");

	runtime = m3_NewRuntime (env, 1024, NULL);
	if (!runtime) FATAL("m3_NewRuntime", "failed");

	result = m3_ParseModule (env, &module, wasm, fsize);
	if (result) FATAL("m3_ParseModule", result);

	result = m3_LoadModule (runtime, module);
	if (result) FATAL("m3_LoadModule", result);

	result = m3_FindFunction (&f, runtime, "fib");
	if (result) FATAL("m3_FindFunction", result);

	pr_warn("Running...\n");

	result = m3_CallV(f, 24);
	if (result) FATAL("m3_Call", result);

	result = m3_GetResultsV (f, &value);
	if (result) FATAL("m3_GetResults: %s", result);

	pr_warn("Result: \n");
	pr_warn("%d", value);
	pr_warn("\n");
}

static int __init wasm_init(void)
{
	pr_warn("wasm init\n");
	run_wasm();
	return 0;
}

static void __exit wasm_fini(void)
{
	pr_warn("wasm fini\n");
}
module_init(wasm_init);
module_exit(wasm_fini);
