/*
 * _convertTuple.h
 *
 * URL:      https://github.com/panchaBhuta/converter
 * Version:  v1.0
 *
 * Copyright (c) 2023-2023 Gautam Dhar
 * All rights reserved.
 *
 * converter is distributed under the BSD 3-Clause license, see LICENSE for details.
 *
 */

#pragma once

#include <converter/_common.h>
#include <converter/_convertS2T.h>
#include <converter/_convertT2S.h>



namespace converter
{
  // [=============================================================[ S2T_FORMAT

  // [===========[ workarounds in case std::apply() doesn't work as expected for a given compiler(MSVC)
  template <size_t IDX, c_S2Tconverter ... S2Tconv>
  inline static void getTupleElements(const std::vector<std::string>& dataVec,
                                      const size_t colIdx,
                                      std::tuple<typename S2Tconv::return_type ...>& dataTuple)
  {
    using _t_tuple_convertors = std::tuple< S2Tconv ...>;
    std::get<IDX>(dataTuple) = std::tuple_element_t<IDX, _t_tuple_convertors>::ToVal(dataVec.at(IDX+colIdx));
    if constexpr(IDX>0)
    {
      // "((IDX>0)?(IDX-1):0)" eliminates infinite compile time looping,
      // and we don't have to define function specialization for getTupleElements<0>()
      getTupleElements< ((IDX>0)?(IDX-1):0), S2Tconv ... >(dataVec,colIdx,dataTuple);
    }
  }
  // ]===========] workarounds in case std::apply() doesn't work as expected for a given compiler(MSVC)

  /**
   * @brief   populate a tuple from a vector of string.
   * @tparam  S2Tconv             converter types that satisfies concept 'c_S2Tconverter'.
   * @param   dataVec             vector of string, having string representation of numeric values.
   * @param   colIdx              start id of dataVec in case vector starts with the column-name.
   * @param   dataTuple           values stored in the tuple after performing string-to-value conversion.
   */
  template <c_S2Tconverter ... S2Tconv>
  inline void GetTuple(const std::vector<std::string>& dataVec,
                       size_t colIdx,
                       std::tuple<typename S2Tconv::return_type ...>& dataTuple)
  {
    //auto write_tuple = [&dataVec,&colIdx] (auto&& ... wrt_result) -> void   # doesnot work for msvc
    auto write_tuple = [&dataVec,&colIdx] (typename S2Tconv::return_type & ... wrt_result) -> void
    {
      //https://stackoverflow.com/questions/65261797/varadic-template-to-tuple-is-reversed
      // work around for make_tuple
      ( (wrt_result = S2Tconv::ToVal(dataVec.at(colIdx++))), ... );  // comma operator ensures the element order is from left to right
    };
    std::apply(write_tuple, dataTuple);
    //getTupleElements<(sizeof...(S2Tconv) -1), S2Tconv ...>(dataVec,colIdx,dataTuple);
  }
  // ]=============================================================] ConvertFromStr



  // [=============================================================[ ConvertFromVal

  // [===========[ workarounds in case std::apply() doesn't work as expected for a given compiler(MSVC)
  template <size_t IDX,c_T2Sconverter ... T2Sconv>
  inline static void setTupleElements(const std::tuple<typename T2Sconv::input_type ...>& dataTuple,
                                      const size_t colIdx,
                                      std::vector<std::string>& dataVec)
  {
    using _t_tuple_convertors = std::tuple< T2Sconv ...>;
    dataVec.at(IDX+colIdx) = std::tuple_element_t<IDX, _t_tuple_convertors>::ToStr(std::get<IDX>(dataTuple));
    if constexpr(IDX>0)
    {
      // "((IDX>0)?(IDX-1):0)" eliminates infinite compile time looping,
      // and we don't have to define function specialization for setTupleElements<0>()
      setTupleElements< ((IDX>0)?(IDX-1):0), T2Sconv ... >(dataTuple,colIdx,dataVec);
    }
  }
  // ]===========] workarounds in case std::apply() doesn't work as expected for a given compiler(MSVC)

  /**
   * @brief   populate a vector of string from a tuple.
   * @tparam  T2Sconv             converter types that satisfies concept 'c_T2Sconverter'.
   * @param   dataTuple           values stored in the tuple after performing string-to-value conversion.
   * @param   colIdx              start id of dataVec in case vector starts with the column-name.
   * @param   dataVec             vector of string, having string representation of numeric values.
   */
  template <c_T2Sconverter ... T2Sconv>
  inline void SetTuple(const std::tuple<typename T2Sconv::input_type ...>& dataTuple,
                       size_t colIdx,
                       std::vector<std::string>& dataVec)
  {
    //https://stackoverflow.com/questions/42494715/c-transform-a-stdtuplea-a-a-to-a-stdvector-or-stddeque
    //auto read_tuple = [&dataVec,&colIdx] (auto&& ... rowElem) -> void   # doesnot work for msvc
    auto read_tuple = [&dataVec,&colIdx] (typename T2Sconv::input_type const& ... rowElem) -> void
    {
      ( (dataVec.at(colIdx++) = T2Sconv::ToStr(rowElem)) , ... );
    };
    std::apply(read_tuple, dataTuple);
    //setTupleElements<(sizeof...(T2Sconv) -1), T2Sconv ... >(dataTuple,colIdx,dataVec);
  }

  /**
   * @brief   Convertor class implementation for tuple type, with underlying elements(of different types)
   *          individually converted to string.
   * @tparam  T_C                   T can be data-type such as int, double etc ;     xOR
   *                                C -> Conversion class statisfying concept 'c_T2Sconverter'.
   */
  template< typename ... T_C >
  struct ConvertFromTuple
  {
    // TODO unit test
    /**
     * @brief   Converts variable holding 'tuple' value to string.
     *          The output string has the format "[ ele0, ele1, ele2 ...]".
     *          Each element of the tuple is converted to string using 't_T2Sconv<T_C>::ToStr(...)'
     * @param   theTuple                 input tuple.
     * @returns string.
     */
    inline static std::string
    ToStr(std::tuple<typename t_T2Sconv_c<T_C>::input_type ...> const& theTuple)
    {
      std::stringstream ss;
      std::apply
      (
        [&ss] (typename t_T2Sconv_c<T_C>::input_type const&... tupleArgs)
        {
          ss << '[';
          std::size_t n{0};
          ((ss << t_T2Sconv_c<T_C>::ToStr(tupleArgs) << (++n != sizeof...(T_C) ? ", " : "")), ...);
          //((ss << tupleArgs << (++n != sizeof...(T_C) ? ", " : "")), ...);
          ss << ']';
        }, theTuple
      );
      return ss.str();
    }
  };

  // ]=============================================================] ConvertFromVal
}
