## class converter::ConvertFromVal< T, T2S_FORMAT_STREAM >

```c++
template<c_NOT_string T, c_formatOSS T2S_FORMAT_STREAM>
struct ConvertFromVal<T, T2S_FORMAT_STREAM>
```

Convertor class implementation for any types, TO string; using 'std::ostringstream'.  

**Template Parameters**
- `T`                     'type' converted from, to string data. (Not Applicable for string to string conversion)
- `T2S_FORMAT_STREAM`      Class which encapsulates conversion parameters/directives such as using 'Locale'.
---

```c++
static std::string ToStr (const T & val)
```
Converts numerical datatype from string holding a possibly-numerical value. 

**Parameters**
- `val` input possibly-numerical value. 

**Returns:**
- Output string if conversion succeeds. This type is any type that's supported by 'std::ostringstream'. Else throws 'std::invalid_argument' on conversion failure. 

---

###### API documentation generated using [Doxygenmd](https://github.com/d99kris/doxygenmd)

