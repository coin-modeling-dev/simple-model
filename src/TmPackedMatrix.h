//
// Created by kingaj on 10/9/18.
//

#ifndef TENSORMODEL_TMPACKEDMATRIX_H
#define TENSORMODEL_TMPACKEDMATRIX_H

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/filesystem.hpp>
#include <boost/bimap.hpp>

#include "HdfWriter.h"
#include "TmIndexed.h"
#include "TmNamed.h"
#include "TmSet.h"


namespace fs=boost::filesystem;
namespace tensormodel {

    typedef std::shared_ptr<TmIndexed> s_TmIndexed;

    class TmPackedMatrix : public TmNamed {
    private:
        double *cpm_elements;
        int *cpm_colindices;
        int *cpm_rowindices;
        int cpm_numEls;
        int max_numEls;
        std::string h5_file_;
        s_TmIndexed sm_rows_;
        s_TmIndexed sm_cols_;
        bool is_loaded;
    public:

        TmPackedMatrix(s_TmIndexed r, s_TmIndexed c, int nnz) : sm_rows_{move(r)}, sm_cols_{move(c)} {
            cpm_elements = (double *) malloc(nnz * sizeof(double));
            cpm_elements = static_cast<double *>(memset(cpm_elements, 0, nnz * sizeof(double)));
            cpm_rowindices = (int *) malloc(nnz * sizeof(int));
            cpm_rowindices = static_cast<int *>(memset(cpm_rowindices, 0, nnz * sizeof(int)));
            cpm_colindices = (int *) malloc(nnz * sizeof(int));
            cpm_colindices = static_cast<int *>(memset(cpm_colindices, 0, nnz * sizeof(int)));
            cpm_numEls = 0;
            max_numEls = nnz;

            h5_file_ = "";
            is_loaded = true;
        }

        void freeArrays() {
            // free arrays
            if (cpm_elements != nullptr) free(cpm_elements);
            if (cpm_rowindices != nullptr) free(cpm_rowindices);
            if (cpm_colindices != nullptr) free(cpm_colindices);

            // nullify pointers
            cpm_elements = nullptr;
            cpm_rowindices = nullptr;
            cpm_colindices = nullptr;

            // reset is_loaded
            is_loaded = false;
        }

        int rowIndex(int i1 = 0, int i2 = 0, int i3 = 0, int i4 = 0, int i5 = 0) {
            if (!is_loaded)
                loadHdf();
            return sm_rows_->index(i1, i2, i3, i4, i5);
        }

        const array<int, TM_MAX_DIM> rowInverseIndex(int n) {
            if (!is_loaded)
                loadHdf();
            array<int, TM_MAX_DIM> v = sm_rows_.get()->invIndex(n);
            return v;
        }

        const array<int, TM_MAX_DIM> &rowGetDimSizes() {
            if (!is_loaded)
                loadHdf();
            return sm_rows_->getDimSizes();
        }

        int colIndex(int i1 = 0, int i2 = 0, int i3 = 0, int i4 = 0, int i5 = 0) {
            if (!is_loaded)
                loadHdf();
            return sm_cols_->index(i1, i2, i3, i4, i5);
        }

        const array<int, TM_MAX_DIM> colInverseIndex(int n) {
            if (!is_loaded)
                loadHdf();
            array<int, TM_MAX_DIM> v = sm_cols_.get()->invIndex(n);
            return v;
        }

        const array<int, TM_MAX_DIM> &colGetDimSizes() {
            if (!is_loaded)
                loadHdf();
            return sm_cols_->getDimSizes();
        }

        int getNumElements() {
            if (!is_loaded)
                loadHdf();
            return this->cpm_numEls;
        }

        int *getRowIndices() {
            if (!is_loaded)
                loadHdf();
            return this->cpm_rowindices;
        }

        int *getColIndices() {
            if (!is_loaded)
                loadHdf();
            return this->cpm_colindices;
        }

        double *getElements() {
            if (!is_loaded)
                loadHdf();
            return this->cpm_elements;
        }


        void setValue(double value, int rowIndx, int colIndx) {
            if (!is_loaded)
                loadHdf();
            assert(cpm_numEls < max_numEls);
            cpm_elements[cpm_numEls] = value;
            cpm_rowindices[cpm_numEls] = rowIndx;
            cpm_colindices[cpm_numEls] = colIndx;
            cpm_numEls++;
        }

