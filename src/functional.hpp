// Hey Emacs, this is -*- coding: utf-8; mode: c++ -*-
#ifndef __rh_ogorod_cpp_utils_functional_hpp__
#define __rh_ogorod_cpp_utils_functional_hpp__

#include <memory>

namespace rh_ogorod::cpp_utils {

template <typename Functor, typename Signature>
struct is_invocable {
 private:
  template <typename C>
  static constexpr auto check(bool) -> std::enable_if_t<
    static_cast<Signature>(&C::operator()) != nullptr,
    std::true_type>;

  template <typename C>
  static constexpr auto check(...) -> std::false_type;

  using result_type = decltype(check<Functor>(true));

 public:
  static constexpr bool value = result_type::value;
};

template <typename F, typename M>
static constexpr bool is_invocable_v = is_invocable<F, M>::value;

namespace impl {

template <class S>
class FunctorPackInterface;

template <typename R, typename... Args>
class FunctorPackInterface<R(Args...) noexcept> {
 public:
  virtual auto operator()(Args... args) -> R = 0;
};

template <typename R, typename... Args>
class FunctorPackInterface<R(Args...)> {
 public:
  virtual auto operator()(Args... args) -> R = 0;
};

template <typename F, typename M>
class FunctorPackBase {
 public:
  using Functor = F;
  using Method = M;

  explicit FunctorPackBase(const Functor& object)
      : FunctorPackBase{&Functor::operator(), object} {}

  explicit FunctorPackBase(Functor&& object)
      : FunctorPackBase{&Functor::operator(), std::move(object)} {}

 protected:
  auto method() -> const Method& {
    return m_method;
  };

  auto functor() -> const Functor& {
    return m_functor;
  };

 private:
  FunctorPackBase(Method method, const Functor& object)
      : m_method{method}, m_functor{object} {}

  FunctorPackBase(Method method, Functor&& object)
      : m_method{method}, m_functor{std::move(object)} {}

  Method m_method;
  Functor m_functor;
};

template <typename F, typename M>
class FunctorPack;

/// /b/; R(Args...)
/// /b/{

template <typename F, typename R, typename... Args>
class FunctorPack<F, R (F::*)(Args...)>
    : public FunctorPackBase<F, R (F::*)(Args...)>,
      public FunctorPackInterface<R(Args...)> {
 private:
  using Base = FunctorPackBase<F, R (F::*)(Args...)>;
  using typename Base::Functor;

 public:
  explicit FunctorPack(const Functor& functor) : Base{functor} {}

  explicit FunctorPack(Functor&& functor) : Base{std::move(functor)} {}

  auto operator()(Args... args) -> R override {
    return (this->functor().*this->method())(std::forward<Args>(args)...);
  };
};

/// /b/}

/// /b/;  R(Args...) const
/// /b/{

template <typename F, typename R, typename... Args>
class FunctorPack<F, R (F::*)(Args...) const>
    : public FunctorPackBase<F, R (F::*)(Args...) const>,
      public FunctorPackInterface<R(Args...)> {
 private:
  using Base = FunctorPackBase<F, R (F::*)(Args...) const>;
  using typename Base::Functor;

 public:
  explicit FunctorPack(const Functor& functor) : Base{functor} {}

  explicit FunctorPack(Functor&& functor) : Base{std::move(functor)} {}

  auto operator()(Args... args) -> R override {
    return (this->functor().*this->method())(std::forward<Args>(args)...);
  };
};

/// /b/}

/// /b/; R(Args...) noexcept
/// /b/{

template <typename F, typename R, typename... Args>
class FunctorPack<F, R (F::*)(Args...) noexcept>
    : public FunctorPackBase<F, R (F::*)(Args...) noexcept>,
      public FunctorPackInterface<R(Args...) noexcept> {
 private:
  using Base = FunctorPackBase<F, R (F::*)(Args...) noexcept>;
  using typename Base::Functor;

 public:
  explicit FunctorPack(const Functor& functor) : Base{functor} {}

  explicit FunctorPack(Functor&& functor) : Base{std::move(functor)} {}

  auto operator()(Args... args) noexcept -> R override {
    return (this->functor().*this->method())(std::forward<Args>(args)...);
  };
};

/// /b/}

/// /b/;  R(Args...) const noexcept
/// /b/{

template <typename F, typename R, typename... Args>
class FunctorPack<F, R (F::*)(Args...) const noexcept>
    : public FunctorPackBase<F, R (F::*)(Args...) const noexcept>,
      public FunctorPackInterface<R(Args...) noexcept> {
 private:
  using Base = FunctorPackBase<F, R (F::*)(Args...) const noexcept>;
  using typename Base::Functor;

 public:
  explicit FunctorPack(const Functor& functor) : Base{functor} {}

  explicit FunctorPack(Functor&& functor) : Base{std::move(functor)} {}

  auto operator()(Args... args) noexcept -> R override {
    return (this->functor().*this->method())(std::forward<Args>(args)...);
  };
};

/// /b/}

}  // namespace impl

template <typename R, typename... Args>
class Function;

template <typename R, typename... Args>
class Function<R(Args...) noexcept> {
 public:
  Function() = default;
  ~Function() = default;

