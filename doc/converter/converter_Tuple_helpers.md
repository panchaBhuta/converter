## class converter::ConvertFromString< T_C >

```c++
template<typename ... T_C>
struct ConvertFromString
```

Convertor class implementation for tuple type, with underlying elements(of different types) individually converted from string.  

**Template Parameters**
 - `T_C`   T can be data-type such as int, double etc ; xOR C -> Conversion class statisfying concept 'c_S2Tconverter'.

---

```c++
static return_type ToVal (std::string const & strTuple, char seperator = ',')
```
Converts comma-seperated string to a 'tuple'. The input string has the format 'ele0,ele1,ele2...'. Each element of the tuple is converted to string using 't_S2Tconv<T_C>::ToVal(...)'. 

**Parameters**
- `theTuple` input tuple. 
- `seperator` char seperator between elements. 

**Returns:**
- string. 

---

---

## class converter::ConvertFromTuple< T_C >

```c++
template<typename ... T_C>
struct ConvertFromTuple
```

Convertor class implementation for tuple type, with underlying elements(of different types) individually converted to string.  

**Template Parameters**
- `T_C`                   T can be data-type such as int, double etc ; xOR C -> Conversion class statisfying concept 'c_T2Sconverter'.

---

```c++
static std::string ToStr (
  std::tuple< typename t_T2Sconv_c< T_C >::input_type ... > const & theTuple,
  char seperator = ',')
```
Converts variable holding 'tuple' value to string. The output string has the format 'ele0,ele1,ele2...'. Each element of the tuple is converted to string using 't_T2Sconv<T_C>::ToStr(...)'. 

**Parameters**
- `theTuple` input tuple. 
- `seperator` char seperator between elements. 

**Returns:**
- string. 

---

---

## Get tuple from a vector of string.

```c++
template <c_S2Tconverter ... S2Tconv>
inline void GetTuple(const std::vector<std::string>& dataVec,
                     size_t colIdx,
                     std::tuple<typename S2Tconv::return_type ...>& dataTuple)
```
populate a tuple from a vector of string.

**Template Parameters**
- `S2Tconv`             converter types that satisfies concept 'c_S2Tconverter'.

**Parameters**
- `dataVec`             vector of string, having string representation of numeric values.
- `colIdx`              start id of dataVec in case vector starts with the column-name.
- `dataTuple`           values stored in the tuple after performing string-to-value conversion.

---

---

## Set a vector of string from a tuple.

```c++
template <c_T2Sconverter ... T2Sconv>
inline void SetTuple(const std::tuple<typename T2Sconv::input_type ...>& dataTuple,
                     size_t colIdx,
                     std::vector<std::string>& dataVec)
```

populate a vector of string from a tuple.

**Template Parameters**
- `T2Sconv`             converter types that satisfies concept 'c_T2Sconverter'.

**Parameters**
- `dataTuple`           values stored in the tuple after performing string-to-value conversion.
- `colIdx`              start id of dataVec in case vector starts with the column-name.
- `dataVec`             vector of string, having string representation of numeric values.