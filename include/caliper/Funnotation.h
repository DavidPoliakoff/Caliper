#ifndef CALIPER_FUNNOTATION_H
#define CALIPER_FUNNOTATION_H
#include <caliper/Caliper.h>
#include <caliper/Annotation.h>
namespace cali{
template<class... Args>
struct Scoped {
static cali::Annotation Annotation(const char* name, int opt=0){
  Caliper proxy = cali::Caliper::CaliperForTags<Args...>();
  return cali::Annotation(name, opt, proxy);
}
};
}
#endif
