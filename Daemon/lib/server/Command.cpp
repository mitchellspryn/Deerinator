#include "Command.hpp"

using namespace std;

Command::Command()
{
  this->errors = vector<const char*>();
  this->Valid = false;
  this->Module = NULL;
  this->Component = NULL;
  this->Property = NULL;
  this->Value = NULL;
  for (int i = 0; i < 256; i++)
  {
    this->workBuf[i] = 0;
  }
  return;
}

Command::~Command()
{
  ;
}

bool Command::setModule(const char* _M)
{
  if ( (!strcmp(_M, "gimble")) || (!strcmp(_M, "accelerator")) || (!strcmp(_M, "deerTracker")))
  {
    this->Module = _M;
    return true;
  }
  else
  {
    this->errors.push_back("BADMOD");
    return false;
  }
}

const char* Command::getModule()
{
  return this->Module;
}

bool Command::setComponent(const char* _C)
{
  if (this->Module == NULL)
  {
    return false;
  }

  if (!strcmp(this->Module, "gimble"))
  {
    if ( (!strcmp(_C, "pan")) || (!strcmp(_C, "tilt")) )
    {
      this->Component = _C;
      return true;
    }
    else
    {
      this->errors.push_back("BADCOMP");
      return false;
    }
  }

  if (!strcmp(this->Module, "accelerator"))
  {
    if ( (!strcmp(_C, "fireCtl")) )
    {
      this->Component = _C;
      return true;
    }
    else
    {
      this->errors.push_back("BADCOMP");
      return false;
    }
  }

  if (!strcmp(this->Module, "deerTracker"))
  {
    this->errors.push_back("BADCOMP");
    return false;
  }

  this->errors.push_back("BADMOD");
  return false;
   
}

const char* Command::getComponent()
{
  return this->Component;
}

bool Command::setProperty(const char* _P)
{
  if (this->Module == NULL || this->Component == NULL)
  {
    return false;
  }

  if (!strcmp(this->Module, "gimble"))
  {
    if ( (!strcmp(this->Component, "pan")) || (!strcmp(this->Component, "tilt")))
    {
      if (!strcmp(_P, "isAttached"))
      {
        this->Property = _P;
        this->Valid = true;
        return true;
      }
      
      if (!strcmp(_P, "setDeg"))
      {
        this->Property = _P;
        return true;
      }

      if (!strcmp(_P, "stepDeg"))
      {
        this->Property = _P;
        return true;
      }

      if (!strcmp(_P, "getDeg"))
      {
        this->Property = _P;
        return true;
      }

      this->errors.push_back("BADPTY");
      return false;
    }
  }

  if (!strcmp(this->Module, "accelerator"))
  {
    if (!strcmp(this->Component, "fireCtl"))
    {
      if (!strcmp(_P, "isAttached"))
      {
        this->Property = _P;
        this->Valid = true;
        return true;
      }

      if (!strcmp(_P, "fireReady"))
      {
        this->Property = _P;
        this->Valid = true;
        return true;
      }

      if (!strcmp(_P, "fire"))
      {
        this->Property = _P;
        this->Valid = true;
        return true;
      }

      if (!strcmp(_P, "waitFire"))
      {
        this->Property = _P;
        this->Valid = true;
        return true;
      }

      this->errors.push_back("BADPTY");
      return false;
    }
  }

  return false;
}

const char* Command::getProperty()
{
  return this->Property;
}

bool Command::setValue(const char* _V)
{
  if (this->Module == NULL || this->Component == NULL || this->Property == NULL || _V == NULL)
  {
    return false;
  }

  if (!strcmp(this->Module, "gimble"))
  {
    if ( (!strcmp(this->Component, "pan")) || (!strcmp(this->Component, "tilt")))
    {
      
      if (!strcmp(this->Property, "setDeg"))
      {
        this->Value = _V;
        this->Valid = true;
        return true;
      }

      if (!strcmp(this->Property, "stepDeg"))
      {
        this->Value = _V;
        this->Valid = true;
        return true;
      }

      if (!strcmp(this->Property, "getDeg"))
      {
        this->Valid = true;
        return true;
      }

    }
  }

  return false;
}

const char* Command::getVal()
{
  return this->Value;
}

bool Command::isValid()
{
  return this->Valid;
}

vector<const char*> Command::getErr()
{
  return this->errors;
}

bool Command::Parse(const char* _Module, const char* _Component, const char* _Property, const char* _Value)
{
  this->Valid = false;
  this->setModule(_Module);
  this->setComponent(_Component);
  this->setProperty(_Property);
  this->setValue(_Value);
  return this->Valid; 
}

bool Command::Parse(const char* str)
{
  this->errors = vector<const char*>();
  this->Valid = false;
  this->Module = NULL;
  this->Component = NULL;
  this->Property = NULL;
  this->Value = NULL;

  /*Divide up command based on initial protocol*/
  strcpy(this->workBuf, str);
  
  char* moduleSeparator = strchr(this->workBuf, ':');
  char* componentSeparator = strchr(this->workBuf, '.');
  char* commandSeparator = strstr(this->workBuf, "=(");
  char* finalBracket = strchr(this->workBuf, ')');


  if (moduleSeparator != NULL && componentSeparator != NULL && commandSeparator != NULL && finalBracket != NULL)
  {
    *moduleSeparator = 0;
    moduleSeparator++;
    *componentSeparator = 0;
    componentSeparator++;
    *commandSeparator = 0;
    commandSeparator++;
    *commandSeparator = 0;
    commandSeparator++;
    *finalBracket = 0;
    this->Parse(this->workBuf, moduleSeparator, componentSeparator, commandSeparator); 
    return true;
  }
  return false;
}

void Command::printCmd(FILE *fp)
{
  if (!this->Valid)
  {
    fprintf(fp, "INVALID COMMAND!/n");
    return;
  }

  fprintf(fp, "%s:%s.%s=(%s)\n", this->Module, this->Component, this->Property, this->Value);
  return;
}
