## class converter::ConvertFromVal< T, T2S_Format_std_TtoS >

Convertor class implementation from integer types TO string. This conversion is achieved using 'std::to_string'.  

---

```c++
template<c_integer_type T> static std::string ToStr (const T & val)
```
Converts integer datatype to string. 

**Parameters**
- `val` numerical value. 

**Returns:**
- string holding a integer represenation. Else throws error on conversion failure. 

---

###### API documentation generated using [Doxygenmd](https://github.com/d99kris/doxygenmd)

