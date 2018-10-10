//
// Created by kingaj on 10/9/18.
//

#ifndef TENSORMODEL_TMPACKEDVECTOR_H
#define TENSORMODEL_TMPACKEDVECTOR_H

#include <coin/CoinPackedVector.hpp>

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

    class TmPackedVector : public TmIndexed, public TmNamed {
    private:
        bool zeroDim;
        CoinPackedVector *cpv;
        std::string h5_file_ = "default";
        bool h5_file_created = false;
        bool is_loaded;
    public:

        TmPackedVector() : TmIndexed(1) {
            cpv = new CoinPackedVector(true);
            try {
                cpv->insert(0, 0.0);
            }
            catch (exception &e) {
                throw e;
            }
            zeroDim = true;
            this->setName("sm_sparse_data");
            h5_file_created = false;
            is_loaded = true;
        }

        explicit TmPackedVector(TmSet const &s1, TmSet const &s2 = TmSet(), TmSet const &s3 = TmSet(),
                                TmSet const &s4 = TmSet(),
                                TmSet const &s5 = TmSet()) :
                TmIndexed(s1.getNum(), s2.getNum(), s3.getNum(), s4.getNum(), s5.getNum()) {

            this->setName("sm_sparse_data");
            cpv = new CoinPackedVector(true);
            this->setDim(1);

            if (!s2.isEmptySet())
                this->setDim(2);
            if (!s3.isEmptySet())
                this->setDim(3);
            if (!s4.isEmptySet())
                this->setDim(4);
            if (!s5.isEmptySet())
                this->setDim(5);

            zeroDim = false;
            h5_file_created = false;
            is_loaded = true;
        }

        void reserve(int n) {
            cpv->reserve(n);
        }

        int getNumElements() {
            loadHdf();
            return cpv->getNumElements();
        }

        double getIndexedValue(int i1 = 0, int i2 = 0, int i3 = 0, int i4 = 0, int i5 = 0) {
            loadHdf();
            return cpv->operator[](index(i1, i2, i3, i4, i5));
        }

        void display() {
            loadHdf();
            cout << "****" << this->getName() << "****\n";
            if (zeroDim) {
                cout << this->getName();
                cout << ":\t" << cpv->operator[](0) << "\n";
            } else {
                for (int n = 0; n < cpv->getNumElements(); n++) {
                    int indx = cpv->getIndices()[n];
                    TmIndexed::display(indx);
                    cout << ":\t" << cpv->getElements()[n] << "\n";
                }
            }
        }

//	  void setValues( string s);



        void insertIndexedValue(double d, int i1 = 0, int i2 = 0, int i3 = 0, int i4 = 0, int i5 = 0) {
            loadHdf();
            //cout << "Setting value "<<d<< " for index "<< i1 << " " << i2 << " " << i3 << " " << i4 << " " << i5 << endl;
            cpv->insert(index(i1, i2, i3, i4, i5), d);
        }

        const CoinPackedVector &getCPV() {
            loadHdf();
            return *cpv;
        }

        void saveHdf(std::string dir_name) {
            loadHdf();
            HdfWriter hdfWriter;
            hsize_t dims[1];
            dims[0] = cpv->getNumElements();

            hdfWriter.initWrite();
            hdfWriter.push("Indices", cpv->getIndices(), 1, dims);
            hdfWriter.push("Elements", cpv->getElements(), 1, dims);

            dims[0] = this->getName().length();
            hdfWriter.push("Name", this->getName().c_str(), 1, dims);

            if (!h5_file_created) {
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

            h5_file_created = true;

            // push TmPackedVector items to HdfWriter
            hdfWriter.completeWrite(h5File);

            // close H5File
            h5File->close();

            // delete CoinPackedVector
            delete cpv;

            // delete h5file
            delete h5File;

            is_loaded = false;
        }

        void loadHdf() {
            if (is_loaded) return;

            if (!h5_file_created) {
                std::cout << "No h5 file." << endl;
                return;
            }

            try {
                hsize_t dims[1];

                auto h5File = new H5File(h5_file_, H5F_ACC_RDONLY);

                // Read indices
                DataSet dataset = h5File->openDataSet("Indices");
                DataSpace dataspace = dataset.getSpace();
                dataspace.getSimpleExtentDims(dims, nullptr);
                auto indices = new int[dims[0]];
                dataset.read(indices, PredType::NATIVE_INT);

                // Read elements
                dataset = h5File->openDataSet("Elements");
                dataspace = dataset.getSpace();
                dataspace.getSimpleExtentDims(dims, nullptr);
                auto elements = new double[dims[0]];
                dataset.read(elements, PredType::NATIVE_DOUBLE);

                // Create CPV
                cpv = new CoinPackedVector();
                cpv->assignVector(dims[0], *&indices, *&elements);

                // TODO: why do these fail .. assignVector is supposed to nullify
                //assert(indices == nullptr);
                //assert(elements == nullptr);

                // Read name
                dataset = h5File->openDataSet("Name");
                dataspace = dataset.getSpace();
                dataspace.getSimpleExtentDims(dims, nullptr);
                auto name_string = new char[dims[0] + 1];
                dataset.read(name_string, PredType::NATIVE_CHAR);

                // Set name
                string name(name_string);
                this->setName(name);

                // clean up
                delete[] name_string;
                delete h5File;

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

            is_loaded = true;

        }

        ~TmPackedVector() {
            if (is_loaded)
                delete cpv;
            if (fs::exists(h5_file_))
                fs::remove(h5_file_);
        }
    };
}
#endif //TENSORMODEL_TMPACKEDVECTOR_H
