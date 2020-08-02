#include "random.h"

#include <ctime>
#include <random>

namespace reone {

static std::default_random_engine g_generator(time(nullptr));

float random(float min, float max) {
    std::uniform_real_distribution<float> distr(min, max);
    return distr(g_generator);
}

} // namespace reone
