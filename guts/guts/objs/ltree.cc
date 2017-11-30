#include "ltree.h"

#include <random>
#include "guts/objs/tube.h"
#include "guts/debug_tools.h"
#include "guts/uniform.h"
#include "guts/random.h"
#include "guts/matrix_ops.h"
#include "guts/internal/containers.h"
#include <glm/gtc/matrix_transform.hpp>

namespace guts {
namespace objs {

namespace {

struct LTreeSegment {
  glm::vec3 start;
  glm::vec3 end;
  float angle_l;
  float angle_3d;
  unsigned int depth;
};

struct BranchStackFrame {
  glm::vec3 last_point;
  float angle_l;
  float angle_3d;
  unsigned int depth;
  bool base_marker = false;
};

glm::mat4 TMPL_TUBE_MATRIX = glm::scale(glm::mat4(1.f), // NOLINT
                                        glm::vec3(0.05, 1.0, 0.05));

glm::vec3 AXIS_Y = glm::vec3(0, 1, 0);
glm::vec3 AXIS_Z = glm::vec3(0, 0, 1);

inline glm::vec3 ProjectSegment(const glm::vec3 &previous,
                                const float angle_l, const float angle_3d) {
  glm::vec4 unit = glm::vec4(0.f, 1.f, 0.f, 1.f);
  auto transform = glm::mat4(1.f);
  transform = glm::rotate(transform, angle_3d, AXIS_Y);
  transform = glm::rotate(transform, angle_l, AXIS_Z);
  return glm::vec3((transform * unit) / unit.w) + previous;
}

} // namespace

LTree::LTree(const std::string &production_pattern,
             unsigned int iterations,
             unsigned int tube_resolution,
             glm::vec4 colour,
             std::unique_ptr<GLObject> component,
             std::unique_ptr<glm::mat4> component_matrix) {

  if (component) {
    this->component = std::move(component);
    guts_assert(component_matrix != nullptr,
                "if component is defined, so too must component_matrix");
    this->component_matrix = std::move(component_matrix);
  } else {
    this->component = std::make_unique<Tube>(tube_resolution, colour);
    this->component_matrix = std::make_unique<glm::mat4>(TMPL_TUBE_MATRIX);
    this->is_using_tube = true;
  }

  // Start the complete pattern with the initial one.
  std::string initial_pattern = internal::StringToUpper(production_pattern);
  std::string complete_pattern = initial_pattern;
  // Run through the full iteration count, replacing 'F' with the whole initial
  // pattern.
  for (int i = 0; i < iterations; i++) {
    std::string tmp;
    tmp.reserve(complete_pattern.length());
    for (char ch : complete_pattern) {
      if (ch == 'F') {
        tmp.append(initial_pattern);
      } else {
        tmp.push_back(ch);
      }
    }
    complete_pattern = std::move(tmp);
  }

  // Create segment vector and set its capacity to save many reallocations.
  std::vector<LTreeSegment> segments;
  segments.reserve(internal::StringCountCharacter('F', complete_pattern));

  // This stack will drive branch instructions, starting at the origin.
  // We also insert a stack protector frame at the bottom to catch
  // malformed production strings which pop more than push.
  std::stack<BranchStackFrame> branch_stack;
  branch_stack.push(BranchStackFrame {
      glm::vec3(0.f),
      0.f,
      0.f,
      0,
      true,
  });

  // RNGs used to vary branching in angle and 3D rotation
  std::mt19937 twister = GetMT_RNG();
  // angle_l_dst for L-System branches, angle_3d_dst for movements in 3D space.
  std::normal_distribution<float> angle_l_dst(0.8f, 0.4f);
  std::uniform_real_distribution<float> angle_3d_dst(-1.5f, 1.5f);

  glm::vec3 last_point = glm::vec4(0.f);
  float current_angle_l = 0.f;
  float current_angle_3d = 0.f;
  unsigned int depth = 0;

  // Run a simple stack machine over the entire completed pattern.
  auto start = complete_pattern.begin();
  auto end = complete_pattern.end();
  for (auto iter = start; iter < end; iter++) {
    char ch = *iter;
    switch (ch) {
      case 'F': {
        // Project a new segment
        if (depth > max_depth) max_depth = depth;
        glm::vec3 new_point = ProjectSegment(last_point, current_angle_l,
                                             current_angle_3d);
        auto segment = LTreeSegment{
            last_point,
            new_point,
            current_angle_l,
            current_angle_3d,
            depth,
        };
        segments.push_back(segment);
        last_point = new_point;
        depth += 1;
        break;
      }
      case '+': {
        // CW branch rotation for L-System, standard move for 3D
        current_angle_l += angle_l_dst(twister);
        current_angle_3d += angle_3d_dst(twister);
        break;
      }
      case '-': {
        // CCW branch rotation for L-System, standard move for 3D
        current_angle_l -= angle_l_dst(twister);
        current_angle_3d += angle_3d_dst(twister);
        break;
      }
      case '[': {
        // Push a stack frame.
        branch_stack.push(BranchStackFrame {
            last_point,
            current_angle_l,
            current_angle_3d,
            depth,
        });
        break;
      }
      case ']': {
        // Pop a stack frame
        BranchStackFrame &frame = branch_stack.top();
        if (frame.base_marker) {
          guts_error("Unbalanced push/pop count in production string "
                         + initial_pattern);
        }
        last_point = frame.last_point;
        current_angle_l = frame.angle_l;
        current_angle_3d = frame.angle_3d;
        depth = frame.depth;
        branch_stack.pop();
        break;
      }
      default: {
        guts_error("Invalid character in production string: " + ch);
      }
    }
  }

  // Check for imbalanced stack calls
  if (!branch_stack.top().base_marker) {
    guts_error("Unbalanced push/pop count in production string "
                   + initial_pattern);
  }

  // Convert segments into transform matrices
  this->transforms.reserve(segments.size());
  for (LTreeSegment segment : segments) {
    glm::mat4 transform = glm::mat4(1.f);
    transform = glm::translate(transform, segment.start);
    transform = glm::rotate(transform, segment.angle_3d, AXIS_Y);
    transform = glm::rotate(transform, segment.angle_l, AXIS_Z);
    this->transforms.push_back(SegmentTransform {
        transform,
        segment.depth,
    });
  }
}

void LTree::OverrideAttributeLayout(GLuint attr_vertices,
                                    GLuint attr_colours,
                                    GLuint attr_normals) {
  if (is_using_tube) {
    // If we've set is_using_tube, we know it's a tube.
    auto tube = dynamic_cast<Tube*>(this->component.get());
    guts_assert(tube != nullptr, "dynamic cast to tube must succeed.");
    tube->OverrideAttributeLayout(attr_vertices, attr_colours, attr_normals);
  } else {
    guts_error("Attempt to override attrs when a custom object is in use!");
  }
}
void LTree::Render(GLRenderMode mode,
                   GLUniform<glm::mat4> &model_uniform,
                   glm::mat4 &model_matrix,
                   GLUniform<glm::mat3> &normal_uniform,
                   glm::mat4 &view_matrix) {
  for (SegmentTransform _transform : transforms) {
    glm::mat4 transform = _transform.transform;
    float scale = (max_depth - _transform.depth)
        / static_cast<float>(max_depth);
    auto scaling_factor = glm::scale(glm::mat4(1.f),
                                     glm::vec3(1.f, scale, 1.f));
    transform = model_matrix * transform
        * scaling_factor * *this->component_matrix;
    glm::mat3 normal = InverseTranspose(glm::mat3(view_matrix * transform));
    model_uniform.Set(transform);
    normal_uniform.Set(normal);
    this->component->Render(mode);
  }
}

} // namespace objs
} // namespace guts