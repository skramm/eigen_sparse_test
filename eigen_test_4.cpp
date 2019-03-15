
/**
\file eigen_test_4.cpp
\brief A speed test comparison of a bare eigen sparse matrix and a std::set wrapper


Arguments:
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

// shouldn't change things (but who knows ?)
constexpr int g_vec_size = 10;

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
	std::set<int>          _idx_set;
	Eigen::SparseMatrix<T> _data;

	EigenSMWrapper( int r, int c ): _data(r,c)
	{}

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
		if( it.row() == row )
			return false;
	}
	return true;
}

/// Allocate the data the will be stored randomly in matrix
std::vector<Eigen::Triplet<MyClass>>
createTriplets( size_t mat_dim, size_t nbValues )
{
	std::vector<Eigen::Triplet<MyClass>> tripletList;
	tripletList.reserve( nbValues );

	for( int i=0; i<nbValues; i++ )
	{
		MyClass object{ 5, 1.2 };
		object.v.resize( g_vec_size );

		int r = 1.0*rand()/RAND_MAX * mat_dim; // insert somewhere
		int c = 1.0*rand()/RAND_MAX * mat_dim;

		tripletList.push_back( Eigen::Triplet<MyClass>( r, c, object ) );
	}
	return tripletList;
}

/// see eigen_test_4.cpp
int main( int argc, const char** argv )
{
	std::srand(time(0));
	std::cout << "Eigen version: " << EIGEN_WORLD_VERSION << '.' << EIGEN_MAJOR_VERSION << '.' << EIGEN_MINOR_VERSION << '\n';
	size_t matDim = 1000;
	if( argc>1 )
		matDim = static_cast<size_t>( std::atoi( argv[1] ) );
	std::cout << "- reserve space for a sparse matrix " << matDim << " x " << matDim << '\n';

	size_t nbValues = 10000;
	if( argc>2 )
		nbValues = static_cast<size_t>( std::atoi( argv[2] ) );

	std::cout << "- Nb values stored in matrix = " << nbValues << '\n';
	std::cout << "   (sparsity ratio=" << 100.0*nbValues/matDim/matDim << "%)\n";

	int nbSearches = 100000;
	if( argc>3 )
		nbSearches = std::atoi( argv[3] );

	std::cout << "- Nb searches in matrix = " << nbSearches << '\n';

	Eigen::SparseMatrix<MyClass> mat1(matDim,matDim);
	EigenSMWrapper<MyClass>      mat2(matDim,matDim);

	srand( time(0) );

	std::cout << "\n1 - create Triplets\n";

	Timing timing0;
	auto tripletList = createTriplets( matDim, nbValues );
	timing0.PrintDuration();

	std::cout << "\n2 - fill sparse matrix:\n";

	{
		std::cout << " - direct\n";
		Timing timing;
		mat1.setFromTriplets( tripletList.begin(), tripletList.end() );
		timing.PrintDuration();
	}

	{
		std::cout << " - using wrapper set\n";
		Timing timing;
		mat2.setFromTriplets( tripletList.begin(), tripletList.end() );
		timing.PrintDuration();
	}

	{
		std::cout << "\n3 - searching for " << nbSearches << " values in matrix...\n";
		Timing timing;
		size_t Nb_1 = 0;
		for( int i=0; i<nbSearches; i++ )
		{
			int r = 1.0*rand()/RAND_MAX * matDim;
			int c = 1.0*rand()/RAND_MAX * matDim;
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
			int r = 1.0*rand()/RAND_MAX * matDim;
			int c = 1.0*rand()/RAND_MAX * matDim;
			if( !mat2.isNull( r, c ) )
				Nb_2++;
		}
		std::cout << " - wrapper1 class: nbvalues=" << Nb_2 << "\n";
		timing.PrintDuration();
	}
}



