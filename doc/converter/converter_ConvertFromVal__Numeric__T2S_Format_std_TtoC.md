## class converter::ConvertFromVal< T, T2S_Format_std_TtoC >

```c++
template<c_integer_type T>
struct ConvertFromVal<T, T2S_Format_std_TtoC >
```
Convertor class implementation from integer types TO string. This conversion is achieved using 'std::to_chars'.  

**Template Parameters**
- `T`                     'type' converted from, to string data.

---

```c++
static std::string ToStr (const T & val)
```
Converts integer datatype to string. 

**Parameters**
- `val` numerical value. 

**Returns:**
- string holding a integer represenation. Else throws error on conversion failure. 

---

```c++
static std::string ToStr_args (const T & val, int base = 10)
```
Converts integer datatype to string. 

**Parameters**
- `val` numerical value. 
- `base` integer base to use: a value between 2 and 36 (inclusive). 

**Returns:**
- string holding a integer represenation. Else throws error on conversion failure. 
---


---
## class converter::ConvertFromVal< T, T2S_Format_std_TtoC >

```c++
template<c_floating_point T>
struct ConvertFromVal<T, T2S_Format_std_TtoC >
```
Convertor class implementation from floating-point types TO string. This conversion is achieved using 'std::to_chars'.

**Template Parameters**
- `T`                     'floating-point type' converted from, to string data.

---

```c++
static std::string ToStr (const T & val)
```
Converts floating-point datatype to string. 

**Parameters**
- `val` numerical value. 

**Returns:**
- string holding a floating-point represenation. Else throws error on conversion failure. 

---

```c++
static std::string ToStr_args (const T & val, std::chars_format fmt)
```
Converts floating-point datatype to string. 

**Parameters**
- `val` numerical value. 
- `fmt` floating-point formatting to use, a bitmask of type 'std::chars_format'

**Returns:**
- string holding a floating-point represenation. Else throws error on conversion failure. 

---

```c++
static std::string ToStr_args (const T & val, std::chars_format fmt, int precision)
```
Converts floating-point datatype to string. 

**Parameters**
- `val` numerical value. 
- `fmt` floating-point formatting to use, a bitmask of type std::chars_format 
- `precision` floating-point precision to use 

**Returns:**
- string holding a floating-point represenation. Else throws error on conversion failure. 

---

###### API documentation generated using [Doxygenmd](https://github.com/d99kris/doxygenmd)

