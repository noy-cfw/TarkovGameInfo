#include "../TarkovGame/Includes.h"
#include "../TarkovGame/Tarkov.h"
#include "hlapi/hlapi.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <random>
#include <chrono>

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

FILE* dfile;
extern int vtTLBHits, vtTLBMisses;

__attribute__((constructor))
static void init()
{
	FILE* out = stdout;
	pid_t pid = 0;
#if (LMODE() != MODE_EXTERNAL())
	pid = getpid();
#endif
	fprintf(out, "Using Mode: %s\n", TOSTRING(LMODE));

	vmread_dfile = out;

	try {
		WinContext ctx(pid);
		ctx.processList.Refresh();

		fprintf(out, "Process List:\nPID\tVIRT\t\t\tPHYS\t\tBASE\t\tNAME\n");
		for (auto& i : ctx.processList)
			fprintf(out, "%.4lx\t%.16lx\t%.9lx\t%.9lx\t%s\n", i.proc.pid, i.proc.process, i.proc.physProcess, i.proc.dirBase, i.proc.name);

		for (auto& i : ctx.processList) {
			if (!strcasecmp("EscapeFromTarkov.exe", i.proc.name)) {
				fprintf(out, "\nLooping process %lx:\t%s\n", i.proc.pid, i.proc.name);

				PEB peb = i.GetPeb();
				short magic = i.Read<short>(peb.ImageBaseAddress);
				fprintf(out, "\tBase:\t%lx\tMagic:\t%hx (valid: %hhx)\n", peb.ImageBaseAddress, magic, (char)(magic == IMAGE_DOS_SIGNATURE));

				fprintf(out, "\tExports:\n");
				for (auto& o : i.modules) {
					fprintf(out, "\t%.8lx\t%.8lx\t%lx\t%s\n", o.info.baseAddress, o.info.entryPoint, o.info.sizeOfModule, o.info.name);
					if (!strcmp("UnityPlayer.dll", o.info.name)) {
					
						
						WinProcess* tarkov = ctx.processList.FindProcNoCase("EscapeFromTarkov.exe");
						uint64_t ModuleAddr = o.info.baseAddress;
                        runImpl(tarkov, ModuleAddr);
						break;
					}
				}
			}
		}
	} catch (VMException& e) {
		fprintf(out, "Initialization error: %d\n", e.value);
	}

	fclose(out);
}

int main()
{
	return 0;
}
