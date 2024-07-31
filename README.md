# Simple Model

Maintain mapping between Sets and Variables for a Linear Program

* Sets are collection of strings
* Variables indexed by multiple sets (row and column variables)
* Matrices corresponding to linear constraints
* Objective function

Stored as HDF5 file
Mapped to OSI model 

Required packages
```bash

# cmake
sudo apt install cmake gcc clang gdb build-essential

# HDF5 and Boost
sudo apt install libhdf5-serial-dev libboost-all-dev

# COIN-OR
sudo apt install coinor-clp coinor-libosi-dev coinor-libcoinutils-dev coinor-libclp-dev
```
