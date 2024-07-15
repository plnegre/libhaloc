libhaloc
=============

Library for HAsh-based LOop Closure detection. This library provides the tools for loop closure detection based on image hashing. Image hashing consists of representing every image with a small vector (hash). Then the hash of image A can be compared with the hash of image B in a super fast way in order to determine if images are similar.

The image hashing implemented in this library is based on floating point features.


## Related paper

[Autonomous Robots][paper]

CITATION:
```bash
@Article{Negre Carrasco2015,
   author="Negre Carrasco, Pep Lluis
   and Bonin-Font, Francisco
   and Oliver-Codina, Gabriel",
   title="Global image signature for visual loop-closure detection",
   journal="Autonomous Robots",
   year="2015",
   pages="1--15",
   issn="1573-7527",
   doi="10.1007/s10514-015-9522-4",
   url="http://dx.doi.org/10.1007/s10514-015-9522-4"
}
```

## How to use libhaloc in your project

libhaloc is built using ROS catkin for convenience, but it has no ROS-dependencies.

To use libhaloc in your project first add the dependency in your package.xml and CMakeLists.txt as follows:

```
# Add this line to your package.xml (if format="2"):
<depend>libhaloc</depend>
```

```
# In your CMakeLists.txt, find the package using:
find_package(libhaloc REQUIRED)

# Include the libhaloc directories
include_directories(
  ${catkin_INCLUDE_DIRS}
  ${libhaloc_INCLUDE_DIRS}
  ...
  )

# Link your executable with libhaloc
target_link_libraries(your_executable_here
  ${catkin_LIBRARIES}
  ${libhaloc_LIBRARIES}
  ...
  )
```

Include the header in your C++ file:
```
#include <libhaloc/haloc.h>
```

Create your haloc object:
```
haloc::Haloc haloc_(2, 1024);  // Two projections, 1024 maximum descriptors
```

## How to call the library

Simply call the following method for every new image:

```
uint image_id = 0;  // The current image identifier. This must be unique for each image.

cv::Mat image;  // Your current image.

std::set<uint> images_to_discard = std::set<uint>{};   // The image IDs you want to discard for this query (e.g. if you don't want to consider the previous N images...)

int best_n_candidates = 3;  // It will retrieve the best N candidates for closing a loop with the current image

std::vector<uint> candidates = haloc_.process(image_id, image, best_n_candidates, discarded);  // This returns a vector of the best N candidates to close a loop with the current image.
```

Note that:
* You are responsible for providing a unique ID for each image. The IDs do not need to be consecutive.
* Candidates are not geometrically validated, i.e. some are false positives. You are responsible for verifying the candidates.

[stereo_slam]: https://github.com/srv/stereo_slam
[paper]: http://link.springer.com/article/10.1007/s10514-015-9522-4
