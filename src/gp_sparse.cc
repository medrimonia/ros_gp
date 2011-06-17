/**************************************************************
 libgp - Gaussian Process library for Machine Learning
 Copyright (C) 2011 Universität Freiburg
 Author: Manuel Blum
 
 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 ***************************************************************/

#include "gp_sparse.h"

namespace libgp {
  
  SparseGaussianProcess::SparseGaussianProcess (size_t input_dim, std::string covf_def) : GaussianProcess(input_dim, covf_def)
  {
  }
  
  SparseGaussianProcess::SparseGaussianProcess (const char * filename) : GaussianProcess(filename)
  {
  }
  
  SparseGaussianProcess::~SparseGaussianProcess ()
  {
  }  
  
  void SparseGaussianProcess::compute()
  {    
    if (cf->get_threshold() == INFINITY) {
      std::cerr << "Warning: no threshold defined, computation will be slow." << std::endl
        << "Use full GP or covariance functions that support thresholding." << std::endl;
    }
    if (sampleset->empty()) return; 
    Eigen::SparseMatrix<double> K(sampleset->size(), sampleset->size());
    //Eigen::DynamicSparseMatrix<double> aux(sampleset->size(), sampleset->size());
    
    alpha.resize(sampleset->size());
    // compute kernel matrix (lower triangle)
    for(size_t i = 0; i < sampleset->size(); ++i) {
      K.startVec(i);
      for(size_t j = i; j < sampleset->size(); ++j) {
        if ((sampleset->x(i)-sampleset->x(j)).norm() <= cf->get_threshold()) {
          K.insertBack(j,i) = cf->get(sampleset->x(i), sampleset->x(j));
        }          
      }
      alpha(i) = sampleset->y(i);
    }
    K.finalize();
    std::cout << 1.0 * K.nonZeros() / (sampleset->size()*sampleset->size()) << std::endl;
    // perform cholesky factorization
    solver.setFlags(Eigen::SupernodalMultifrontal);
    solver.compute(K);
    solver.solveInPlace(alpha);
  }
  
}