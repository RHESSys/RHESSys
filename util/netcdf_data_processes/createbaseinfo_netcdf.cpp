#define MAXFILENAME 200
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <limits.h>
#define MAXID 50000
#include "pubtools.h"
#include "esrigridclass.h"

#include <vector>

static void show_usage(std::string name)
{
    std::cerr << "Usage: " << name << " <option(s)> SOURCES"
              << "Options:\n"
              << "\t-h,\t\t--help\t\tShow this help message\n"
              << "\t-si,\t\t--cell_id \t\t<cell_id_ascii_filename> \t\tSpecify the cell id (ESRI Grid ASCII format))\n"
              << "\t-sl,\t\t--leaf_area_index \t<lai_GIS_filename> \t\t\tSpecify the leaf area index (m2/m2: ESRI Grid ASCII format)\n"
              << "\t-se,\t\t--elevation \t<elevation_GIS> \t\t\tSpecify the elevation (meter: ESRI Grid ASCII format)\n"
              << "\t-ss,\t\t--screenheight \t<screen_hight_GIS> \t\t\tSpecify the screen hight for temperature observation (meter: ESRI Grid ASCII format)\n"
              << "\t-sgx,\t\t--geocor_lon \t<geocoordinate_lon_GIS> \t\tSpecify the longitude (DD: ESRI Grid ASCII format)\n"
              << "\t-sgy,\t\t--geocor_lat \t<geocoordinate_lat_GIS> \t\tSpecify the latitude (DD: ESRI Grid ASCII format)\n"
              << "\t-spx,\t\t--prjcor_x \t<projected_coordinate_x_GIS> \t\tSpecify the projected coordinate x-axis (meter: ESRI Grid ASCII format)\n"
              << "\t-spy,\t\t--prjcor_y \t<projected_coordinate_y_GIS> \t\tSpecify the projected coordinate y-axis (meter: ESRI Grid ASCII format)\n"
              << "\t-um,\t\t--ppt_coef \t<multiplier_to_meter_per_day> \t\tSpecify the convertion coefficient for daily precipitation (multiplier: a float number)\n"
              << "\t-ut,\t\t--unit_temperature \t<K: Kelvin C:Celsius> \t\t\tSpecify the convertion coefficient for daily precipitation (multiplier: a float number)\n"
              << "\t-y,\t\t--start_counting_year \t<start_year_for_counting_time> \t\tSpecify the start year for counting time dimention days (year: an int number)\n"
              << "\t-od,\t\t--offset_days \t<offset_days_for_start_year> \t\tSpecify the offset days from start year for counting time dimention days (days: an int number)\n"
              << "\t-l,\t\t--leapyear \t<1: with leap years; 0: w/o> \t\tSpecify is the data has leap years (1 or 0)\n"
              << "\t-ntx,\t\t--var_tmax_name \t<variable_name_for_tmax_netcdf> \t\tSpecify the variable name for daily maximum temperature in the netcdf file\n"
              << "\t-ftx,\t\t--file_tmax_name \t<netcdf_file_name_for_tmax> \t\tSpecify the netcdf file name for daily maximum temperature (full or relative path to world file)\n"
              << "\t-ntn,\t\t--var_tmin_name \t<variable_name_for_tmin_netcdf> \t\tSpecify the variable name for daily minimum temperature in the netcdf file\n"
              << "\t-ftn,\t\t--file_tmin_name \t<netcdf_file_name_for_tmin> \t\tSpecify the netcdf file name for daily minimum temperature (full or relative path to world file)\n"
              << "\t-nppt,\t\t--var_ppt_name \t<variable_name_for_ppt_netcdf> \t\tSpecify the variable name for daily precipitation in the netcdf file\n"
              << "\t-fppt,\t\t--file_ppt_name \t<netcdf_file_name_for_ppt> \t\tSpecify the netcdf file name for daily precipitation (full or relative path to world file)\n"
              << "\t-fo,\t\t--outbase \t\t<output_baseinfo_filename> \t\tSpecify the output file name for base information"
              << std::endl;
}

