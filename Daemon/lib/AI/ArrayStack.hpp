#ifndef ARRAYSTACK_HPP
#define ARRAYSTACK_HPP

#include <stdio.h>
#include <stdlib.h>
#include <vector>


template<class T>
class ArrayStack
{
  public:
    ArrayStack(int _size)
    {
      this->stack = (T*)calloc(_size, sizeof(T));

      if (this->stack == NULL)
      {
        this->valid = false;
      }

      this->valid = !(this->stack == NULL);

      if (this->valid)
      {
        this->cursor = stack;
      }
    }

    ~ArrayStack()
    {
      if (this->valid)
      {
        free(this->stack);
      }
    }

    T* data()
    {
      return this->stack;
    }

    void push(T data)
    {
      *(this->cursor) = data;
      this->cursor++;
      return;
    }

    void reset()
    {
      this->cursor = stack;
    }

    int nextIndex()
    {
      return (this->cursor - this->stack);
    }

    bool isValid()
    {
      return this->valid;
    }

  private:
   T* stack;
   T* cursor;
   bool valid;
};

#endif
