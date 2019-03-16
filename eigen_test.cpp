
/**
\file eigen_test.cpp
\brief A speed test for Eigen sparse matrices implementation

Arguments:
-# sparsity coeff
*/

#include <eigen3/Eigen/SparseCore>
#include <vector>
#include <iostream>
#include <fstream>
#include <set>
#include "timing.hpp"

int g_tab_val[] = { 1, 2, 5 };
char g_sep = ';';

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

/// Return true if element at \c row, \c col is empty
/**
see http://stackoverflow.com/questions/42053467/
*/
template<typename T>
bool isNull( const Eigen::SparseMatrix<T>& mat, int row, int col )
{
//#ifdef COUNT_SEARCH_LOOPS
	size_t c=0;
//#endif
	for( typename Eigen::SparseMatrix<T>::InnerIterator it(mat, col); it; ++it )
	{
		c++;
		if( it.row() == row )
		{
			std::cout << "*c=*" << c << '\n';
			return false;
		}
	}
	return true;
}

/// Fills the sparse matrix
void
fillMatrix( Eigen::SparseMatrix<MyClass>& mat, size_t matDim, size_t nbValues )
{
	std::vector<Eigen::Triplet<MyClass>> tripletList;
	tripletList.reserve( nbValues );

	for( int i=0; i<nbValues; i++ )
	{
		MyClass object{ 5, 1.2 };
		object.v.resize( g_vec_size );

		int r = 1.0*rand()/RAND_MAX * matDim; // insert somewhere
		int c = 1.0*rand()/RAND_MAX * matDim;

		tripletList.push_back( Eigen::Triplet<MyClass>( r, c, object ) );
	}
}

size_t
searchMatrix( const Eigen::SparseMatrix<MyClass>& mat, size_t matDim, size_t nbSearches )
{
	size_t Nb = 0;
	for( int i=0; i<nbSearches; i++ )
	{
		int r = 1.0*rand()/RAND_MAX * matDim;
		int c = 1.0*rand()/RAND_MAX * matDim;
		if( !isNull( mat, r, c ) )
			Nb++;
	}
	return Nb;
}


/// see eigen_test.cpp
/**
arg: sparsity coeff, expressed in %: "0.1" means that if we have 1M values (1000x1000) then we will have 0.001*1M = 1000 values stored in the matrix
*/
int main( int argc, const char** argv )
{
	int nbStepsSearch = 7;
	int nbStepsMatSize = 8;

	std::srand(time(0));
	std::cout << "# Eigen version: " << EIGEN_WORLD_VERSION << '.' << EIGEN_MAJOR_VERSION << '.' << EIGEN_MINOR_VERSION << '\n';

	double sparsity = 0.1;
	if( argc>1 )
		sparsity = std::atof( argv[1] );


	std::ofstream fout( "data.dat" );
	assert( fout.is_open() );

	fout << "# i;matDim;nbValues;fill_duration;j;nbSearches;search_duration;nb values found\n";
	fout << "# sparsity = " << sparsity << "%\n";

	size_t pow1 = 100;
	for( auto j=0; j<nbStepsMatSize; j++ )
	{
		if( !(j%3) )
			pow1 *= 10;
		size_t matDim = g_tab_val[j%3] * pow1;
		size_t nbValues = sparsity/100.0 * matDim * matDim;

		Eigen::SparseMatrix<MyClass> mat(matDim,matDim);

		std::cout << j << ": matDim=" << matDim << 'x' << matDim << ", nb values=" << nbValues;

		Timing timing1;
		fillMatrix( mat, matDim, nbValues );
		auto durFill = timing1.getDuration();
		std::cout << ", durFill=" << durFill << " ms\n";
		size_t pow2 = 1000;
		for( auto i=0; i<nbStepsSearch; i++ )
		{
			if( !(i%3) )
				pow2 *= 10;
			size_t nbSearches = g_tab_val[i%3] * pow2;
			Timing timing2;
			auto n = searchMatrix( mat, matDim, nbSearches );
			fout << j << g_sep << matDim << g_sep << nbValues << g_sep << durFill << g_sep << i << g_sep << nbSearches << g_sep << timing2.getDuration() << g_sep << n << '\n';
		}
		fout << std::endl;

	}
}



