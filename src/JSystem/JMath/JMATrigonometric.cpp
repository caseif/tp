#include "JSystem/JSystem.h" // IWYU pragma: keep

#include "JSystem/JMath/JMATrigonometric.h"
#include "math.h"
#include "global.h"

static f32 dummy() {
    return 0.0f;
}

inline f64 getConst() {
    return 6.2831854820251465;
}

namespace JMath {

TSinCosTable<13, f32> sincosTable_ ATTRIBUTE_ALIGN(32);

TAtanTable<1024, f32> atanTable_ ATTRIBUTE_ALIGN(32);

TAsinAcosTable<1024, f32> asinAcosTable_ ATTRIBUTE_ALIGN(32);

}  // namespace JMath
