#ifndef ARRAY2D_H
#define ARRAY2D_H

#include <stdlib.h>
#include <new>


namespace mycv
	{
	template <typename TYPE>
	class Arr2D
		{
		public:
		Arr2D();
		void init(int a, int b);
		~Arr2D();
		void resize(int a, int b);
		//void zero();
		
		TYPE ** value;
		struct Size
			{
			int a;
			int b;
			} size;
		};
		
	}

//конструктор
//=======================================
template <typename TYPE>
mycv::Arr2D<TYPE>::Arr2D()
	{
	//пустой массив
	this->value = NULL;
	this->size.a = 0;
	this->size.b = 0;
	}
//=======================================

//init
//=======================================	
template <typename TYPE>
void mycv::Arr2D<TYPE>::init(int a, int b)
	{
	this->size.a=0;
	this->size.b=0;
	
	//выделяем память
	try 
		{
		this->value = new TYPE* [a];
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
			this->value[i] = new TYPE [b];
			}
		catch (std::bad_alloc err)
			{
			this->value=NULL;
			}
		}
			
	//запоминаем размеры массива
	this->size.a=a;
	this->size.b=b;
	}
//=======================================

//деструктор
//=======================================
template <typename TYPE>
mycv::Arr2D<TYPE>::~Arr2D()
	{
	//освобождаем память
	for(int i=0; i<this->size.a; i++)
		{
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
void mycv::Arr2D<TYPE>::resize(int a, int b)
	{
	//освобождаем память
	for(int i=0; i<this->size.a; i++)
		{
		delete [] this->value[i];
		}
	delete [] this->value;
	
	//выделяем память
	try 
		{
		this->value = new TYPE* [a];
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
			this->value[i] = new TYPE [b];
			}
		catch (std::bad_alloc err)
			{
			this->value=NULL;
			}
		}
			
	//запоминаем размеры массива
	this->size.a=a;
	this->size.b=b;
	}
//=======================================

////обнуление содержимого
////=======================================
//template <typename TYPE>
//void mycv::Arr2D<TYPE>::zero()
//	{
//	for(int a=0; a<this->size.a; a++)
//		{
//		for(int b=0; b<this->size.b; b++)
//			{
//			this->value[a][b]=0;
//			}
//		}
//	}
////=======================================

#endif
