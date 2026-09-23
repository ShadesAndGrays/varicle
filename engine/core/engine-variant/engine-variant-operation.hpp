#pragma once

#include "engine-variant.hpp"
#include "engine/core/lerp.hpp"

namespace varicle {

template <class... Ts> struct overloaded : Ts... {
    using Ts::operator()...;
};

enum class OpType {
    Assign,   // Replace the old value entirely
    Add,      // Add to the current value
    Multiply, // Add to the current value
    Lerp      // Smoothly blend toward a value
};

namespace op {
struct Assign {
    EngineVariant value;
};
struct Add {
    EngineVariant value;
};
struct Mul {
    EngineVariant value;
};
struct Lerp {
    EngineVariant value;
    float         alpha;

    // Defaults to linear
    CurveFunc curve = Curve::linear;
    // Defaults to In
    EaseMode ease = EaseMode::In;
};
} // namespace op

using Operation = std::variant<op::Assign, op::Add, op::Mul, op::Lerp>;

struct VariantOpRequest {
    EngineVariant source;    // The variable we want to change
    Operation     operation; // How we want to change it

    static VariantOpRequest
    assign(const EngineVariant& source, const EngineVariant& val) {
        return { source, op::Assign{ val } };
    };

    static VariantOpRequest
    add(const EngineVariant& source, const EngineVariant& val) {
        return { source, op::Add{ val } };
    };

    static VariantOpRequest
    mul(const EngineVariant& source, const EngineVariant& val) {
        return { source, op::Mul{ val } };
    };

    static VariantOpRequest lerp(
        const EngineVariant& source,
        const EngineVariant& target_val,
        float                alpha,
        CurveFunc            curve = Curve::linear,
        EaseMode             ease  = EaseMode::In
    ) {
        return { source,
                 op::Lerp{
                     target_val,
                     alpha,
                     curve,
                     ease,
                 } };
    };
};

class VariantOpManager {

  public:
    VariantOpManager() {}
    static EngineVariant Execute(const VariantOpRequest& req) {
        EngineVariant result;

        std::visit(
            Overloaded{
                [&](const op::Assign& op) { result = op.value; },
                [&](const op::Add& op) {
                    result.data = std::visit(
                        [](const auto& a,
                           const auto& b) -> EngineVariant::InternalVariant {
                            if constexpr (requires { a + b; })
                                return a + b;
                            return a;
                        },
                        req.source.data,
                        op.value.data
                    );
                },
                [&](const op::Mul& op) {
                    result.data = std::visit(
                        [](const auto& a,
                           const auto& b) -> EngineVariant::InternalVariant {
                            if constexpr (requires { a * b; })
                                return a * b;
                            return a;
                        },
                        req.source.data,
                        op.value.data
                    );
                },
                [&](const op::Lerp& op) {
                    const float eased_t =
                        LerpUtil::ease(op.alpha, op.curve, op.ease);

                    result.data = std::visit(
                        [eased_t](
                            const auto& a, const auto& b
                        ) -> EngineVariant::InternalVariant {
                            using T = std::decay_t<decltype(a)>;
                            using U = std::decay_t<decltype(b)>;

                            if constexpr (std::is_same_v<T, U>) {
                                if constexpr (requires {
                                                  LerpUtil::lerp(a, b, eased_t);
                                              }) {
                                    return LerpUtil::lerp(a, b, eased_t);
                                } else if constexpr (
                                    std::is_same_v<T, std::string>
                                ) {
                                    return eased_t >= 0.5 ? b : a;
                                }
                            }
                            return a;
                        },
                        req.source.data,
                        op.value.data
                    );
                },
            },
            req.operation
        );

        return result;
    }
};

} // namespace varicle
