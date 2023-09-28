//
// One header library for SAPPOROBDD and TdZdd C++ version
//
// Copyright (c) 2017 -- 2023 Jun Kawahara
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software
// and associated documentation files (the "Software"), to deal in the Software without
// restriction, including without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all copies or
// substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
// BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#ifndef SBDD_HELPER_TDZDD_H
#define SBDD_HELPER_TDZDD_H

#include <tdzdd/DdStructure.hpp>
#include <tdzdd/eval/ToBDD.hpp>
#include <tdzdd/eval/ToZBDD.hpp>
#include <tdzdd/spec/SapporoBdd.hpp>
#include <tdzdd/spec/SapporoZdd.hpp>

#ifdef __cplusplus // only support C++

namespace sbddh {

sbddextended_INLINE_FUNC
BDD tdzdd2sbdd(const tdzdd::DdStructure<2>& dd)
{
    return dd.evaluate(tdzdd::ToBDD());
}

sbddextended_INLINE_FUNC
ZBDD tdzdd2szbdd(const tdzdd::DdStructure<2>& dd)
{
    return dd.evaluate(tdzdd::ToZBDD());
}

sbddextended_INLINE_FUNC
tdzdd::DdStructure<2> sbdd2tdzdd(const BDD& f)
{
    tdzdd::SapporoBdd sbdd(f);
    return tdzdd::DdStructure<2>(sbdd);
}

sbddextended_INLINE_FUNC
tdzdd::DdStructure<2> sbdd2tdzdd(const ZBDD& f)
{
    tdzdd::SapporoZdd sbdd(f);
    return tdzdd::DdStructure<2>(sbdd);
}

sbddextended_INLINE_FUNC
tdzdd::DdStructure<2> szbdd2tdzdd(const ZBDD& f)
{
    tdzdd::SapporoZdd sbdd(f);
    return tdzdd::DdStructure<2>(sbdd);
}

} // end of namespace sbddh

#endif

#endif // SBDD_HELPER_TDZDD_H
