/**
 * @file haloc.h
 *
 * @brief Class for loop closure detection using image hashes
 *
 * @author Pep Lluis Negre Carrasco
 * @date 2018
 */

#pragma once

#include <opencv2/opencv.hpp>
#include <opencv2/features2d/features2d.hpp>

#include "libhaloc/hash.h"

namespace haloc {

class Haloc {
 public:
  /**
   * @brief      Class constructor.
   * 
   * @param[in]  num_proj  The number of projections
   * @param[in]  max_desc  The maximum number of descriptors
   */
  Haloc(const int &num_proj, const int &max_desc);

  /**
   * @brief      Class destructor.
   */
  ~Haloc() = default;

  /**
   * @brief      Process the image to get the loop closure candidates
   *
   * @param[in]  image_id          The unique image identifier
   * @param[in]  image             The image
   * @param[in]  num_candidates    The number of candidates to return
   * @param[in]  images_to_ignore  The images to ignore (vector of image ids)
   *
   * @return     The candidates
   */
  std::optional<std::vector<uint>> process(
    const uint &image_id,
    const cv::Mat &image,
    const int &num_candidates,
    const std::set<uint> &images_to_ignore = {});

 protected:
  /**
   * @brief     Calculate the descriptors of the image
   * 
   * @param[in]  image  The image
   * 
   * @return    The descriptors
   */
  cv::Mat calcDesc(const cv::Mat &image);

  /**
   * @brief      Calculate the similarities between the hash and the rest of the images
   * 
   * @param[in]  hash              The current hash
   * @param[in]  images_to_ignore  The images to ignore (vector of image ids)
   * 
   * @return     The similarities
   */
  std::map<uint, float> calcSimilarities(
    const std::vector<float> &hash,
    const std::set<uint> &images_to_ignore = {});

  /**
   * @brief      Get the best candidates
   * 
   * @param[in]  similarities    The similarities
   * @param[in]  num_candidates  The number of candidates
   * 
   * @return     The best candidates
   */
  std::vector<uint> getBestCandidates(
    std::map<uint, float> &similarities,
    const int &num_candidates);

 private:
  cv::Ptr<cv::SIFT> sift_;                            //! < SIFT detector
  std::unique_ptr<haloc::Hash> hash_;                 //! < For the hashes generation
  std::map<uint, std::vector<float>> hashes_table_;   //! < To store the hashes of the images. Format: <image_id, hash>
};

}  // namespace haloc
