#ifndef ESRIGRIDCLASS_H
#define ESRIGRIDCLASS_H

#include "esrigridclass.h"
#include "pubtools.h"
#include <typeinfo>
#include <string.h>
#include "stdio.h"
#include "stdlib.h"
#if __GNUC__
#include <cxxabi.h>
#endif
template <class Grid_Type> class EsriGridClass
{
    bool Mem_Allocated;
    bool bInt;  //true if integer
    int ncols;       //columns
    int nrows;       //rows
    double xllcorner;
    double yllcorner;
    double cellsize;
    int num_valid_cells;
    Grid_Type **value;
    Grid_Type novalue;
public:
    EsriGridClass();
    ~EsriGridClass();
    int getNcols();
    int getNrows();
    double getXll();
    double getYll();
    double getCellsize();
    double getGridXll(int col);
    double getGridYll(int row);
    bool getBInt();
    Grid_Type **getValueArray();
    void setNcols(int X);
    void setNrows(int X);
    void setXll(double X);
    void setYll(double X);
    void setCellsize(double X);
    void setValue(int row,int col,Grid_Type X);
    void setNodataValue(Grid_Type X);
    Grid_Type getValue(int row,int col);
    Grid_Type getNodataValue();
    Grid_Type getMinValue();
    Grid_Type getMaxValue();
    bool IsValidCell(int row,int col);
    bool IsWithinGridWindow(int row,int col);                                    //150526
    int getCellNumWithinRange(Grid_Type minv,Grid_Type maxv);
    int getCellNumValid();
    void AllocateMem();
    void InitAllNovalue();
    bool readAsciiGridFile(std::string filename);
    void writeAsciiGridFile(std::string filename);  //implement later M.Liu
    void CopyHeadInformation(EsriGridClass<Grid_Type> &from);
};
//______________________________________________________________________________
template <class Grid_Type> EsriGridClass<Grid_Type>::EsriGridClass()
{
    Mem_Allocated = 0;
#if __GNUC__
    int status;
    const std::type_info &ti = typeid(Grid_Type);
    char *realname = abi::__cxa_demangle(ti.name(), 0, 0, &status);
    if (strcmp(realname,"int") == 0) bInt = true;
    else bInt = false;
    free(realname);
#else
    if (strcmp(typeid(Grid_Type).name(),"int") == 0) bInt = true;
    else bInt = false;
#endif
};
//______________________________________________________________________________
template <class Grid_Type> int EsriGridClass<Grid_Type>::getNcols()
{
    return ncols;
};
template <class Grid_Type> int EsriGridClass<Grid_Type>::getNrows()
{
    return nrows;
};
template <class Grid_Type> double EsriGridClass<Grid_Type>::getXll()
{
    return xllcorner;
};
template <class Grid_Type> double EsriGridClass<Grid_Type>::getYll()
{
    return yllcorner;
};
template <class Grid_Type> double EsriGridClass<Grid_Type>::getCellsize()
{
    return cellsize;
};
//______________________________________________________________________________
template <class Grid_Type> double EsriGridClass<Grid_Type>::getGridXll(int col)
{
    //col: from left to right
    double xll;
    if (col>=ncols) nrerror("EsriGridCLass::getGridXll:outof grid range");
    xll = xllcorner + col * cellsize;
    return xll;
};
//______________________________________________________________________________
template <class Grid_Type> double EsriGridClass<Grid_Type>::getGridYll(int row)
{
    //row: from upper to lower
    double yll;
    if (row>=nrows) nrerror("EsriGridCLass::getGridYll:outof grid range");
    yll = yllcorner + (nrows - 1 - row) * cellsize;
    return yll;
};
//______________________________________________________________________________
template <class Grid_Type> Grid_Type EsriGridClass<Grid_Type>::getValue(int row,int col)
{
    if ((row>=nrows) || (col>=ncols)) nrerror("EsriGridCLass::getValue:outof grid range");
    return value[row][col];
};
//______________________________________________________________________________
template <class Grid_Type> Grid_Type EsriGridClass<Grid_Type>::getNodataValue()
{
    return novalue;
};
//______________________________________________________________________________
template <class Grid_Type> void EsriGridClass<Grid_Type>::setNcols(int X) {ncols = X;}
template <class Grid_Type> void EsriGridClass<Grid_Type>::setNrows(int X) {nrows = X;}
template <class Grid_Type> void EsriGridClass<Grid_Type>::setXll(double X) {xllcorner = X;}
template <class Grid_Type> void EsriGridClass<Grid_Type>::setYll(double X) {yllcorner = X;}
template <class Grid_Type> void EsriGridClass<Grid_Type>::setCellsize(double X) {cellsize = X;}
template <class Grid_Type> bool EsriGridClass<Grid_Type>::getBInt() {return bInt;}
template <class Grid_Type> void EsriGridClass<Grid_Type>::setValue(int row,int col,Grid_Type X) {value[row][col] = X;}
template <class Grid_Type> void EsriGridClass<Grid_Type>::setNodataValue(Grid_Type X) {novalue = X;}
template <class Grid_Type> bool EsriGridClass<Grid_Type>::readAsciiGridFile(std::string filename)
//______________________________________________________________________________
{
    FILE *fgrid;
    char errormessage[100];
    char ctmp[200];
    int totalvalidcells = 0;
    if ((fgrid = fopen(filename.c_str(),"r")) == NULL) {
        sprintf(errormessage,"Cannot open gridfile:%s",filename.c_str());
        std::cerr << "Warning:" << errormessage << std::endl;
        return false;
    }
    fseek(fgrid,0,SEEK_SET);
    fscanf(fgrid,"%s %d",ctmp,&ncols);
    //fgets(ctmp,50,fgrid);
    fscanf(fgrid,"%s %d",ctmp,&nrows);
    //fgets(ctmp,50,fgrid);
    fscanf(fgrid,"%s %lf",ctmp,&xllcorner);
    //fgets(ctmp,50,fgrid);
    fscanf(fgrid,"%s %lf",ctmp,&yllcorner);
    //fgets(ctmp,50,fgrid);
    fscanf(fgrid,"%s %lf",ctmp,&cellsize);
    //fgets(ctmp,50,fgrid);
    if (bInt == true) fscanf(fgrid,"%s %d",ctmp,&novalue);
    else fscanf(fgrid,"%s %f",ctmp,&novalue);
    //fgets(ctmp,50,fgrid);
    AllocateMem();
    for (int i = 0; i < nrows; i++) {
        for (int j = 0; j < ncols; j++) {
            if (bInt) fscanf(fgrid,"%d",&value[i][j]);
            else fscanf(fgrid,"%f",&value[i][j]);
            if (value[i][j] != novalue) totalvalidcells++;
        }
        //fgets(ctmp,50,fgrid);
    }
    fclose(fgrid);
    num_valid_cells = totalvalidcells;
    return true;
}
//______________________________________________________________________________
template <class Grid_Type> int EsriGridClass<Grid_Type>::getCellNumWithinRange(Grid_Type minv,Grid_Type maxv)
{
    //minv<=grid_value<=maxv
    int totalnum = 0;
    for (int i = 0; i < nrows; i++) {
        for (int j = 0; j < ncols; j++) {
            if ((value[i][j] >= minv) && (value[i][j] <= maxv)) totalnum++;
        }
    }
    return totalnum;
}
//______________________________________________________________________________
template <class Grid_Type> Grid_Type EsriGridClass<Grid_Type>::getMinValue()
{
    //minv<=grid_value<=maxv
    Grid_Type minv = 999999;
    for (int i = 0; i < nrows; i++) {
        for (int j = 0; j < ncols; j++) {
            if (value[i][j] <= minv) minv = value[i][j];
        }
    }
    return minv;
}
//______________________________________________________________________________
template <class Grid_Type> Grid_Type EsriGridClass<Grid_Type>::getMaxValue()
{
    //minv<=grid_value<=maxv
    Grid_Type maxv = -999999;
    for (int i = 0; i < nrows; i++) {
        for (int j = 0; j < ncols; j++) {
            if (value[i][j] > maxv) maxv = value[i][j];
        }
    }
    return maxv;
}
//______________________________________________________________________________
template <class Grid_Type> int EsriGridClass<Grid_Type>::getCellNumValid()
{
    //minv<=grid_value<=maxv
    int totalnum = 0;
    for (int i = 0; i < nrows; i++) {
        for (int j = 0; j < ncols; j++) {
            if ((value[i][j] != novalue)) totalnum++;
        }
    }
    return totalnum;
}
//______________________________________________________________________________
template <class Grid_Type> void EsriGridClass<Grid_Type>::AllocateMem()
{
    if (!Mem_Allocated) {
        value = alloc_2d_array<Grid_Type>(nrows,ncols,"EsriGridClass");
        //value = new Grid_Type *[nrows];
        //for (int i = 0; i < nrows; i++) {
        //    value[i] = new Grid_Type[ncols];
        //}
        Mem_Allocated = true;
    }
}
//______________________________________________________________________________
template <class Grid_Type> EsriGridClass<Grid_Type>::~EsriGridClass()
{
#ifdef Destruct_Monitor
    //std::cout<<"~EsriGridClass:"<<std::endl;
#endif
    if (Mem_Allocated) {
        delete_2d_array<Grid_Type>(value,nrows);
    //    for (int i = 0; i < nrows; i++) {
    //        free(value[i]);
    //    }
    //    free(value);
    }
#ifdef Destruct_Monitor
    //std::cout<<"~EsriGridClass done."<<std::endl;
#endif
}
//______________________________________________________________________________
template <class Grid_Type> void EsriGridClass<Grid_Type>::CopyHeadInformation(EsriGridClass<Grid_Type> &from)
{
    nrows = from.getNrows();
    ncols = from.getNcols();
    xllcorner = from.getXll();
    yllcorner = from.getYll();
    cellsize = from.getCellsize();
    novalue = from.getNodataValue();
    bInt = from.getBInt();
}
//______________________________________________________________________________
template <class Grid_Type> void EsriGridClass<Grid_Type>::writeAsciiGridFile(std::string filename)
{
    //Output Arc/Info grid ascii format output
    //implement later M.Liu
    FILE *fgrid;
    char errormessage[100];
    int totalvalidcells = 0;
    if ((fgrid = fopen(filename.c_str(),"w")) == NULL) {
        sprintf(errormessage,"Cannot create gridfile:%s",filename.c_str());
        nrerror(errormessage);
    }
    fprintf(fgrid,"ncols %d\n",ncols);
    fprintf(fgrid,"nrows %d\n",nrows);
    fprintf(fgrid,"xllcorner %lf\n",xllcorner);
    fprintf(fgrid,"yllcorner %lf\n",yllcorner);
    fprintf(fgrid,"cellsize %lf\n",cellsize);
    if (bInt) fprintf(fgrid,"NODATA_value %d\n",novalue);
    else fprintf(fgrid,"NODATA_value %lf\n",novalue);
    for (int i = 0; i < nrows; i++) {
        for (int j = 0; j < ncols; j++) {
            if (bInt) fprintf(fgrid,"%d ",value[i][j]);
            else fprintf(fgrid,"%lf ",value[i][j]);
            if (value[i][j] != novalue) totalvalidcells++;
        }
        fprintf(fgrid,"\n");
    }
    fclose(fgrid);
    num_valid_cells = totalvalidcells;
}
//______________________________________________________________________________
template <class Grid_Type> void EsriGridClass<Grid_Type>::InitAllNovalue()
{
    #pragma omp parallel for num_threads(NUMCORES_TO_USE)
    for (long ij = 0; ij < nrows * ncols; ij++) {
        long i = (long)(ij / ncols);
        long j = ij % ncols;
        value[i][j] = novalue;
    }
}
//______________________________________________________________________________
template <class Grid_Type> Grid_Type** EsriGridClass<Grid_Type>::getValueArray()
{
    return value;
}
//______________________________________________________________________________
template <class Grid_Type> bool EsriGridClass<Grid_Type>::IsValidCell(int row,int col)
{
    bool valid(false);
    if (!IsWithinGridWindow(row,col)) valid = false;
    else if (value[row][col] == novalue) valid = false;
    else valid = true;
    return valid;
}
//______________________________________________________________________________
template <class Grid_Type> bool EsriGridClass<Grid_Type>::IsWithinGridWindow(int row,int col)
{
    if (row >= nrows || row < 0 || col >= ncols || col < 0) return false;
    else return true;
}
#endif // ESRIGRIDCLASS_H
