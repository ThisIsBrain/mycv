#ifndef ARRAY3D_H
#define ARRAY3D_H

#include <stdlib.h>
#include <new>


namespace mycv
	{
	template <typename TYPE>
	class Arr3D
		{
		public:
		Arr3D();
		void init(int a, int b, int c);
		~Arr3D();
		void resize(int a, int b, int c);
		
		TYPE *** value;
		struct Size
			{
			int a;
			int b;
			int c;
			} size;
		};
		
	}

//конструктор
//=======================================
template <typename TYPE>
mycv::Arr3D<TYPE>::Arr3D()
	{
	//пустой массив
	this->value = NULL;
	this->size.a = 0;
	this->size.b = 0;
	this->size.c = 0;
	}
//=======================================

//init
//=======================================	
template <typename TYPE>
void mycv::Arr3D<TYPE>::init(int a, int b, int c)
	{
	this->size.a=0;
	this->size.b=0;
	this->size.c=0;
	
	//выделяем память
	try 
		{
		this->value = new TYPE** [a];
		}
	catch (std::bad_alloc err)
		{
		this->value=NULL;
		return;
		}
		
	for(int i=0; i<a; i++)
		{
		try
			{
			this->value[i] = new TYPE* [b];
			}
		catch (std::bad_alloc err)
			{
			this->value=NULL;
			return;
			}
		
		for(int j=0; j<b; j++)
			{
			try
				{
				this->value[i][j] = new TYPE [c];
				}
			catch (std::bad_alloc err)
				{
				this->value=NULL;
				return;
				}
			}
		}
			
	//запоминаем размеры массива
	this->size.a=a;
	this->size.b=b;
	this->size.c=c;
	}
//=======================================


//деструктор
//=======================================
template <typename TYPE>
mycv::Arr3D<TYPE>::~Arr3D()
	{
	//освобождаем память
	for(int i=0; i<this->size.a; i++)
		{
		for(int j=0; j<this->size.b; j++)
			{
			delete [] this->value[i][j];
			}
		delete [] this->value[i];
		}
	delete [] this->value;
	this->value=NULL;
	}
//=======================================

//изменение размера массива
//(значения будут потеряны)
//=======================================
template <typename TYPE>
void mycv::Arr3D<TYPE>::resize(int a, int b, int c)
	{
	//освобождаем память
	for(int i=0; i<this->size.a; i++)
		{
		for(int j=0; j<this->size.b; j++)
			{
			delete [] this->value[i][j];
			}
		delete [] this->value[i];
		}
	delete [] this->value;
	
	//выделяем память
	try 
		{
		this->value = new TYPE** [a];
		}
	catch (std::bad_alloc err)
		{
		this->value=NULL;
		return;
		}
		
	for(int i=0; i<a; i++)
		{
		try
			{
			this->value[i] = new TYPE* [b];
			}
		catch (std::bad_alloc err)
			{
			this->value=NULL;
			return;
			}
		
		for(int j=0; j<b; j++)
			{
			try
				{
				this->value[i][j] = new TYPE [c];
				}
			catch (std::bad_alloc err)
				{
				this->value=NULL;
				return;
				}
			}
		}
			
	//запоминаем размеры массива
	this->size.a=a;
	this->size.b=b;
	this->size.c=c;
	}
//=======================================


#endif
