#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <vector>

class Command
{
  public:
    Command();
    ~Command();

    bool Parse(const char* _str);
    bool Parse(const char* _Module, const char* _Component, const char* _Property, const char* _Value);

    const char* getModule();
    const char* getComponent();
    const char* getProperty();
    const char* getVal();

    bool isValid();
    void printCmd(FILE* fp);
    std::vector<const char*> getErr();

  private:

    bool setModule(const char* _M);
    bool setComponent(const char* _C);
    bool setProperty(const char* _P);
    bool setValue(const char* _V);

    const char* Module;
    const char* Component;
    const char* Property; 
    const char* Value;

    char workBuf[256];

    bool Valid;

    std::vector<const char*> errors;
}; 

#endif
