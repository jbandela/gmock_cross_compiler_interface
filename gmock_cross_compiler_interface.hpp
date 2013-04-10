#include<cross_compiler_interface\interface_unknown.hpp>
#include <gmock\gmock.h>



template<template<class> class Interface>
struct mock:public Interface<mock<Interface>>{

};


namespace cross_compiler_interface{
    	// size only


	template<template<class> class Iface,template<class> class Other, int Id,class R, class... Parms>
	struct cross_function<Iface<mock<Other>>,Id,R(Parms...)>
    {
        ::testing::FunctionMocker<R(Parms...)> f_;
        Iface<mock<Other>>* pthis_;

	template<class T>
	cross_function(T t):pthis_(t){}

    R operator()(Parms... p){
        f_.SetOwnerAndName(pthis_, typeid(*this).name());
        return f_.Invoke(p...);
    }
    template<class... T>
    ::testing::MockSpec<R(Parms...)>& With(const T&... t){
        f_.RegisterOwner(pthis_);
        return f_.With(t...);
    }
	};

}


#define CC_GMOCK_EXPECT_CALL_IMPL_(obj, call) \
    ((obj).##call).InternalExpectedAt(__FILE__, __LINE__, #obj, #call)
#define CC_EXPECT_CALL(obj, call) CC_GMOCK_EXPECT_CALL_IMPL_(obj, call)