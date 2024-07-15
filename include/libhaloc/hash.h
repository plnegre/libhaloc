/**
 * @file hash.h
 *
 * @brief Class to generate the image hashes for the HALOC algorithm.
 *
 * @author Pep Lluis Negre Carrasco
 * @date 2018
 */

#pragma once

#include <Eigen/Eigen>
#include <Eigen/Dense>

#include <opencv2/core/core.hpp>
#include <opencv2/core/eigen.hpp>

namespace haloc {

class Hash {
 public:
  /**
   * @brief      Class constructor.
   * 
   * @param[in]  num_proj  The number of projections
   * @param[in]  max_desc  The maximum number of descriptors
   */
  Hash(const int &num_proj, const int &max_desc);

  /**
   * @brief      Class destructor.
   */
  ~Hash() = default;

  /**
   * @brief      Calculates the image hash
   *
   * @param[in]  desc      The floating-point descriptors.
   *
   * @return     The image hash.
   */
  std::vector<float> calcHash(const cv::Mat &desc);

  /**
   * @brief      Compute the similarity between two hashes. The smallest, the more similar.
   *
   * @param[in]  hash_1  The hash 1.
   * @param[in]  hash_2  The hash 2.
   *
   * @return     similarity value (the smallest, the better)
   */
  float calcSimilarity(
    const std::vector<float> &hash_1,
    const std::vector<float> &hash_2);

 protected:
  /**
   * @brief      Determines if class is initialized.
   *
   * @return     True if initialized, False otherwise.
   */
  inline bool isInitialized() const {return initialized_;}

  /**
   * @brief      Init the hash calculator.
   */
  void init();

  /**
   * @brief      Initializes the random vectors for projections.
   */
  void initProjections();

  /**
   * @brief      Calculates a random vector.
   *
   * @param[in]  size  The size.
   * @param[in]  seed  The seed.
   *
   * @return     The random vector.
   */
  std::vector<float> computeRandomVector(const int &size, const int &seed);

  /**
   * @brief      Makes a vector unitary.
   *
   * @param[in]  x     The input vector.
   *
   * @return     The output unit vector.
   */
  std::vector<float> unitVector(const std::vector<float> &x);

 private:
  bool initialized_;                     //!> True when class has been initialized
  int num_proj_;                         //!> The number of projections
  int max_desc_;                         //!> The maximum number of descriptors
  std::vector< std::vector<float> > r_;  //!> Vector of random values
};

}  // namespace haloc
