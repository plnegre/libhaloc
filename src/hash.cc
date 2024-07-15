/**
 * @file hash.cc
 *
 * @brief Class to generate the image hashes for the HALOC algorithm.
 *
 * @author Pep Lluis Negre Carrasco
 * @date 2018
 */

#include <numeric>
#include <iostream>

#include "libhaloc/hash.h"

namespace haloc {

Hash::Hash(const int &num_proj, const int &max_desc) :
  initialized_{false},
  num_proj_{num_proj},
  max_desc_{max_desc} {}

std::vector<float> Hash::calcHash(const cv::Mat &desc) {
  // Initialize first time
  if (!isInitialized()) {
    init();
  }

  // Sanity checks
  if (desc.rows == 0) {
    std::cerr << "[Hash]: ERROR -> Descriptor matrix is empty." << std::endl;
    return std::vector<float>{};
  }

  // Adjust the number of descriptors (this should never happen, or very rarely)
  cv::Mat adjusted_desc;
  if (desc.rows > r_[0].size()) {
    const auto diff = desc.rows - r_[0].size();
    // Delete the difference random rows in the descriptor matrix
    std::vector<int> indices(desc.rows);
    std::iota(indices.begin(), indices.end(), 0);
    std::random_shuffle(indices.begin(), indices.end());
    adjusted_desc = cv::Mat(desc.rows - diff, desc.cols, desc.type());
    for (int i=0; i < adjusted_desc.rows; i++) {
      desc.row(indices[i]).copyTo(adjusted_desc.row(i));
    }
  } else {
    adjusted_desc = desc; // No need to adjust
  }

  // Project the descriptors
  const float desc_length = static_cast<float>(adjusted_desc.rows);
  std::vector<float> hash(r_.size()*adjusted_desc.cols);
  for (uint i=0; i < r_.size(); i++) {
    for (int n=0; n < adjusted_desc.cols; n++) {
      float desc_sum = 0.0;
      for (uint m=0; (m < adjusted_desc.rows) && (m < max_desc_); m++) {
        const float projected = r_[i][m]*adjusted_desc.at<float>(m, n);
        const float projected_normalized = (projected + 1.0) / 2.0;
        desc_sum += projected_normalized;
      }
      hash[i*n] = desc_sum / desc_length;
    }
  }

  return hash;
}

float Hash::calcSimilarity(
    const std::vector<float> &hash_a,
    const std::vector<float> &hash_b) {
  // Sanity checks
  if (hash_a.size() != hash_b.size()) {
    std::cerr << "[Hash:] ERROR -> The hashes have different sizes." << std::endl;
    return -1.0;
  }

  // Compute the similarity
  float sim = 0.0;
  for (uint i=0; i < hash_a.size(); i++) {
    sim += pow(hash_a[i] - hash_b[i], 2.0);
  }
  return sqrt(sim);
}

void Hash::init() {
  initProjections();
  initialized_ = true;
}

void Hash::initProjections() {
  // Initializations
  const int seed = time(NULL);
  r_.clear();

  // The size of the descriptors may vary...
  // But, we limit the number of descriptors per bucket.

  // We will generate N-orthogonal vectors creating a linear system of type Ax=b
  // Generate a first random vector
  const auto r = computeRandomVector(max_desc_, seed);
  r_.push_back(unitVector(r));

  // Generate the set of orthogonal vectors
  for (uint i=1; i < num_proj_; i++) {
    // Generate a random vector of the correct size
    auto new_v = computeRandomVector(max_desc_ - i, seed + i);

    // Get the right terms (b)
    Eigen::VectorXf b(r_.size());
    for (uint n=0; n < r_.size(); n++) {
      auto cur_v = r_[n];
      float sum = 0.0;
      for (uint m=0; m < new_v.size(); m++)
        sum += new_v[m]*cur_v[m];
      b(n) = -sum;
    }

    // Get the matrix of equations (A)
    Eigen::MatrixXf A(i, i);
    for (uint n=0; n < r_.size(); n++) {
      uint k = 0;
      for (uint m=r_[n].size()-i; m < r_[n].size(); m++) {
        A(n, k) = r_[n][m];
        k++;
      }
    }

    // Apply the solver
    const Eigen::VectorXf x = A.colPivHouseholderQr().solve(b);

    // Add the solutions to the new vector
    for (uint n=0; n < r_.size(); n++) {
      new_v.push_back(x(n));
    }

    // Push the new vector
    r_.push_back(unitVector(new_v));
  }
}

std::vector<float> Hash::computeRandomVector(const int &size, const int &seed) {
  std::vector<float> h(size);
  const float rand_max = static_cast<float>(RAND_MAX);
  for (int i=0; i < size; i++) {
    h[i] = static_cast<float>(rand())/rand_max;
  }
  return h;
}

std::vector<float> Hash::unitVector(const std::vector<float> &x) {
  // Compute the norm
  float sum = 0.0;
  for (uint i=0; i < x.size(); i++)
    sum += pow(x[i], 2.0);
  float x_norm = sqrt(sum);

  // x^ = x/|x|
  std::vector<float> out;
  for (uint i=0; i < x.size(); i++)
    out.push_back(x[i] / x_norm);

  return out;
}

} // namespace haloc
