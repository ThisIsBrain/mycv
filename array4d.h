#ifndef ARRAY4D_H
#define ARRAY4D_H

#include <stdlib.h>
#include <new>


namespace mycv
	{
	template <typename TYPE>
	class Arr4D
		{
		public:
		Arr4D();
		void init(int a, int b, int c, int d);
		~Arr4D();
		void resize(int a, int b, int c, int d);
		
		TYPE **** value;
		struct Size
			{
			int a;
			int b;
			int c;
			int d;
			} size;
		};
		
	}

//конструктор
//=======================================
template <typename TYPE>
mycv::Arr4D<TYPE>::Arr4D()
	{
	//пустой массив
	this->value = NULL;
	this->size.a = 0;
	this->size.b = 0;
	this->size.c = 0;
	this->size.d = 0;
	}
//=======================================

//init
//=======================================	
template <typename TYPE>
void mycv::Arr4D<TYPE>::init(int a, int b, int c, int d)
	{
	this->size.a=0;
	this->size.b=0;
	this->size.c=0;
	this->size.d=0;
	
	//выделяем память

	this->value = new TYPE*** [a];		
	for(int i=0; i<a; i++)
		{
		this->value[i] = new TYPE** [b];
		for(int j=0; j<b; j++)
			{
			this->value[i][j] = new TYPE* [c];
			for(int k=0; k<c; k++)
				{
				this->value[i][j][k] = new TYPE [d];
				}
			}
		}
			
	//запоминаем размеры массива
	this->size.a=a;
	this->size.b=b;
	this->size.c=c;
	this->size.d=d;
	}
//=======================================


//деструктор
//=======================================
template <typename TYPE>
mycv::Arr4D<TYPE>::~Arr4D()
	{
	//освобождаем память
	for(int i=0; i<this->size.a; i++)
		{
		for(int j=0; j<this->size.b; j++)
			{
			for(int k=0; k<this->size.c; k++)
				{
				delete [] this->value[i][j][k];
				}
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
void mycv::Arr4D<TYPE>::resize(int a, int b, int c, int d)
	{
	//освобождаем память
	for(int i=0; i<this->size.a; i++)
		{
		for(int j=0; j<this->size.b; j++)
			{
			for(int k=0; k<this->size.c; k++)
				{
				delete [] this->value[i][j][k];
				}
			delete [] this->value[i][j];
			}
		delete [] this->value[i];
		}
	delete [] this->value;
	
	//выделяем память
	this->value = new TYPE*** [a];		
	for(int i=0; i<a; i++)
		{
		this->value[i] = new TYPE** [b];
		for(int j=0; j<b; j++)
			{
			this->value[i][j] = new TYPE* [c];
			for(int k=0; k<c; c++)
				{
				this->value[i][j][k] = new TYPE [d];
				}
			}
		}
			
	//запоминаем размеры массива
	this->size.a=a;
	this->size.b=b;
	this->size.c=c;
	this->size.d=d;
	}
//=======================================


#endif
