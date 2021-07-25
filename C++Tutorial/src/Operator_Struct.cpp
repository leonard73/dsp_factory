/* Public --- struct */
#include <iostream>
#include <string.h>
#define LOG_MESSAGE(tag,mes) std::cout<<"["<<tag<<"]: "<<mes<<std::endl;
typedef struct MonMatrix_UShort_t
{
    unsigned short *data;
    unsigned int rows,cols;
    MonMatrix_UShort_t(): rows(0),cols(0){LOG_MESSAGE("MonMatrix_UShort_t","create");data=new unsigned short;}
    MonMatrix_UShort_t(unsigned int setRows,unsigned int setCols): rows(setRows),cols(setCols)
    {
        data=new unsigned short [rows*cols];
        LOG_MESSAGE("MonMatrix_UShort_t","create with rows and cols");
    }
    MonMatrix_UShort_t(unsigned int setRows,unsigned int setCols,unsigned short *setData): rows(setRows),cols(setCols)
    {
        data=new unsigned short [rows*cols];
        LOG_MESSAGE("MonMatrix_UShort_t","create with rows and cols");
        memcpy(data,setData,setRows*setCols*sizeof(setData[0]));
    }
    ~MonMatrix_UShort_t()
    {
        delete[] data;
        LOG_MESSAGE("MonMatrix_UShort_t","destory");    
    }
    MonMatrix_UShort_t ADD(const MonMatrix_UShort_t & secondMatrix)
    {
        if(secondMatrix.cols!=cols || secondMatrix.rows!=rows) 
        {
            return MonMatrix_UShort_t(rows,cols,data);
        }
        else
        {
            MonMatrix_UShort_t retMatrix(rows,cols,data);
            for(unsigned int i=0;i<rows*cols;i++)
            {
                retMatrix.data[i] = data[i] + secondMatrix.data[i];
            } 
            return MonMatrix_UShort_t(retMatrix.rows,retMatrix.cols,retMatrix.data);
        }
    }
    MonMatrix_UShort_t MINUS(const MonMatrix_UShort_t & secondMatrix)
    {
        if(secondMatrix.cols!=cols || secondMatrix.rows!=rows) 
        {
            return MonMatrix_UShort_t(rows,cols,data);
        }
        else
        {
            MonMatrix_UShort_t retMatrix(rows,cols,data);
            for(unsigned int i=0;i<rows*cols;i++)
            {
                retMatrix.data[i] = data[i] - secondMatrix.data[i];
            } 
            return MonMatrix_UShort_t(retMatrix.rows,retMatrix.cols,retMatrix.data);
        }
    }
    MonMatrix_UShort_t MUL(const MonMatrix_UShort_t & secondMatrix)
    {
        if(secondMatrix.cols!=cols || secondMatrix.rows!=rows) 
        {
            return MonMatrix_UShort_t(rows,cols,data);
        }
        else
        {
            MonMatrix_UShort_t retMatrix(rows,cols,data);
            for(unsigned int i=0;i<rows*cols;i++)
            {
                retMatrix.data[i] = data[i] * secondMatrix.data[i];
            } 
            return MonMatrix_UShort_t(retMatrix.rows,retMatrix.cols,retMatrix.data);
        }
    }
    void SHOW_2D()
    {
        std::cout<<std::endl;
        std::cout<<"Matrix Rows = "<<rows<<" Cols = "<<cols<<std::endl;
        for(int i=0;i<rows;i++)
        {
            std::cout<<"  ";
            for(int j=0;j<cols;j++)
            {
                std::cout<<" "<< data[rows*i+j] << " ,";
            }
            std::cout<<std::endl;
        }
        std::cout<<std::endl;
    }
    MonMatrix_UShort_t operator+(const MonMatrix_UShort_t & secondMatrix)
    {
        return (*this).ADD(secondMatrix);
    }
    MonMatrix_UShort_t operator-(const MonMatrix_UShort_t & secondMatrix)
    {
        return (*this).MINUS(secondMatrix);
    }
    MonMatrix_UShort_t operator*(const MonMatrix_UShort_t & secondMatrix)
    {
        return (*this).MUL(secondMatrix);
    }
}MonMatrix_UShort;
int main()
{
    LOG_MESSAGE("Create Struct and use Operator"," ");
    MonMatrix_UShort matrix_obj1(3,3);
    MonMatrix_UShort matrix_obj2(3,3);
    for(int i=0;i<9;i++)
    {
        matrix_obj1.data[i] = i*i+2*i+1;
        matrix_obj2.data[i] = 4*i-2;
    }
    MonMatrix_UShort matrix_obj3 = (matrix_obj1+matrix_obj2)*matrix_obj1-matrix_obj2;
    matrix_obj1.SHOW_2D();
    matrix_obj2.SHOW_2D();
    matrix_obj3.SHOW_2D();
    return 0;
}