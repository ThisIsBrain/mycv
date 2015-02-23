#ifndef ARRAY1D_H
#define ARRAY1D_H

#include <stdlib.h>
#include <new>


namespace mycv
	{
	template <typename TYPE>
	class Arr1D
		{
		public:
		Arr1D();
		void init(int a);
		~Arr1D();
		void resize(int a);
		//void zero();
		
		TYPE * value;
		struct Size
			{
			int a;
			} size;
		};
		
	}

//конструктор
//=======================================
template <typename TYPE>
mycv::Arr1D<TYPE>::Arr1D()
	{
	//пустой массив
	this->value = NULL;
	this->size.a = 0;
	}
//=======================================

//init
//=======================================	
template <typename TYPE>
void mycv::Arr1D<TYPE>::init(int a)
	{
	this->size.a=0;
	
	//выделяем память
	try 
		{
		this->value = new TYPE [a];
		}
	catch (std::bad_alloc err)
		{
		this->value=NULL;
		return;
		}
			
	//запоминаем размеры массива
	this->size.a=a;
	}
//=======================================

//деструктор
//=======================================
template <typename TYPE>
mycv::Arr1D<TYPE>::~Arr1D()
	{
	//освобождаем память
	delete [] this->value;
	this->value=NULL;
	}
//=======================================

//изменение размера массива
//(значения будут потеряны)
//=======================================
template <typename TYPE>
void mycv::Arr1D<TYPE>::resize(int a)
	{
	//освобождаем память
	delete [] this->value;
	
	//выделяем память
	try 
		{
		this->value = new TYPE [a];
		}
	catch (std::bad_alloc err)
		{
		this->value=NULL;
		return;
		}
		
	//запоминаем размеры массива
	this->size.a=a;
	}
//=======================================


#endif
