#include <iostream>
#include <string.h>
#include <sys/time.h> 
#include <stdlib.h>
#include <arm_neon.h>
#include <bmp.h>
#include <math.h>
#include <Core>
#include <Dense>
#include <IterativeLinearSolvers>
using namespace std;
using namespace Eigen;
void example1_create_matrixDouble2x2()
{
  MatrixXd m(2,2);
  m(0,0) = 3;
  m(1,0) = 2.5;
  m(0,1) = -1;
  m(1,1) = m(1,0) + m(0,1);
  std::cout << m << std::endl;
}
void example2_multiplication_two_matrixDouble2x2()
{
  MatrixXd m = MatrixXd::Random(3,3);
  m = (m + MatrixXd::Constant(3,3,1.2)) * 50;
  cout << "m =" << endl << m << endl;
  VectorXd v(3);
  v << 1, 2, 3;
  cout << "m * v =" << endl << m * v << endl;
}
void example3_add_sub_two_matrixDouble2x2()
{
  Matrix2d a;
  a << 1, 2,
       3, 4;
  MatrixXd b(2,2);
  b << 2, 3,
       1, 4;
  std::cout << "a + b =\n" << a + b << std::endl;
  std::cout << "a - b =\n" << a - b << std::endl;
  std::cout << "Doing a += b;" << std::endl;
  a += b;
  std::cout << "Now a =\n" << a << std::endl;
  Vector3d v(1,2,3);
  Vector3d w(1,0,0);
  std::cout << "-v + w - v =\n" << -v + w - v << std::endl;
}
void example4_dot_cross_two_VectorDouble3x3()
{
  Vector3d v(1,2,3);
  Vector3d w(0,1,2);
  cout << "Dot product: " << v.dot(w) << endl;
  double dp = v.adjoint()*w; // automatic conversion of the inner product to a scalar
  cout << "Dot product via a matrix product: " << dp << endl;
  cout << "Cross product:\n" << v.cross(w) << endl;
}
int main(int argc,char *argv[])
{
    cout<< "run <example1_create_matrixDouble2x2> "<< endl;
    example1_create_matrixDouble2x2();
    cout<< "run <example2_multiplication_two_matrixDouble2x2> "<< endl;
    example2_multiplication_two_matrixDouble2x2();    
    cout<< "run <example3_add_sub_two_matrixDouble2x2> "<< endl;
    example3_add_sub_two_matrixDouble2x2();   
    cout<< "run <example4_dot_cross_two_VectorDouble3x3> "<< endl;
    example4_dot_cross_two_VectorDouble3x3();  
    return 0;
}


