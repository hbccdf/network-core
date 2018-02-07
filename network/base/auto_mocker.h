#pragma once
#include <memory>
#include <cpp_free_mock.h>

#define make_mocker(function) auto_close_mocker(MOCKER(function))

template<typename T>
std::shared_ptr<T> auto_close_mocker(std::shared_ptr<T> mocker)
{
  return std::shared_ptr<T>(mocker.get(), [](T* ptr) { ptr->RestoreToReal(); });
}

template<typename T>
class auto_hooker
{
public:
  typedef std::shared_ptr<T> mocker_t;
  typedef typename T::StubFunctionType stub_func_t;
  typedef typename T::FunctionType func_t;
  typedef typename ::testing::MockSpec<stub_func_t> mock_spec_t;
  typedef ::testing::internal::TypedExpectation<stub_func_t> except_t;

public:
  auto_hooker(mocker_t mocker, const char* file = "", int line = 0, const char* obj = "")
    : mocker_(mocker), file_(file), line_(line), obj_name_(obj)
  {}

  auto_hooker(auto_hooker&& other)
  {
    mocker_ = other.mocker_;
    file_ = other.file_;
    line_ = other.line_;
    obj_name_ = other.obj_name_;
    except_ = other.except_;

    other.mocker_ = nullptr;
    other.file_ = nullptr;
    other.line_ = -1;
    other.obj_name_ = nullptr;
    other.except_ = nullptr;
  }

  auto_hooker& operator=(auto_hooker&& other)
  {
    mocker_ = other.mocker_;
    file_ = other.file_;
    line_ = other.line_;
    obj_name_ = other.obj_name_;
    except_ = other.except_;

    other.mocker_ = nullptr;
    other.file_ = nullptr;
    other.line_ = -1;
    other.obj_name_ = nullptr;
    other.except_ = nullptr;
    return *this;
  }

  auto_hooker(const auto_hooker& other)
  {
    mocker_ = other.mocker_;
    file_ = other.file_;
    line_ = other.line_;
    obj_name_ = other.obj_name_;
    except_ = other.except_;
  }

  auto_hooker& operator=(const auto_hooker& other)
  {
    mocker_ = other.mocker_;
    file_ = other.file_;
    line_ = other.line_;
    obj_name_ = other.obj_name_;
    except_ = other.except_;
    return *this;
  }

private:
  mocker_t mocker_ = nullptr;
  except_t* except_ = nullptr;
  const char* file_ = nullptr;
  int line_ = -1;
  const char* obj_name_ = nullptr;

public:

  template<typename ... P>
  auto_hooker& except(const P&... p)
  {
    /*if (except_ != nullptr)
      return *this;*/
    except_ = &((*mocker_).gmock_CppFreeMockStubFunction(p ...).InternalExpectedAt(file_, line_, obj_name_, mocker_->functionName.c_str()));
    return *this;
  }

  template<typename ... P>
  auto_hooker& operator()(const P&... p)
  {
    return except(p ...);
  }

  except_t& on()
  {
    return *except_;
  }

  template<typename C>
  auto_hooker<T>& times(const C& count)
  {
    except_->Times(count);
    return *this;
  }

  template<typename ACTION>
  auto_hooker<T>& will(const ACTION& action, typename std::enable_if<!std::is_pod<ACTION>::value>::type* = 0)
  {
    except_->WillOnce(action);
    return *this;
  }

  auto_hooker<T>& will()
  {
    except_->WillOnce(testing::Return());
    return *this;
  }

  template<typename ACTION>
  auto_hooker<T>& will(const ACTION& action, typename std::enable_if<std::is_pod<ACTION>::value>::type* = 0)
  {
    except_->WillOnce(testing::Return(action));
    return *this;
  }

  template<typename ACTION>
  auto_hooker<T>& will_all(const ACTION& action, typename std::enable_if<!std::is_pod<ACTION>::value>::type* = 0)
  {
    except_->WillRepeatedly(action);
    return *this;
  }
  template<typename ACTION>
  auto_hooker<T>& will_all(const ACTION& action, typename std::enable_if<std::is_pod<ACTION>::value>::type* = 0)
  {
    except_->WillRepeatedly(testing::Return(action));
    return *this;
  }
  auto_hooker<T>& will_all()
  {
    except_->WillRepeatedly(testing::Return());
    return *this;
  }
};


template<typename T>
auto_hooker<T> make_auto_hooker(std::shared_ptr<T> mocker, const char* file = "", int line = 0, const char* obj = "")
{
  return auto_hooker<T>(mocker, file, line, obj);
}