  // NOLINTNEXTLINE(google-explicit-constructor)
  Function(std::nullptr_t) : m_funPackInterfaceSPtr{} {}

  Function(const Function& other)
      : m_funPackInterfaceSPtr{other.m_funPackInterfaceSPtr} {}

  // NOLINTNEXTLINE(google-explicit-constructor)
  Function(Function&& other) noexcept
      : m_funPackInterfaceSPtr{std::move(other.m_funPackInterfaceSPtr)} {}

  template <
    typename Functor,
    typename FunctorT = std::remove_cvref_t<Functor>,
    typename MethodVX = R (FunctorT::*)(Args...) noexcept,
    typename = std::enable_if_t<std::is_class_v<FunctorT>>,
    typename = std::enable_if_t<!std::is_same_v<Function, FunctorT>>,
    typename = std::enable_if_t<is_invocable_v<FunctorT, MethodVX>>>
  // NOLINTNEXTLINE(google-explicit-constructor)
  Function(const Functor& functor)
      : m_funPackInterfaceSPtr{
          std::make_shared<impl::FunctorPack<FunctorT, MethodVX>>(functor)} {}

  template <
    typename Functor,
    typename FunctorT = std::remove_cvref_t<Functor>,
    typename MethodVX = R (FunctorT::*)(Args...) noexcept,
    typename = std::enable_if_t<std::is_class_v<FunctorT>>,
    typename = std::enable_if_t<!std::is_same_v<Function, FunctorT>>,
    typename = std::enable_if_t<is_invocable_v<FunctorT, MethodVX>>>
  // NOLINTNEXTLINE(google-explicit-constructor)
  Function(Functor&& functor)
      : m_funPackInterfaceSPtr{
          std::make_shared<impl::FunctorPack<FunctorT, MethodVX>>(
            std::forward<Functor>(functor)
          )} {}

  template <
    typename Functor,
    typename FunctorT = std::remove_cvref_t<Functor>,
    typename MethodVX = R (FunctorT::*)(Args...) noexcept,
    typename MethodCX = R (FunctorT::*)(Args...) const noexcept,
    typename = std::enable_if_t<std::is_class_v<FunctorT>>,
    typename = std::enable_if_t<!std::is_same_v<Function, FunctorT>>,
    typename = std::enable_if_t<!is_invocable_v<FunctorT, MethodVX>>,
    typename = std::enable_if_t<is_invocable_v<FunctorT, MethodCX>>>
  // NOLINTNEXTLINE(google-explicit-constructor)
  Function(const Functor& functor)
      : m_funPackInterfaceSPtr{
          std::make_shared<impl::FunctorPack<FunctorT, MethodCX>>(functor)} {}

  template <
    typename Functor,
    typename FunctorT = std::remove_cvref_t<Functor>,
    typename MethodVX = R (FunctorT::*)(Args...) noexcept,
    typename MethodCX = R (FunctorT::*)(Args...) const noexcept,
    typename = std::enable_if_t<std::is_class_v<FunctorT>>,
    typename = std::enable_if_t<!std::is_same_v<Function, FunctorT>>,
    typename = std::enable_if_t<!is_invocable_v<FunctorT, MethodVX>>,
    typename = std::enable_if_t<is_invocable_v<FunctorT, MethodCX>>>
  // NOLINTNEXTLINE(google-explicit-constructor)
  Function(Functor&& functor)
      : m_funPackInterfaceSPtr{
          std::make_shared<impl::FunctorPack<FunctorT, MethodCX>>(
            std::forward<Functor>(functor)
          )} {}

  auto operator=(std::nullptr_t) -> Function& {
    m_funPackInterfaceSPtr.reset();
    return *this;
  }

  auto operator=(const Function& other) -> Function& {
    if (this != &other) {
      m_funPackInterfaceSPtr = other.m_funPackInterfaceSPtr;
    }

    return *this;
  }

  auto operator=(Function&& other) noexcept -> Function& {
    if (this != &other) {
      m_funPackInterfaceSPtr = std::move(other.m_funPackInterfaceSPtr);
    }

    return *this;
  }

  auto operator()(Args... args) noexcept -> R {
    auto& funPackInterface = *m_funPackInterfaceSPtr;
    return funPackInterface(std::forward<Args>(args)...);
  };

  auto operator()(Args... args) const noexcept -> R {
    auto& funPackInterface = *m_funPackInterfaceSPtr;
    return funPackInterface(std::forward<Args>(args)...);
  };

  explicit operator bool() const {
    return static_cast<bool>(m_funPackInterfaceSPtr);
  }

