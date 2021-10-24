#include <iostream>
#include <array>
#include <vector>
#include <functional>
#include <utility>
#define LOG_MESSAGE(tag,mes) std::cout<<"["<<tag<<"]: "<<mes<<std::endl;
namespace MonMatrix
{
    typedef struct demReturn_t
    {
        unsigned int row;
        unsigned int col;
    }demReturn;
    class Matrix_D
    {
        public: double *data;
                unsigned int row;
                unsigned int col;
        public: Matrix_D(unsigned int setRow,unsigned int setCol):row(setRow),col(setCol){data=new double[row*col];LOG_MESSAGE("Matrix_D","create");}
        public: Matrix_D():row(2),col(2){data=new double[row*col];LOG_MESSAGE("Matrix_D","create");}
        public: std::array<unsigned int,2> getDemention_array(){
            return std::array<unsigned int,2>{row,col};
        }
        public: std::vector<unsigned int> getDemention_vector(){
            std::vector<unsigned int> ret_vector;
            ret_vector.reserve(2);
            ret_vector.push_back(row);
            ret_vector.push_back(col);
            return ret_vector;
        }
        public: std::pair<unsigned int,unsigned int> getDemention_pair(){
            return std::pair<unsigned int,unsigned int>{row,col};
        }
        public: std::tuple<unsigned int,unsigned int> getDemention_tuple(){
            return std::make_tuple(row,col);
        }
        public: demReturn getDemention_struct(){
            return demReturn{.row=row,.col=col};
        }
        public: ~Matrix_D(){
            delete[] data;
            LOG_MESSAGE("Matrix_D","destory");
        }
    };
};
int main()
{
    {
        MonMatrix::Matrix_D mat_d(5,4);
        auto dem2= mat_d.getDemention_array();
        LOG_MESSAGE("getDemention_array row is ",dem2[0]);
        LOG_MESSAGE("getDemention_array col is ",dem2[1]);
    }
    {
        MonMatrix::Matrix_D mat_d(6,9);
        auto dem2= mat_d.getDemention_vector();
        LOG_MESSAGE("getDemention_vector mat_d row is ",dem2.data()[0]);
        LOG_MESSAGE("getDemention_vector mat_d col is ",dem2.data()[1]);
    }    
    {
        MonMatrix::Matrix_D mat_d(7,5);
        auto dem2= mat_d.getDemention_pair();
        LOG_MESSAGE("getDemention_pair mat_d row is ",dem2.first);
        LOG_MESSAGE("getDemention_pair mat_d col is ",dem2.second );
    }    
    {
        MonMatrix::Matrix_D mat_d(3,4);
        auto dem2= mat_d.getDemention_tuple();
        LOG_MESSAGE("getDemention_tuple mat_d row is ",std::get<0>(dem2));
        LOG_MESSAGE("getDemention_tuple mat_d col is ",std::get<1>(dem2));
    }    
    {
        MonMatrix::Matrix_D mat_d(1,3);
        auto dem2= mat_d.getDemention_struct();
        LOG_MESSAGE("getDemention_struct mat_d row is ",dem2.row);
        LOG_MESSAGE("getDemention_struct mat_d col is ",dem2.col);
    }    
    return 0;
}