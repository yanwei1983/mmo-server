#ifndef CALLSTACKDUMPER_H
#define CALLSTACKDUMPER_H

#include <string>

std::string GetStackTraceString(int skip = 0, int depth = 32);

bool DumpStack(int skip = 0, int depth = 32);
bool DumpStackFile(int skip = 0, int depth = 32);


#endif /* CALLSTACKDUMPER_H */
