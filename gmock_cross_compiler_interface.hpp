#include<cross_compiler_interface\interface_unknown.hpp>
#include <gmock\gmock.h>

namespace cross_compiler_interface{

	template<template<class> class Iface>
	struct mock:
        private vtable_n<sizeof(Iface<size_only>)/sizeof(cross_function<Iface<size_only>,0,void()>)>,
        public Iface<mock<Iface>>
    { // Mocking


		mock(){

		}

		void set_runtime_parent(use_interface<Iface> parent){
			vtable_n_base* vnb = this;
			vnb->runtime_parent_ = parent.get_portable_base();
		}

		enum{num_functions = sizeof(Iface<size_only>)/sizeof(cross_function<Iface<size_only>,0,void()>)};

		// Simple checksum that takes advantage of sum of squares
		// Note that on implementations where the MAX_SIZE is not able to accommodate array elements in the millions this could fail
		enum{checksum = sizeof(Iface<checksum_only>)/sizeof(cross_function<InterfaceBase<checksum_only>,0,void()>)};

		// Simple check to catch simple errors where the Id is misnumbered uses sum of squares
		static_assert(checksum==(num_functions * (num_functions +1)*(2*num_functions + 1 ))/6,"The Id's for a cross_function need to be ascending order from 0, you have possibly repeated a number");

		using  vtable_n<sizeof(Iface<size_only>)/sizeof(cross_function<Iface<size_only>,0,void()>)>::get_portable_base;
		const use_interface<Iface> get_use_interface(){return use_interface<Iface>(reinterpret_portable_base<Iface>(get_portable_base()));}
	};




    namespace detail{

        template<class F>
        struct mock_helper{};

        template<class R, class... Parms>
        struct mock_helper<R(Parms...)>
        {
            ::testing::FunctionMocker<R(Parms...)> f_;

            template<class T>
            mock_helper(T t){
                f_.SetOwnerAndName(t, typeid(*t).name());
                f_.RegisterOwner(t);

            }

            std::function<R(Parms...)> get_invoke_func(){
                return [this](Parms... p){
                    return f_.Invoke(p...);
                };
            }
            template<class... T>
            ::testing::MockSpec<R(Parms...)>& With(const T&... t){
                return f_.With(t...);
            }
        };
    }

    template<template<class> class Iface,template<class> class T, int Id,class F>
	struct cross_function<Iface<mock<T>>,Id,F>:public detail::cross_function_implementation<true,Iface,Id + Iface<mock<T>>::base_sz,F>,
        public detail::mock_helper<F>
    {
		enum{N = Id + Iface<mock<T>>::base_sz};
		typedef detail::cross_function_implementation<true,Iface,Id + Iface<mock<T>>::base_sz,F> cfi_t;
		cross_function(Iface<mock<T>>* pi):cfi_t(
			static_cast<mock<T>*>(pi)->get_portable_base()),
            mock_helper<F>(this)
        
        {	
				static_assert(static_cast<int>(N) < mock<T>::num_functions,"Error in calculating size of vtable");
                cfi_t::set_function(*this,this->get_invoke_func());

		}

	};

    	// For Mocking
	template<template<class> class Iface, template<class> class T,int Id,class F1, class F2,class Derived,class FuncType>
	struct custom_cross_function<Iface<mock<T>>,Id,F1,F2,Derived,FuncType>:public FuncType,
		public detail::custom_cross_function_op_call<Derived,F1>,
        public detail::mock_helper<F1>
    { 
	private:
		friend detail::custom_cross_function_op_call<Derived,F1>;
		portable_base* p_;
	public:
		typedef custom_cross_function base_t;

		enum{N = Iface<mock<T>>::base_sz + Id};
	private:
		typedef detail::custom_function_vtable_functions<Iface,Derived,N,FuncType, F2> vtable_functions_t;
	public:

		custom_cross_function(Iface<mock<T>>* pi):p_(static_cast<mock<T>*>(pi)->get_portable_base()),
        mock_helper<F1>(this)
        {
			auto vn = static_cast<vtable_n_base*>(p_);
			vn->set_data(N,static_cast<FuncType*>(this));
			vn->add(N,&vtable_functions_t::vtable_entry_function);
			static_assert(static_cast<int>(N) < mock<T>::num_functions,"Error in calculating size of vtable");
            set_function(this->get_invoke_func());
		}

		typedef typename std::function<F1>::result_type ret;

		typedef typename detail::fn_ptr_helper<F2>::fn_ptr_t vtable_fn_ptr_t;
		typedef detail::mem_fn_helper<F1> tm;
		template<class C, typename tm:: template inner<C,Iface,N>::MFT mf>
		void set_mem_fn (C* c){
			typedef typename tm:: template inner<C,Iface,N>::MFT MF;
			typedef typename tm:: template inner<C,Iface,N>::ret_t R;
			typedef typename tm:: template inner<C,Iface,N>::vte_t vte_t;


			typedef vtable_n_base vn_t;
			vn_t* vn =static_cast<vn_t*>(p_);
			vn->set_data(N,c);
			vn->update(N,&vtable_functions_t:: template vtable_entry_function_mem_fn<C,MF,mf>);

		}

	protected:

		template<class Func>
		void set_function(Func f){
			detail::get_function<N,FuncType>(p_) = f;
		}

		vtable_fn_ptr_t get_vtable_fn()const{
			return reinterpret_cast<vtable_fn_ptr_t>(p_->vfptr[N]);
		}

		portable_base* get_portable_base()const{
			return p_;
		};


	};

}


#define CC_GMOCK_EXPECT_CALL_IMPL_(obj, call) \
    ((obj).##call).InternalExpectedAt(__FILE__, __LINE__, #obj, #call)
#define CC_EXPECT_CALL(obj,call) CC_GMOCK_EXPECT_CALL_IMPL_(obj, call)