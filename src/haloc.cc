/**
 * @file haloc.cc
 *
 * @brief Class for loop closure detection using image hashes
 *
 * @author Pep Lluis Negre Carrasco
 * @date 2018
 */

#include "libhaloc/haloc.h"

namespace haloc {

Haloc::Haloc(const int &num_proj, const int &max_desc) :
    hash_{std::make_unique<Hash>(num_proj, max_desc)},
    sift_{cv::SIFT::create(max_desc - 5)} {}  // For some reason, SIFT returns max_desc+1 descriptors (or even more)

std::optional<std::vector<uint>> Haloc::process(
    const uint &image_id,
    const cv::Mat &image,
    const int &num_candidates,
    const std::set<uint> &images_to_ignore) {
  // Check if the image is empty
  if (image.empty()) {
    std::cerr << "[Haloc]: ERROR -> The image is empty." << std::endl;
    return std::nullopt;
  }

  // Detect the keypoints and compute the descriptors
  const auto desc = calcDesc(image);

  // Calculate the hash
  const auto hash = hash_->calcHash(desc);
  if (hash.empty()) {
    std::cerr << "[Haloc]: ERROR -> The hash is empty." << std::endl;
    return std::nullopt;
  }

  // Store the hash
  hashes_table_[image_id] = hash;

  // Compare the hash with the rest of the images
  auto similarities = calcSimilarities(hash, images_to_ignore);

  // Get the best N candidates
  const auto candidates = getBestCandidates(similarities, num_candidates);
  if (candidates.empty()) {
    std::cerr << "[Haloc]: WARNING -> No candidates found." << std::endl;
    return std::nullopt;
  }

  // Return the candidates
  return candidates;
}

cv::Mat Haloc::calcDesc(const cv::Mat &image) {
  cv::Mat desc;
  std::vector<cv::KeyPoint> kps;
  sift_->detectAndCompute(image, cv::noArray(), kps, desc);
  return desc;
}

std::map<uint, float> Haloc::calcSimilarities(
    const std::vector<float> &hash,
    const std::set<uint> &images_to_ignore) {
  std::map<uint, float> similarities;
  for (const auto &hash_pair : hashes_table_) {
    // Check if the image is in the ignore list
    if (images_to_ignore.find(hash_pair.first) != images_to_ignore.end()) {
      continue;
    }

    // Calculate the similarity
    const auto similarity = hash_->calcSimilarity(hash, hash_pair.second);
    if (similarity <= 0.0) continue; // Discard bad matches

    // Store the similarity
    similarities[hash_pair.first] = similarity;
  }

  return similarities;
}

std::vector<uint> Haloc::getBestCandidates(
    std::map<uint, float> &similarities,
    const int &num_candidates) {
  // Get the best N candidates
  std::vector<uint> candidates;
  while (candidates.size() < num_candidates && !similarities.empty()) {
    // Get the image with the smallest similarity
    const auto min_sim = std::min_element(similarities.begin(), similarities.end(),
      [](const std::pair<int, float> &p1, const std::pair<int, float> &p2) {
        return p1.second < p2.second;
      });

    // Store the candidate
    candidates.push_back(min_sim->first);

    // Remove the best similarity
    similarities.erase(min_sim);
  }

  return candidates;
}

} // namespace haloc
