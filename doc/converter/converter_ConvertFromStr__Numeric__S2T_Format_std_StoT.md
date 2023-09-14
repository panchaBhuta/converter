## class converter::ConvertFromStr< T, S2T_Format_std_StoT< T, PROCESS_ERR > >

```c++
template <c_integer_type T, FailureS2Tprocess PROCESS_ERR>
struct ConvertFromStr<T, S2T_Format_std_StoT<T, PROCESS_ERR> >
```

Convertor class implementation for integer types FROM string. This conversion is achieved using either of 'std::stoi' or 'std::stol', and similar std-functions based on type 'T'.

**Template Parameters**
- `T`                  'integer-type' converted to, from string data.
- `PROCESS_ERR`        enum variable of type FailureS2Tprocess.

---

```c++
static return_type ToVal (const std::string & str)
```
Converts string holding a integer represenation to integer datatype. 

**Parameters**
- `str` input string. 

**Returns:**
- Numerical value if conversion succeeds. Else throws error on conversion failure. 

---

```c++
static T ToVal_args (const std::string & str, std::size_t * pos = nullptr, int base = 10)
```
Converts string holding a integer represenation to integer datatype. 

**Parameters**
- `str` input string. 
- `pos` address of an integer to store the number of characters processed. 
- `base` integer base to use: a value between 2 and 36 (inclusive). 

**Returns:**
- Numerical value if conversion succeeds. Else throws error on conversion failure. 

---

---

```c++
template <c_floating_point T, FailureS2Tprocess PROCESS_ERR>
struct ConvertFromStr<T, S2T_Format_std_StoT<T, PROCESS_ERR> >
```
Convertor class implementation for floating types from string. This conversion is achieved using either of 'std::stof' or 'std::stod', or 'std::stdold' based on type 'T'.

**Template Parameters**
- `T`                     'floating-type' converted to, from string data.
- `PROCESS_ERR`            enum variable of type FailureS2Tprocess.

---

```c++
static return_type ToVal (const std::string & str)
```
Converts string holding a floating-number represenation to floating datatype. 

**Parameters**
- `str` input string. 

**Returns:**
- Numerical value if conversion succeeds. Else throws error on conversion failure. 

---

```c++
static T ToVal_args (const std::string & str, std::size_t * pos = nullptr)
```
Converts string holding a floating-number represenation to floating datatype. 

**Parameters**
- `str` input string. 
- `pos` address of an integer to store the number of characters processed. 

**Returns:**
- Numerical value if conversion succeeds. Else throws error on conversion failure. 

---

###### API documentation generated using [Doxygenmd](https://github.com/d99kris/doxygenmd)