 private:
  using FunctorPackInterface = impl::FunctorPackInterface<R(Args...) noexcept>;
  std::shared_ptr<FunctorPackInterface> m_funPackInterfaceSPtr;
};

template <typename R, typename... Args>
class Function<R(Args...)> {
 public:
  Function() = default;
  ~Function() = default;

  // NOLINTNEXTLINE(google-explicit-constructor)
  Function(std::nullptr_t) : m_funPackInterfaceSPtr{} {}

  Function(const Function& other)
      : m_funPackInterfaceSPtr{other.m_funPackInterfaceSPtr} {}

  // NOLINTNEXTLINE(google-explicit-constructor)
  Function(Function&& other) noexcept
      : m_funPackInterfaceSPtr{std::move(other.m_funPackInterfaceSPtr)} {}

  template <
    typename Functor,
    typename FunctorT = std::remove_cvref_t<Functor>,
    typename MethodV = R (FunctorT::*)(Args...),
    typename = std::enable_if_t<std::is_class_v<FunctorT>>,
    typename = std::enable_if_t<!std::is_same_v<Function, FunctorT>>,
    typename = std::enable_if_t<is_invocable_v<FunctorT, MethodV>>>
  // NOLINTNEXTLINE(google-explicit-constructor)
  Function(const Functor& functor)
      : m_funPackInterfaceSPtr{
          std::make_shared<impl::FunctorPack<FunctorT, MethodV>>(functor)} {}

  template <
    typename Functor,
    typename FunctorT = std::remove_cvref_t<Functor>,
    typename MethodV = R (FunctorT::*)(Args...),
    typename = std::enable_if_t<std::is_class_v<FunctorT>>,
    typename = std::enable_if_t<!std::is_same_v<Function, FunctorT>>,
    typename = std::enable_if_t<is_invocable_v<FunctorT, MethodV>>>
  // NOLINTNEXTLINE(google-explicit-constructor)
  Function(Functor&& functor)
      : m_funPackInterfaceSPtr{
          std::make_shared<impl::FunctorPack<FunctorT, MethodV>>(
            std::forward<Functor>(functor)
          )} {}

  template <
    typename Functor,
    typename FunctorT = std::remove_cvref_t<Functor>,
    typename MethodV = R (FunctorT::*)(Args...),
    typename MethodC = R (FunctorT::*)(Args...) const,
    typename = std::enable_if_t<std::is_class_v<FunctorT>>,
    typename = std::enable_if_t<!std::is_same_v<Function, FunctorT>>,
    typename = std::enable_if_t<!is_invocable_v<FunctorT, MethodV>>,
    typename = std::enable_if_t<is_invocable_v<FunctorT, MethodC>>>
  // NOLINTNEXTLINE(google-explicit-constructor)
  Function(const Functor& functor)
      : m_funPackInterfaceSPtr{
          std::make_shared<impl::FunctorPack<FunctorT, MethodC>>(functor)} {}

  template <
    typename Functor,
    typename FunctorT = std::remove_cvref_t<Functor>,
    typename MethodV = R (FunctorT::*)(Args...),
    typename MethodC = R (FunctorT::*)(Args...) const,
    typename = std::enable_if_t<std::is_class_v<FunctorT>>,
    typename = std::enable_if_t<!std::is_same_v<Function, FunctorT>>,
    typename = std::enable_if_t<!is_invocable_v<FunctorT, MethodV>>,
    typename = std::enable_if_t<is_invocable_v<FunctorT, MethodC>>>
  // NOLINTNEXTLINE(google-explicit-constructor)
  Function(Functor&& functor)
      : m_funPackInterfaceSPtr{
          std::make_shared<impl::FunctorPack<FunctorT, MethodC>>(
            std::forward<Functor>(functor)
          )} {}

  auto operator=(std::nullptr_t) -> Function& {
    m_funPackInterfaceSPtr.reset();
    return *this;
  }

  auto operator=(const Function& other) -> Function& {
    if (this != &other) {
      m_funPackInterfaceSPtr = other.m_funPackInterfaceSPtr;
    }

    return *this;
  }

  auto operator=(Function&& other) noexcept -> Function& {
    if (this != &other) {
      m_funPackInterfaceSPtr = std::move(other.m_funPackInterfaceSPtr);
    }

    return *this;
  }

  auto operator()(Args... args) noexcept -> R {
    auto& funPackInterface = *m_funPackInterfaceSPtr;
    return funPackInterface(std::forward<Args>(args)...);
  };

  auto operator()(Args... args) const noexcept -> R {
    auto& funPackInterface = *m_funPackInterfaceSPtr;
    return funPackInterface(std::forward<Args>(args)...);
  };

  explicit operator bool() const {
    return static_cast<bool>(m_funPackInterfaceSPtr);
  }

 private:
  using FunctorPackInterface = impl::FunctorPackInterface<R(Args...)>;
  std::shared_ptr<FunctorPackInterface> m_funPackInterfaceSPtr;
};

}  // namespace rh_ogorod::cpp_utils

#endif  // __rh_ogorod_cpp_utils_functional_hpp__
