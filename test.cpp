#include <cross_compiler_interface\interface_unknown.hpp>
#include <gtest\gtest.h>
#include <gmock\gmock.h>
#include "gmock_cross_compiler_interface.hpp"


template<class T>
struct TestInterface:public cross_compiler_interface::define_interface<T>{

    cross_compiler_interface::cross_function<TestInterface,0,int(int)> F1;

    TestInterface():F1(this){}
    

};

TEST(Test1,Test1){
    mock<TestInterface> m;
    using testing::_;
    CC_EXPECT_CALL(m,F1.With(1)).Times(1);
    m.F1(1);

}

int main(int argc, char** argv) {
  // The following line must be executed to initialize Google Mock
  // (and Google Test) before running the tests.
  ::testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}