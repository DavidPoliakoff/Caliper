// Copyright (c) 2015, Lawrence Livermore National Security, LLC.  
// Produced at the Lawrence Livermore National Laboratory.
//
// This file is part of Caliper.
// Written by David Boehme, boehme3@llnl.gov.
// LLNL-CODE-678900
// All rights reserved.
//
// For details, see https://github.com/scalability-llnl/Caliper.
// Please also see the LICENSE file for our additional BSD notice.
//
// Redistribution and use in source and binary forms, with or without modification, are
// permitted provided that the following conditions are met:
//
//  * Redistributions of source code must retain the above copyright notice, this list of
//    conditions and the disclaimer below.
//  * Redistributions in binary form must reproduce the above copyright notice, this list of
//    conditions and the disclaimer (as noted below) in the documentation and/or other materials
//    provided with the distribution.
//  * Neither the name of the LLNS/LLNL nor the names of its contributors may be used to endorse
//    or promote products derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS
// OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
// LAWRENCE LIVERMORE NATIONAL SECURITY, LLC, THE U.S. DEPARTMENT OF ENERGY OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
// ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

/// \file CaliFunctional.h
/// Caliper C++ Functional Annotation Utilities

#ifndef CALI_GOTCHA_H
#define CALI_GOTCHA_H

#include "Annotation.h"
#include "Caliper.h"
#include "cali_definitions.h"
// TODO: fluid, use or discard
#include <csignal>
#include "common/Variant.h"

#include <cstring>
#include <iostream>
#include <functional>
#include <tuple>
#include <type_traits>

#include<gotcha/gotcha.h>
#include "CaliFunctional.h"

namespace cali{

template<class F>
struct function_traits;
 
template<class R, class... Args>
struct function_traits<R(*)(Args...)> : public function_traits<R(Args...)>
{};
 
//via: https://functionalcpp.wordpress.com/2013/08/05/function-traits/
template<class R, class... Args>
struct function_traits<R(Args...)>
{
    using return_type = R;
    //using arg_tuple = decltype(std::make_tuple(...Args)); 
    using function_pointer_type = R(*)(Args...);
    using std_function_type = std::function<R(Args...)>;
    //template <template <typename...> typename T> using apt = T<Args...>;
    static constexpr std::size_t arity = sizeof...(Args);
 
    template <std::size_t N>
    struct argument
    {
        static_assert(N < arity, "error: invalid parameter index.");
        using type = typename std::tuple_element<N,std::tuple<Args...>>::type;
    };
};  

template<int N, class F>
struct runtime_wrapper;
 
template<int N, class R, class... Args>
struct runtime_wrapper<N,R(*)(Args...)> : public runtime_wrapper<N,R(Args...)>
{
   static R(**original_call())(Args...){
      static R(*original_call_data)(Args...);
      return &original_call_data;
   } 
   static R(**original_call_copy())(Args...){
      static R(*original_call_data)(Args...);
      return &original_call_data;
   } 
   static gotcha_binding_t*& bindings(){
     static gotcha_binding_t* bindings_data = new gotcha_binding_t;
     return bindings_data;
   }
   //static struct gotcha_binding_t bindings[1];
   static cali::ArgWrappedFunction<R(*)(Args...)>*& instrumented_version(){
     static cali::ArgWrappedFunction<R(*)(Args...)>* instrumented_version_data;
     return instrumented_version_data;
   }
   static bool& in_cali(){
     static bool in_cali_data = false;
     return in_cali_data;
   }
   runtime_wrapper(const char* name, R(*to_wrap)(Args...)){
     (*original_call_copy()) = to_wrap;
     bindings()[0].name = name;
     bindings()[0].wrapper_pointer = (void*)redirect<R>;
     bindings()[0].function_address_pointer = original_call();
     instrumented_version() = new ArgWrappedFunction<R(*)(Args...)>(cali::wrap_function_and_args(name, *(original_call())));
     gotcha_wrap(bindings(),1,"nothing_good");
     *(instrumented_version()) = ArgWrappedFunction<R(*)(Args...)>(cali::wrap_function_and_args(name, *(original_call())));
   }
   template<typename shadow_R = R>
   static auto redirect(Args... args) -> typename std::enable_if<
        !std::is_same<shadow_R, void>::value,
        shadow_R>::type {
     Caliper c = Caliper::sigsafe_instance(); 
     if (!c || in_cali()){
       auto call_target = *(original_call());
       return call_target(args...);
     }
     else{
       in_cali() = true;
       R ret_val = (*(instrumented_version()))(args...);
       in_cali() = false;
       return ret_val;
     }

   }
   template<typename shadow_R = R>
   static auto redirect(Args... args) -> typename std::enable_if<
        std::is_same<shadow_R, void>::value,
        shadow_R>::type {
     Caliper c = Caliper::sigsafe_instance(); 
     if (!c || in_cali()){
       auto call_target = *(original_call());
       return call_target(args...);
     }
     else{
       in_cali() = true;
       (*(instrumented_version()))(args...);
       in_cali() = false;
     }

   }
};
 
//via: https://functionalcpp.wordpress.com/2013/08/05/function-traits/
template<int N,class R, class... Args>
struct runtime_wrapper<N,R(Args...)>
{};  


template<int N,class R, class... Args >
auto gotcha_instrument_function(const char* name, R(*callable)(Args...)) -> runtime_wrapper<N,R(*)(Args...)>*  {
  return new runtime_wrapper<N,R(*)(Args...)>(name,callable);
}

#define cali_runtime_instrument(name,callee) (cali::gotcha_instrument_function<__COUNTER__>(name,callee))
} // end namespace cali
#endif
