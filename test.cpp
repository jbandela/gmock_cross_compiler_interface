#include <cross_compiler_interface\interface_unknown.hpp>
#include <gtest\gtest.h>
#include <gmock\gmock.h>
#include "gmock_cross_compiler_interface.hpp"

using cross_compiler_interface::cross_function;

template<class T>
struct TestInterface:public cross_compiler_interface::define_interface_unknown<T,
	// {24A98713-374C-4975-808C-08C9E3E6CA28}
	cross_compiler_interface::uuid<
	0x24A98713,0x374C,0x4975,0x80,0x8C,0x08,0xC9,0xE3,0xE6,0xCA,0x28
	>
>{

    cross_function<TestInterface,0,int(int)> F1;
    cross_function<TestInterface,1,int(int)> F2;

    TestInterface():F1(this),F2(this){}
    

};

TEST(SimpleMockTests,Test1){
    using cross_compiler_interface::mock;
    using cross_compiler_interface::use_unknown;
    mock<TestInterface> m;
    CC_EXPECT_CALL(m,F1.With(1)).Times(1);
    CC_EXPECT_CALL(m,AddRef.With()).Times(1);

    CC_ON_CALL(m,F2.With(testing::_)).WillByDefault(testing::Return(200));

    auto use = m.get_use_interface();

    use_unknown<TestInterface> it(use,true);


    use.F1(1);

    ASSERT_EQ(200,use.F2(2));

}

int main(int argc, char** argv) {
  // The following line must be executed to initialize Google Mock
  // (and Google Test) before running the tests.
  ::testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}