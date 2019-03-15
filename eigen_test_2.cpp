
/**
\file eigen_test_2.cpp
\brief A speed test comparison of a bare eigen sparse matrix and a wrapper, for a predicate if values are present or not.

arguments:
-# size of matrix n (matrix will be n x n ). Default is 1000
-# nb of non-null values in the matrix. Default is
-# nb of searches performed
*/

#include <eigen3/Eigen/SparseCore>
#include <vector>
#include <iostream>
#include <set>
#include "timing.hpp"

typedef std::chrono::high_resolution_clock MyClock;
typedef std::chrono::time_point<MyClock> MyTimePoint;
typedef std::chrono::duration<MyTimePoint::rep,MyTimePoint::period> MyDuration;


/// the object stored inside
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

/// a wrapper over Eigen Sparse Matrix, adds a std::set of linearized positions where the non-null values are
template<typename T>
struct EigenSMWrapper
{
	Eigen::SparseMatrix<T> _data;
	std::set<int>          _idx_set;

	EigenSMWrapper( int r, int c ): _data(r,c)
	{}

	void insert( int r, int c, const T& t )
	{
		_data.insert( r, c ) = t;
		_idx_set.insert( r * _data.cols() + c );
	}
	bool isNull( int r, int c ) const
	{
		int idx = r * _data.cols() + c;
		if( _idx_set.find( idx ) == _idx_set.cend() )
			return true;
		return false;
	}
	template<typename InputIterators>
	void setFromTriplets( const InputIterators& ib, const InputIterators& ie )
	{
		_data.setFromTriplets( ib, ie );
		for( auto it = ib;it != ie; ++it )
			_idx_set.insert( it->row() * _data.cols() + it->col() );
	}
};

/// Return true if element at \c row, \c col is empty
/**
see http://stackoverflow.com/questions/42053467/
*/
template<typename T>
bool isNull( const Eigen::SparseMatrix<T>& mat, int row, int col )
{
    for( typename Eigen::SparseMatrix<T>::InnerIterator it(mat, col); it; ++it )
    {
        if (it.row() == row)
			return false;
    }
    return true;
}



/// see eigen_test_2.cpp
int main( int argc, const char** argv )
{
	std::cout << "Eigen version: " << EIGEN_WORLD_VERSION << '.' << EIGEN_MAJOR_VERSION << '.' << EIGEN_MINOR_VERSION << '\n';
	int mat_dim = 1000;
	if( argc>1 )
		mat_dim = std::atoi( argv[1] );
	std::cout << "- reserve space for a sparse matrix " << mat_dim << " x " << mat_dim << '\n';

	int nbValues = 10000;
	if( argc>2 )
		nbValues = std::atoi( argv[2] );

	std::cout << "- Nb values stored in matrix = " << nbValues << '\n';

	int nbSearches = 100000;
	if( argc>3 )
		nbSearches = std::atoi( argv[3] );

	std::cout << "- Nb searches in matrix = " << nbSearches << '\n';

	Eigen::SparseMatrix<MyClass> mat1(mat_dim,mat_dim);
	EigenSMWrapper<MyClass>      mat2(mat_dim,mat_dim);

	srand( time(0) );

	std::cout << "1 - fill sparse matrix:\n";

	{
		std::cout << " - direct\n";
		Timing timing;
		std::vector<Eigen::Triplet<MyClass>> tripletList;
		tripletList.reserve( nbValues );

		for( int i=0; i<nbValues; i++ )
		{
			MyClass object{ 5, 1.2 };
			object.v.resize(5);

			int r = 1.0*rand()/RAND_MAX * mat_dim; // insert somewhere
			int c = 1.0*rand()/RAND_MAX * mat_dim;

			tripletList.push_back( Eigen::Triplet<MyClass>( r, c, object ) );
		}
		mat1.setFromTriplets( tripletList.begin(), tripletList.end() );
		timing.PrintDuration();
	}

	{
		std::cout << " - using wrapper\n";
		Timing timing;
		std::vector<Eigen::Triplet<MyClass>> tripletList;
		tripletList.reserve( nbValues );

		for( int i=0; i<nbValues; i++ )
		{
			MyClass object{ 5, 1.2 };
			object.v.resize(5);

			int r = 1.0*rand()/RAND_MAX * mat_dim;
			int c = 1.0*rand()/RAND_MAX * mat_dim;

			tripletList.push_back( Eigen::Triplet<MyClass>( r, c, object ) );
		}
		mat2.setFromTriplets( tripletList.begin(), tripletList.end() );
		timing.PrintDuration();
	}


	{
		std::cout << "- searching for " << nbSearches << " values in matrix...\n";
		Timing timing;
		size_t Nb_1 = 0;
		for( int i=0; i<nbSearches; i++ )
		{
			int r = 1.0*rand()/RAND_MAX * mat_dim;
			int c = 1.0*rand()/RAND_MAX * mat_dim;
			if( !isNull( mat1, r, c ) )
				Nb_1++;
		}
		std::cout << "  Results:\n - direct eigen matrix: nbvalues=" << Nb_1 << "\n";
		timing.PrintDuration();
	}
	{
		Timing timing;
		size_t Nb_2 = 0;
		for( int i=0; i<nbSearches; i++ )
		{
			int r = 1.0*rand()/RAND_MAX * mat_dim;
			int c = 1.0*rand()/RAND_MAX * mat_dim;
			if( !mat2.isNull( r, c ) )
				Nb_2++;
		}
		std::cout << " - wrapper mclass: nbvalues=" << Nb_2 << "\n";
		timing.PrintDuration();
	}
}