        void saveHdf(std::string dir_name) {
            HdfWriter hdfWriter;
            hdfWriter.initWrite();

            // remove gaps
            hsize_t dims[1];
            dims[0] = cpm_numEls;

            // save in indices format
            hdfWriter.push("RowIndices", cpm_rowindices, 1, dims);
            hdfWriter.push("ColIndices", cpm_colindices, 1, dims);
            hdfWriter.push("Elements", cpm_elements, 1, dims);
            dims[0] = this->getName().length();
            hdfWriter.push("Name", this->getName().c_str(), 1, dims);

            // save row and column indices
            dims[0] = TM_MAX_DIM;
            int rowDimSizes[TM_MAX_DIM];
            int colDimSizes[TM_MAX_DIM];
            for (int i = 0; i < TM_MAX_DIM; i++) {
                rowDimSizes[i] = rowGetDimSizes()[i];
                colDimSizes[i] = colGetDimSizes()[i];
            }
            hdfWriter.push("rowDimSizes", rowDimSizes, 1, dims);
            hdfWriter.push("colDimSizes", colDimSizes, 1, dims);


            if (h5_file_.length() == 0) {
                // generate uuid file name
                boost::uuids::random_generator gen;
                boost::uuids::uuid u = gen();
                std::string file_name = boost::uuids::to_string(u);

                // generate h5 path
                fs::path fname = "H5_" + file_name + "_sm_sparsedata.h5";
                fs::path dir_path = dir_name;
                if (!fs::exists(dir_path)) {
                    fs::create_directories(dir_path);
                }
                fs::path h5_path = dir_path / fname;

                // store h5 path as string
                h5_file_ = h5_path.string();
            }

            // open H5File
            auto h5File = new H5File(h5_file_.c_str(), H5F_ACC_TRUNC);

            // push TmPackedVector items to HdfWriter
            hdfWriter.completeWrite(h5File);

            // close H5File
            h5File->close();

            // delete arrays
            freeArrays();


            // set loaded to false
            is_loaded = false;
        }

        void loadHdf() {
            if (is_loaded) return;

            if (h5_file_.length() == 0) {
                std::cout << "No h5 file." << endl;
            }

            try {
                hsize_t dims[1];

                auto h5File = new H5File(h5_file_, H5F_ACC_RDONLY);

                // Read indices
                DataSet dataset = h5File->openDataSet("RowIndices");
                DataSpace dataspace = dataset.getSpace();
                dataspace.getSimpleExtentDims(dims, nullptr);
                assert((unsigned int) dims[0] == (unsigned int) this->cpm_numEls &&
                       "Number of elements read from HDF file not same as written");
                cpm_rowindices = (int *) malloc(sizeof(int) * dims[0]);
                dataset.read(cpm_rowindices, PredType::NATIVE_INT);

                dataset = h5File->openDataSet("ColIndices");
                dataspace = dataset.getSpace();
                dataspace.getSimpleExtentDims(dims, nullptr);
                cpm_colindices = (int *) malloc(sizeof(int) * dims[0]);
                dataset.read(cpm_colindices, PredType::NATIVE_INT);

                // Read elements
                dataset = h5File->openDataSet("Elements");
                dataspace = dataset.getSpace();
                dataspace.getSimpleExtentDims(dims, nullptr);
                cpm_elements = (double *) malloc(sizeof(double) * dims[0]);
                dataset.read(cpm_elements, PredType::NATIVE_DOUBLE);

                cpm_numEls = dims[0];
                max_numEls = dims[0];

                // Get the indices
                dataset = h5File->openDataSet("rowDimSizes");
                dataspace = dataset.getSpace();
                dataspace.getSimpleExtentDims(dims, nullptr);
                auto rowDimSizes = (int *) malloc(sizeof(int) * dims[0]);
                dataset.read(rowDimSizes, PredType::NATIVE_INT);
                array<int, TM_MAX_DIM> row_array{};

                dataset = h5File->openDataSet("colDimSizes");
                dataspace = dataset.getSpace();
                dataspace.getSimpleExtentDims(dims, nullptr);
                auto colDimSizes = (int *) malloc(sizeof(int) * dims[0]);
                dataset.read(colDimSizes, PredType::NATIVE_INT);
                array<int, TM_MAX_DIM> col_array{};

                for (int i = 0; i < TM_MAX_DIM; ++i) {
                    row_array[i] = rowDimSizes[i];
                    col_array[i] = colDimSizes[i];
                }

                std::shared_ptr<TmIndexed> rowtemp = std::make_shared<TmIndexed>(row_array);
                sm_rows_ = rowtemp;
                std::shared_ptr<TmIndexed> coltemp = std::make_shared<TmIndexed>(col_array);
                sm_cols_ = coltemp;

                is_loaded = true;

            }

                // catch failure caused by the H5File operations
            catch (FileIException &error) {
                FileIException::printErrorStack();
                throw;
            }

                // catch failure caused by the DataSet operations
            catch (DataSetIException &error) {
                DataSetIException::printErrorStack();
                throw;
            }

                // catch failure caused by the DataSpace operations
            catch (DataSpaceIException &error) {
                DataSpaceIException::printErrorStack();
                throw;
            }

        }

        ~TmPackedMatrix() {
            // delete arrays
            freeArrays();
            if (fs::exists(h5_file_))
                fs::remove(h5_file_);
        }
    };
}
#endif //TENSORMODEL_TMPACKEDMATRIX_H
