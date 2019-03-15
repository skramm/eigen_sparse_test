/**
\file eigen_test_1.cpp
\brief A test of using sparse matrix with Eigen3
*/


#include <eigen3/Eigen/SparseCore>
#include <vector>
#include <iostream>

/// Dummy class, used in eigen_test_1.cpp only
struct MyClass
{
	int a;
	float b;
	std::vector<int> v;

	MyClass(){}
	MyClass( int aa, float bb ) : a(aa), b(bb) {}
	MyClass( int aa): a(aa) {}
	MyClass( const MyClass& other ) // copy constructor
	{
		a = other.a;
		b = other.b;
		v = other.v;
	}
	MyClass& operator=( int x )
	{
		assert( x==0 );
		return *this;
	}

	MyClass& operator += ( const MyClass& x )
	{
		return *this;
	}
/// operator for a = b + c
	const MyClass& operator + ( const MyClass& c ) const
	{
		return *this;
	}
};

/// Print an eigen sparse matrix content
void PrintMat( const Eigen::SparseMatrix<MyClass>& mat )
{
	std::cout << "Matrix content:\n";
	for (int k=0; k<mat.outerSize(); ++k )
		for( Eigen::SparseMatrix<MyClass>::InnerIterator it(mat,k); it; ++it )
			std::cout << "row=" << it.row() << " col=" << it.col()
				<< ": a=" << it.value().a
				<< " b=" << it.value().b
				<< " vect size=" << it.value().v.size() << "\n";
}

/// see eigen_test_1.cpp
int main( int argc, const char** argv )
{
	std::cout << "Eigen version: " << EIGEN_WORLD_VERSION << '.' << EIGEN_MAJOR_VERSION << '.' << EIGEN_MINOR_VERSION << '\n';

	int n = 1000;
	if( argc>1 )
		n=std::atoi( argv[1]);
	std::cout << "-reserve space for a sparse matrix " << n << " x " << n << '\n';
	Eigen::SparseMatrix<MyClass> mat(n,n); // 1000000 positions

	MyClass a{ 5, 1.2 };
	a.v.resize(5);
	mat.insert( 3, 4 ) = a;   // insert single element
	PrintMat( mat );

	MyClass b{ 6, 2.3 };
	b.v.resize(9);
	mat.coeffRef( 3, 4 ) = b; // update single element
	PrintMat( mat );


	std::vector<Eigen::Triplet<MyClass>> tripletList;
	for(int i=0; i<10; i++)
	{
		MyClass a{i*2,i*3.0f};
		tripletList.push_back( Eigen::Triplet<MyClass>(i,i*10,a) );
	}
	mat.setFromTriplets( tripletList.begin(), tripletList.end() );
	PrintMat( mat );

	int row = 3;
	int col = 4;
	std::cout << "Get elem at (" << row << ',' << col << "): \n";
	MyClass  get1 = mat.coeff( 3, 2 );
	const MyClass& get2 = mat.coeffRef( 3, 2 );
/*	if( get1 == MyClass() )
		std:: cout << "none\n";
	else*/
	std:: cout << "value1 = " << get1.a << std::endl;
	std:: cout << "value2 = " << get2.a << std::endl;

	Eigen::SparseMatrix<MyClass> mat2 = mat;
	Eigen::SparseMatrix<MyClass> mat3;
	mat3 = mat;
	std:: cout << "value3 = " << mat3.coeffRef( 3, 2 ).a << std::endl;
//	std::cin.ignore();
}