#define make_hooker(obj, func) \
  auto obj = make_auto_hooker(make_mocker(func), __FILE__, __LINE__, #obj)

#define mock_func(...) __VA_ARGS__




template<typename T> 
class auto_mocker
{
public:
  typedef T except_t;
public:
  auto_mocker(except_t* p_except) : except_(p_except) {}

  auto_mocker(auto_mocker&& other)
  {
    except_ = other.except_;
    other.except_ = nullptr;
  }

  auto_mocker& operator=(auto_mocker&& other)
  {
    except_ = other.except_;
    other.except_ = nullptr;
    return *this;
  }

  auto_mocker(const auto_mocker& other)
  {
    except_ = other.except_;
  }

  auto_mocker& operator=(const auto_mocker& other)
  {
    except_ = other.except_;
    return *this;
  }

private:
  except_t* except_ = nullptr;

public:
  except_t& on()
  {
    return *except_;
  }

  template<typename C>
  auto_mocker<T>& times(const C& count)
  {
    except_->Times(count);
    return *this;
  }

  template<typename ACTION>
  auto_mocker<T>& will(const ACTION& action, typename std::enable_if<!std::is_pod<ACTION>::value>::type* = 0)
  {
    except_->WillOnce(action);
    return *this;
  }

  auto_mocker<T>& will()
  {
    except_->WillOnce(testing::Return());
    return *this;
  }

  template<typename ACTION>
  auto_mocker<T>& will(const ACTION& action, typename std::enable_if<std::is_pod<ACTION>::value>::type* = 0)
  {
    except_->WillOnce(testing::Return(action));
    return *this;
  }

  template<typename ACTION>
  auto_mocker<T>& will_all(const ACTION& action, typename std::enable_if<!std::is_pod<ACTION>::value>::type* = 0)
  {
    except_->WillRepeatedly(action);
    return *this;
  }
  template<typename ACTION>
  auto_mocker<T>& will_all(const ACTION& action, typename std::enable_if<std::is_pod<ACTION>::value>::type* = 0)
  {
    except_->WillRepeatedly(testing::Return(action));
    return *this;
  }
  auto_mocker<T>& will_all()
  {
    except_->WillRepeatedly(testing::Return());
    return *this;
  }
};

template<typename T>
auto_mocker<T> make_auto_mocker(T* t)
{
  return auto_mocker<T>(t);
}

#define except_mocker(obj, call) \
  make_auto_mocker(&(EXPECT_CALL(obj, call)))


#define add_failure ADD_FAILURE
#define add_failure_at ADD_FAILURE_AT

//#define fail() FAIL()
//#define succeed() SUCCEED()

#define expect_throw EXPECT_THROW
#define expect_no_throw EXPECT_NO_THROW
#define expect_any_throw EXPECT_ANY_THROW
#define assert_throw ASSERT_THROW
#define assert_no_throw ASSERT_NO_THROW
#define assert_any_throw ASSERT_ANY_THROW

#define expect_true EXPECT_TRUE
#define expect_false EXPECT_FALSE
#define assert_true ASSERT_TRUE
#define assert_false ASSERT_FALSE

#define expect_eq EXPECT_EQ
#define expect_ne EXPECT_NE
#define expect_le EXPECT_LE
#define expect_lt EXPECT_LT
#define expect_ge EXPECT_GE
#define expect_gt EXPECT_GT

#define assert_eq ASSERT_EQ
#define assert_ne ASSERT_NE
#define assert_le ASSERT_LE
#define assert_lt ASSERT_LT
#define assert_ge ASSERT_GE
#define assert_gt ASSERT_GT

#define expect_streq EXPECT_STREQ
#define expect_strne EXPECT_STRNE
#define expect_strcaseeq EXPECT_STRCASEEQ
#define expect_strcasene EXPECT_STRCASENE

#define assert_streq ASSERT_STREQ
#define assert_strne ASSERT_STRNE
#define assert_strcaseeq ASSERT_STRCASEEQ
#define assert_strcasene ASSERT_STRCASENE

#define expect_float_eq EXPECT_FLOAT_EQ
#define expect_double_eq EXPECT_DOUBLE_EQ
#define assert_float_eq ASSERT_FLOAT_EQ
#define assert_double_eq ASSERT_DOUBLE_EQ
#define expect_near EXPECT_NEAR
#define assert_near ASSERT_NEAR

#define expect_hresult_succeeded EXPECT_HRESULT_SUCCEEDED
#define assert_hresult_succeeded ASSERT_HRESULT_SUCCEEDED
#define expect_hresult_failed EXPECT_HRESULT_FAILED
#define assert_hresult_failed ASSERT_HRESULT_FAILED

#define assert_no_fatal_failure ASSERT_NO_FATAL_FAILURE
#define expect_no_fatal_failure EXPECT_NO_FATAL_FAILURE

#define scoped_trace SCOPED_TRACE

#define assert_is_null(p) ((p) == nullptr)
#define assert_not_null(p) ((p) != nullptr)