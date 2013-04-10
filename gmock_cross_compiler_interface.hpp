#include<cross_compiler_interface\interface_unknown.hpp>
#include <gmock\gmock.h>



template<template<class> class Interface>
struct mock:public Interface<mock<Interface>>{

};


namespace cross_compiler_interface{
    	// size only

    namespace detail{
        template<class F>
        struct gmock_cross_function_helper_base{};

        template<class R, class... Parms>
        struct gmock_cross_function_helper_base<R(Parms...)>
        {



        };

    }
	template<template<class> class Iface,template<class> class Other, int Id,class R, class... Parms>
	struct cross_function<Iface<mock<Other>>,Id,R(Parms...)>
    {
        ::testing::FunctionMocker<R(Parms...)> f_;
        Iface<mock<Other>>* pthis_;

	template<class T>
	cross_function(T t):pthis_(t){}

    R operator()(Parms... p){
        f_.SetOwnerAndName(pthis_, typeid(*this).name());
        f_.Invoke(p...);
    }

    ::testing::MockSpec<R(Parms...)>& operator()(const testing::Matcher<Parms>&... p){
        f_.RegisterOwner(pthis_);
        return f_.With(p...);
    }
	};

}


#define CC_GMOCK_EXPECT_CALL_IMPL_(obj, call) \
    ((obj).##call).InternalExpectedAt(__FILE__, __LINE__, #obj, #call)
#define CC_EXPECT_CALL(obj, call) CC_GMOCK_EXPECT_CALL_IMPL_(obj, call)