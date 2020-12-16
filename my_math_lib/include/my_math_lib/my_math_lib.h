
#include "ros/ros.h"
#include "iostream"
using namespace std;


namespace my_math_lib{

template <typename T>
	class Complex{

	    friend ostream& operator<<(ostream &out, Complex &c)
	    {
		out<<c.a << " + " << c.b << "i";
		return out;
	    }
	private:
		T a;
		T b;

	public:

		Complex(T a, T b)
		{
		    this->a = a;
		    this->b = b;
		}
		
	
		Complex<T> operator+(Complex &c)
		{
		    Complex<T> tmp(this->a+c.a, this->b+c.b);
		    return tmp;
		}		  
	};
	


}// namespace my_math_lib
