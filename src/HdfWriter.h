//
// Created by kingaj on 3/4/18.
//

#ifndef TENSOR_MODEL_HDFWRITER_H
#define TENSOR_MODEL_HDFWRITER_H


#include <iostream>
#include <string>
#include <utility>
#include <sstream>
#include <map>

#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>
#include <boost/container/small_vector.hpp>
#include <boost/ptr_container/ptr_vector.hpp>

#include <H5Cpp.h>

using namespace std;
using namespace H5;
using namespace boost::filesystem;


/*
 * Nice solution to check if file exists
 */
#ifdef _WIN32
#include <io.h>
#define access    _access_s
#else

#include <unistd.h>
//#include "opm/simulators/hdf/HdfWriterManager.h"

#endif

/*
 * Base class for writing
 * - initWrite() clears write-list and generates new h5 file name
 * - push() appends data onto write-list
 * - completeWrite() walks through write-list and stores into h5 file
 * -
 */

class HdfWriter {
public:
    // type mappings
    typedef enum {
        INT, DOUBLE, CHAR
    } StoreType;

    void whereIsHdfWriter(const char *file, int line) {
        this->file = file;
        this->line = line;
        std::cout << "Creating HdfWriter at file: " << file << ", line: " << line << endl;
    }

    std::string file;
    int line;

public:


    // Collect data for writing into a single object
    // write objects
    class WriteObject {
    public:
        string data_name{};
        void *data{};
        hsize_t rank{};
        hsize_t *dims{};
        StoreType storeType{};
        unsigned int sizeof_data{};


        WriteObject(const string &t_data_name, const int *t_data, const hsize_t t_rank, const hsize_t *t_dims)
                : data_name(t_data_name), rank(t_rank) {
            this->sizeof_data = sizeof(int);
            this->storeType = INT;
            this->copy_over(t_data, t_dims);
        }

        WriteObject(const string &t_data_name, const double *t_data, const hsize_t t_rank, const hsize_t *t_dims)
                : data_name(t_data_name), rank(t_rank) {
            this->sizeof_data = sizeof(double);
            this->storeType = DOUBLE;
            this->copy_over(t_data, t_dims);
        }

        WriteObject(const string &t_data_name, const char *t_data, const hsize_t t_rank, const hsize_t *t_dims)
                : data_name(t_data_name), rank(t_rank) {
            this->sizeof_data = sizeof(char);
            this->storeType = CHAR;
            this->copy_over(t_data, t_dims);
        }

        void copy_over(const void *t_data, const hsize_t *t_dims) {
            // copy dims
            this->dims = (hsize_t *) malloc(this->rank * sizeof(hsize_t));
            memcpy(this->dims, t_dims, this->rank * sizeof(hsize_t));

            // copy data
            unsigned int data_size = 1;
            for (unsigned int i = 0; i < this->rank; ++i) {
                data_size *= t_dims[i];
            }
            this->data = malloc(data_size * this->sizeof_data);
            memcpy(this->data, t_data, data_size * this->sizeof_data);
        }

        ~WriteObject() {
            free(this->dims);
            free(this->data);
        }
    };

    typedef boost::ptr_vector<WriteObject> WriteObjectsContainer;

    // container for write objects
    WriteObjectsContainer writeObjects;

public:

    // Push int
    inline void push(const string &data_name, const int *data, hsize_t rank, hsize_t *dims) {
        writeObjects.push_back(new WriteObject(data_name, data, rank, dims));
    }

    // Push double
    inline void push(const string &data_name, const double *data, hsize_t rank, hsize_t *dims) {
        writeObjects.push_back(new WriteObject(data_name, data, rank, dims));
    }

    // Push char
    inline void push(const string &data_name, const char *data, hsize_t rank, hsize_t *dims) {
        writeObjects.push_back(new WriteObject(data_name, data, rank, dims));
    }

    inline void initWrite() {
        writeObjects.clear();
    }

    void completeWrite(H5File *h5File) {
        //Exception::dontPrint();

        // Iterate through write objects
        WriteObjectsContainer::const_iterator end = writeObjects.end();
        for (WriteObjectsContainer::iterator w = writeObjects.begin(); w != end; ++w) {

            WriteObject &wobj = (*w);

            try {

                // Create dataspace
                DataSpace dataSpace(wobj.rank, wobj.dims);

                // Set data type
                DataType dataType(getDataType(wobj.storeType));

                // Create dataset.
                DataSet dataSet = h5File->createDataSet(wobj.data_name, dataType, dataSpace);

                // Write data into the file.
                dataSet.write(wobj.data, dataType);
            }


                // catch failure caused by the H5File operations
            catch (FileIException &error) {
                throw;
            }

                // catch failure caused by the DataSet operations
            catch (DataSetIException &error) {
                throw;
            }

                // catch failure caused by the DataSpace operations
            catch (DataSpaceIException &error) {
                throw;
            }
        }

    }


private:
    // utility functions
    DataType getDataType(StoreType t) {
        switch (t) {
            case INT:
                return PredType::NATIVE_INT;
            case DOUBLE:
                return PredType::NATIVE_DOUBLE;
            case CHAR:
                return PredType::NATIVE_CHAR;
            default:
                throw runtime_error("Invalid StoreType");
        }
    }

};


#endif //TENSOR_MODEL_HDFWRITER_H
