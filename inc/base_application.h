#ifndef __BASE_APPLICATION_H__
#define __BASE_APPLICATION_H__
#pragma once

class BaseApplication
{
public:
  BaseApplication() = default;

  virtual ~BaseApplication() = default;
  
  BaseApplication(const BaseApplication&) = delete;
  BaseApplication& operator=(const BaseApplication&) = delete;

  virtual int Run(int argc, char* argv[]) = 0;

private:

};

#endif //__BASE_APPLICATION_H__