int main(int argc, char *argv[])
{
    enum netcdfvar {TMAX,TMIN,PPT,VARACCOUNT};
    std::string varname[VARACCOUNT] {"air_temperature","air_temperature","precipitation_flux"};
    std::string varunit[VARACCOUNT] {"K","K","kg m-2 s-1"};
    std::string varfilenames[VARACCOUNT];
    std::string outbasefile;
    enum gis_grids {CELLID,LAI,ELEVATION,SCREENHIGHT,LON,LAT,PROJX,PROJY,GISCOUNTS};
    EsriGridClass<float> gisgriddata[GISCOUNTS];
    bool gisgriddata_valid[GISCOUNTS];
    std::string gisgriddata_filename[GISCOUNTS];
    double ppt_multplier = 0;                                                    //Precipitation data multiplier to meter/day
    int start_year_counting = 1900;
    int offset = 0;                                                              //Offset days from start year (for time dimention counting)
    int leap_year = 1;
    if (argc < 14) {
       show_usage(argv[0]);
       return 1;
    }
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if ((arg == "-h") || (arg == "--help")) {
            show_usage(argv[0]);
            return 0;
        } else if ((arg == "-si") || (arg == "--cell_id")) {
            if (i + 1 < argc) {
                gisgriddata_filename[CELLID] = argv[++i];
            }
        } else if ((arg == "-sl") || (arg == "--leaf_area_index")) {
            if (i + 1 < argc) {
                gisgriddata_filename[LAI] = argv[++i];
            }
        } else if ((arg == "-se") || (arg == "--elevation")) {
            if (i + 1 < argc) {
                gisgriddata_filename[ELEVATION] = argv[++i];
            }
        } else if ((arg == "-ss") || (arg == "--screenheight")) {
            if (i + 1 < argc) {
                gisgriddata_filename[SCREENHIGHT] = argv[++i];
            }
        } else if ((arg == "-sgx") || (arg == "--geocor_lon")) {
            if (i + 1 < argc) {
                gisgriddata_filename[LON] = argv[++i];
            }
        } else if ((arg == "-sgy") || (arg == "--geocor_lat")) {
            if (i + 1 < argc) {
                gisgriddata_filename[LAT] = argv[++i];
            }
        } else if ((arg == "-spx") || (arg == "--prjcor_x")) {
            if (i + 1 < argc) {
                gisgriddata_filename[PROJX] = argv[++i];
            }
        } else if ((arg == "-spy") || (arg == "--prjcor_y")) {
            if (i + 1 < argc) {
                gisgriddata_filename[PROJY] = argv[++i];
            }
        } else if ((arg == "-um") || (arg == "--ppt_coef")) {
            if (i + 1 < argc) {
                ppt_multplier = atof(argv[++i]);
            }
        } else if ((arg == "-ut") || (arg == "--unit_temperature")) {
            if (i + 1 < argc) {
                varunit[TMAX] = argv[++i];
                varunit[TMIN] = varunit[TMAX];
            }
        } else if ((arg == "-y") || (arg == "--start_counting_year")) {
            if (i + 1 < argc) {
                start_year_counting = atoi(argv[++i]);
            }
        } else if ((arg == "-od") || (arg == "--offset_days")) {
            if (i + 1 < argc) {
                offset = atoi(argv[++i]);
            }
        } else if ((arg == "-l") || (arg == "--leapyear")) {
            if (i + 1 < argc) {
                leap_year = atoi(argv[++i]);
            }
        } else if ((arg == "-ntx") || (arg == "--var_tmax_name")) {
            if (i + 1 < argc) {
                varname[TMAX] = argv[++i];
            }
        } else if ((arg == "-ntn") || (arg == "--var_tmin_name")) {
            if (i + 1 < argc) {
                varname[TMIN] = argv[++i];
            }
        } else if ((arg == "-nppt") || (arg == "--var_ppt_name")) {
            if (i + 1 < argc) {
                varname[PPT] = argv[++i];
            }
        } else if ((arg == "-ftx") || (arg == "--file_tmax_name")) {
            if (i + 1 < argc) {
                varfilenames[TMAX] = argv[++i];
            }
        } else if ((arg == "-ftn") || (arg == "--file_tmin_name")) {
            if (i + 1 < argc) {
                varfilenames[TMIN] = argv[++i];
            }
        } else if ((arg == "-fppt") || (arg == "--file_ppt_name")) {
            if (i + 1 < argc) {
                varfilenames[PPT] = argv[++i];
            }
        } else if ((arg == "-fo") || (arg == "--outbase")) {
            if (i + 1 < argc) {
                outbasefile = argv[++i];
            }
        } else {
            std::cerr << "Wrong arguments!\n";
            return 0;
        }
    }
    //create output base info file
    FILE *io_file;
    if ((io_file = fopen(outbasefile.c_str(),"w")) == NULL) {
          fprintf(stderr,"cannot create base file:%s\n",outbasefile.c_str());
          return 1;
    }
    //read grid data
    for (int i = 0; i < GISCOUNTS; i++) {
        gisgriddata_valid[i] = gisgriddata[i].readAsciiGridFile(gisgriddata_filename[i]);
    }
    if (!gisgriddata_valid[CELLID]) nrerror("ERROR: GIS data (CELLID) cannot open!!!");
    int valid_cells = gisgriddata[CELLID].getCellNumValid();                     //toal valid cells
    //outfile
    fprintf(io_file,"%d grid_cells\n",valid_cells);
    fprintf(io_file,"%d year_start_index\n",start_year_counting);
    fprintf(io_file,"%d day_offset\n",offset);
    fprintf(io_file,"%d leap_year_include\n",leap_year);
    fprintf(io_file,"%f precip_multiplier\n",ppt_multplier);
    fprintf(io_file,"%s temperature_unit\n",varunit[TMAX].c_str());
    fprintf(io_file,"%s netcdf_tmax_filename\n",varfilenames[TMAX].c_str());
    fprintf(io_file,"%s netcdf_var_tmax\n",varname[TMAX].c_str());
    fprintf(io_file,"%s netcdf_tmin_filename\n",varfilenames[TMIN].c_str());
    fprintf(io_file,"%s netcdf_var_tmin\n",varname[TMIN].c_str());
    fprintf(io_file,"%s netcdf_rain_filename\n",varfilenames[PPT].c_str());
    fprintf(io_file,"%s netcdf_var_rain\n",varname[PPT].c_str());
    for (int i = 0; i < gisgriddata[CELLID].getNrows(); i++) {
        for (int j = 0; j < gisgriddata[CELLID].getNcols(); j++) {
            if (gisgriddata[CELLID].IsValidCell(i,j)) {
                fprintf(io_file,"%i base_station_id\n", (int)gisgriddata[CELLID].getValue(i,j));
                fprintf(io_file,"%f lon\n",             gisgriddata_valid[LON]          ? (float)gisgriddata[LON].getValue(i,j)         : -9999.0);
                fprintf(io_file,"%f lat\n",             gisgriddata_valid[LAT]          ? (float)gisgriddata[LAT].getValue(i,j)         : -9999.0);
                fprintf(io_file,"%f xc\n",              gisgriddata_valid[PROJX]        ? (float)gisgriddata[PROJX].getValue(i,j)       : -9999.0);
                fprintf(io_file,"%f yc\n",              gisgriddata_valid[PROJY]        ? (float)gisgriddata[PROJY].getValue(i,j)       : -9999.0);
                fprintf(io_file,"%f z_coordinate\n",    gisgriddata_valid[ELEVATION]    ? (float)gisgriddata[ELEVATION].getValue(i,j)   : 0.0);
                fprintf(io_file,"%f effective_lai\n",   gisgriddata_valid[LAI]          ? (float)gisgriddata[LAI].getValue(i,j)         : 2.0);
                fprintf(io_file,"%f screen_height\n",   gisgriddata_valid[SCREENHIGHT]  ? (float)gisgriddata[SCREENHIGHT].getValue(i,j) : 2.0);
            }
        }
    }
    fclose(io_file);
    printf("Success!\n");
    return 0;
}

